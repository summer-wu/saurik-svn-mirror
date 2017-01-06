#!/usr/bin/env bash
set -e

. ./scratch/settings.sh
URL="https://gnet.ucsb.edu/surfer/form/emulator/$SESSIONKEY?SessionKey=$SESSIONKEY&SCREEN_ID=$SCREENID"

while (($#)); do
    URL="$URL&$1"
    shift
done

./get.sh "$URL"
