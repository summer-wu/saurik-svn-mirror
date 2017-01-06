#!/usr/bin/env bash
base="$(dirname "$0")"
exec java -cp "$("${base}"/lib.sh)" "$@"
