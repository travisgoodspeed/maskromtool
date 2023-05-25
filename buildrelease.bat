REM Delete old build.
rmdir /s /q build
rmdir /s /q Release

REM Import the Qt and VS2022 paths.
set PATH=C:\Qt\6.3.1\msvc2019_64\bin;%PATH%
REM "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsMSBuildCmd"

REM Create the solution file.
mkdir build
cd build
cmake ..

REM Compile the .exe into Release\
msbuild maskromtool.sln /property:Configuration=Release

REM Add the required Qt libraries.
ROM Use --pdb to include debugging symbols.
windeployqt Release
echo moving the release
move Release ..\
cd ..

echo Finishing packing Release\
