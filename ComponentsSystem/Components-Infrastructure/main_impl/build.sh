#!/bin/bash

# Builds the project into <project root>/build (Debug by default, Release with "release")

# Set to 1 to show CMake output instead of writing it into the logs directory
LOCAL_debugShowCmakeOutput=0

if [[ -z ${MAIN_PROJECT_ROOTDIR:-} || ! -d $MAIN_PROJECT_ROOTDIR ]]; then
    LOG_ERROR "Invalid project root directory: ${MAIN_PROJECT_ROOTDIR:-<empty>}"
    exit 1
fi

if [[ -z ${LOC_LOGS_DIR:-} || -z ${LOG_START_TIMESTAMP:-} ]]; then
    LOG_ERROR "LOC_LOGS_DIR and LOG_START_TIMESTAMP are not set, run this script via main.sh"
    exit 1
fi

LOCAL_buildDir="$MAIN_PROJECT_ROOTDIR/build"
LOCAL_buildType="Debug"
[[ ${1:-} == "release" ]] && LOCAL_buildType="Release"

LOG_SEPARATOR
LOG_INFO "Starting $LOCAL_buildType project build..."

mkdir -p "$LOCAL_buildDir" || { LOG_ERROR "Failed to create build directory"; exit 1; }
find "$LOCAL_buildDir" -mindepth 1 -delete || {
    LOG_ERROR "Failed to clean build directory"
    LOG_INFO "===> Maybe, you tried to build with sudo and now - without?"
    exit 1
}
cd "$LOCAL_buildDir" || { LOG_ERROR "Failed to enter build directory"; exit 1; }

LOCAL_configureLog="$LOC_LOGS_DIR/build_configure_$LOG_START_TIMESTAMP.log"
LOCAL_buildLog="$LOC_LOGS_DIR/build_$LOG_START_TIMESTAMP.log"

if (( LOCAL_debugShowCmakeOutput )); then
    cmake -DCMAKE_BUILD_TYPE="$LOCAL_buildType" -GNinja .. ||
        { LOG_ERROR "Failed to configure project"; exit 1; }
    cmake --build . --target all -- -j"$(nproc)" ||
        { LOG_ERROR "Failed to build project"; exit 1; }
else
    cmake -DCMAKE_BUILD_TYPE="$LOCAL_buildType" -GNinja .. &> "$LOCAL_configureLog" ||
        { LOG_ERROR "Failed to configure project, see $LOCAL_configureLog"; exit 1; }
    cmake --build . --target all -- -j"$(nproc)" &> "$LOCAL_buildLog" ||
        { LOG_ERROR "Failed to build project, see $LOCAL_buildLog"; exit 1; }
fi

LOG_OK "Project build succeed"
LOG_SEPARATOR
