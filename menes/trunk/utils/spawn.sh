set -m

function kill_() {
    kill "$1" -- "-$pid_"
}

function sigterm_() {
    signal_=1
    kill_ -SIGTERM
    signaled_=15
}

function sigint_() {
    signal_=1
    kill_ -SIGINT
    signaled_=2
}

function spawn() {
    "$@" &
    pid_=$!

    trap 'sigterm_' SIGTERM
    trap 'sigint_' SIGINT

    signal_=1
    while [[ $signal_ -eq 1 ]]; do
        signal_=0
        wait "$pid_"
        declare result=$?
    done

    trap SIGINT
    trap SIGTERM

    return $result
}

function rekill_() {
    declare result="$?"

    if [[ ${signaled_+@} ]]; then
        trap $signaled_
        kill "-$signaled_" 0
    fi

    return "$result"
}

function reexit_() {
    rekill_
    exit $?
}
