function cfg:init-mk() {
    echo '_CFG_EMPTY := '
}

function cfg:set-mk() {
    escaped="$(escape.sh "$2")"

    case "$1" in
        ($) echo "CFG_$escaped := $3";;
        (\#) echo "CFG_$escaped := $3";;
        (!) [ "@$3" = '@true' ] && echo "CFG_$escaped = \$(_CFG_EMPTY)";;
    esac
}

add-save 'mk'
