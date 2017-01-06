#!/bin/sh
if stat --version >/dev/null 2>&1; then
    exec stat -c "%Y" -- "$@"
else
    exec stat -f "%m" -- "$@"
fi
