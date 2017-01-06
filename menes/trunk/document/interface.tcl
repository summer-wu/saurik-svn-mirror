############################################################################
#                          Interface - TCL Script                          #
#                         by Jay Freeman  (saurik)                         #
#                                                                          #
#                               Version: 3.73                              #
#                         Last Updated: 10/29/2000                         #
#                                                                          #
#                       E-Mail: (saurik@saurik.com)                        #
#                   SaurikSoft (www.saurik.com, #saurik)                   #
#                &: 'fixtext' proc by Roy Keene (MenInBlak)                #
#      &: Nickname Completion Improvements by Lee Perkins (NeW_Jadaa)      #
#                                                                          #
#    Interface was originally designed to make smooth over the forwarding  #
#  messages that people would see when a channel was being linked by your  #
#  average linker (our own BotMaster in this case). Soon, however, saurik  #
#  wanted to do other things, such as send back falsified ping reports to  #
#  people since he hated people who would ping him to see how lagged they  #
#  were (he was in the channel almost ALL the time). Interface has become  #
#  much more than just a linker-smoother-over since then and now has many  #
#  features to make life easier. :)                                        #
############################################################################

#
#/noq                            Auto-closes all open queries
#/gversion                       Returns your CTCP version reply.
#/sversion <version reply>       Sets your CTCP version reply.
#/ghighlight                     Returns your highlight string.
#/shighlight <highlight string>  Sets your highlight string.
#/gfinger                        Returns your CTCP finger reply.
#/sfinger <finger reply>         Sets your CTCP finger reply.
#/sping [<on|off|fake>]          Returns/Sets the mode that ping tools is currently set to.
#/stime [<on|off|fake>]          Returns/Sets the mode that the time system is currenty set to.
#/sctcp [<on|off>]               Returns/Sets whethere CTCP Cloak is activated.
#/split [<#channel|off>]         Tells XiRCON if you want it to try to hunt for a split to regain a channel.
#/sdcc [<on|off>]                Returns/Sets whether DCC transfers should be accepted or not.
#/stdcc [<on|off>]               Returns/Sets whether TDCC transfers should be accepted or not.
#/schat [<on|off>]               Returns/Sets whether DCC chats should be accepted or not.
#/sraw [<on|off>]                Returns/Sets whether or not "raw" mode should be active or not.
#/sbeep [<on|off>]               Returns/Sets whether or not XiRCON should beep when a new Query window is created by an incoming message.
#/sncmode [<off|channel|global>] Returns/Sets whether or not nicknames should be auto completed for you, and where XiRCON should get its nicknames from.
#

set stars "[color join]·[color nick]•[color error]·[color default]"
set preN "·•·"
# « »

on load {
    if {[get_cookie PingR]        == ""} {set_cookie PingR        "Off"}
    if {[get_cookie TimeR]        == ""} {set_cookie TimeR        "Off"}
    if {[get_cookie PageR]        == ""} {set_cookie PageR        "Off"}
    if {[get_cookie VersionR]     == ""} {set_cookie VersionR     "Does it matter? Let's just say it is something YOU wouldn't understand."}
    if {[get_cookie FingerR]      == ""} {set_cookie FingerR      "No ... N.O. ... No"}
    if {[get_cookie TDCC]         == ""} {set_cookie TDCC         "On"}
    if {[get_cookie DCC]          == ""} {set_cookie DCC          "On"}
    if {[get_cookie JoinsIC]      == ""} {set_cookie JoinsIC      "On"}
    if {[get_cookie JoinsIS]      == ""} {set_cookie JoinsIS      "Off"}
    if {[get_cookie PartsIC]      == ""} {set_cookie PartsIC      "On"}
    if {[get_cookie PartsIS]      == ""} {set_cookie PartsIS      "Off"}
    if {[get_cookie QuitsIC]      == ""} {set_cookie QuitsIC      "On"}
    if {[get_cookie QuitsIS]      == ""} {set_cookie QuitsIS      "Off"}
    if {[get_cookie CTCPCloak]    == ""} {set_cookie CTCPCloak    "Off"}
    if {[get_cookie CTCPExamine]  == ""} {set_cookie CTCPExamine  "Off"}
    if {[get_cookie PromptLength] == ""} {set_cookie PromptLength "12"}
    if {[get_cookie StripUUIDs]   == ""} {set_cookie StripUUIDs   "Off"}
    if {[get_cookie NiceChan]     == ""} {set_cookie NiceChan     "On"}
    if {[get_cookie RawChat]      == ""} {set_cookie RawChat      "Off"}
    if {[get_cookie GetDir]       == ""} {set_cookie GetDir       ""}
    if {[get_cookie preN]         == ""} {set_cookie preN         "·•·"}
    if {[get_cookie HType]        == ""} {set_cookie HType        "Line"}
    if {[get_cookie Highlight]    == ""} {set_cookie Highlight    "[my_nick] [string range [my_nick] 0 2]: [string range [my_nick] 0 3]:"}
    if {[get_cookie NCMode]       == ""} {set_cookie NCMode       "Channel"}
    if {[get_cookie NCLine]       == ""} {set_cookie NCLine       "\$n\002:\002 \$m"}
    echo "$stars Loaded script \"[color join]S[color error]a[color nick]u[color action]r[color ctcp]i[color nick]k[color join]S[color quit]o[color error]f[color join]t[color default] [color quit]I[color error]n[color action]t[color nick]e[color ctcp]r[color join]f[color channel]a[color ctcp]c[color part]e[color default] Improvements[color default]\" \ae1:)[color default]" status status
    complete
}
alias dialog {set_cookie Dialog [arg];/run dialogs;complete}
on unload {echo "$stars Unloaded script \"[color join]S[color error]a[color nick]u[color action]r[color ctcp]i[color nick]k[color join]S[color quit]o[color error]f[color join]t[color default] [color quit]I[color error]n[color action]t[color nick]e[color ctcp]r[color join]f[color channel]a[color ctcp]c[color part]e[color default] Improvements[color default]\" \ae1:([color default]" status status;complete}

alias colorz {
    echo "\a1a a1a\a2a a2a\a3a a3a\a4a a4a\a5a a5a\a6a a6a\a7a a7a\a8a a8a\a9a a9a\a10a a10a\a11a a11a\a12a a12a\a13a a13a\a14a a14a\a15a a15a"
    echo "\a1b a1b\a2b a2b\a3b a3b\a4b a4b\a5b a5b\a6b a6b\a7b a7b\a8b a8b\a9b a9b\a10b a10b\a11b a11b\a12b a12b\a13b a13b\a14b a14b\a15b a15b"
    echo "\a1c a1c\a2c a2c\a3c a3c\a4c a4c\a5c a5c\a6c a6c\a7c a7c\a8c a8c\a9c a9c\a10c a10c\a11c a11c\a12c a12c\a13c a13c\a14c a14c\a15c a15c"
    echo "\a1d a1d\a2d a2d\a3d a3d\a4d a4d\a5d a5d\a6d a6d\a7d a7d\a8d a8d\a9d a9d\a10d a10d\a11d a11d\a12d a12d\a13d a13d\a14d a14d\a15d a15d"
    echo "\a1e a1e\a2e a2e\a3e a3e\a4e a4e\a5e a5e\a6e a6e\a7e a7e\a8e a8e\a9e a9e\a10e a10e\a11e a11e\a12e a12e\a13e a13e\a14e a14e\a15e a15e"
    echo "\a1f a1f\a2f a2f\a3f a3f\a4f a4f\a5f a5f\a6f a6f\a7f a7f\a8f a8f\a9f a9f\a10f a10f\a11f a11f\a12f a12f\a13f a13f\a14f a14f\a15f a15f"
    complete
}

#on url_select {
#/run [lindex [args] 0]
#complete
#}

#---------------------------------------------------------------------------
## General: Connection Cookie Support
#---------------------------------------------------------------------------
on 014 {set COOKIE [lindex [args] 1]}
on connect {
  if {[info exists COOKIE]} {
    echo "Trying Cookie: $COOKIE"
    /raw COOKIE $COOKIE
  }
}

#---------------------------------------------------------------------------
## General: Spec char's in channel support.
#---------------------------------------------------------------------------
alias sChan {
    if {[arg] == "off"} {
        set_cookie NiceChan "Off"
        echo "$stars Nice channel names is now set to [color error]Off[color default]."
    } elseif {[arg] == "on"} {
        set_cookie NiceChan "On"
        echo "$stars Nice channel names is now set to [color error]On[color default]."
    } else {
        echo "$stars Nice channel names is currently set to [color error][get_cookie NiceChan][color default]."
    }
    complete
}
proc nicechan { s1 } {
    if {[get_cookie NiceChan] == "On"} {
        set tmp ""
        for {set i 0} {$i < [string length $s1]} {incr i +1} {
            set letter [string range $s1 $i $i]
            if {$letter == ""} {
                set tmp "$tmp[color error]b[color default]"
            } elseif {$letter == ""} {
                set tmp "$tmp[color error]k[color default]"
            } elseif {$letter == ""} {
                set tmp "$tmp[color error]o[color default]"
            } elseif {$letter == ""} {
                set tmp "$tmp[color error]r[color default]"
            } elseif {$letter == ""} {
                set tmp "$tmp[color error]u[color default]"
            } elseif {$letter == " "} {
                set tmp "$tmp[color error]_[color default]"
            } else {
                set tmp "$tmp[color channel][string tolower [string range $s1 $i $i]][color default]"
            }
        }
        return $tmp
    } else {
        return "[color channel]$s1[color default]"
    }

}

#---------------------------------------------------------------------------
## Eggdrop Pings
#---------------------------------------------------------------------------
on ctcp_reply {
    if { [lindex [args] 1] == "ECHO" || [lindex [args] 1] == "ERRMSG"} {
        set temp0 [lindex [args] 2]
        if {[lindex $temp0 0] == "CLIENTINFO:"} {
            set temp0 [lindex $temp0 1]
        }
        set temp1 [expr [clock seconds] - $temp0]
        set temp2 "[color ctcp]$preN CTCP [lindex [args] 1] reply from [color nick][nick][color ctcp]: $temp1 second"
        if {$temp1 != 1} {
            set temp2 $temp2\s
        }
        echo $temp2 status
        complete
    }
}

on NOTICE {
    if {[llength [lindex [args] 1]] == 2 && [lindex [lindex [args] 1] 0] == "Now:"} {
        set temp1 [expr [clock seconds] - [lindex [lindex [args] 1] 1]]
        set temp2 "[color ctcp]$preN CTCP EMAIL reply from [color nick][nick][color ctcp]: $temp1 second"
        if {$temp1 != 1} {
            set temp2 $temp2\s
        }
        echo $temp2 status
        complete
    }
}

