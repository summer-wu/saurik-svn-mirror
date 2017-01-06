#!/usr/bin/env bash
set -e

. ./scratch/settings.sh
case ${SCREENID} in
    searchforclasses.login) CURSORPOS=345;;
    classesresults.classresults) CURSORPOS=1770;;
esac

./submit.sh C_POS="${CURSORPOS}" F_${CURSORPOS}_3="$1"
