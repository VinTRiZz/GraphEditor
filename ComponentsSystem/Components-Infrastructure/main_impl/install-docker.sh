#!/bin/bash

# Installs Docker engine, Compose plugin and rootless support

LOG_SEPARATOR
LOG_INFO "Starting Docker installation ..."

LOCAL_privileged=()
[[ $(id -u) -ne 0 ]] && LOCAL_privileged=(sudo)

LOCAL_die() {
    LOG_ERROR "$*"
    exit 1
}

"${LOCAL_privileged[@]}" apt update || LOCAL_die "Failed to update package lists"
"${LOCAL_privileged[@]}" apt install -y ca-certificates curl ||
    LOCAL_die "Failed to install prerequisites"

"${LOCAL_privileged[@]}" install -m 0755 -d /etc/apt/keyrings
"${LOCAL_privileged[@]}" curl -fsSL \
    https://download.docker.com/linux/debian/gpg \
    -o /etc/apt/keyrings/docker.asc ||
    LOCAL_die "Failed to download Docker GPG key"
"${LOCAL_privileged[@]}" chmod a+r /etc/apt/keyrings/docker.asc

LOCAL_codename=$(. /etc/os-release && echo "$VERSION_CODENAME")
[[ -n $LOCAL_codename ]] || LOCAL_die "Cannot determine distribution codename"

"${LOCAL_privileged[@]}" tee /etc/apt/sources.list.d/docker.sources >/dev/null <<EOF
Types: deb
URIs: https://download.docker.com/linux/debian
Suites: $LOCAL_codename
Components: stable
Signed-By: /etc/apt/keyrings/docker.asc
EOF

"${LOCAL_privileged[@]}" apt update || LOCAL_die "Failed to update package lists"
"${LOCAL_privileged[@]}" apt install -y docker-ce docker-ce-cli containerd.io \
    docker-buildx-plugin docker-compose-plugin ||
    LOCAL_die "Failed to install Docker engine"

"${LOCAL_privileged[@]}" apt install -y uidmap || LOCAL_die "Failed to install uidmap"

if [[ $(id -u) -eq 0 ]]; then
    LOG_WARNING "Running as root, skipping rootless setup: run it as a regular user"
elif ! command -v dockerd-rootless-setuptool.sh >/dev/null; then
    LOG_WARNING "dockerd-rootless-setuptool.sh not found, skipping rootless setup"
else
    dockerd-rootless-setuptool.sh install ||
        LOCAL_die "Failed to set up rootless Docker"
fi

LOG_OK "Docker installation completed"
LOG_SEPARATOR
