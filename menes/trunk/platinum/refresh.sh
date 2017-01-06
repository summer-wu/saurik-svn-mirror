#!/usr/bin/env bash
set -e

. ./scratch/settings.sh
./get.sh "https://gnet.ucsb.edu/surfer/refresh/$SESSIONKEY?SessionKey=$SESSIONKEY&SCREEN_ID=$SCREENID"
