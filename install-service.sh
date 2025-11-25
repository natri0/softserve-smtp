#!/bin/bash

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

sed -e "s|<INSTALL_PATH>|${SCRIPT_DIR}|g" \
    -e "s|<USERNAME>|${USER}|g" \
    softserve-smtp.service | sudo tee /etc/systemd/system/softserve-smtp.service > /dev/null

sudo systemctl daemon-reload

sudo systemctl start softserve-smtp

sudo systemctl status softserve-smtp --no-pager -l