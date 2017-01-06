#!/bin/bash
set -m
old=''
while sleep 1; do
    new=$(find -name '.svn' -prune -o -name '*.xsl' -prune -o -name node -prune -o -not -type d -exec cat {} \; | md5sum)
    if [[ $new != $old ]]; then
        kill %-
        wait %-
        ./execute.sh &
        old=$new
    fi
done
