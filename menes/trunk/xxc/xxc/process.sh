#!/usr/bin/env bash
menes="$(dirname "$0")/../.."

temp="$(dirname "$2")/.$(basename "$2")"
type="$(echo "$1" | sed -e "s/^.*\.\([^.]*\)$/\1/")"

set -e

$menes/xxc/xxc/header-$type.sh "$1"
$menes/utils/java.sh xxc.Main "$menes" < "$1"
$menes/xxc/xxc/footer-$type.sh "$1"
