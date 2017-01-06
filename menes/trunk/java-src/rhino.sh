#!/bin/bash
base="$(dirname "$0")"
exec "${base}"/java.sh org.mozilla.javascript.tools.shell.Main "$@"
