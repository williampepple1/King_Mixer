#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <array>
#include <atomic>
#include <mutex>
#include <vector>
#include <functional>

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

struct InstanceInfo
{
    std::atomic<bool> alive{ false };
    juce::String trackName;
    bool isMaster = false;

    std::atomic<uint32_t> paramVersion{ 0 };
    InstanceParamSnapshot params;
    InstanceLevelSnapshot levels;

    std::atomic<bool> soloFromMaster{ false };
    std::atomic<bool> muteFromMaster{ false };

    std::atomic<uint32_t> masterPushVersion{ 0 };
    InstanceParamSnapshot masterPushedParams;
    std::atomic<bool> hasMasterPush{ false };

    void* processorPtr = nullptr;
};

class InstanceHub
{
public:
    static InstanceHub& getInstance()
    {
        static InstanceHub hub;
        return hub;
    }

    int registerInstance(void* processorPtr, const juce::String& name, bool isMaster)
    {
        std::lock_guard<std::mutex> lock(mtx);
        for (int i = 0; i < kMaxInstances; ++i)
        {
            if (!slots[i].alive.load())
            {
                slots[i].alive.store(true);
                slots[i].trackName = name;
                slots[i].isMaster = isMaster;
                slots[i].processorPtr = processorPtr;
                slots[i].soloFromMaster.store(false);
                slots[i].muteFromMaster.store(false);
                slots[i].hasMasterPush.store(false);
                slots[i].paramVersion.store(0);
                slots[i].masterPushVersion.store(0);
                return i;
            }
        }
        return -1;
    }

    void unregisterInstance(int slotId)
    {
        if (slotId < 0 || slotId >= kMaxInstances) return;
        std::lock_guard<std::mutex> lock(mtx);
        slots[slotId].alive.store(false);
        slots[slotId].processorPtr = nullptr;
        slots[slotId].trackName.clear();
    }

    void updateTrackName(int slotId, const juce::String& name)
    {
        if (slotId < 0 || slotId >= kMaxInstances) return;
        std::lock_guard<std::mutex> lock(mtx);
        slots[slotId].trackName = name;
    }

    void setIsMaster(int slotId, bool isMaster)
    {
        if (slotId < 0 || slotId >= kMaxInstances) return;
        slots[slotId].isMaster = isMaster;
    }

    void pushParamSnapshot(int slotId, const InstanceParamSnapshot& snap)
    {
        if (slotId < 0 || slotId >= kMaxInstances) return;
        slots[slotId].params = snap;
        slots[slotId].paramVersion.fetch_add(1);
    }

    void pushLevelSnapshot(int slotId, const InstanceLevelSnapshot& snap)
    {
        if (slotId < 0 || slotId >= kMaxInstances) return;
        slots[slotId].levels = snap;
    }

    void pushParamsToTrack(int slotId, const InstanceParamSnapshot& snap)
    {
        if (slotId < 0 || slotId >= kMaxInstances) return;
        slots[slotId].masterPushedParams = snap;
        slots[slotId].masterPushVersion.fetch_add(1);
        slots[slotId].hasMasterPush.store(true);
    }

    void setSoloFromMaster(int slotId, bool solo)
    {
        if (slotId < 0 || slotId >= kMaxInstances) return;
        slots[slotId].soloFromMaster.store(solo);
    }

    void setMuteFromMaster(int slotId, bool mute)
    {
        if (slotId < 0 || slotId >= kMaxInstances) return;
        slots[slotId].muteFromMaster.store(mute);
    }

    struct TrackView
    {
        int slotId = -1;
        juce::String name;
        bool isMaster = false;
        bool alive = false;
        InstanceParamSnapshot params;
        InstanceLevelSnapshot levels;
        bool solo = false;
        bool mute = false;
    };

    std::vector<TrackView> getTrackViews() const
    {
        std::vector<TrackView> views;
        for (int i = 0; i < kMaxInstances; ++i)
        {
            if (slots[i].alive.load() && !slots[i].isMaster)
            {
                TrackView v;
                v.slotId = i;
                v.name = slots[i].trackName;
                v.isMaster = slots[i].isMaster;
                v.alive = true;
                v.params = slots[i].params;
                v.levels = slots[i].levels;
                v.solo = slots[i].soloFromMaster.load();
                v.mute = slots[i].muteFromMaster.load();
                views.push_back(v);
            }
        }
        return views;
    }

    bool hasMasterInstance() const
    {
        for (int i = 0; i < kMaxInstances; ++i)
            if (slots[i].alive.load() && slots[i].isMaster)
                return true;
        return false;
    }

    bool isAnySoloed() const
    {
        for (int i = 0; i < kMaxInstances; ++i)
            if (slots[i].alive.load() && slots[i].soloFromMaster.load())
                return true;
        return false;
    }

    InstanceInfo& getSlot(int slotId) { return slots[slotId]; }
    const InstanceInfo& getSlot(int slotId) const { return slots[slotId]; }

private:
    InstanceHub() = default;
    std::array<InstanceInfo, kMaxInstances> slots;
    mutable std::mutex mtx;
};
