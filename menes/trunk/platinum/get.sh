#!/usr/bin/env bash
set -e

wget -nv -O - "$1" > ./scratch/screen.html
../utils/tidy.sh ./scratch/screen.html > ./scratch/screen.xml
./update.sh
mkdir -p screens
cp ./scratch/screen.html screens/$SESSIONKEY.html
