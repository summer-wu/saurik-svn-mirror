#!/usr/bin/tcl

proc uuidgen {} {
    return [exec uuidgen -t]
}

set fin [open [lindex $argv 0]]
while {[gets $fin line] != -1} {
    if {[string length $line] != 0} {
        switch [lindex $line 0] {
            T {
                set time [lindex $line 1]
            }

            H {
                echo $line
            }

            R {
                echo "INSERT INTO 
            }
        }
    }
}
close $fin
