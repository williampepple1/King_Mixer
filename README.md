<p align="center">
  <img src="Resources/icon.png" alt="King Mixer" width="128" height="128">
</p>

<h1 align="center">King Mixer</h1>

<p align="center">
  <strong>Intelligent VST3 Channel Strip Plugin</strong>
</p>

---

A JUCE-based VST3 channel strip plugin that applies rule-based mixing presets based on genre and instrument selection. Place it on any track in your DAW for instant, professional-quality processing.

## Features

- **8-Band Interactive Parametric EQ**
  - 7 filter types per band: Peak, Low Shelf, High Shelf, Low Cut, High Cut, Band Pass, Notch
  - Draggable nodes on a real-time spectrum analyzer
  - Mouse wheel Q control, double-click reset, right-click context menu

- **Compressor** with real-time gain reduction meter, scrolling timeline, and transfer curve

- **Saturation** with tanh soft-clip waveshaper, before/after waveform comparison, and transfer curve

- **Advanced Reverb Engine**
  - Predelay, Decay, Size, Attack/Shape
  - Hi/Bass Damping, Early & Late Diffusion, Modulation, Output EQ
  - 5 Modes (Concert Hall, Room, Chamber, Cathedral, Plate) and 4 Colors (Clean, 1970s, 1980s, Now)
  - Dry/wet waveform overlay and decay ring visualization

- **Stereo Width** (mid-side processing)

- **Input/Output Gain Staging** with stereo VU meters and correlation meter

- **64 curated mix presets** (8 genres x 8 instruments) based on professional mixing conventions

- **5 selectable UI themes**: Charcoal (default), Midnight, Mocha, Royal Gold, Arctic

- **Mix Amount** knob and **Bypass** toggle

- Full state save/load via DAW sessions

- Available as **VST3** and **Standalone** application

### Supported Genres

Rock, Pop, Hip-Hop, EDM, Jazz, R&B, Metal, Classical

### Supported Instruments

Vocals, Drums, Bass, Electric Guitar, Acoustic Guitar, Keys/Synths, Strings, Brass

## Installation

Download the installer or ZIP from the releases:

- **Installer** (`KingMixerInstaller.exe`) — installs to your system VST3 directory automatically
- **ZIP** (`KingMixer_VST3.zip`) — extract manually to your VST3 folder

No external dependencies required.

## Building from Source

### Requirements

- CMake 3.22+
- C++17 compatible compiler (MSVC 2022, GCC 10+, Clang 12+)
- Git (for FetchContent to download JUCE)

### Steps

```bash
cmake -B build -S .
cmake --build build --config Release
```

### Output

- **VST3**: `build/AssistedMixing_artefacts/Release/VST3/King Mixer.vst3/`
- **Standalone**: `build/AssistedMixing_artefacts/Release/Standalone/King Mixer.exe`

To install the VST3 manually, copy the `.vst3` folder to:
- **Windows**: `C:\Program Files\Common Files\VST3\`
- **macOS**: `~/Library/Audio/Plug-Ins/VST3/`
- **Linux**: `~/.vst3/`

## Usage

1. Insert **King Mixer** on a track
2. Select the **Genre** and **Instrument**
3. Click **Apply Rule** to load recommended settings
4. Fine-tune using the tabbed interface (EQ, Comp, Sat, Reverb, Gain/Mix)

## Project Structure

```
Source/
  PluginProcessor.h/.cpp       - Audio processor, DSP chain, APVTS
  PluginEditor.h/.cpp          - Tabbed UI with header controls and theme selector
  DSP/
    GainStage.h/.cpp           - Input/output gain
    ParametricEQ.h/.cpp        - 8-band parametric EQ with 7 filter types
    Compressor.h/.cpp          - Dynamics compressor with level metrics
    Saturation.h/.cpp          - Tanh waveshaper saturation
    StereoWidth.h/.cpp         - Mid-side stereo width control
    ReverbSend.h/.cpp          - Advanced reverb with damping, diffusion, modulation, EQ
  Rules/
    GenreInstrumentDefs.h      - Genre and Instrument enums
    MixRule.h                  - MixRule struct
    MixRuleDatabase.h/.cpp     - 64 genre+instrument mix rules
  Analysis/
    SpectrumAnalyzer.h/.cpp    - FFT spectrum analysis
    LevelMeter.h/.cpp          - Peak/RMS level tracking
    WaveformBuffer.h/.cpp      - Circular waveform buffer
  UI/
    CustomLookAndFeel.h/.cpp   - Theme system with 5 presets
    EQPanel.h/.cpp             - Interactive EQ with draggable nodes
    CompressorPanel.h/.cpp     - Compressor visuals (GR meter, timeline, curve)
    SaturationPanel.h/.cpp     - Waveshaper curve and waveform comparison
    ReverbPanel.h/.cpp         - Reverb controls with decay ring and waveform overlay
    GainMixPanel.h/.cpp        - VU meters and stereo correlation
```

## License

This project uses JUCE under the [JUCE Personal/Small Business License](https://juce.com/legal/juce-8-licence/).
