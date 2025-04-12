@echo off
echo Cleaning build files...
rd /s /q obj
rd /s /q bin
echo Creating empty directories...
mkdir obj
mkdir obj\Debug
mkdir obj\Release
mkdir bin
mkdir bin\Debug
mkdir bin\Release
echo Done! 