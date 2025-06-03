#!/bin/bash

# Usage: ./fetch_bitstream.sh FolderName

# Set variables
REMOTE_USER="tr720"
REMOTE_HOST="shell3.doc.ic.ac.uk"
REMOTE_BASE_PATH="/homes/tr720/bitstreams"
LOCAL_DEST="."

# Check for input
if [ -z "$1" ]; then
    echo "Usage: $0 <FolderName>"
    exit 1
fi

FOLDER_NAME="$1"

scp -r "/Volumes/UNTITLED/Tanvi/${FOLDER_NAME}" "${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_BASE_PATH}/${FOLDER_NAME}"
diskutil eject /Volumes/UNTITLED