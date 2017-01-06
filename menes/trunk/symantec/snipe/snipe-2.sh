#!/bin/bash
while [ 1 ]; do
    contest06 -p:snipe2.asm -i:10000 -q -L:/proc/self/fd/1 |
        grep ^0327 |
        head -n 10 |
        sed -re 's/^.*r3 = [0-9]+, \[([0-9]+)\] = ([0-9]+).*$/\1 \2/'
done >> output.log
