function find-first() {
    declare replaced="${1#*$2}"
    if [[ ${#replaced} -eq ${#1} ]]; then
        return -1
    else
        return $((${#1} - ${#replaced} - ${#2}))
    fi
}
