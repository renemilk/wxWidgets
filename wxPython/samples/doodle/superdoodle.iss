[Setup]
AppName = SuperDoodle
AppVerName = SuperDoodle 1.0
AppCopyright = Copyright � 2001 Total Control Software
DefaultGroupName = SuperDoodle
DefaultDirName = {pf}\SuperDoodle
OutputBaseFilename = SuperDoodle-setup
AlwaysCreateUninstallIcon = yes
OutputDir = dist
WizardStyle = modern
UninstallStyle = modern
AppPublisherURL = http://wxPython.org/


[Files]
Source: "c:\WINNT\SYSTEM32\MSVCRT.dll";   DestDir: "{sys}"; CopyMode: alwaysskipifsameorolder; Flags: sharedfile uninsneveruninstall;
Source: "c:\WINNT\SYSTEM32\MSVCIRT.dll";  DestDir: "{sys}"; CopyMode: alwaysskipifsameorolder; Flags: sharedfile uninsneveruninstall;
Source: "dist\superdoodle\superdoodle.exe";    DestDir: "{app}";
Source: "dist\superdoodle\_sre.pyd";           DestDir: "{app}";
Source: "dist\superdoodle\htmlc.pyd";          DestDir: "{app}";
Source: "dist\superdoodle\python20.dll";       DestDir: "{app}";
Source: "dist\superdoodle\PyWinTypes20.dll";   DestDir: "{app}";
Source: "dist\superdoodle\utilsc.pyd";         DestDir: "{app}";
Source: "dist\superdoodle\win32api.pyd";       DestDir: "{app}";
Source: "dist\superdoodle\wx23_0.dll";         DestDir: "{app}";
Source: "dist\superdoodle\wxc.pyd";            DestDir: "{app}";


[Icons]
Name: "{group}\SuperDoodle (wxPython sample app)";  Filename: "{app}\superdoodle.exe"


