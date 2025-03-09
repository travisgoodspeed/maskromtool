REM Delete old build.
rmdir /s /q build
rmdir /s /q Release


REM Don't forget to pull the new release.
git pull


REM Import the Qt and VS2022 paths.
set PATH=C:\Qt\Tools\CMake_64\bin;C:\Qt\6.8.2\msvc2022_64\bin;%PATH%
REM "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsMSBuildCmd"

REM Create the solution file.
mkdir build
cd build
cmake ..


REM Build goodasm.exe
cd extern\goodasm
msbuild goodasm.sln /property:Configuration=Release -maxcpucount
cd ..\..

REM Compile the .exe into Release\
msbuild maskromtool.sln /property:Configuration=Release -maxcpucount

REM Add the required Qt libraries.
REM Use --pdb to include debugging symbols.
windeployqt Release
echo moving the release
move Release ..\
copy extern\goodasm\Release\goodasm.exe ..\Release\
cd ..

REM Adding files that windeployqt forgets.
copy C:\Qt\6.8.2\msvc2022_64\bin\Qt6Charts.dll Release\
copy C:\Qt\6.8.2\msvc2022_64\bin\Qt6OpenGL.dll Release\
copy C:\Qt\6.8.2\msvc2022_64\bin\Qt6OpenGLWidgets.dll Release\
copy C:\Qt\6.8.2\msvc2022_64\bin\Qt6Widgets.dll Release\
copy C:\Qt\6.8.2\msvc2022_64\bin\Qt6Gui.dll Release\
copy C:\Qt\6.8.2\msvc2022_64\bin\Qt6PrintSupport.dll Release\
copy C:\Qt\6.8.2\msvc2022_64\bin\Qt6Pdf.dll Release\
copy C:\Qt\6.8.2\msvc2022_64\bin\Qt6Svg.dll Release\
copy C:\Qt\6.8.2\msvc2022_64\bin\Qt6Core.dll Release\

echo Finishing packing Release.  Now making installer.


REM Including the path.
set PATH=C:\Qt\Tools\QtInstallerFramework\4.8\bin;C:\Qt\Tools\CMake_64\bin;C:\Qt\6.8.2\msvc2022_64\bin;C:\Qt\Tools\QtInstallerFramework\4.8\bin;%PATH%

REM Copying packages.
xcopy/y/s Release\* Deployment\packages\com.maskromtool.maskromtool\data\

cd Deployment
echo Building the installer executable.
binarycreator.exe -c config\config.xml -p packages -f MaskRomToolInstaller
move MaskRomToolInstaller.exe maskromtool-win-x86_64.exe
copy C:\Qt\vcredist\vcredist_msvc2022_x64.exe vcredist_msvc2022_x64.exe

REM Zip the redist with the installer.
tar -a -c -f maskromtool-win-x86_64.zip maskromtool-win-x86_64.exe vcredist_msvc2022_x64.exe

cd ..

echo "Release files are available in Deployment\."
