:start

"C:\Program Files\CMake\bin\cmake.EXE" --build build --config Debug --target Polyrack_Standalone -j 18 --

@cd build\Polyrack_artefacts\Debug\Standalone
start /wait Polyrack.exe
@cd ..\..\..\..

@echo.
@set /p userInput=Press Q to Quit, anything else to Restart: 
@if /i "%userInput%"=="Q" exit /b
@goto START