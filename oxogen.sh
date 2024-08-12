#!/bin/bash

if [ -z "${1}" ]; then
    echo "Usage: $0 <distro config file name>"
    echo "Example: $0 OxOfficeLinux"
    exit 1
fi

CONFIG_NAME="${1}"
CONFIG_FILE="distro-configs/${CONFIG_NAME}.conf"
if [ ! -f "${CONFIG_FILE}" ]; then
    echo "Error: Config file not found: ${CONFIG_FILE}"
    exit 1
fi

if [ ! -d "${HOME}/lotarballs" ]; then
    mkdir -p "${HOME}/lotarballs"
fi
EXTERNAL_TAR_DIR="${HOME}/lotarballs"

# 取得第第二個之後的所有參數
shift
./autogen.sh --with-distro="${CONFIG_NAME}" --with-external-tar="${EXTERNAL_TAR_DIR}" $*
