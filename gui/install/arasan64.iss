[Setup]
AppName=Arasan (64-bit)
AppVerName=Arasan 26.0
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
DefaultDirName={commonpf}\Arasan\26.0
DefaultGroupName=Arasan 26.0
AppCopyright=Copyright 1994-2026 by Jon Dart. All Rights Reserved.
Compression=bzip
OutputBaseFilename=arasan64-v26.0
OutputDir=gui\install
SourceDir=..\..
UsePreviousAppDir=no
UsePreviousGroup=no

[Registry]
Root: HKA64; Subkey: "Software\Arasan"; Flags: uninsdeletekeyifempty 
Root: HKA64; Subkey: "Software\Arasan\Arasan"; Flags: uninsdeletekey

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
Source: "gui\x64\release\arasan-64.exe"; DestDir: "{app}"; Components: GUI
Source: "gui\res\arasan.ico"; DestDir: "{app}"; Components: GUI
Source: "gui\help\arasan.chm"; DestDir: "{app}"; Components: GUI
Source: "gui\pieces\*"; DestDir: "{app}\pieces"; Flags: recursesubdirs createallsubdirs; Components: GUI
Source: "book\chess-openings\*"; DestDir: "{app}\chess-openings"; Components: GUI
Source: "win64\release\arasanx-64.exe"; DestDir: "{app}"; Components: engine
Source: "network\arasanv8-20260906.nnue"; DestDir: "{app}"; Components: engine
Source: "book\book.bin"; DestDir: "{app}"; Components: book
Source: "LICENSE"; DestDir: "{app}\LICENSES"; Components: engine; DestName: "LICENSE.txt"
Source: "gui\lunasvg\LICENSE"; DestDir: "{app}\LICENSES"; DestName: "LICENSE-lunasvg.txt"; Components: GUI;
Source: "gui\pieces\celtic\LICENSE.txt"; DestDir: "{app}\LICENSES"; DestName: "LICENSE-celtic"; Components: GUI;
Source: "gui\pieces\spatial\LICENSE.txt"; DestDir: "{app}\LICENSES"; DestName: "LICENSE-spatial.txt"; Components: GUI;
Source: "src\syzygy\LICENSE"; DestDir: "{app}\LICENSES"; DestName: "LICENSE-syzygy.txt"; Components: engine;

[Icons]
Name: "{group}\Arasan (64-bit)"; Components: GUI; Filename: {app}\arasan-64.exe; Comment: "Arasan Chess Program (64-bit)"; IconFileName: {app}\arasan.ico
Name: "commondesktop}\Arasan (64-bit)"; Components: GUI; Filename: {app}\arasan-64.exe;

[UninstallDelete]
Type: filesandordirs; Name: "{app}"


