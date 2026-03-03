#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <array>
#include <atomic>
#include <mutex>
#include <vector>

static constexpr int kMaxInstances = 64;

struct InstanceParamSnapshot
{
    float inputGain = 0.0f;
    float outputGain = 0.0f;

    struct EQBand { float freq = 1000.f; float gain = 0.f; float q = 1.f; int type = 0; bool enabled = true; };
    std::array<EQBand, 8> eqBands;

    float compThreshold = -20.f, compRatio = 2.f, compAttack = 10.f, compRelease = 100.f, compMakeup = 0.f;
    float satDrive = 0.f, satMix = 50.f;
    float stereoWidth = 0.f;

    float revMix = 0.f, revPredelay = 0.f, revDecay = 0.1f, revSize = 0.f;
    float revDampHiFreq = 20000.f, revDampHiShelf = 0.f;
    float revDampBassFreq = 20.f, revDampBassMult = 1.f;
    float revAttack = 0.f, revEarlyDiff = 0.f, revLateDiff = 0.f;
    float revModRate = 0.01f, revModDepth = 0.f;
    float revEqHighCut = 20000.f, revEqLowCut = 5.f;
    int revMode = 0, revColor = 0;

    float mixAmount = 0.f;
    bool bypass = false;

    int genreIndex = 0;
    int instrumentIndex = 0;
};

struct InstanceLevelSnapshot
{
    float peakL = 0.f, peakR = 0.f;
    float rmsL = 0.f, rmsR = 0.f;
    float gainReductionDB = 0.f;
};

class InstanceHub
{
public:
    static InstanceHub& getInstance()
    {
        static InstanceHub hub;
        return hub;
    }

    int registerInstance(const juce::String& name, bool isMaster)
    {
        const juce::SpinLock::ScopedLockType lock(spinLock);
        for (int i = 0; i < kMaxInstances; ++i)
        {
            if (!alive[i].load(std::memory_order_acquire))
            {
                trackNames[i] = name;
                isMasterFlags[i] = isMaster;
                soloFlags[i].store(false, std::memory_order_relaxed);
                muteFlags[i].store(false, std::memory_order_relaxed);
                instrumentIndices[i].store(0, std::memory_order_relaxed);
                hasPush[i].store(false, std::memory_order_relaxed);
                pushVersions[i].store(0, std::memory_order_relaxed);
                paramVersions[i].store(0, std::memory_order_relaxed);
                alive[i].store(true, std::memory_order_release);
                return i;
            }
        }
        return -1;
    }

    void unregisterInstance(int id)
    {
        if (!isValid(id)) return;
        const juce::SpinLock::ScopedLockType lock(spinLock);
        alive[id].store(false, std::memory_order_release);
        trackNames[id].clear();
    }

    void updateTrackName(int id, const juce::String& name)
    {
        if (!isValid(id)) return;
        const juce::SpinLock::ScopedLockType lock(spinLock);
        trackNames[id] = name;
    }

    void setInstrument(int id, int instrIdx)
    {
        if (!isValid(id)) return;
        instrumentIndices[id].store(juce::jlimit(0, 7, instrIdx), std::memory_order_relaxed);
    }

    int getInstrument(int id) const
    {
        if (!isValid(id)) return 0;
        return instrumentIndices[id].load(std::memory_order_relaxed);
    }

    void setIsMaster(int id, bool m)
    {
        if (!isValid(id)) return;
        const juce::SpinLock::ScopedLockType lock(spinLock);
        isMasterFlags[id] = m;
    }

    // Called from the audio thread — lock-free except for the spinlock-guarded copy
    void pushParamSnapshot(int id, const InstanceParamSnapshot& snap)
    {
        if (!isValid(id)) return;
        const juce::SpinLock::ScopedLockType lock(spinLock);
        paramSnaps[id] = snap;
        paramVersions[id].fetch_add(1, std::memory_order_release);
    }

    void pushLevelSnapshot(int id, const InstanceLevelSnapshot& snap)
    {
        if (!isValid(id)) return;
        const juce::SpinLock::ScopedLockType lock(spinLock);
        levelSnaps[id] = snap;
    }

    void pushParamsToTrack(int id, const InstanceParamSnapshot& snap)
    {
        if (!isValid(id)) return;
        const juce::SpinLock::ScopedLockType lock(spinLock);
        pushedSnaps[id] = snap;
        pushVersions[id].fetch_add(1, std::memory_order_release);
        hasPush[id].store(true, std::memory_order_release);
    }

