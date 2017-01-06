#!/usr/bin/env bash
menes="$(dirname "$0")/.."
source "$menes/utils/bashrc.sh"
source "$menes/utils/assert.sh"
source "$menes/utils/spawn.sh"

echo "$MENES_DEBUG" | grep ":cc-call:" >/dev/null && "$menes/utils/shescape.sh" "$0" "$@" >/dev/tty

cc="$1"
shift

ebl:array flags
declare mode=''

OPTIND=1
while getopts C:F:gL:l:O:o:S:s: OPTKEY; do
    case $OPTKEY in
        (C)
            declare mode='-lib'
            flags.+= "-out:$OPTARG"
        ;;

        (F) ;;

        (g)
            # XXX: -debug doesn't work on Linux (NtQueryInformationToken)
            #flags.+= -debug
        ;;

        (L) flags.+= "-libpath:$OPTARG";;
        (l) flags.+= "$OPTARG.lib";;

        (O) case $OPTARG in
            (most);;
            (some);;
            (none);;
        esac;;

        (o)
            declare mode='-link'
            flags.+= "-out:$OPTARG"
        ;;

        (S)
            declare mode='-link'
            flags.+= -dll "-out:$OPTARG"
        ;;

        (s)
            declare mode='-lib'
            flags.+= "-out:$OPTARG"
        ;;

        (\?)
            shift $((OPTIND - 1))
            fatal "$(basename $0): unsupported flag: $1"
        ;;
    esac
done
shift $((OPTIND - 1))

echo "$MENES_DEBUG" | grep ":cc-exec:" >/dev/null && "$menes/utils/shescape.sh" $cc $mode -nologo "${flags[@]}" "$@" >/dev/tty

spawn $cc $mode -nologo "${flags[@]}" "$@"
reexit_
