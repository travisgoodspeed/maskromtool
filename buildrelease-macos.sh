#!/bin/bash
set -e


## This is the script that builds release bundles for macOS, leaving
## the result in release/ in .app and .zip formats.  This used to be
## universal, but now I only build for arm64 because x86_64 macs are
## so old.


## These paths are hardcoded to prevent Homebrew for gunking things
## up.  Be sure to install *everything* in that version of Qt, or
## you'll be missing important libraries like QtCharts.
export QTDIR=~/Qt/6.9.2/macos/bin
export CMAKE=~/Qt/6.9.2/macos/bin/qt-cmake
export DEPLOYQT=~/Qt/6.9.2/macos/bin/macdeployqt



## Remove the old releases.
rm -rf build release

## We screwed up the 2024-06-23 release on macOS/x86_64 by forgetting to pull, so always do that.
git submodule init
git submodule update --remote
git pull


mkdir build
(cd build && $CMAKE .. && make -j 8)

mkdir release
cp -rf build/maskromtool.app build/gatorom build/extern/goodasm/goodasm release/
# Set the rpath so that the CLI tools run.
(cd release && install_name_tool -add_rpath @executable_path/maskromtool.app/Contents/Frameworks goodasm )
(cd release && install_name_tool -add_rpath @executable_path/maskromtool.app/Contents/Frameworks gatorom )
# Hardcoding the executable is bad, but the Homebrew version will break the build.
$DEPLOYQT release/maskromtool.app -dmg -sign-for-notarization="Developer ID Application: Travis Goodspeed"
# Verify the signature
codesign --verify --verbose  release/maskromtool.app

# Zip up a release.
(cd release && zip -r maskromtool-macos-arm64.zip maskromtool.app gatorom goodasm)


echo "See release/ for the release files."

