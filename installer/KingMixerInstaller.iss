[Setup]
AppName=King Mixer
AppVersion=1.1.0
AppPublisher=KingMixer
AppPublisherURL=https://github.com
DefaultDirName={commoncf}\VST3\King Mixer.vst3
DirExistsWarning=no
DisableProgramGroupPage=yes
OutputDir=..\
OutputBaseFilename=KingMixerInstaller
Compression=lzma2/ultra64
SolidCompression=yes
PrivilegesRequired=admin
UninstallDisplayName=King Mixer VST3
WizardStyle=modern
DisableDirPage=no
InfoBeforeFile=about.txt
DisableWelcomePage=no
SetupIconFile=..\Resources\icon.ico
UninstallDisplayIcon={app}\Contents\Resources\icon.ico
WizardImageFile=..\Resources\icon.png
WizardSmallImageFile=..\Resources\icon.png

[Messages]
WelcomeLabel1=Welcome to King Mixer Setup
WelcomeLabel2=This will install King Mixer v1.1.0 on your computer.%n%nKing Mixer is a professional VST3 channel strip plugin with an 8-band interactive EQ, compressor, saturation, advanced reverb engine, stereo width control, and a Master Bus Controller that lets you manage every track instance from one place.%n%nFeatures include 64 genre/instrument presets, real-time spectrum analysis, solo/mute from the master bus, 5 selectable UI themes, and FabFilter-inspired visual feedback.%n%nIt is recommended that you close any running DAWs before continuing.
SelectDirLabel3=King Mixer VST3 will be installed into the following folder.%nThe default location is the standard VST3 directory. Most DAWs scan this folder automatically.
InfoBeforeClickLabel=Please read the following information about King Mixer before continuing.

[Files]
Source: "..\build\AssistedMixing_artefacts\Release\VST3\King Mixer.vst3\Contents\x86_64-win\King Mixer.vst3"; DestDir: "{app}\Contents\x86_64-win"; Flags: ignoreversion
Source: "..\build\AssistedMixing_artefacts\Release\VST3\King Mixer.vst3\Contents\Resources\moduleinfo.json"; DestDir: "{app}\Contents\Resources"; Flags: ignoreversion
Source: "..\Resources\icon.ico"; DestDir: "{app}\Contents\Resources"; Flags: ignoreversion

[Icons]
Name: "{group}\Uninstall King Mixer"; Filename: "{uninstallexe}"

[Code]
function NextButtonClick(CurPageID: Integer): Boolean;
begin
  Result := True;
end;
