#!/bin/bash
while [ 1 ]; do
    echo $seed
    MALLOC_CHECK_=0 contest06 -p:snipe.asm -q -L:/proc/self/fd/1 -i:10000 |
        grep 'r1 = [0-9]*, \[[1-9][0-9]*\] = [1-9]' |
        sed -re 's/.*\[([0-9]*)] = ([0-9]*).*/\1 \2/; s/([0-9]*) ([0-9]*)/\/home\/saurik\/src\/extraneous\/rpl-1.4.0\/src\/rpl "L\1: " "L\1: \2 " drone.txt/' |
        bash
done
