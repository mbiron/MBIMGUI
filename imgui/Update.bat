@echo off

rem Update all submodules 

cd IconFontCppHeaders && git pull && cd..
cd Imgui && git pull && cd..
cd Imgui_club && git pull && cd..
cd Imgui_toggle && git pull && cd..
cd Implot && git pull && cd..
cd Imspinner && git pull && cd..

pause