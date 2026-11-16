#!/bin/bash

# Installs missing project dependencies from the configured depends file

# Set to 1 to show APT output instead of writing it into the logs directory
LOCAL_debugShowAptOutput=0

LOG_SEPARATOR
LOG_INFO "Starting dependency installation ..."

if [[ $(id -u) -ne 0 ]]; then
    LOG_ERROR "Invalid permissions. Try this script as superuser"
    exit 1
fi

LOCAL_aptLog="$LOC_LOGS_DIR/install_deps_$LOG_START_TIMESTAMP.log"
LOCAL_failed=0

while IFS= read -r LOCAL_dep; do
    [[ -z $LOCAL_dep ]] && continue

    if dpkg-query -W -f='${Status}' "$LOCAL_dep" 2>/dev/null | grep -q "ok installed"; then
        LOG_WARNING "Dependency \"$LOCAL_dep\" installed, skipped"
        continue
    fi

    LOG_INFO "Installing dependency: $LOCAL_dep"
    if (( LOCAL_debugShowAptOutput )); then
        apt install -y "$LOCAL_dep" < /dev/null
    else
        printf '\n\n\n========================= PACKET: %s\n' "$LOCAL_dep" &>> "$LOCAL_aptLog"
        apt install -y "$LOCAL_dep" < /dev/null &>> "$LOCAL_aptLog"
    fi

    if [[ $? -eq 0 ]]; then
        LOG_OK "Installed: $LOCAL_dep"
    else
        LOG_ERROR "Installation failed: $LOCAL_dep"
        LOCAL_failed=1
    fi
done < "$LOC_DEPSFILE"

if (( LOCAL_failed )); then
    LOG_ERROR "Dependency installation completed with errors, see $LOCAL_aptLog"
    LOG_SEPARATOR
    exit 1
fi

LOG_OK "Dependency installation completed"
LOG_SEPARATOR
