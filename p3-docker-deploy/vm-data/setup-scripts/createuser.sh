#!/bin/bash

# This script creates a new user
# (optionally with a specified password, otherwise a default one is generated)
# and updates their .bashrc script to set the (netid specific)
# OFFSET environment variable and adds the `invoke` script to their path

if [ "$#" -lt 1 ]; then
    echo "Usage: createuser.sh <username> <password>?"
    exit 1
fi

if [ "$#" -gt 1 ]; then
    PASS="$2"
else
    PASS=$(tr -cd '[:alnum:]' < /dev/urandom | fold -w30 | head -n1)
    echo "using generated password: $PASS"
fi

NAME="$1"

useradd -s /bin/bash -p $(openssl passwd -1 "$PASS") -m "$NAME"
echo "PATH=$PATH:/home/cs3410/bin" | tee -a /home/"$NAME"/.bashrc
