function ebl:array() {
    declare name="$1"
    shift

    eval '
        '"$name"'=()
        _'"$name"'_end=0

        function '"$name"'.+=() {
            while [[ $# -ne 0 ]]; do
                '"$name"'[_'"$name"'_end++]="$1"
                shift
            done
        }
    '

    "$name.+=" "$@"
}
