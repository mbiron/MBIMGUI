@echo off

set __DOXYGEN_BIN__="C:\Program Files\doxygen\bin\doxygen.exe"
set __DOXYFILE__=".\Doxyfile"

echo Running Doxygen...

:: erasing previous files
del /Q doc\html\*.*

%__DOXYGEN_BIN__% %__DOXYFILE__% 1>doxygen_stdout.txt 2>doxygen_stderr.txt

echo Done!

doc\html\index.html