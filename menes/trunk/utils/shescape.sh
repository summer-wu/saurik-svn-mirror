#!/usr/bin/env bash
shopt -s extglob
while [[ $# -ne 0 ]]; do
    value=$1
    value=${value//\"/\\\"}
    value=${value//\'/\\\'}
    value=${value//\ /\\\ }

    echo -ne "$value"

    shift

    if [[ $# -ne 0 ]]; then
        echo -ne ' '
    fi
done
echo
