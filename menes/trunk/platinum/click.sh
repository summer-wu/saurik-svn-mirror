#!/usr/bin/env bash
set -e

. ./scratch/settings.sh
./get.sh "https://gnet.ucsb.edu/surfer/click/$2?SessionKey=$SESSIONKEY&SCREEN_ID=$SCREENID&C_POS=$1"
