#!/usr/bin/env bash
set -e

./get.sh "https://gnet.ucsb.edu/surfer/gold/login"
./submit.sh F_490_7="$1" F_505_4="$2"
./submit.sh F_1224_1="N"

. ./scratch/settings.sh
if [[ $SCREENID = 'systemmessage.login' ]]; then
    ./sendkeys.sh '@E'
fi
