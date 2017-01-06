#!/bin/sh
while [ 1 ]; do
    nc -v -l -p "$1" -e "$2" 2>&1 | grep "^connect" | echo -n `sed -e "s/^connect to \[.*\] from .* \[\(.*\)\] [0-9]*$/\1/"` >> "$2".log
done
