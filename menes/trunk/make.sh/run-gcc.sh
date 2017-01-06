#!/usr/bin/env bash
menes="$(dirname "$0")/.."
source "$menes/utils/bashrc.sh"
source "$menes/utils/assert.sh"
source "$menes/utils/spawn.sh"

echo "$MENES_DEBUG" | grep ":cc-call:" >/dev/null && "$menes/utils/shescape.sh" "$0" "$@" >/dev/tty

cc="$1"
shift

ebl:array flags
ebl:array preflags
ebl:array postflags

function set-link() {
    preflags.+= -Wl,--whole-archive
    postflags.+= -Wl,--no-whole-archive
}

OPTIND=1
while getopts C:c:D:F:f:gH:I:L:l:r:S:O:U: OPTKEY; do
    case $OPTKEY in
        (C)
            flags.+= -o "$OPTARG"
            set-link
        ;;

        (c)
            target="$OPTARG"
            flags.+= -c -o "$OPTARG"
        ;;

        (D) flags.+= -D "$OPTARG";;
        (F) flags.+= "-F$OPTARG";;
        (f) flags.+= -framework "$OPTARG";;
        (g) flags.+= "-g3";;

        (H)
            target="$OPTARG"
            flags.+= -c -o "$OPTARG"
        ;;

        (I) flags.+= -I "$OPTARG";;

        (L) flags.+= "-L$OPTARG";;
        (l) flags.+= "-l$OPTARG";;

        (O) case $OPTARG in
            (most) flags.+= -O3;;
            (some) flags.+= -O1;;
            (none) flags.+= -O0;;
        esac;;

        (r) flags.+= "-Wl,-rpath=$OPTARG";;

        (S)
            flags.+= -shared -o "$OPTARG" "-Wl,-soname=${OPTARG##*/}"
            set-link
        ;;

        (U) flags.+= -U "$OPTARG";;

        (\?)
            shift $((OPTIND - 1))
            fatal "$(basename $0): unsupported flag: $1"
        ;;
    esac
done
shift $((OPTIND - 1))

ebl:array command $cc -H "${flags[@]}" "${preflags[@]}" "$@" "${postflags[@]}"

echo "$MENES_DEBUG" | grep ":cc-exec:" >/dev/null && "$menes/utils/shescape.sh" "${command[@]}" >/dev/tty

if [[ ${target+@} ]]; then
    {
        spawn "${command[@]}" 3>&2 2>&1 1>&3 | (
            declare line
            while IFS='' read -r line; do
                case "$line" in
                    ('Multiple include guards may be useful for:')
                        while IFS='' read -r line; do
                            case "$line" in
                                (?(.)/*) ;;
                                (*) break;;
                            esac
                        done
                    ;;

                    (*(.)?(\!)\ *)
                        header="${line#* }"
                        if [[ $header == !(/*) ]]; then
                            echo "$header"
                        fi
                    ;;

                    (*) echo "$line" 1>&2;;
                esac
            done | sort -u > "$target.dep"
        ) 2>&1
    } 3>&2 2>&1 1>&3 3<>/dev/null
    result="$PIPESTATUS"
else
    spawn "${command[@]}"
    result=$?
fi

rekill_
exit $result
