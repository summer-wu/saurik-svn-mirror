function cfg:set-sh() {
    escaped="$(escape.sh "$2")"

    case "$1" in
        ($) echo "CFG_$escaped='$3'";;
        (\#) echo "CFG_$escaped=$3";;
        (!) [ "@$3" = '@true' ] && echo "CFG_$escaped=''";;
    esac
}

add-save 'sh'
