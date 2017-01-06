source "$menes/utils/color.sh"

function profile() {
    declare target="$1"
    shift

    declare start="$(date '+%s%N')"

    "$@"
    declare result=$?

    case "$result" in
        (0) status='---';;
        (129) status='HUP';;
        (130) status='INT';;
        (137) status='KIL';;
        (139) status='SEG';;
        (143) status='TRM';;
        (*) status="$result";;
    esac

    declare stop="$(date '+%s%N')"

    declare diff=$((${stop%N} - ${start%N}))

    if [[ ${stop:${#stop} - 1:1} == 'N' ]]; then
        declare diff="${diff}000000000"
    fi

    declare sec=$((diff / 1000000000))
    declare nsec=$((diff - sec * 1000000000))

    if [[ $result -eq 0 ]]; then
        declare color="$C_DKGREEN"
    else
        declare color="$C_DKRED"
    fi

    printf '[%s%3s%s] %0.2d:%0.2d.%0.3d: %s\n' "$color" "$status" "$C_CLEAR" $((sec / 60)) $((sec % 60)) $((nsec / 1000000)) "$target" 1>&2

    return ${result}
}
