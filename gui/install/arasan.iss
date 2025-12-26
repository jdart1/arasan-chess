[Setup]
AppName=Arasan
AppVerName=Arasan 25.3
DefaultDirName={commonpf}\Arasan\25.3
DefaultGroupName=Arasan 25.3
AppCopyright=Copyright 1994-2025 by Jon Dart. All Rights Reserved.
Compression=bzip
OutputBaseFilename=arasan25.3
OutputDir=gui\install
SourceDir=..\..
UsePreviousAppDir=no
UsePreviousGroup=no

[Registry]
Root: HKA; Subkey: "Software\Arasan"; Flags: uninsdeletekeyifempty 
Root: HKA; Subkey: "Software\Arasan\Arasan"; Flags: uninsdeletekey

[Types]
Name: "Full"; Description: "Full installation"
Name: "ChessEngine"; Description: "Chess Engine only"

[Components]
Name: "GUI"; Description: "GUI Files"; Types: Full; Flags: fixed
Name: "book"; Description: "Opening Book"; Types: Full ChessEngine
Name: "engine"; Description: "Chess Engine"; Types: Full ChessEngine

[Tasks]
Name: desktopicon; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; Components: GUI
Name: desktopicon\common; Description: "For all users"; GroupDescription: "Additional icons:"; Components: GUI; Flags: exclusive
Name: desktopicon\user; Description: "For the current user only"; GroupDescription: "Additional icons:"; Components: GUI; Flags: exclusive unchecked

[Files]
Source: "gui\release\arasan.exe"; DestDir: "{app}"; Components: GUI
Source: "gui\res\arasan.ico"; DestDir: "{app}"; Components: GUI
Source: "gui\help\arasan.chm"; DestDir: "{app}"; Components: GUI
Source: "gui\fonts\alpha.ttf"; DestDir: "{commonfonts}"; Components: GUI; FontInstall: "Chess Alpha"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "gui\fonts\Berlin.ttf"; DestDir: "{commonfonts}"; Components: GUI; FontInstall: "Chess Berlin"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "gui\fonts\merifont.ttf"; DestDir: "{commonfonts}"; Components: GUI; FontInstall: "Chess Merida"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "gui\fonts\mayafont.ttf"; DestDir: "{commonfonts}"; Components: GUI; FontInstall: "Chess Maya"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "gui\fonts\chessmar.ttf"; DestDir: "{commonfonts}"; Components: GUI; FontInstall: "Chess Marroquin"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "gui\fonts\tt_usual.ttf"; DestDir: "{commonfonts}"; Components: GUI; FontInstall: "Chess Usual"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "book\chess-openings\*"; DestDir: "{app}\chess-openings"; Components: GUI
Source: "win32\release\arasanx-32.exe"; DestDir: "{app}"; Components: engine
Source: "network\arasanv5-20251222.nnue"; DestDir: "{app}"; Components: engine
Source: "book\book.bin"; DestDir: "{app}"; Components: book
Source: "LICENSE"; DestDir: "{app}"; Components: engine;

[Icons]
Name: "{group}\Arasan"; Components: GUI; Filename: {app}\arasan.exe; Comment: "Arasan Chess Program"; IconFileName: {app}\arasan.ico
Name: "{commondesktop}\Arasan"; Components: GUI; Filename: {app}\arasan.exe;

[UninstallDelete]
Type: filesandordirs; Name: "{app}"
Type: files; Name: "{%APPDATA}\Arasan\arasan.lrn"

