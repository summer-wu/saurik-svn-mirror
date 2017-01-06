function cfg:set-h() {
    escaped="$(escape.sh "$2")"

    case "$1" in
        ($) echo "#define CFG_$escaped \"$3\"";;
        (\#) echo "#define CFG_$escaped $3";;
        (!) [ "@$3" = '@true' ] && echo "#define CFG_$escaped";;
    esac
}

add-save 'h'
