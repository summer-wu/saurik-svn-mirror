#!/bin/bash

function dis_() {
    ./disarm $((0x$1 - (0x$2 << 8))) data/$3.{dat,map,sym} >data/$3.arm
}

function dis() {
    dis_ $(eval "echo \${$1}") "$1"
}

a2n='c031c000 0'
bec='18000000 8'
bot='18000000 c'
bss='22000000 8'
cry='cc000000 0'
eep='cc000000 0'
fls='cc000000 0'
llb='cc000000 0'
log='cc000000 0'
nor='a0000000 0'
rom='f30e5000 0'
sys='cc000000 8'
wtf='22000000 8'

make

if [[ $# == 0 ]]; then
    for xxx in bec bot bss cry eep fls llb sys wtf; do
        dis $xxx
    done
else
    dis "$1"
    vim "data/$1.arm"
fi
