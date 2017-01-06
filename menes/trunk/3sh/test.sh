#!/usr/bin/env bash
function test {
    echo "----- $1 -----"
    "$@" tests.sh | sed -e '
        s/pipe:\[[0-9]\+]/pipe:[#]/g
        s/socket:\[[0-9]\+]/socket:[#]/g
        s/\/proc\/[0-9]\+\//\/proc\/#\//g
    '
}

for sh in 'bash -O extglob' ksh 3sh; do test $sh; done