#---------------------------------------------------------------------------
## Nicer Names Interface
#---------------------------------------------------------------------------
on 353 {
    lecho "$preN [nicechan [lindex [args] 2]] [color nick][lindex [args] 3][color default]"
    complete
}
on names {
    lecho "$preN [nicechan [lindex [args] 2]] [color nick][lindex [args] 3][color default]"
    complete
}
on 366 {
    lecho "$preN [nicechan [lindex [args] 1]] [lindex [args] 2]"
    complete
}

#---------------------------------------------------------------------------
## Split Detection
#---------------------------------------------------------------------------
set lastsplit ""

alias split {
    if {[arg] == "off"} {
        set_cookie splitwatch ""
        echo "$stars XiRCON will no longer hunt for a split."
    } elseif {[arg] == ""} {
        if {[get_cookie splitwatch] == ""} {
            echo "$stars XiRCON is not currently hunting for a split."
        } else {
            echo "$stars XiRCON is currently hunting for a split, so as to capture [color channel][get_cookie splitwatch][color default]."
        }
    } else {
        set_cookie splitwatch [arg]
        echo "$stars XiRCON will now watch for a split, move to the server, join [color channel][arg][color default], and lock it."
    }
    complete
}
on quit {
    set splitstring [lindex [args] 0]
    set splitstring [arg]
    set periods 0
    set spaces 0
    set lastspace 0
    for {set i 1} {$i < [string length $splitstring] - 1} {incr i +1} {
        if {[string match "." [string range $splitstring $i $i]]} {incr periods +1}
        if {[string match " " [string range $splitstring $i $i]]} {
            incr spaces +1
            set lastspace $i
        }
    }
    if {$spaces == 1 && $periods > 3 && $periods < 7 && [string match "*.*.* *.*.*" $splitstring]} {
        beep
        incr lastspace +1
        set server [string range $splitstring $lastspace end]
        if {$lastsplit != $splitstring} {
            set lastsplit $splitstring
            incr lastspace -2
            
            echo "$preN [color nick]$server[color default] has split from [color nick][string range $splitstring 0 $lastspace][color default]" status status
            if {[get_cookie splitwatch] != ""} {
                /newserver $server
            }
        }
        unset server
    }
    unset i
    unset lastspace
    unset spaces
    unset periods
    unset splitstring
}

on join {
    if {[nick] == [my_nick]} {
        if {[lindex [args] 0] == [get_cookie splitwatch]} {
            set_cookie splitwatch ""
        }
    }
}

on 376 {
    if {[get_cookie splitwatch] != ""} {
        /join [get_cookie splitwatch]
        /mode [get_cookie splitwatch] +i
    }
}

#---------------------------------------------------------------------------
## DCC: Chat Interface Improvements for Eggdrop and Otherwise
#---------------------------------------------------------------------------
alias sRaw {
    if {[arg] == "on"} {
        set_cookie RawChat "On"
        echo "$stars Raw chat is now set to: [color error]On[color default]."
    } elseif {[arg] == "off"} {
        set_cookie RawChat "Off"
        echo "$stars Raw chat is now set to: [color error]Off[color default]."
    } else {
        echo "$stars Raw chat is currently set to: [color error][get_cookie RawChat][color default]."
    }
    complete
}
proc decho { s1 s2 } {
    if {[get_cookie RawChat] == "Off"} {set s1 "[color nick]<$s2>[color default] $s1"}
    echo $s1 chat $s2
}
on chat_send {
    if {[get_cookie RawChat] == "Off"} {
        echo "[color nick]<[my_nick]>[color default] [lindex [args] 0]" chat [nick]
    } else {
        echo "[color nick]>[color default] [lindex [args] 0]" chat [nick]
    }
    complete
}
on chatx_text {
    set msg [lindex [args] 0]
    set eggTime ""
    set eggChan ""
    set eggMembers ""
    set eggChops ""
    set eggBans ""
    set eggUser ""
    set eggBot ""
    set eggMisc ""
    if {[string match "\[\[\]*:*\] *: * member* (* chop*), * ban*" $msg]} {
        set msg [string range $msg 1 end]
        scan $msg "%\[^\]\]\] %\[^ :\] : %\[^ \] %*\[^ \] ( %\[^ \] %*\[^)\]), %\[^ \]" eggTime eggChan eggMembers eggChops eggBans
        set msg "\[$eggTime\] [nicechan $eggChan] - [color join]$eggMembers Member"
        if {$eggMembers > 1 || $eggMembers == 0} {set msg [append $msg $msg s]}
        set msg "$msg[color default] - [color channel]$eggChops Chop"
        if {$eggChops > 1 || $eggChops == 0} {set msg [append $msg $msg s]}
        set msg "$msg[color default] - [color error]$eggBans Ban"
        if {$eggBans > 1 || $eggBans == 0} {set msg [append $msg $msg s]}
        set msg "$msg[color default]"
        decho $msg [nick]
    } elseif {[string match "\[*\]\[*\]\[*\] (*) * is now away: *" $msg]} {
        scan $msg "$preN (%\[^)\]) %\[^ \] is now away: %s" eggBot eggUser eggMisc
        decho "$preN ([color nick]$eggBot[color default]) [color nick]$eggUser[color default] is now away: $eggMisc" [nick]
    } elseif {[string match "\[*\]\[*\]\[*\] * is now away: *" $msg]} {
        scan $msg "$preN %\[^ \] is now away: %s" eggUser eggMisc
        decho "$preN [color nick]$eggUser[color default] is now away: $eggMisc" [nick]
    } elseif {[string match "\[*\]\[*\]\[*\] (*) * is no longer away." $msg]} {
        scan $msg "$preN (%\[^)\]) %\[^ \] is no longer away." eggBot eggUser
        decho "$preN ([color nick]$eggBot[color default]) [color nick]$eggUser[color default] is no longer away." [nick]
    } elseif {[string match "\[*\]\[*\]\[*\] * is no longer away." $msg]} {
        scan $msg "$preN %\[^ \] is no longer away." eggUser
        decho "$preN [color nick]$eggUser[color default] is no longer away." [nick]
    } elseif {[string match "\[\[\]*:*\]" [lindex $msg 0]] && [string match "#*#" [lindex $msg 1]]} {
        set msg [string range $msg 1 end]
        scan $msg "%\[^\]\]\] #%\[^#\]# " eggTime eggUser
        decho "\[$eggTime\] #[color nick]$eggUser[color default]# [string range $msg [expr [string length $eggUser] + 10] end]" [nick]
    } else {
        decho $msg [nick]
    }
    complete
}

on chat_text {
    decho [lindex [args] 0] [nick]
    complete
}

#---------------------------------------------------------------------------
## DCC: Chat Filter
#---------------------------------------------------------------------------
alias sChat {
    if {[arg] == "on"} {
        set_cookie Chat "On"
        echo "$stars DCC Chat is now set to: [color error]On[color default]."
    } elseif {[arg] == "off"} {
        set_cookie Chat "Off"
        echo "$stars DCC Chat is now set to: [color error]Off[color default]."
    } else {
        echo "$stars DCC Chat is currently set to: [color error][get_cookie Chat][color default]."
    }
    complete
}
on ctcp {
    if { [lindex [args] 1] == "DCC" } {
        if {[string match "CHAT*" [lindex [args] 2]]} {
            echo "$stars"
            if {[get_cookie Chat] == "Off"} {
                /raw PRIVMSG [nick] :Sorry, I am not here right now, do not DCC CHAT me, I am busy.
                echo "$stars DCC CHAT attempt by [color nick][nick][color default] ([lindex [args] 2]) has been rejected" status status
                complete
            }
        }
    }
}

#---------------------------------------------------------------------------
## DCC: Send Filter/Extended
#---------------------------------------------------------------------------
alias sDCC {
    if {[arg] == "on"} {
        set_cookie DCC "On"
        echo "$stars DCC is now set to: [color error]On[color default]."
    } elseif {[arg] == "off"} {
        set_cookie DCC "Off"
        echo "$stars DCC is now set to: [color error]Off[color default]."
    } else {
        echo "$stars DCC is currently set to: [color error][get_cookie DCC][color default]."
    }
    complete
}
on ctcp {
    global tcl_precision
    set tcl_precision 17
    if {[lindex [args] 1] == "DCC"} {
        if {[string match "SEND*" [lindex [args] 2]] || [string match "TSEND*" [lindex [args] 2]]} {
            echo "$stars [args]"
            if {[get_cookie DCC] == "Off"} {
                echo "$stars DCC attempt by [color nick][nick][color default] ([lindex [args] 2]) has been rejected" status status
                complete
            }
        }
    }
}

#---------------------------------------------------------------------------
## DCC: TSend Filter
#---------------------------------------------------------------------------
alias sTDCC {
    if {[arg] == "on"} {
        set_cookie TDCC "On"
        echo "$stars TDCC is now set to: [color error]On[color default]."
    } elseif {[arg] == "off"} {
        set_cookie TDCC "Off"
        echo "$stars TDCC is now set to: [color error]Off[color default]."
    } else {
        echo "$stars TDCC is currently set to: [color error][get_cookie TDCC][color default]."
    }
    complete
}
on ctcp {
    if {[lindex [args] 1] == "DCC"} {
        if {[string match "TSEND*" [lindex [args] 2]] != 0} {
            if {[get_cookie TDCC] == "Off"} {
                /msg [nick] Sorry, I do not accept TDCC's, please send again as a normal DCC, or do not send at all.
                echo "$stars TDCC attempt by [color nick][nick][color default] ([lindex [args] 2]) has been rejected" status status
                complete
            }
        }
    }
}

