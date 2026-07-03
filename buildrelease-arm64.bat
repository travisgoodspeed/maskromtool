REM Set the ARM64 development mode.
REM %comspec% /k
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" arm64
REM "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsarm64.bat"


echo on

REM Delete old build.
rmdir /s /q build
rmdir /s /q Release

REMech o Don't forget to pull the new release.
git pull
git submodule init
git submodule update --remote




REM Import the Qt and VS2022 paths.
REM set PATH=C:\Qt\Tools\CMake\bin;C:\Qt\6.11.1\msvc2022_arm64\bin;%PATH%
set PATH=%PATH%;C:\Qt\Tools\CMake\bin;C:\Qt\6.11.1\msvc2022_arm64\bin
REM call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsMSBuildCmd"
REM call "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsMSBuildCmd"
call "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd"  -host_arch=arm64 -arch=arm64
call "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsMSBuildCmd"  -host_arch=arm64 -arch=arm64


REM Create the solution file.
mkdir build
cd build
REM cmake -A ARM64 .. || goto fail
REM It's bad to hardocde the path, but Qt's cmake doesn't support Visual Studio 18 yet.
"C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" -G "Visual Studio 18 2026" -A ARM64 .. || goto fail


REM Build goodasm.exe
cd extern\goodasm
msbuild goodasm.sln /property:Configuration=Release -maxcpucount || goto fail
cd ..\..

REM Compile the .exe into Release\
msbuild maskromtool.sln /property:Configuration=Release -maxcpucount || goto fail

REM Add the required Qt libraries.
REM Use --pdb to include debugging symbols.
windeployqt Release || goto fail
echo moving the release
move Release ..\ || goto fail
copy extern\goodasm\Release\goodasm.exe ..\Release\ || goto fail
cd ..

REM Adding files that windeployqt forgets.
copy C:\Qt\6.11.1\msvc2022_arm64\bin\*.dll Release\ || goto fail

REM Adding Yara lib if it exists.
copy c:\bin\yara_x_capi.dll Release\ || goto fail

echo Finishing packing Release.  Now making installer.


REM Including the path.
set PATH=C:\Qt\Tools\QtInstallerFramework\4.8\bin;C:\Qt\Tools\CMake_64\bin;C:\Qt\6.11.1\msvc2022_arm64\bin;C:\Qt\Tools\QtInstallerFramework\4.11\bin;%PATH%

REM Copying packages.
xcopy/y/s Release\* Deployment\packages\com.maskromtool.maskromtool\data\

cd Deployment
echo Building the installer executable.
binarycreator.exe -c config\config.xml -p packages -f MaskRomToolInstaller || goto fail
move MaskRomToolInstaller.exe maskromtool-win-arm64.exe || goto fail
copy C:\Qt\vcredist\*redist*.exe  || goto fail

REM Zip the redist with the installer.
erase maskromtool-win-*.zip
tar -a -c -f maskromtool-win-arm64.zip maskromtool-win-arm64.exe *redist*.exe  || goto fail

cd ..

echo "Release files are available in Deployment\."
goto done


:fail

@echo "Compiling failed."

:done
