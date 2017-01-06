#!/bin/bash

function infoline {
    echo "info: XJB11, Jay Freeman, saurik@cs.ucsb.edu, 805 895-7209, UCSB"
}

infoline

eval "$(./xjb11.asm |
    sed -re 's/;.*//' |
    grep -v '^ *$' |
    tr '\n' '%' |
    sed -re '
        s/ *([a-zA-Z0-9_]*:) *%/\1 /g
        s/[^%:]*%/ %/g
    ' |
    tr '%' '\n' |
    nl |
    sed -re '
        s/^ *([0-9]*)/IC=\1;/
        s/([a-zA-Z0-9_]*):/\1=$(((IC-1) * 3));/g
    '

    cat <<EOF
        LB=\$((IC * 3))
EOF)"

#echo "data {"

#contest06 -z:<(
#    infoline
    . ./xjb11.asm
#) |
#    tail +3 |
#    head -n $IC |
#    sed -re '
#        s/^.*\(/    /
#        s/\)$//
#        s/ ([0-9A-F])/ 0x\1/g
#    ' > output.dat

#RB=$((IC - repairable / 3))
#cat output.dat
#tail -n $RB output.dat
#tail -n $RB output.dat

#TC=$((IC + RB * 2))
TC=$IC

#echo "}"

echo $LB 1>&2
function to16 {
    for ((n=$1; n != 17; ++n)); do
        echo -n "$n "
    done
}

echo
echo -n "data { "

start=$(((IC - 2) * 3 % 16))
to16 $((start + 1))

for ((i=0; i != (3600 - 3 * TC - (16 - start)) / 16 - 1; ++i)); do
    to16 1
done

echo "}"
