#!/usr/bin/env bash
while read -r line; do
    echo "$(uuidgen -t) $line"
done
