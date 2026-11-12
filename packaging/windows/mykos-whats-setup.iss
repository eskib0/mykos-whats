; ==============================================================================
; Script de Inno Setup 6 para MyKOS-Whats (Instalador Oficial de Windows)
; Genera: dist\MyKOS-Whats-Setup-1.0.0.exe
; ==============================================================================

#define MyAppName "MyKOS-Whats"
#define MyAppFullName "MyKOS-Whats (WhatsApp Web Desktop)"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "Ruben (eskib0)"
#define MyAppURL "https://github.com/eskib0/mykos-whats"
#define MyAppExeName "MyKOS-Whats.exe"

[Setup]
AppId={{9B78D82E-643A-4D3B-B03C-91500FF27A81}
AppName={#MyAppFullName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}/issues
AppUpdatesURL={#MyAppURL}/releases
DefaultDirName={localappdata}\Programs\MyKOS-Whats
DefaultGroupName=MyKOS
AllowNoIcons=yes
OutputDir=..\..\dist
OutputBaseFilename=MyKOS-Whats-Setup-{#MyAppVersion}
SetupIconFile=..\..\resources\icons\mykos-whats.ico
UninstallDisplayIcon={app}\resources\icons\mykos-whats.ico
Compression=lzma2/ultra64
SolidCompression=yes
WizardStyle=modern
PrivilegesRequired=lowest
DisableDirPage=no
DisableProgramGroupPage=no
ChangesAssociations=yes

[Languages]
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: checkedonce
Name: "registerprotocol"; Description: "Asociar enlaces de WhatsApp (whatsapp://)"; GroupDescription: "Integración en Windows:"; Flags: checkedonce

[Files]
; Archivos empaquetados por windeployqt y binario principal
Source: "..\..\dist\win64\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\..\resources\icons\mykos-whats.ico"; DestDir: "{app}\resources\icons"; Flags: ignoreversion
Source: "..\..\README.md"; DestDir: "{app}"; Flags: ignoreversion; Flags: skipifsourcedoesntexist

[Icons]
Name: "{group}\{#MyAppFullName}"; Filename: "{app}\{#MyAppExeName}"; IconFilename: "{app}\resources\icons\mykos-whats.ico"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; IconFilename: "{app}\resources\icons\mykos-whats.ico"; Tasks: desktopicon

[Registry]
; Registro de protocolo URL whatsapp://
Root: HKCU; Subkey: "Software\Classes\whatsapp"; ValueType: string; ValueName: ""; ValueData: "URL:WhatsApp Protocol"; Flags: uninsdeletekey; Tasks: registerprotocol
Root: HKCU; Subkey: "Software\Classes\whatsapp"; ValueType: string; ValueName: "URL Protocol"; ValueData: ""; Flags: uninsdeletekey; Tasks: registerprotocol
Root: HKCU; Subkey: "Software\Classes\whatsapp\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\resources\icons\mykos-whats.ico,0"; Flags: uninsdeletekey; Tasks: registerprotocol
Root: HKCU; Subkey: "Software\Classes\whatsapp\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Flags: uninsdeletekey; Tasks: registerprotocol

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
