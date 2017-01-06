#!/usr/bin/tcl

set mask(0) 0
set mask(1) 0
set mask(2) 0
set mask(3) 0

set string [lindex $argv 0]
for {set x 0} {$x != [string length $string]} {incr x} {
    set character [string index $string $x]
    set ordinal [ctype ord $character]
    set index [expr $ordinal / 32]
    set offset [expr $ordinal % 32]
    set mask($index) [expr $mask($index) | (1 << $offset)]
}

echo [format "0x%x, 0x%x, 0x%x, 0x%x" $mask(0) $mask(1) $mask(2) $mask(3)]
