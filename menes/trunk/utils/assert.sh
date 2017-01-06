function fatal() {
    echo "$1" 1>&2
    exit 1
}

function assert() {
    "$@" || fatal "assert: $*"
}
