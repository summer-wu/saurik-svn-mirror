#!/usr/bin/env bash
javac -Xlint:deprecation -Xlint:unchecked -cp "$(./lib.sh)" "$@"