#---------------------------------------------------------------------------
## Join/Part/Quit in Status/Channel
#---------------------------------------------------------------------------
alias sJoinsIS {
    if { [arg] == "on" } {
        set_cookie JoinsIS "On"
        echo "$stars Joins in status is now set to: [color error]On[color default]." [window_type] [window_name]
    } elseif { [arg] == "off" } {
        set_cookie JoinsIS "Off"
        echo "$stars Joins in status is now set to: [color error]Off[color default]." [window_type] [window_name]
    } else {
        echo "$stars Joins in status is currently set to: [color error][get_cookie JoinsIS][color default]." [window_type] [window_name]
    }
    complete
}
alias sPartsIS {
    if { [arg] == "on" } {
        set_cookie PartsIS "On"
        echo "$stars Parts in status is now set to: [color error]On[color default]." [window_type] [window_name]
    } elseif { [arg] == "off" } {
        set_cookie PartsIS "Off"
        echo "$stars Parts in status is now set to: [color error]Off[color default]." [window_type] [window_name]
    } else {
        echo "$stars Parts in status is currently set to: [color error][get_cookie PartsIS][color default]." [window_type] [window_name]
    }
    complete
}
alias sQuitsIS {
    if { [arg] == "on" } {
        set_cookie QuitsIS "On"
        echo "$stars Quits in status is now set to: [color error]On[color default]." [window_type] [window_name]
    } elseif { [arg] == "off" } {
        set_cookie QuitsIS "Off"
        echo "$stars Quits in status is now set to: [color error]Off[color default]." [window_type] [window_name]
    } else {
        echo "$stars Quits in status is currently set to: [color error][get_cookie QuitsIS][color default]." [window_type] [window_name]
    }
    complete
}
alias sJoinsIC {
    if { [arg] == "on" } {
        set_cookie JoinsIC "On"
        echo "$stars Joins in channel is now set to: [color error]On[color default]." [window_type] [window_name]
    } elseif { [arg] == "off" } {
        set_cookie JoinsIC "Off"
        echo "$stars Joins in channel is now set to: [color error]Off[color default]." [window_type] [window_name]
    } else {
        echo "$stars Joins in channel is currently set to: [color error][get_cookie JoinsIC][color default]." [window_type] [window_name]
    }
    complete
}
alias sPartsIC {
    if { [arg] == "on" } {
        set_cookie PartsIC "On"
        echo "$stars Parts in channel is now set to: [color error]On[color default]." [window_type] [window_name]
    } elseif { [arg] == "off" } {
        set_cookie PartsIC "Off"
        echo "$stars Parts in channel is now set to: [color error]Off[color default]." [window_type] [window_name]
    } else {
        echo "$stars Parts in channel is currently set to: [color error][get_cookie PartsIC][color default]." [window_type] [window_name]
    }
    complete
}
alias sQuitsIC {
    if { [arg] == "on" } {
        set_cookie QuitsIC "On"
        echo "$stars Quits in channel is now set to: [color error]On[color default]." [window_type] [window_name]
    } elseif { [arg] == "off" } {
        set_cookie QuitsIC "Off"
        echo "$stars Quits in channel is now set to: [color error]Off[color default]." [window_type] [window_name]
    } else {
        echo "$stars Quits in channel is currently set to: [color error][get_cookie QuitsIC][color default]." [window_type] [window_name]
    }
    complete
}
## ADD CODE 1: QUITS IN CHANNEL
on join {
    if {[nick] == [my_nick]} {
        echo "[color join]$preN Now talking in [nicechan [lindex [args] 0]]" channel [lindex [args] 0]
    }
    if {[get_cookie JoinsIS] == "On"} {echo "[color join]$preN Join ([nicechan [lindex [args] 0]][color join]):[color default] [color nick][nick][color default] [color join]([user]@[host])[color default]" status}
    if {[get_cookie JoinsIC] == "On"} {echo "[color join]$preN Join:[color default] [color nick][nick][color default] [color join]([user]@[host])[color default]" channel [lindex [args] 0]}
    complete
}
on part {
    if {[nick] == [my_nick]} {
        echo "[color part]$preN No longer talking in [nicechan [lindex [args] 0]]" status
    }
    if {[get_cookie PartsIS] == "On"} {echo "[color part]$preN Part ([nicechan [lindex [args] 0]][color part]):[color default] [color nick][nick][color default] [color part]([user]@[host])[color default]" status}
    if {[get_cookie PartsIC] == "On"} {echo "[color part]$preN Part:[color default] [color nick][nick][color default] [color part]([user]@[host])[color default]" channel [lindex [args] 0]}
    complete
}

#---------------------------------------------------------------------------
## CTCP: Examine
#---------------------------------------------------------------------------
alias sCTCPEx {
    if {[arg] == "off"} {
        set_cookie CTCPExamine "Off"
        echo "$stars CTCP Examine is now set to [color error]Off[color default]."
    } elseif {[arg] == "on"} {
        set_cookie CTCPExamine "On"
        echo "$stars CTCP Examine is now set to [color error]On[color default]."
    } else {
        echo "$stars CTCP Examine is currently set to [color error][get_cookie CTCPExamine][color default]."
    }
    complete
}
on ctcp {
    if {[get_cookie CTCPCloak] == "On" && [lindex [args] 1] != "ACTION"} {
        set CTCPMsg ""
        if {[lindex [args] 2] != ""} {set CTCPMsg " [lindex [args] 2]"}
        echo "[color CTCP]$preN CTCP [lindex [args] 1]$CTCPMsg (Cloak) from [color nick][nick][color default]" status
        unset CTCPMsg
        complete
    }
}
on ctcp {
    if {[get_cookie CTCPExamine] == "On"} {
        echo ":[nick]![user]@[host] PRIVMSG [lindex [args] 0] :\001[string range [args] [expr [string length [lindex [args] 0]] + 1] end]\001" status
   }
}
on ctcp_reply {
    if {[get_cookie CTCPExamine] == "On"} {
        echo ":[nick]![user]@[host] NOTICE [lindex [args] 0] :\001[string range [args] [expr [string length [lindex [args] 0]] + 1] end]\001" status status
    }
}

#---------------------------------------------------------------------------
## CTCP: Complete Cloak
#---------------------------------------------------------------------------
alias sCTCP {
    if {[arg] == "off"} {
        set_cookie CTCPCloak "Off"
        echo "$stars CTCP Cloak is now set to [color error]Off[color default]."
    } elseif {[arg] == "on"} {
        set_cookie CTCPCloak "On"
        echo "$stars CTCP Cloak is now set to [color error]On[color default]."
    } else {
        echo "$stars CTCP Cloak is currently set to [color error][get_cookie CTCPCloak][color default]."
    }
    complete
}
on ctcp {
    if {[get_cookie CTCPCloak] == "On" && [lindex [args] 1] != "ACTION"} {
        set CTCPMsg ""
        if {[lindex [args] 2] != ""} {set CTCPMsg " [lindex [args] 2]"}
        echo "[color CTCP]$preN CTCP [lindex [args] 1]$CTCPMsg (Cloak) from [color nick][nick][color default]" status
        unset CTCPMsg
        complete
    }
}

#---------------------------------------------------------------------------
## Invite: Current window and Join-Last
#---------------------------------------------------------------------------
on invite {
    set lastInvite [lindex [args] 1]
    echo "$preN [color nick][nick][color default] invites you to join [color channel][lindex [args] 1]" status
    if {[window type] != "status"} {
        echo "$preN [color nick][nick][color default] invites you to join [color channel][lindex [args] 1]"
    }
    complete
}

#---------------------------------------------------------------------------
## CTCP: Time System
#---------------------------------------------------------------------------
alias sTime {
    if {[arg] == "off"} {
        set_cookie TimeR "Off"
        echo "$stars CTCP time reply is now set to [color error]Off[color default]."
    } elseif {[arg] == "on"} {
        set_cookie TimeR "On"
        echo "$stars CTCP time reply is now set to [color error]On[color default]."
    } elseif {[arg] == "fake"} {
        set_cookie TimeR "Fake"
        echo "$stars CTCP time reply is now set to [color error]Fake[color default]."
    } else {
        echo "$stars CTCP time reply is currently set to [color error][get_cookie TimeR][color default]."
    }
    complete
}
on ctcp {
    if {[lindex [args] 1] == "TIME"} {
        echo "[color CTCP]$preN CTCP TIME ([get_cookie TimeR]) from [color nick][nick][color default]" status status
        if {[get_cookie TimeR] == "Off"} {
            complete
        } elseif { [get_cookie TimeR] == "Fake" } {
            /raw notice [nick] :\001TIME Time circuits currently set to: Thu Mar 27 12:00:00 1985\001
            complete
        }
    }
}

