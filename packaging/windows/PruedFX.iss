#ifndef PluginSource
  #error PluginSource must point to the validated VST3 bundle
#endif
#ifndef OutputPath
  #define OutputPath "..\..\delivery"
#endif
[Setup]
AppId={{00D09F44-1DC5-48EB-9F49-D3B4520C7804}
AppName=Prued FX
AppVersion=2.0.4
AppPublisher=Yuay Sound Records
AppPublisherURL=https://ko-fi.com/yuaylooper
DefaultDirName={commoncf64}\VST3\Prued FX.vst3
DisableDirPage=yes
DisableProgramGroupPage=yes
Uninstallable=no
CreateAppDir=yes
PrivilegesRequired=admin
PrivilegesRequiredOverridesAllowed=commandline
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
MinVersion=10.0
OutputDir={#OutputPath}
OutputBaseFilename=Prued-FX-2.0.4-Windows-x64-Setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
CloseApplications=yes
RestartApplications=no
SetupLogging=yes
VersionInfoVersion=2.0.4.0
InfoBeforeFile=..\INSTALL-WINDOWS-TH-EN.txt
[Files]
Source: "{#PluginSource}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

