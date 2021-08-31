#!/bin/sh

if [ -z "$OUTPUT_DIR_NAME" ]; then
    echo "Empty output directory name: defaulting to \"output\"."
    OUTPUT_DIR_NAME="output"
else
    echo "Output directory name: \"$OUTPUT_DIR_NAME\"."
fi

SOFTDEVICE_PATH="SDK/components/softdevice/s132/hex/s132_nrf52_6.1.1_softdevice.hex"

while [ ! -z "$1" ]; do

    if [ -z "$2" ]; then
        echo "No board number provided: stopping..."
        exit 1
    fi

    HEX_FILE_NAME="$OUTPUT_DIR_NAME/$1/$1_merged.hex"

    if [ ! -e "$HEX_FILE_NAME" ]; then
        echo "$1: example not built."
        exit 1
    fi

    nrfjprog --program "$HEX_FILE_NAME" -f nrf52 --chiperase --reset --snr "$2"

    shift 2
done