    bool consumePushedParams(int id, InstanceParamSnapshot& outSnap, uint32_t& lastVersion)
    {
        if (!isValid(id)) return false;
        if (!hasPush[id].load(std::memory_order_acquire)) return false;

        uint32_t ver = pushVersions[id].load(std::memory_order_acquire);
        if (ver == lastVersion) return false;

        {
            const juce::SpinLock::ScopedLockType lock(spinLock);
            outSnap = pushedSnaps[id];
        }
        lastVersion = ver;
        hasPush[id].store(false, std::memory_order_release);
        return true;
    }

    void setSoloFromMaster(int id, bool solo)
    {
        if (!isValid(id)) return;
        soloFlags[id].store(solo, std::memory_order_relaxed);
    }

    void setMuteFromMaster(int id, bool mute)
    {
        if (!isValid(id)) return;
        muteFlags[id].store(mute, std::memory_order_relaxed);
    }

    bool getSolo(int id) const { return isValid(id) && soloFlags[id].load(std::memory_order_relaxed); }
    bool getMute(int id) const { return isValid(id) && muteFlags[id].load(std::memory_order_relaxed); }

    struct TrackView
    {
        int slotId = -1;
        juce::String name;
        bool isMaster = false;
        InstanceParamSnapshot params;
        InstanceLevelSnapshot levels;
        bool solo = false;
        bool mute = false;
        int instrumentIndex = 0;
    };

    std::vector<TrackView> getTrackViews() const
    {
        std::vector<TrackView> views;
        views.reserve(kMaxInstances);
        {
            const juce::SpinLock::ScopedLockType lock(spinLock);
            for (int i = 0; i < kMaxInstances; ++i)
            {
                if (alive[i].load(std::memory_order_relaxed) && !isMasterFlags[i])
                {
                    TrackView v;
                    v.slotId = i;
                    v.name = trackNames[i];
                    v.isMaster = false;
                    v.params = paramSnaps[i];
                    v.levels = levelSnaps[i];
                    v.solo = soloFlags[i].load(std::memory_order_relaxed);
                    v.mute = muteFlags[i].load(std::memory_order_relaxed);
                    v.instrumentIndex = instrumentIndices[i].load(std::memory_order_relaxed);
                    views.push_back(std::move(v));
                }
            }
        }
        return views;
    }

    bool isSlotAlive(int id) const
    {
        return isValid(id) && alive[id].load(std::memory_order_acquire);
    }

    InstanceLevelSnapshot getLevels(int id) const
    {
        if (!isValid(id)) return {};
        const juce::SpinLock::ScopedLockType lock(spinLock);
        return levelSnaps[id];
    }

    InstanceParamSnapshot getParams(int id) const
    {
        if (!isValid(id)) return {};
        const juce::SpinLock::ScopedLockType lock(spinLock);
        return paramSnaps[id];
    }

    juce::String getTrackName(int id) const
    {
        if (!isValid(id)) return {};
        const juce::SpinLock::ScopedLockType lock(spinLock);
        return trackNames[id];
    }

    bool isAnySoloed() const
    {
        for (int i = 0; i < kMaxInstances; ++i)
            if (alive[i].load(std::memory_order_acquire) && soloFlags[i].load(std::memory_order_relaxed))
                return true;
        return false;
    }

private:
    InstanceHub() = default;

    static bool isValid(int id) { return id >= 0 && id < kMaxInstances; }

    mutable juce::SpinLock spinLock;

    std::array<std::atomic<bool>, kMaxInstances> alive{};
    std::array<juce::String, kMaxInstances> trackNames;
    std::array<bool, kMaxInstances> isMasterFlags{};

    std::array<InstanceParamSnapshot, kMaxInstances> paramSnaps;
    std::array<std::atomic<uint32_t>, kMaxInstances> paramVersions{};
    std::array<InstanceLevelSnapshot, kMaxInstances> levelSnaps;

    std::array<InstanceParamSnapshot, kMaxInstances> pushedSnaps;
    std::array<std::atomic<uint32_t>, kMaxInstances> pushVersions{};
    std::array<std::atomic<bool>, kMaxInstances> hasPush{};

    std::array<std::atomic<bool>, kMaxInstances> soloFlags{};
    std::array<std::atomic<bool>, kMaxInstances> muteFlags{};

    std::array<std::atomic<int>, kMaxInstances> instrumentIndices{};
};
