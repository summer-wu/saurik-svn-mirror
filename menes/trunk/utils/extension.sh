function last() {
    shift $(($# - 1))
    echo $1
}

function extension() {
    echo ${1##*.}
}
