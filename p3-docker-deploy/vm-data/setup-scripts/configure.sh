#!/bin/bash

# This file generates a pseudorandom string
# from the given netid and writes it into the file /home/cs3410/configure/netid
# This will be used to make each student's starting stack addresses different
# Every 16 characters should move the stack starting location.

if [ "$#" -lt 1 ]; then
    echo "Usage: configure.sh <netid>"
    exit 1
fi

NETID="$1"
NUM_HEX_DIGITS=3

# add zero at end to make it a multiple of 16
OFFSET_AMT=$((0x$(sha1sum <<<"$NETID"|cut -c1-"$NUM_HEX_DIGITS")))"0"
OFF=$(
for (( i=1; i<=$OFFSET_AMT; i++ ))
do
    echo -n "0"
done)
#echo "Creating offset of size: $OFFSET_AMT for user $NETID"
#echo "Configuring default environment for NETID: $NETID"
# echo $OFF
echo "$OFF" > /home/cs3410/configure/offset
echo "$NETID" > /home/cs3410/configure/netid
## set this as a default environment variable for all users
echo "export OFFSET=$OFF" | tee /etc/profile.d/conf_env.sh > /dev/null
