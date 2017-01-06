config_="$menes/config/cfg"

function cfg:get() {
    source $config_/menes.sh
    declare -p "CFG_$(escape.sh "$1")" | cut -d '=' -f 2 | sed -e 's/^"//; s/"$//'
}

function is-true() {
    cfg:get "$@" >/dev/null
}

declare -a saves_

function add-save() {
    saves_[$((${#saves_[@]} + 1))]="$1"
    "cfg:init-$1" >"$config_/menes.$1" 2>/dev/null
}

function cfg:set() {
    case "$1" in
        ($|\#|!)
            declare type="$1"
            declare name="$2"
            declare value="$3"
        ;;

        (*)
            declare type="${2:0:1}"
            declare name="$1"
            declare value="${2:1}"
        ;;
    esac

    declare reason=''

    case "$type" in
        (\#) if ! [ "$value" -eq "$value" ] 2>/dev/null; then
            fatal "invalid number: $value."
        fi;;

        (!)
            if [[ ${value:0:1} == '%' ]]; then
                declare reason="${value:1}"
                declare value='false'
            elif [[ ! ( $value == 'true' || $value == 'false' ) ]]; then
                fatal "invalid boolean: $value."
            fi
        ;;
    esac

    echo -n "  $name: ($type) $value"

    if [[ ${reason:+@} ]]; then
        echo " ($reason)"
    else
        echo
    fi

    for ((i = 0; i != ${#saves_[@]}; ++i)); do
        declare ext="${saves_[$i + 1]}"
        "cfg:set-$ext" "$type" "$name" "$value" >>"$config_/menes.$ext"
    done

    test "@$type" != '@!' -o "@$value" = '@true' 1>&2
}

function check() {
    case "$1" in
        ($|\#|!)
            type="$1"
            shift
        ;;

        (*)
            type=''
        ;;
    esac

    name=$(name-"$@" 2>/dev/null)
    if [[ $? -ne 0 ]]; then
        name="$@"
    fi

    value="$("$@")"
    if [[ -z ${type:+@} ]]; then
        type="${value:0:1}"
        value="${value:1}"
    fi

    cfg:set "$type" "$name" "$value"
}

function executable() {
    success test -x "$(which "$1" 2>/dev/null)"
}

function success() {
    "$@" >/dev/null 2>/dev/null && echo '!true' || echo '!false'
}

function name-success() {
    echo "$@"
}

for save in "$menes"/config-sh.d/save-*.sh; do
    source "$save"
done
