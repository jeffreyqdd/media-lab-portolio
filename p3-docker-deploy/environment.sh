#!/bin/bash

export CS3410_ROOT=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export CS3410_PASSKEYS="$CS3410_ROOT/assets/passkeys"
export CS3410_DOCKERFILES="$CS3410_ROOT/assets/dockerfiles"
export CS3410_BINDMOUNTS="$CS3410_ROOT/assets/bindmounts"
export CS3410_COMPOSE="$CS3410_ROOT/assets/compose"

export CS3410_ROSTER="$CS3410_ROOT/assets/roster.csv"
export CS3410_PASSKEY_STUBS="$CS3410_ROOT/assets/passkey-stubs.txt"
export CS3410_LOG="$CS3410_ROOT/logs"

export CS3410_GRADING="$CS3410_ROOT/grading"

export PATH=$CS3410_ROOT/bin:$PATH
export PYTHONPATH=$CS3410_ROOT

alias t=trogdor
