#!/bin/sh
TIME=`which time`

MSG=$1
shift

if $TIME --version 2>&1 | grep "GNU time" > /dev/null; then
    $TIME --format="[%x] %E: $MSG" $@
else
    $@
    ERR=$?
    echo [$ERR] $MSG
    exit $ERR
fi
