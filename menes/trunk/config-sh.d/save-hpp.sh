function cfg:set-hpp() {
    escaped="$(escape.sh "$2")"

    case "$1" in
        ($) echo "static const char *cfg$escaped = \"$3\";";;
        (\#) echo "static const int cfg$escaped = $3;";;
        (!) echo "static const bool cfg$escaped = $3;";;
    esac
}

add-save 'hpp'
