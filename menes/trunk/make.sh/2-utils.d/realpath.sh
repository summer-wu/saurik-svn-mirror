function realpath() {
    $(cfg:get 'gnu readlink') -f "$1"
}