#---------------------------------------------------------------------------
## CTCP: Ping Tools
#---------------------------------------------------------------------------
alias sPing {
    if {[arg] == "off"} {
        set_cookie PingR "Off"
        echo "$stars CTCP ping reply is now set to [color error]Off[color default]."
    } elseif {[arg] == "on"} {
        set_cookie PingR "On"
        echo "$stars CTCP ping reply is now set to [color error]On[color default]."
    } elseif {[arg] == "fake"} {
        set_cookie PingR "Fake"
        echo "$stars CTCP ping reply is now set to [color error]Fake[color default]."
    } else {
        echo "$stars CTCP ping reply is currently set to [color error][get_cookie PingR][color default]."
    }
    complete
}
on ctcp {
    if {[lindex [args] 1] == "PING"} {
        echo "[color CTCP]$preN CTCP PING ([get_cookie PingR]) [lindex [args] 2] from [color nick][nick][color default]" status status
        if {[get_cookie PingR] == "On"} {
            /raw notice [nick] :\001PING [lindex [args] 2]\001
        } elseif { [get_cookie PingR] == "Fake" } {
            /raw notice [nick] :\001PING [random 872000000 872390000]\001
        }
        complete
    }
    set firstchr [string range [lindex [args] 1] 0 0]
    if { $firstchr == "0" || $firstchr == "1" || $firstchr == "2" || $firstchr == "3" || $firstchr == "4" || $firstchr == "5" || $firstchr == "6" || $firstchr == "7" || $firstchr == "8" || $firstchr == "9" } {
        /CTCP [nick] - That was NOT appreciated.
        foreach c [channels] {/raw kick $c [nick] :I really don't care what my ping reply is.}
    }
}

#---------------------------------------------------------------------------
## CTCP: Page System
#---------------------------------------------------------------------------
alias sPage {
    if {[arg] == "off"} {
        set_cookie PageR "Off"
        echo "$stars CTCP paging is now set to [color error]Off[color default]."
    } elseif {[arg] == "on"} {
        set_cookie PageR "On"
        echo "$stars CTCP paging is now set to [color error]On[color default]."
#    } elseif {[arg] == "page"} {
#        set_cookie PageR "Page"
#        echo "$stars CTCP paging is now set to [color error]Page[color default]."
    } elseif {[arg] == "reject"} {
        set_cookie PageR "Reject"
        echo "$stars CTCP paging is now set to [color error]Reject[color default]."
    } else {
        echo "$stars CTCP paging is currently set to [color error][get_cookie PageR][color default]."
    }
    complete
}
on ctcp {
    if {[lindex [args] 1] == "PAGE"} {
        if {[get_cookie PageR] == "On"} {
            lecho "[color CTCP]$preN CTCP PAGE ([get_cookie PageR]) from [color nick][nick][color default]"
            beep
        } else {
            echo "[color CTCP]$preN CTCP PAGE ([get_cookie PageR]) from [color nick][nick][color default]" status
            if {[get_cookie PageR] == "Reject"} {
                /raw PRIVMSG [nick] :Please do NOT page me!
            }
        }
        complete
    }
}

#---------------------------------------------------------------------------
## CTCP: Finger/Version Reply
#---------------------------------------------------------------------------
alias sVersion {
    set_cookie VersionR [arg]
    echo "$stars Your CTCP Version reply is now set to: [color channel][get_cookie VersionR]"
    complete
}
alias gVersion {
    echo "$stars Your CTCP Version reply is currently set to: [color channel][get_cookie VersionR]"
    complete
}

alias sFinger {
    set_cookie FingerR [arg]
    echo "$stars Your CTCP Finger reply is now set to: [color channel][get_cookie FingerR]"
    complete
}
alias gFinger {
    echo "$stars Your CTCP Finger reply is currently set to: [color channel][get_cookie FingerR]"
    complete
}

on ctcp {
    if {[lindex [args] 1] == "FINGER"} {
        if {[lindex [args] 2] == ""} {
            echo "[color CTCP]$preN CTCP FINGER from [color nick][nick][color default]" status status
            /raw notice [nick] :\001FINGER [get_cookie FingerR]\001
            complete
        }
    }
    if {[lindex [args] 1] == "VERSION"} {
        if {[lindex [args] 2] == ""} {
            echo "[color CTCP]$preN CTCP VERSION from [color nick][nick][color default]" status status
            /raw notice [nick] :\001VERSION [get_cookie VersionR]\001
            complete
        }
    }
}

#---------------------------------------------------------------------------
## Whois: Various Improvements
#---------------------------------------------------------------------------
set DNSready 0

#% 200 (TRACELINK):      "Link <version & debug level> <destination> <next server>"
#% 201 (TRACECONNECTING): "Try. <class> <server>"
#% 202 (TRACEHANDSHAKE): "H.S. <class> <server>"
#% 203 (TRACEUNKNOWN):   "???? <class> [<client IP address in dot form>]"
#% 204 (TRACEOPERATOR):  "Oper <class> <nick>"
#% 205 (TRACEUSER):      "User <class> <nick>"
#% 206 (TRACESERVER):    "Serv <class> <int>S <int>C <server> <nick!user|*!*>@<host|server>"
#% 208 (TRACENEWTYPE):   "<newtype> 0 <client name>"
#% 211 (STATSLINKINFO):  "<linkname> <sendq> <sent messages> <sent bytes> <received messages> <received bytes> <time open>"
#% 212 (STATSCOMMANDS):  "<command> <count>"
#% 213 (STATSCLINE):     "C <host> * <name> <port> <class>"
#% 214 (STATSNLINE):     "N <host> * <name> <port> <class>"
#% 215 (STATSILINE):     "I <host> * <host> <port> <class>"
#% 216 (STATSKLINE):     "K <host> * <username> <port> <class>"
#% 218 (STATSYLINE):     "Y <class> <ping frequency> <connect frequency> <max sendq>"
#% 219 (ENDOFSTATS):     "<stats letter> :End of /STATS report"
#% 221 (MODEIS):         "<user mode string>"
#% 241 (STATSLLINE):     "L <hostmask> * <servername> <maxdepth>"
#% 242 (STATSUPTIME):    ":Server Up %d days %d:%02d:%02d"
#% 243 (STATSOLINE):     "O <hostmask> * <name>"
#% 244 (STATSHLINE):     "H <hostmask> * <servername>"
#% 245 (STATSSLINE):     Undocumented
#% 246 (STATSTLINE):     Undocumented
#% 249 (STATSDEBUG):     Undocumented at this time
#% 251 (LUSERCLIENT):    ":There are <integer> users and <integer> invisible on <integer> servers"
#% 252 (LUSEROP):        "<integer> :operator(s) online"
#% 253 (LUSERUNKNOWN):   "<integer> :unknown connection(s)"
#% 254 (LUSERCHANNELS):  "<integer> :channels formed"
#% 255 (LUSERME):        ":I have <integer> clients and <integer> servers"
#% 256 (ADMINME):        "<server> :Administrative info"
#% 257 (ADMINLOC1):      ":<admin info>"
#% 258 (ADMINLOC2):      ":<admin info>"
#% 259 (ADMINEMAIL):     ":<admin info>"
#% 261 (TRACELOG):       "File <logfile> <debug level>"
#% 262 (TRACEPING):
#% 263 (LOAD2HI):        Server load is temporarily too heavy. Please wait
#% 275 (STATSDLINE):     "%c %s %s"
#% 280 (GLIST):          Undocumented at this time
#% 281 (ENDOFGLIST):     Undocumented at this time
#% 300 (NONE):
#% 301 (AWAY):           "<nick> :<away message>"
#% 302 (USERHOST):       ":[<reply>{<space><reply>}]"
#                        <reply> ::= <nick>['*'] '=' <'+'|'-'><hostname>
#% 303 (ISON):           ":[<nick> {<space><nick>}]"
#% 305 (UNAWAY):         ":You are no longer marked as being away"
#% 306 (NOWAWAY):        ":You have been marked as being away"
#% 310 (WHOISHELPOP):
#% 311 (WHOISUSER):      "<nick> <user> <host> * :<real name>"
on 311 {
    set tmpDNS [lindex [args] 3]
    set DNSready 1
    lecho "$preN [color nick][lindex [args] 1][color default] is [lindex [args] 2][color default]@[lindex [args] 3] ([lindex [args] 5][color default])"
    complete
}
#% 312 (WHOISSERVER):    "<nick> <server> :<server info>"
on 312 {
    lecho "$preN   on IRC via [color nick][lindex [args] 2][color default] ([lindex [args] 3])"
    complete
}
#% 313 (WHOISOPERATOR):  "<nick> :is an IRC operator"
on 313 {
    lecho "$preN   [color nick][lindex [args] 1][color default] [lindex [args] 2]"
    complete
}
#% 314 (WHOWASUSER):     "<nick> <user> <host> * :<real name>"
#% 315 (ENDOFWHO): "<name> :End of /WHO list"
on 315 {
    lecho "$preN [lindex [args] 2]"
    complete
}
#% 317 (WHOISIDLE):      "<nick> <integer> :seconds idle"
on 319 {}
#% 318 (ENDOFWHOIS):     "<nick> :End of /WHOIS list"
on 318 {
    if {$DNSready != 0} {
        set DNSready 0
        /dns $tmpDNS
        unset tmpDNS
    }
    lecho "$preN [lindex [args] 2]"
    complete
}
#% 319 (WHOISCHANNELS): "<nick> :{[@|+]<channel><space>}"
on 319 {
    set tmp "$preN   on channels:"
    foreach c [lindex [args] 2] {
        set tmp "$tmp [nicechan $c]"
    }
    lecho $tmp
    unset tmp
    complete
}
#% 321 (LISTSTART):     "Channel :Users  Name"
#% 322 (LIST):          "<channel> <# visible> :<topic>"
#% 323 (LISTEND):       ":End of /LIST"
#% 324 (CHANNELMODEIS): "<channel> <mode> <mode params>"
on 329 {
    if { [lindex [args] 2] < 1000000 } {
        echo "$preN [nicechan [lindex [args] 1]][color default] was created when dinosaurs roamed the earth ;)" status
        complete
    }
}
#% 331 (NOTOPIC):       "<channel> :No topic is set"
#% 332 (TOPIC):         "<channel> :<topic>"
#% 341 (INVITING):      "<channel> <nick>"
#% 342 (SUMMONING):     "<user> :Summoning user to IRC"
#% 351 (VERSION):       "<version>.<debuglevel> <server> :<comments>"
#% 352 (WHOREPLY):      "<channel> <user> <host> <server> <nick> <H|G>[*][@|+] :<hopcount> <real name>"
#% 353 (NAMREPLY):      "<channel> :[[@|+]<nick> [[@|+]<nick> [...]]]"
#% 364 (LINKS):         "<mask> <server> :<hopcount> <server info>"
#% 365 (ENDOFLINKS):    "<mask> :End of /LINKS list"
#% 366 (ENDOFNAMES):    "<channel> :End of /NAMES list"
#% 367 (BANLIST):       "<channel> <banid>"
#% 368 (ENDOFBANLIST):  "<channel> :End of channel ban list"
#% 369 (ENDOFWHOWAS):   "<nick> :End of WHOWAS"
#% 371 (INFO):          ":<string>"
#% 372 (MOTD):          ":- <text>"
#% 374 (ENDOFINFO):     ":End of /INFO list"
#% 375 (MOTDSTART):     ":- <server> Message of the day - "
#% 376 (ENDOFMOTD):     ":End of /MOTD command"
#% 377 (ISASERVICE):    Undocumented
#% 381 (YOUREOPER):     ":You are now an IRC operator"
#% 382 (REHASHING):     "<config file> :Rehashing"
#% 391 (TIME):          "<server> :<string showing server's local time>"
#% 392 (USERSSTART):    ":UserID   Terminal  Host"
#% 393 (USERS):         ":%-8s %-9s %-8s"
#% 394 (ENDOFUSERS):    ":End of users"
#% 395 (NOUSERS):       ":Nobody logged in"

on mode {
    if {[nick] == ""} {
        echo "[color change]$preN Mode change [color mode]\"[string range [arg] [expr [string length [lindex [args] 0]] + 1] end]\"[color change] by [color nick][host]" channel [lindex [args] 0]
    } else {
        echo "[color change]$preN Mode change [color mode]\"[string range [arg] [expr [string length [lindex [args] 0]] + 1] end]\"[color change] by [color nick][nick]" channel [lindex [args] 0]
    }
    complete
}

on nick {
    foreach c [channels] {
        if {[nicks $c [nick]] != "" || [nicks $c +[nick]] != "" || [nicks $c @[nick]] != ""} {
            echo "[color change]$preN [color nick][nick][color change] is now known as [color nick][lindex [args] 0]" channel $c
        }
    }
    complete
}

on quit {
    if {[get_cookie QuitsIS] == "On"} {echo "[color quit]$preN Quit:[color default] [color nick][nick][color default] [color quit]([lindex [args] 0])[color default]" status}
    if {[get_cookie QuitsIC] == "On"} {
        foreach c [channels] {
            if {[nicks $c [nick]] != "" || [nicks $c +[nick]] != "" || [nicks $c @[nick]] != ""} {
                echo "[color quit]$preN Quit:[color default] [color nick][nick][color default] [color quit]([lindex [args] 0])[color default]" channel $c
            }
        }
    }
    complete
}

#•·øØ¤°

on topic {
    echo "[color change]$preN Topic changed to [color mode]\"[lindex [args] 1]\"[color change] by [color nick][nick]" channel [lindex [args] 0]
    complete
}

on wallops {
    echo "$preN [color nick][host][color default] • [color notice][lindex [args] 0]" status
    complete
}

#*** WALLOPS: <progteam.dyn.ml.org> AdminOper saurik used MODE #crp -o TestNick
#WALLOPS :AdminOper saurik used MODE #crp +o TestNick

#---------------------------------------------------------------------------
## Desynched Message Improvements
#  (help from: Roy Keene)
#---------------------------------------------------------------------------
on 482 {
    lecho "[color error]$preN[color default] [lindex [args] 1] [lindex [args] 2]"
    if {[am_on_channel [lindex [args] 1]]} {
        if {[nicks [lindex [args] 1] "@[my_nick]"]=="@[my_nick]"} {
            echo "[color error]$preN NOTE: The last 482 (you don't have ops error) recieved for [lindex [args] 1] might have been caused by DESYNC seeing that you are an op in that channel :([color default]" status
            echo "[color error]$preN NOTE: The last 482 (you don't have ops error) recieved for [lindex [args] 1] might have been caused by DESYNC seeing that you are an op in that channel :([color default]" channel [lindex [args] 1]
        }
    }
    complete
}
#"482: [raw_args] | [nick] | [user] | [host]" status status

#---------------------------------------------------------------------------
## Network ID
#---------------------------------------------------------------------------
#set IRCNetwork ""
#on 001 {
#    set IRCNetwork ""
#    for {set i 3} {$i < [expr [llength [lindex [args] 1]] - 1]} {incr i +1} {
#        set IRCNetwork "$IRCNetwork [lindex [lindex [args] 1] $i]"
#    }
#    set IRCNetwork [string range $IRCNetwork 1 end]
#    UpdateTitle $IRCNetwork
#}

#on timer {window set_title "<$IRCNetwork> [my_nick] +[mode] [server]" status}

#proc UpdateTitle { s1 } {
#    window set_title "<$s1> [my_nick] +[mode] [server]" status
#}

#on mode {
#    if {[lindex [args] 0] == [my_nick]} {
#        echo "DAMN!! $IRCNetwork"
#        UpdateTitle $IRCNetwork
#    }
#}

#---------------------------------------------------------------------------
## CTCP: Sound Support
#---------------------------------------------------------------------------

on ctcp {
    if {[lindex [args] 1] == "SOUND"} {
        if {[lindex [args] 0] == [my_nick]} {
            if {[window exists query [nick]] == 0} {/query [nick]}
            set windowtype "query"
            set windowtosendto [nick]
        } else {
            set windowtype "channel"
            set windowtosendto [lindex [args] 0]
        }
        echo "[color mode]\[[color error][nick][color mode]\][color channel] [lindex [args] 2]" $windowtype $windowtosendto
        complete
    }
}

#---------------------------------------------------------------------------
## NickName Completion, outgoing message enhancements.
#---------------------------------------------------------------------------
alias sNCMode {
    if {[arg] == "off"} {
        set_cookie NCMode "Off"
        echo "$stars Nickname Completion is now set to [color error]Off[color default]."
    } elseif {[arg] == "global"} {
        set_cookie NCMode "Global"
        echo "$stars Nickname Completion is now set to [color error]Global[color default]."
    } elseif {[arg] == "channel"} {
        set_cookie NCMode "Channel"
        echo "$stars Nickname Completion is now set to [color error]Channel[color default]."
    } else {
        echo "$stars Nickname Completion is currently set to [color error][get_cookie NCMode][color default]."
    }
    complete
}
alias sNCLine {
    if {[arg] == ""} {
        echo "$stars Nickname Completion Line is currently set to [color error][get_cookie NCLine][color default]."
    } else {
        set_cookie NCLine [join [arg]]
        echo "$stars Nickname Completion Line is now set to [color error][join [arg]][color default]."
    }
    complete
}

alias PRIVMSG {
    set m [raw_args]
    set m [string range $m [expr [string first " " $m] + 1] end]
    if {[get_cookie NCMode] != "Off" && [lindex [args] 0] == [channel] && [string match "?*:" [lindex [args] 1]]} {
        set niq [string tolower [string trimright [lindex [args] 1] ":"]]
        set n ""
        if {[get_cookie NCMode] == "Channel"} {
            foreach nik [nicks [channel] "*$niq*"] {
                if {[string match "$niq" [string tolower [nnick $nik]]]} {
                    set n [nnick $nik]
                }
            }
            if {$n == ""} {
                foreach nik [nicks [channel] "*$niq*"] {
                    if {[string match "$niq*" [string tolower [nnick $nik]]]} {
                        set n [nnick $nik]
                    }
                }
            }
        } elseif {[get_cookie NCMode] == "Global"} {
            foreach tmpchan [channels] {
                foreach nik [nicks $tmpchan "*$niq*"] {
                    if {[string match "$niq*" [string tolower [nnick $nik]]]} {
                        set n [nnick $nik]
                    }
                }
            }
        }
        if {$n != ""} {
            set m [string range $m [expr [string first " " $m] + 1] end]
            set m [fixtext \$m $m [fixtext \$n $n [get_cookie NCLine]]]
        }
    }
    set wtype "query"
    switch [string index [lindex [args] 0] 0] {
        "#" {set wtype "channel"}
        "$" {set wtype "channel"}
        "&" {set wtype "channel"}
    }
    if {[window exists $wtype [lindex [args] 0]]} {
        set promptlen [get_cookie PromptLength]
        incr promptlen -[string length [my_nick]]
        echo "[color mode]<[color nick][my_nick][color mode]>[color default][spaces $promptlen] $m" $wtype [lindex [args] 0]
    } else {
        echo "[color nick]-> *[lindex [args] 0]*[color default] $m"
    }
    /raw "PRIVMSG [lindex [args] 0] :$m"
    complete
}
alias ME {
    switch [window_type] {
        channel {
            set promptlen [get_cookie PromptLength]
            incr promptlen -[string length [my_nick]]
            echo "[color mode]•[color default] [color nick][my_nick][color default][spaces $promptlen] [color action][raw_args]" [window_type] [window_name]
            /raw "PRIVMSG [window_name] :\001ACTION [raw_args]\001"
        }
        query {
            set promptlen [get_cookie PromptLength]
            incr promptlen -[string length [my_nick]]
            echo "[color mode]•[color default] [color nick][my_nick][color default][spaces $promptlen] [color action][raw_args]" [window_type] [window_name]
            /raw "PRIVMSG [window_name] :\001ACTION [raw_args]\001"
        }
        chat {
            say "\001ACTION [raw_args]\001"
        }
    }
    complete
}
proc fixtext {v a text} {
    set at [instr 0 $text "$v"]
    if {$at != "-1"} {
        set reslt "[string range $text 0 [expr $at -1]]$a[string range $text [expr $at +[string length $v]] [string length $text]]"
    } else {
        set reslt $text
    }
    return $reslt
}

#---------------------------------------------------------------------------
## Visual Enhancements, Highlighting, and LinkBot Support
#---------------------------------------------------------------------------

alias sHType {
    if {[arg] == "nick"} {
        set_cookie HType "Nick"
        echo "$stars Highlighting is now set to [color error]Nick[color default]."
    } elseif {[arg] == "line"} {
        set_cookie HType "Line"
        echo "$stars Highlighting is now set to [color error]Line[color default]."
    } else {
        echo "$stars Highlighting is currently set to [color error][get_cookie HType][color default]."
    }
    complete
}
alias sHighlight {
    set_cookie Highlight [arg]
    echo "$stars Your highlight string is now set to: [color channel][get_cookie Highlight]"
    complete
}
alias gHighlight {
    echo "$stars Your highlight string is currently set to: [color channel][get_cookie Highlight]"
    complete
}

proc isLinker { s1 } {
    if {[string match "SSn?" $s1] || $s1 == "V|Spy" || [string match "V|Spy?" $s1]} {
        return "@"
    } elseif {$s1 == "ProgTeam" || $s1 == "Prog-Team" || $s1 == "VBBBN" || $s1 == "VBB-BN"} {
        return "%"
    } elseif {[string match "Y?200?" $s1] || [string match "Y?2000Bug" $s1] || $s1 == "Yr2Test"} {
        return "&"
    } elseif {[string match "aa2?a" $s1]} {
        return "·"
    } elseif {$s1 == "DaChair" || $s1 == "\{V\}"} {
        return "•"
    } elseif {$s1 == "Uniserver" || $s1 == "Leviathon" || [string match "NetLink?" $s1] || $s1 == "SaurikLnk" || $s1 == "RawHard" || $s1 == "netrelay" || $s1 == "RawLink" || $s1 == "NL" || [string length $s1] == 1 || $s1 == "NetLink"} {
        return "@"
    } else {
        return ""
    }
}

on ctcp {
    if {[lindex [args] 1] == "ACTION"} {
        set target [lindex [args] 0]
        set from [nick]
        set msg [lindex [args] 2]
        set server ""
        set system ""
        set temp 0
        set code ""
        while {[string match "*?@?*" [lindex $msg 0]] != 0 && [isLinker $from] != ""} {
            set code [isLinker $from]
            set from [lindex $msg 0]
            set temp [string length $from]
            set msg [string range $msg [expr $temp + 1] end]
            if {[string match "<*@*>" $from]} {
                set system "<>"
                set from [string range $from 1 [expr $temp - 2]]
            } elseif {[string match "\[\[\]*@*\]" $from]} {
                set system "\[\]"
                set from [string range $from 1 [expr $temp - 2]]
            } else {
                set system ""
            }
            set temp [instr 0 $from "@"]
            set server [string range $from [expr $temp + 1] end]
            set from [string range $from 0 [expr $temp - 1]]
        }
        unset temp
        set promptlen [get_cookie PromptLength]
        incr promptlen -[string length $from]
        if {$target == [my_nick]} {
            if {[window exists query [nick]] == 0} {/query [nick]}
            set windowtype "query"
            set windowtosendto "[nick]"
        } else {
            set windowtype "channel"
            set windowtosendto "$target"
        }
        if {$server == ""} {
            set isHigh 0
            foreach h [get_cookie Highlight] {
                if {[string match "*$h*" [string tolower $msg]]} {set isHigh 1}
            }
            if {$isHigh} {
                beep
                if {[get_cookie HType] == "Line"} {
                    echo "[color highlight]• $from[color highlight][spaces $promptlen] $msg" channel $target
                } elseif {[get_cookie HType] == "Nick"} {
                    echo "[color highlight]• $from[color default][spaces $promptlen] $msg" channel $target
                }
            } else {
                echo "[color mode]•[color default] [color action]$from[color default][spaces $promptlen] [color action]$msg" channel $target
            }
            unset isHigh
        } else {
            incr promptlen +5
            incr promptlen -[string length $server]
            if {$system == "<>" || $system == "\[\]"} {
                set isHigh 0
                foreach h [get_cookie Highlight] {
                    if {[string match "*$h*" [string tolower $msg]]} {set isHigh 1}
                }
                if {$isHigh} {
                    beep
                    if {[get_cookie HType] == "Line"} {
                        echo "[color highlight]• $from[color highlight]$code$server[color highlight][spaces $promptlen] $msg" $windowtype $windowtosendto
                    } elseif {[get_cookie HType] == "Nick"} {
                        echo "[color highlight]• $from[color highlight]$code$server[color default][spaces $promptlen] $msg" $windowtype $windowtosendto
                    }
                } else {
                    echo "[color mode]•[color default] [color action]$from[color mode]$code[color channel]$server[color default][spaces $promptlen] [color action]$msg[color default]" $windowtype $windowtosendto
                }
                unset isHigh
            } else {
               set linktype [string range $msg 4 7]
                if {$linktype == "join"} {
                    if {[instr 1 $msg " ("] == -1} {
                        echo "[color join]$preN Join[color mode]$code[color channel]$server[color join]: [color nick]$from" $windowtype $windowtosendto
                    } else {
                        echo "[color join]$preN Join[color mode]$code[color channel]$server[color join]: [color nick]$from[color join][string range $msg [instr 1 $msg " ("] end]" $windowtype $windowtosendto
                    }
                } elseif {$linktype == "part"} {
                    if {[instr 1 $msg " ("] == -1} {
                        echo "[color part]$preN Part[color mode]$code[color channel]$server[color part]: [color nick]$from" $windowtype $windowtosendto
                    } else {
                        echo "[color part]$preN Part[color mode]$code[color channel]$server[color part]: [color nick]$from[color part][string range $msg [instr 1 $msg " ("] end]" $windowtype $windowtosendto
                    }
                } elseif {$linktype == "left"} {
                    if {[instr 1 $msg " ("] == -1} {
                        echo "[color part]$preN Part[color mode]$code[color channel]$server[color part]: [color nick]$from"  $windowtype $windowtosendto
                    } else {
                        echo "[color part]$preN Part[color mode]$code[color channel]$server[color part]: [color nick]$from[color part][string range $msg [instr 1 $msg " ("] end]" $windowtype $windowtosendto
                    }
                } elseif {$linktype == "quit"} {
                    echo "[color quit]$preN Quit[color mode]$code[color channel]$server[color quit]: [color nick]$from[color quit] [string range $msg 13 end]"  $windowtype $windowtosendto
                } elseif {$linktype == "Quit"} {
                    echo "[color quit]$preN Quit[color mode]$code[color channel]$server[color quit]: [color nick]$from[color quit] [string range $msg 9 end]"  $windowtype $windowtosendto
                } elseif {$linktype == "ow k"} {
                    echo "[color change]$preN [color nick]$from[color mode]$code[color channel]$server[color change] is now known as [color nick][string range $msg 16 end]" $windowtype $windowtosendto
                } elseif {$linktype == "kick"} {
                    if {[string index $msg 0] == "w"} {
                        set tmpNick ""
                        scan $msg "was kicked by %\[^ \]" tmpNick
                        echo "[color change]$preN [color nick]$from[color mode]$code[color channel]$server[color change] has been kicked from [color channel][lindex [args] 0][color change] by [color nick]$tmpNick[color change] [string range $msg [expr 15 + [string length $tmpNick]] end]" $windowtype $windowtosendto
                        unset tmpNick
                    } else {
                        set tmpNick ""
                        set tmpChan ""
                        scan $msg "has kicked %\[^ \] from %\[^ \]" tmpNick tmpChan
                        echo "[color change]$preN [color nick]$tmpNick[color mode]$code[color channel]$server[color change] has been kicked from [color channel]$tmpChan[color change] by [color nick]$from[color change] [string range $msg [expr 18 + [expr [string length $tmpChan] + [string length $tmpNick]]] end]" $windowtype $windowtosendto
                        unset tmpNick
                        unset tmpChan
                    }
                } elseif {$linktype == "chan"} {
                    echo "[color change]$preN Topic[color mode]$code[color channel]$server[color change] changed to [color mode][string range $msg 26 end][color change] by [color nick]$from" $windowtype $windowtosendto
                } elseif {$linktype == "ges "} {
                    echo "[color change]$preN Topic[color mode]$code[color channel]$server[color change] changed to [color mode][string range $msg 17 end][color change] by [color nick]$from" $windowtype $windowtosendto
                } elseif {$linktype == "set "} {
                    set msg "[string range $msg [expr [instr 8 $msg " "] + 1] end]"
                    if {![string match "\"*\"" $msg]} {
                        set msg "\"$msg\""
                    }
                    echo "[color change]$preN Mode[color mode]$code[color channel]$server[color change] change [color mode]$msg[color change] by [color nick]$from" $windowtype $windowtosendto
                } elseif {$linktype == "mode"} {
                    set msg "[string range $msg [expr [instr 5 $msg " "] + 1] end]"
                    if {![string match "\"*\"" $msg]} {
                        set msg "\"$msg\""
                    }
                    echo "[color change]$preN Mode[color mode]$code[color channel]$server[color change] change [color mode]$msg[color change] by [color nick]$from" $windowtype $windowtosendto
                } else {
                    echo "[color mode]*[color default] [color action]$from[color mode]$code[color channel]$server[color default][spaces $promptlen] [color action]$msg[color default]" $windowtype $windowtosendto
                }
                unset linktype
            }
        }
        unset windowtype
        unset windowtosendto
        unset promptlen
        unset code
        unset system
        unset server
        unset msg
        unset from
        unset target
        complete
    }
}

alias sGet {
    set_cookie GetDir [arg]
    if {[get_cookie GetDir] == ""} then {
        echo "$stars Get functionality is now [color error]Off[color default]."
    } else {
        echo "$stars Your get directory is now set to: [color channel][get_cookie GetDir]"
    }
    complete
}
alias gGet {
    if {[get_cookie GetDir] == ""} then {
        echo "$stars Get functionality is currently [color error]Off[color default]."
    } else {
        echo "$stars Your get directory is currently set to: [color channel][get_cookie GetDir]"
    }
    complete
}
alias sLinkers {
    set_cookie LinkBots "[arg] "
    echo "$stars These are now the linkbots: [color channel]Off[color default]."
    complete
}
alias gLinkers {
    echo "$stars These are the current linkbots: [color channel]Off[color default]."
    complete
}

## GET BACK FAST
on privmsg {
    if {[string match "get *" [lindex [args] 1]]} {if {[get_cookie GetDir] != "" && ![string match "*\\*" [lindex [args] 1]]} {/dcc send [nick] [get_cookie GetDir]\\[string range [lindex [args] 1] 4 end].zip}}
    set target [lindex [args] 0]
    set from [nick]
    set msg [lindex [args] 1]
    set server ""
    set system ""
    set temp 0
    set code ""
    while {[string match "*@*" [string range $msg 0 [expr [string first " " $msg] - 1]]] && [isLinker $from] != ""} {
        set code [isLinker $from]
        set from [string range $msg 0 [expr [string first " " $msg] - 1]]
        set temp [string length $from]
        set msg [string range $msg [expr $temp + 1] end]
        if {[string match "<*@*>" $from]} {
            set system "<>"
            set from [string range $from 1 [expr $temp - 2]]
        } elseif {[string match "\[\[\]*@*\]" $from]} {
            set system "\[\]"
            set from [string range $from 1 [expr $temp - 2]]
        } elseif {[string match "\[*\]*@*\[*\]" $from]} {
            set system "**"
            set from [string range $from 1 [expr $temp - 2]]
        } else {
            set system ""
        }
        set temp [instr 0 $from "@"]
        set server [string range $from [expr $temp + 1] end]
        set from [string range $from 0 [expr $temp - 1]]
    }
    unset temp
    set promptlen [get_cookie PromptLength]
    incr promptlen -[string length $from]
    if {$target == [my_nick]} {
        if {[window exists query [nick]] == 0} {/query [nick]}
        set windowtype "query"
        set windowtosendto "[nick]"
    } else {
        set windowtype "channel"
        set windowtosendto "$target"
    }
    if {$server == "" || $system == ""} {
        set isHigh 0
        foreach h [get_cookie Highlight] {
            if {[string match "*$h*" $[string tolower $msg]]} {set isHigh 1}
        }
        if {$isHigh} {
            beep
            if {[get_cookie HType] == "Line"} {
                echo "[color highlight]<[nick][color highlight]>[spaces $promptlen] [lindex [args] 1]" $windowtype $windowtosendto
            } elseif {[get_cookie HType] == "Nick"} {
                echo "[color highlight]<[nick][color highlight]>[color default][spaces $promptlen] [lindex [args] 1]" $windowtype $windowtosendto
            }
        } else {
            echo "[color mode]<[color error][nick][color mode]>[color default][spaces $promptlen] [lindex [args] 1]" $windowtype $windowtosendto
        }
        unset isHigh
    } else {
       if {$target == [my_nick]} {
            if {[window exists query [nick]] == 0} {/query [nick]}
            set windowtype "query"
            set windowtosendto "[nick]"
        } else {
            set windowtype "channel"
            set windowtosendto "$target"
        }
        incr promptlen +5
        incr promptlen -[string length $server]
        if {$system == "**"} {
            echo "[color mode]*[color error]$from[color mode]$code[color channel]$server[color mode]*[color highlight][spaces $promptlen] $msg" $windowtype $windowtosendto
        } else {
            set isHigh 0
            foreach h [get_cookie Highlight] {
                if {[string match "*$h*" [string tolower $msg]]} {set isHigh 1}
            }
            if {$isHigh} {
                beep
                if {[get_cookie HType] == "Line"} {
                    echo "[color highlight]<$from[color highlight]$code$server[color highlight]>[spaces $promptlen] $msg" $windowtype $windowtosendto
                } elseif {[get_cookie HType] == "Nick"} {
                    echo "[color highlight]<$from[color highlight]$code$server[color highlight]>[color default][spaces $promptlen] $msg" $windowtype $windowtosendto
                }
            } else {
                echo "[color mode]<[color error]$from[color mode]$code[color channel]$server[color mode]>[color default][spaces $promptlen] $msg" $windowtype $windowtosendto
            }
            unset isHigh
        }
    }
    unset windowtype
    unset windowtosendto
    unset promptlen
    unset code
    unset system
    unset server
    unset msg
    unset from
    unset target
    if {[string match "SSn?" [nick]]} {
        if {[lindex [lindex [args] 1] 0] == "OpWith"} {
            /raw PRIVMSG [nick] :[lindex [lindex [args] 1] 4]
        }
        complete
    }
    complete
}

#---------------------------------------------------------------------------
## General Procedures - OLD
#---------------------------------------------------------------------------
proc arg {} {return [join [args]]}
proc stricmp { s1 s2 } {return [string compare [string tolower $s1] [string tolower $s2]]}
proc is_ip_addr { addr } {return [regexp {([0-9]+)\.([0-9]+)\.([0-9]+)\.([0-9]+)} $addr]}
proc longip { ip } {
    global tcl_precision
    set tcl_precision 17
    set result 0
    regexp {([0-9]+)\.([0-9]+)\.([0-9]+)\.([0-9]+)} $ip s b3 b2 b1 b0
    if {![string compare $ip $s]} {
        set total 0
        set total [expr $total + double($b3) * pow(256,3)]
        set total [expr $total + double($b2) * pow(256,2)]
        set total [expr $total + double($b1) * pow(256,1)]
        set total [expr $total + double($b0) * pow(256,0)]
        set result [format "%10.0f" $total]
    }
    return $result
}
proc mask { type mask } {
    set n "*"
    set u "*"
    set a "*"
    scan $mask "%\[^!\]!%\[^@\]@%s" n u a
    set n [string trimleft $n "@+"]
    set u [string trimleft $u "~"]
    set h $a
    set d ""
    if { [is_ip_addr $a] } {
        set a [split $a .]
        set a [lreplace $a end end *]
    } else {
        set a [split $a .]
        if { [llength $a] > 2 } { set a [lreplace $a 0 0 *] }
    }
    set d [join $a .]
    switch "$type" {
        "0" { return "*!$u@$h" }
        "1" { return "*!*$u@$h" }
        "2" { return "*!*@$h" }
        "3" { return "*!*$u@$d" }
        "4" { return "*!*@$d" }
        "5" { return "$n!$u@$h" }
        "6" { return "$n!*$u@$h" }
        "7" { return "$n!*@$h" }
        "8" { return "$n!*$u@$d" }
        "9" { return "$n!*@$d" }
    }
    return "$n!$u@$h"
}
proc am_on_channel { channel } {
    set c [string tolower $channel]
    set n [llength [channels]]
    for { set i 0 } { $i < $n } { incr i } {
        if { [string tolower [lindex [channels] $i]] == $c } {
            return 1
        }
    }
    return 0
}

#---------------------------------------------------------------------------
## General Procedures - NEW
#---------------------------------------------------------------------------

proc normalize {s} {
    set g ""
    for {set i 0} {$i < [string length $s]} {incr i} {
        if {[string index $s $i] == "\\"} { incr i }
        set g $g[string index $s $i]
    }
    return $g
}
 
proc nnick {nick} {
  return [string trimleft $nick "@%+"]
}

proc lrmdups { s1 } {
    set s1 [lsort $s1]
    for { set i 0 } { $i < [llength $s1] } { incr i } {
        while { [lindex $s1 [expr $i + 1]] == [lindex $s1 $i] } {
            set s1 [lreplace $s1 $i [expr $i + 1] [lindex $s1 $i]]
        }
    }
    return $s1
}

proc spaces { i1 } {
    set tempSpace ""
    for { set i 0 } { $i < $i1 } { incr i } {set tempSpace " $tempSpace"}
    return $tempSpace
}
proc instr { i1 s1 s2 } {
    for {set i $i1} {$i < [string length $s1]} {incr i +1} {
        set f $i
        incr f +[string length $s2]
        incr f -1
        if {[string range $s1 $i $f] == $s2} {return $i}
    }
    return -1
}
proc lecho { s1 } {
    echo $s1 status
    if {[window_type] != "status"} {
        echo $s1
    }
}

#---------------------------------------------------------------------------
## Simple Aliases
#---------------------------------------------------------------------------
alias noq  {foreach wind [queries *] { window close query $wind };complete}
alias m    {/msg  [arg];complete}
alias j    {/join [arg];complete}
alias p    {/part [arg];complete}
alias op   {/mode [channel] +oooo [arg];complete}
alias deop {/mode [channel] -oooo [arg];complete}
alias ping {/ctcp [arg] ping;complete}
alias amsg {foreach c [channels] {/msg $c [arg]};complete}
alias ame  {foreach c [channels] {/ctcp $c ACTION [arg];echo "[color nick]* [my_nick][color action] [arg]" channel $c};complete}
alias onotice {
    set ops [nicks [channel] @*]
    set list ""
    if { [llength $ops] } {
        echo "[color notice]Ops-> \002(ops/\002[channel]\002)\002 [arg]"
        foreach op $ops {
            if { $op != "@[my_nick]" } {
                lappend list [string trimleft $op @]
                if { ![expr [llength $list] % 10] } {
                    set list [join $list ,]
                    /raw notice $list :\002(ops/\002[channel]\002)\002 [arg]
                    set list ""
                }
            }
        }
        if { [llength $list] } {
            set list [join $list ,]
            echo $list
            /raw notice $list :(\002ops/\002[channel]\002)\002 [arg]
        }
    }
    complete
}
alias run {if { [catch { exec [arg] & } msg] } {echo "$preN $msg"};complete}
alias clearall {
    window clear status
    foreach w [channels] {window clear channel $w}
    foreach w [queries]  {window clear query $w}
    foreach w [chats]    {window clear chat $w}
    complete
}
alias scripts {
    echo "Loaded scripts:"
    set i 0
    foreach s [scripts] {
        set pos1 [string last "\\" $s]
        set pos2 [string last "." $s]
        set final "[color nick]\[[color error][incr i][color nick]\] [color default][string range $s 0 [expr $pos1 - 1]][color error]\\[color mode]"
        if {$pos1 < $pos2} {
            set final "$final[string range $s [expr $pos1 + 1] [expr $pos2 - 1]][color error].[color default][string range $s [expr $pos2 + 1] end]"
        } else {
            set final "$final[string range $s [expr $pos1 + 1] end"
        }
        echo $final
    }
    complete
}
alias tcl {
    if { [catch { eval [arg] } msg] } {
        echo "Error executing TCL command \"[join [args]]\" : "
        echo $msg
    }
    complete
}
alias splay {
    if { [llength [args]] } {
        if { ![stricmp [arg] "stop"] || ![stricmp [arg] "off"] } {
            mmstop all
        } else {
            set msg [mmplay [arg]]
            if { [string length $msg] } { echo "[color error]$preN $msg" }
        }
    } else {
        echo "$preN USAGE: /[event] <filename>"
    }
    complete
}
alias beep {beep;complete}

#---------------------------------------------------------------------------
# Ban Alias
#---------------------------------------------------------------------------
set ban_nicks ""
set ban_chans ""
set ban_types ""
alias ban {
    set chan [string tolower [channel]]
    set nick [string tolower [lindex [args] 0]]
    set type [lindex [args] 1]
    if { [am_on_channel $nick] } {
        set chan $nick
        set nick [string tolower [lindex [args] 1]]
        set type [lindex [args] 2]
    }
    if { ![string length $type] } { set type "5" }
    if { $nick != "" && $chan != "" } {
        lappend ban_nicks $nick
        lappend ban_chans $chan
        lappend ban_types $type
        whois $nick
    }
    complete
}
on whois {
    beep
    set nick [string tolower [nick]]
    set i [lsearch $ban_nicks $nick]
    while { $i > -1 } {
        set chan [lindex $ban_chans $i]
        set type [lindex $ban_types $i]
        set ban_nicks [lreplace $ban_nicks $i $i]
        set ban_chans [lreplace $ban_chans $i $i]
        set ban_types [lreplace $ban_types $i $i]
        /mode $chan +b [mask $type [nick]![user]@[host]]
        set i [lsearch $ban_nicks $nick]
    }
}
on 401 {
    set nick [string tolower [lindex [args] 1]]
    set i [lsearch $ban_nicks $nick]
    while { $i > -1 } {
        set ban_nicks [lreplace $ban_nicks $i $i]
        set ban_chans [lreplace $ban_chans $i $i]
        set ban_types [lreplace $ban_types $i $i]
        set i [lsearch $ban_nicks $nick]
    }
}

#---------------------------------------------------------------------------
# UserList-Related Aliases
#---------------------------------------------------------------------------
alias auser {
    set u [lindex [args] 0]
    if { [string length $u] } {
        set i [add_user [mask X $u]]
        echo "$preN Added [get_user $i mask] at position $i" status
    } else {echo "$preN USAGE: /[event] <mask>"}
    complete
}
alias duser {
    set u [lindex [args] 0]
    if { [string length $u] } {
        set i [find_user [mask X $u]]
        if { $i >= 0 } {
            set mask [get_user $i mask]
            delete_user $i
            echo "$preN Deleted $mask"
        } else {
            echo "$preN No users found matching [mask X $u]"
        }
    } else {echo "$preN USAGE: /[event] <mask>"}
    complete
}

#---------------------------------------------------------------------------
# Domain Name Resolution Alias (DNS)
#---------------------------------------------------------------------------
set dns_nicks ""
alias dns {
    set host [arg]
    if { [string match *.* $host] } {
        lookup $host
        echo "$preN Attempting to resolve $host" status
    } else {
        lappend dns_nicks [string tolower $host]
        whois $host
        echo "$preN Retrieving information for $host" status
    }
    complete
}
on lookup {
    set request [lindex [args] 0]
    set result  [lindex [args] 1]
    if { [string length $result] } {
        lecho "$preN Resolved $request to $result"
    } else {
        lecho "[color error]$preN ERROR: Could not resolve $request"
    }
}
on whois {
    beep
    set nick [string tolower [nick]]
    set i [lsearch $dns_nicks $nick]
    while { $i > -1 } {
        lookup [host]
        echo "$preN Attempting to resolve [host]" status
        set dns_nicks [lreplace $dns_nicks $i $i]
        set i [lsearch $dns_nicks $nick]
    }
}
on 401 {
    set nick [lindex [args] 1]
    set i [lsearch $dns_nicks $nick]
    while { $i > -1 } {
        set dns_nicks [lreplace $dns_nicks $i $i]
        set i [lsearch $dns_nicks $nick]
    }
}

#---------------------------------------------------------------------------
# Timer Alias
#---------------------------------------------------------------------------
set timers(0) ""
unset timers(0)
alias timer* {
    set id [string range [lindex [args] 0] 5 end]
    set args [lrange [args] 1 end]
    if { ![string length $id] || ![string compare [string tolower $id] s] } {
        set id 0
    }
    set off 0
    set beg 0
    set i [lsearch $args -o]
    if { $i < 0 } { set i [lsearch $args -O] }
    if { $i >= 0 } {
        set off 1
        set args [lreplace $args $i $i]
    }
    set i [lsearch $args *:*]
    if { $i >= 0 } {
        catch { set beg [clock scan [lindex $args $i]] }
        set args [lreplace $args $i $i]
    }
    set rep [lindex $args 0]
    set int [lindex $args 1]
    set cmd [join [lrange $args 2 end]]
    if { ![string compare [string tolower $rep] off] } {
        kill_timer $id
    } elseif { [llength $args] >= 3 } {
        make_timer $id $off $beg $rep $int $cmd
    } else {
        show_timers
    }
    complete
}
on timer {
    set names [array names timers]
    foreach id $names {process_timer $id}
}
proc make_timer { id off beg rep int cmd } {
    global timers
    global preN
    if { ![string compare $id 0] } { set id [get_timer_id] }
    set timers($id) [list $off $beg $rep $int $cmd 0 0]
    echo "$preN Timer$id activated"
}
proc kill_timer { id } {
    global timers
    global preN
    if { [catch { unset timers($id) }] } {echo "$preN Timer$id is not active"} else {echo "$preN Timer$id halted"}
}
proc show_timers { } {
    global timers
    global preN
    set names [array names timers]
    set count 0
    foreach id $names {
        set t $timers($id)
        set o "$preN Timer$id"
        if { [lindex $t 1] > 0 } {append o " [clock format [lindex $t 1] -format %H:%M:%S]"}
        append o " [lindex $t 2]x"
        append o " [lindex $t 3]s"
        append o " [lindex $t 4]"
        echo $o
        incr count
    }
    if { !$count } { echo "$preN No active timers" }
}
proc get_timer_id { } {
    global timers
    set i 1
    set names [array names timers]
    while { [lsearch $names $i] >= 0 } { incr i }
    return $i
}
proc process_timer { id } {
    global timers
    set ct  [clock seconds]
    set off [lindex $timers($id) 0]
    set beg [lindex $timers($id) 1]
    set rep [lindex $timers($id) 2]
    set int [lindex $timers($id) 3]
    set cmd [lindex $timers($id) 4]
    set lr  [lindex $timers($id) 5]
    set lt  [lindex $timers($id) 6]
    if { ![connected] && !$off }   {return}
    if { $ct < $beg }              {return}
    if { $ct < [expr $lt + $int] } {return}
    set beg 0
    incr lr
    set lt $ct
    eval $cmd
    if { $lr == $rep } {
        kill_timer $id
        return
    }
    set timers($id) [list $off $beg $rep $int $cmd $lr $lt]
}

#---------------------------------------------------------------------------
# Notify Events
#---------------------------------------------------------------------------
on notify {
    set msg ""
    append msg "[color notice]$preN [color nick][nick] ([user]@[host])"
    append msg "[color notice] is on IRC"
    echo $msg status
}
on denotify {
    set msg ""
    append msg "[color notice]$preN [color nick][nick] ([user]@[host])"
    append msg "[color notice] has left IRC"
    echo $msg status
}

#---------------------------------------------------------------------------
## Hotkeys
#---------------------------------------------------------------------------
hotkey alt+1 {window focus status;status_page 0;complete}
hotkey alt+2 {window focus status;status_page 1;complete}
hotkey alt+3 {window focus status;status_page 2;complete}
hotkey alt+4 {window focus status;status_page 3;complete}
hotkey alt+5 {window focus status;status_page 4;complete}

proc modetogg {chan mode} {
    if {[string match "*$mode*" [lindex [mode $chan] 0]]} {
        /mode $chan -$mode
    } else {
        /mode $chan +$mode
    }
}
hotkey alt+i {modetogg [channel] i}
hotkey alt+n {modetogg [channel] n}
hotkey alt+s {modetogg [channel] s}
hotkey alt+t {modetogg [channel] t}
hotkey alt+p {modetogg [channel] p}
hotkey alt+m {modetogg [channel] m}
hotkey alt+k {modetogg [channel] k}
hotkey alt+l {modetogg [channel] l}

hotkey F12 {
    set chan [prompt "Join" "Enter the name of the channel you wish to join:" ""]
    if {$chan == ""} {
        complete
    } else {
        if {[string range $chan 0 0] == "#" || [string range $chan 0 0] == "&"} {
            /join $chan
        } else {
            /join #$chan
        }
    }
}
hotkey control+i {
    if {[info exists lastInvite]} {
        /join $lastInvite
    } else {
        echo "[color error]$preN[color default] You need to be invited to a channel first..."
    }
    complete
}
hotkey control+t {/timestamp toggle;complete}
hotkey control+y {/raw part [channel];/raw join [channel];complete}
hotkey control+s {
    set lcb [input get_text]
    set lcv [input get_sel_start]
    set lct [input get_sel_length]
    set lcb "[string range $lcb 0 [expr $lcv - 1]] [string range $lcb [expr $lcv + $lct] end]"

    input set_text $lcb

    input set_sel_start [expr $lcv + 1]
    unset lct
    unset lcv
    unset lcb
    complete
}
hotkey control+g {
    set lcb [input get_text]
    set lcv [input get_sel_start]
    set lct [input get_sel_length]
    set lcb "[string range $lcb 0 [expr $lcv - 1]][string range $lcb [expr $lcv + $lct] end]"

    input set_text $lcb

    input set_sel_start [expr $lcv + 1]
    unset lct
    unset lcv
    unset lcb
    complete
}
hotkey control+d {
    set lcb [input get_text]
    set lcv [input get_sel_start]
    set lct [input get_sel_length]
    set lcb "[string range $lcb 0 [expr $lcv - 1]]¯[string range $lcb [expr $lcv + $lct] end]"

    input set_text $lcb

    input set_sel_start [expr $lcv + 1]
    unset lct
    unset lcv
    unset lcb
    complete
}
hotkey control+q {
    set lcb [input get_text]
    set lcv [input get_sel_start]
    set lct [input get_sel_length]
    set lcb "[string range $lcb 0 [expr $lcv - 1]]¿[string range $lcb [expr $lcv + $lct] end]"

    input set_text $lcb

    input set_sel_start [expr $lcv + 1]
    unset lct
    unset lcv
    unset lcb
    complete
}
hotkey control+m {
    set lcb [input get_text]
    set lcv [input get_sel_start]
    set lct [input get_sel_length]
    set lcb "[string range $lcb 0 [expr $lcv - 1]]¸[string range $lcb [expr $lcv + $lct] end]"

    input set_text $lcb

    input set_sel_start [expr $lcv + 1]
    unset lct
    unset lcv
    unset lcb
    complete
}

#---------------------------------------------------------------------------
## Menu: Status Popup
#---------------------------------------------------------------------------
menu status "&Join #saurik" {/join #saurik}
menu status "*&Join #vbbbn"  {/join #vbbbn}
menu status "&Join #XiRCON" {/join #XiRCON}
menu status ""
menu status "&Settings->&General" {/dialog general}
menu status "&Settings->&Users"   {/dialog users}
menu status ""
menu status "&MOTD"          {/motd}
menu status ""
menu status "&Away"          {/away "since [clock format [clock seconds]]"}
menu status "&Back"          {/away}

#---------------------------------------------------------------------------
## Menu: Channel Popup
#---------------------------------------------------------------------------
menu channel "*&Properties"        {show_props channel [arg]}
menu channel ""
menu channel "&Settings->&General" {/dialog general}
menu channel "&Settings->&Users"   {/dialog users}

#---------------------------------------------------------------------------
## Menu: Query Popup
#---------------------------------------------------------------------------
menu query "*&Whois"  {/whois [arg]}
menu query ""
menu query "&Ping"    {/ctcp [arg] ping}
menu query "&Version" {/ctcp [arg] version}
menu query "&Time"    {/ctcp [arg] time}
menu query ""
menu query "&Finger"  {/finger [arg]}
menu query ""
menu query "&Settings->&General" {/dialog general}
menu query "&Settings->&Users"   {/dialog users}

#---------------------------------------------------------------------------
## Menu: Chat Popup
#---------------------------------------------------------------------------
menu chat "*&Whois"   {/whois [arg]}
menu chat ""
menu chat "&Ping"     {/ctcp [arg] ping}
menu chat "&Version"  {/ctcp [arg] version}
menu chat "&Time"     {/ctcp [arg] time}
menu chat ""
menu chat "&Finger"   {/finger [arg]}

#---------------------------------------------------------------------------
## Menu: Users Popup
#---------------------------------------------------------------------------
menu users "&Whois"     {foreach a [args] {/whois $a}}
menu users "*&Query"    {foreach a [args] {/query $a}}
menu users "DCC &Chat"  {foreach a [args] {/dcc chat $a}}
menu users "CTCP C&hat"  {foreach a [args] {/ctcp $a chat}}
menu users ""
menu users "&Ping"      {foreach a [args] {/ctcp $a ping}}
menu users "Ping&1 (Echo)" {foreach a [args] {/ctcp $a echo [clock seconds]}}
menu users "Ping&2 (ErrMsg)" {foreach a [args] {/ctcp $a errmsg [clock seconds]}}
menu users "Ping&3 (ClientInfo)" {foreach a [args] {/ctcp $a clientinfo [clock seconds]}}
menu users "Ping&4 (EMail)" {foreach a [args] {/msg $a EMAIL [clock seconds]}}
menu users "&Version"   {foreach a [args] {/ctcp $a version}}
menu users "&Time"      {foreach a [args] {/ctcp $a time}}
menu users "T&rout"      {foreach a [args] {/ctcp $a trout}}
menu users ""
menu users "&Op"      {for { set i 0 } { $i < [llength [args]] } { incr i +4 } {/mode [channel] +oooo [join [lrange [args] $i [expr $i + 3]]]}}
menu users "&DeOp"    {for { set i 0 } { $i < [llength [args]] } { incr i +4 } {/mode [channel] -oooo [join [lrange [args] $i [expr $i + 3]]]}}
menu users ""
menu users "&Voice"   {for { set i 0 } { $i < [llength [args]] } { incr i +4 } {/mode [channel] +vvvv [join [lrange [args] $i [expr $i + 3]]]}}
menu users "&UnVoice" {for { set i 0 } { $i < [llength [args]] } { incr i +4 } {/mode [channel] -vvvv [join [lrange [args] $i [expr $i + 3]]]}}
menu users ""
menu users "&Control->&Op6"      {for { set i 0 } { $i < [llength [args]] } { incr i +6 } {/mode [channel] +oooooo [join [lrange [args] $i [expr $i + 5]]]}}
menu users "&Control->&DeOp6"    {for { set i 0 } { $i < [llength [args]] } { incr i +6 } {/mode [channel] -oooooo [join [lrange [args] $i [expr $i + 5]]]}}
menu users "&Control->"
menu users "&Control->&Voice6"   {for { set i 0 } { $i < [llength [args]] } { incr i +6 } {/mode [channel] +vvvvvv [join [lrange [args] $i [expr $i + 5]]]}}
menu users "&Control->&UnVoice6" {for { set i 0 } { $i < [llength [args]] } { incr i +6 } {/mode [channel] -vvvvvv [join [lrange [args] $i [expr $i + 5]]]}}
menu users ""
menu users "&Kick!"     {foreach a [args] {/kick $a}}
menu users "&Ban!"      {foreach a [args] {/ban  $a}}
