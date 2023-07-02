@echo off
SET THEFILE=C:\Users\Kirill\Desktop\CFD\ascii-chess 1968\project1.exe
echo Linking %THEFILE%
C:\lazarus\fpc\3.2.2\bin\x86_64-win64\ld.exe -b pei-x86-64  -Xt --gc-sections  -s --subsystem windows --entry=_WinMainCRTStartup    -o "C:\Users\Kirill\Desktop\CFD\ascii-chess 1968\project1.exe" "C:\Users\Kirill\Desktop\CFD\ascii-chess 1968\link19160.res"
if errorlevel 1 goto linkend
goto end
:asmend
echo An error occurred while assembling %THEFILE%
goto end
:linkend
echo An error occurred while linking %THEFILE%
:end
