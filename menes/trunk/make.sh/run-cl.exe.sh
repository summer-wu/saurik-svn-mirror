#!/usr/bin/env bash
menes="$(dirname "$0")/.."
source "$menes/utils/bashrc.sh"
source "$menes/utils/assert.sh"
source "$menes/utils/spawn.sh"

echo "$MENES_DEBUG" | grep ":cc-call:" >/dev/null && "$menes/utils/shescape.sh" "$0" "$@" >/dev/tty

cc="$1"
shift

ebl:array cflags
ebl:array lflags

lflags.+= -link

OPTIND=1
while getopts C:c:D:F:gH:I:l:O:U: OPTKEY; do
    case $OPTKEY in
        (C) cflags.+= "-Fe$OPTARG";;

        (c)
            target="$OPTARG"
            cflags.+= -c -Yu "-Fo$OPTARG"
        ;;

        (D) cflags.+= "-D$OPTARG";;

        (g)
            # XXX: -Zi doesn't work on Linux (CryptoAPI)
            #cflags.+= -Zi
        ;;

        (H)
            target="$OPTARG"
            # XXX: using -TP here is stupid
            cflags.+= -c -Yc "-Fp$OPTARG" "-Fo$OPTARG.obj" -TP
        ;;

        (I) cflags.+= "-I$OPTARG";;
        (l) lflags.+= "$OPTARG.lib";;

        (O) case $OPTARG in
            (most) cflags.+= -Ox;;
            (some) cflags.+= -O2;;
            (none) cflags.+= -Od;;
        esac;;

        (U) cflags.+= "-U$OPTARG";;

        (\?)
            shift $((OPTIND - 1))
            fatal "$(basename $0): unsupported flag: $1"
        ;;
    esac
done
shift $((OPTIND - 1))

if [[ ${#lflags[@]} -eq 1 ]]; then
    unset 'lflags[0]'
fi

echo "$MENES_DEBUG" | grep ":cc-exec:" >/dev/null && "$menes/utils/shescape.sh" $cc -nologo "${cflags[@]}" "${lflags[@]}" "$@" >/dev/tty

if [[ ${target+@} ]]; then
    spawn $cc -nologo -showIncludes "${cflags[@]}" "$@" "${lflags[@]}" | (
        declare line
        IFS=''
        read -r line

        IFS=''
        while read -r line; do
            case "$line" in
                ('Note: including file: '*)
                    declare header="${line##Note: including file: *( )}"
                    if [[ $header == */dosdevices/[a-z]:* ]]; then
                        declare header="${header##*/dosdevices/}"
                    fi

                    # XXX: this might be going the wrong direction
                    declare header="${header//\\//}"

                    if [[ $header == z:/* ]]; then
                        declare header="${header#z:}"
                    fi

                    echo "${header##*(.[\\/])}"
                ;;

                (*) echo "$line" 1>&2;;
            esac
        done | sort -u >"$target.dep"
    ) 2>&1
    result=${PIPESTATUS%% *}
else
    spawn $cc -nologo -showIncludes "${cflags[@]}" "$@" "${lflags[@]}"
    result=$?
fi

rekill_
exit $result
