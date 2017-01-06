#!/usr/bin/env bash
menes="$(dirname "$0")/.."
source "$menes/utils/bashrc.sh"
source "$menes/utils/assert.sh"
source "$menes/utils/spawn.sh"

echo "$MENES_DEBUG" | grep ":cc-call:" >/dev/null && "$menes/utils/shescape.sh" "$0" "$@" >/dev/tty

cc="$1"
shift

ebl:array flags
declare target

OPTIND=1
while getopts s: OPTKEY; do
    case $OPTKEY in
        (s)
            declare target="$OPTARG"
        ;;

        (\?)
            shift $((OPTIND - 1))
            fatal "$(basename $0): unsupported flag: $1"
        ;;
    esac
done
shift $((OPTIND - 1))

echo "$MENES_DEBUG" | grep ":cc-exec:" >/dev/null && "$menes/utils/shescape.sh" $cc -rsv "$target" "${flags[@]}" "$@" >/dev/tty

spawn $cc -rs "$target" "${flags[@]}" "$@"
reexit_
