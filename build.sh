#!/bin/bash

if [ -z "$1" ]; then
    echo "No program name provided: stopping..."
    exit 1
fi

if [ -z "$BUILD_DIR_NAME" ]; then
    echo "Empty build directory name: defaulting to \"build\"."
    BUILD_DIR_NAME="build"
else
    echo "Build directory name: \"$BUILD_DIR_NAME\"."
fi

if [ -z "$OUTPUT_DIR_NAME" ]; then
    echo "Empty output directory name: defaulting to \"output\"."
    OUTPUT_DIR_NAME="output"
else
    echo "Output directory name: \"$OUTPUT_DIR_NAME\"."
fi

while [ ! -z "$1" ]; do

    if [ -d "$BUILD_DIR_NAME" ]; then
        rm -r "$BUILD_DIR_NAME"
    fi

    program_source_dir=""

    if [ -d "$1" ]; then
        program_source_dir="$1"
    elif [ -d "tests/$1" ]; then
        program_source_dir="tests/$1"
    else
        echo "$1: program does not exist"
        exit 1
    fi

    cmake -B "$BUILD_DIR_NAME"                                                  \
        -D CMAKE_TOOLCHAIN_FILE="$(pwd)/resources/cmake/arm-none-eabi.cmake"    \
        -D NRF5_SDK_PATH="$(pwd)/BLE_SDK"                                       \
        -D NRF5_BOARD="pca10040"                                                \
        -D NRF5_SOFTDEVICE_VARIANT="s132"                                       \
        -D NRF5_SDKCONFIG_PATH="$program_source_dir/config/"                    \
        -D LUOS_CONFIG_PATH="$program_source_dir/config/"                       \
        -D SDK_ROOT="$(pwd)/BLE_SDK"                                            \
        -D SOFTDEVICE="s132_6.1.1"

    mkdir -p "$OUTPUT_DIR_NAME/$1"

    cmake --build "$BUILD_DIR_NAME" --target "merge_$1"

    echo "Copying produced hex file to $OUTPUT_DIR_NAME/$1"
    cp "$BUILD_DIR_NAME/${program_source_dir}/$1_merged.hex" "$OUTPUT_DIR_NAME/$1"

    shift
done
