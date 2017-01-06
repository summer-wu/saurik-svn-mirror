#!/usr/bin/env bash
set -e

. ./scratch/settings.sh
./get.sh "https://gnet.ucsb.edu/surfer/LogOff?SessionKey=$SESSIONKEY"
