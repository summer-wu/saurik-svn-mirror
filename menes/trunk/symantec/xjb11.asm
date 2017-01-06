#!/bin/bash

dWeak=12
dPoke=72
dTravel=84

iGoto=99

opNop=0x0000
opCall=0x8004
opRet=0x0005
opJmp=0x8006
opJne=0x800C
opTest=0xA018
opTravel=0x801B

z=r0

a0=r1
a1=r2

l0=r3
l1=r4
t0=r5
t1=r6

pAttack=r7
pCheckAll=r8
pCheckCode=r9
pCheckData=r10
pDamage=r11

x=r12
y=r13

vReturn=3096

vLocal0=3100
vToxicLocal0=3100
vCollectLocal0=3132

vLocal1=3172
vToxicLocal1=3172
vCollectLocal1=3204

vLocal2=3244
vToxicLocal2=3244
vCollectLocal2=3276

vLocalEnd=3316

vToxicRemote=3400
vCollectRemote=3432

vGoto=3576
vStatus=3578
dDirection=3579

sStart=0
sSweep=1
sDrone=2
sReady=3
sWaiting=4
sLining=5
sClear=6

cUp=0
cDown=1
cRight=2
cLeft=3

label_=0
function For {
    label0=$(./ppid)-$((label_++))
    label1=$(./ppid)-$((label_++))
    label2=$(./ppid)-$((label_++))

    if [[ $# -eq 5 ]]; then
        step="$4"
        code="$5"
    else
        step=1
        code="$4"
    fi

    cat <<EOF
        mov $1, $2
    label-$label0:
        $(echo "$code" | sed -re '
            s/\bBreak\b/jmp label-'"$label2"'/g
            s/\bContinue\b/jmp label-'"$label1"'/g
        ')
    label-$label1:
        add $1, $step
        cmp $1, $3
        jl label-$label0
    label-$label2:
EOF
}

function Poke {
    cat <<EOF
    mov r0, $1
    poke $2, $3
EOF
}

function Peek {
    cat <<EOF
    mov $1, $2
    peek $1, $3
EOF
}

function Fail {
    echo "FAILURE: $1" 1>&2
    exit 1
}

function While_ {
    label=$(./ppid)-$((label_++))
    
    cat <<EOF
    label-$label:
        $2
        $1 label-$label
EOF
}

function If_ {
    label0=$(./ppid)-$((label_++))
    label1=$(./ppid)-$((label_++))

    if [[ -z $3 ]]; then
        cat <<EOF
            $1 label-$label0
            $2
        label-$label0:
EOF
    else
        cat <<EOF
            $1 label-$label0
            $2
            jmp label-$label1
        label-$label0:
            $3
        label-$label1:
EOF
    fi
}

function If {
    cat <<EOF
        cmp $1, $3
EOF

    If_ $(case $2 in
        (==) echo "jne";;
        (!=) echo "je";;
        (\>=) echo "jl";;
        (\<=) echo "jg";;
        (\>) echo "jle";;
        (\<) echo "jge";;
        (*) Fail "$2";;
    esac) "$4" "$5"
}

function Proc {
    cat <<EOF
    $1:
        $2
        ret
EOF
}

function Forever {
    While_ jmp "$1"
}

function Goto {
    cat <<EOF
        cmp [$vGoto], 0
        jne transfer
EOF
}

function Nop {
    for ((i=0; i != $1; ++i)); do
        echo "nop"
    done
}

function filter {
    sed -re 's/([A-Za-z0-9_])-([A-Za-z0-9_])/\1_\2/g' | grep -v '^ *$'
}

function Dissassemble {
    {
        echo "info: XJB11, Jay Freeman, saurik@cs.ucsb.edu, 805 895-7209, UCSB"
        echo "$1" 
    } | filter | contest06 -z:/proc/self/fd/0 |
        tail +2 |
        grep -v '(0000 0000 0000)' |
        sed -e '
            s/^.*(//
            s/)$//
        '
    exit 0
}

function Compare {
    cat << EOF
        $(If "[$l0]" == "[$l0 + $(($1 * 1))]" "
            $(If "[$l0]" != "[$l0 + $(($1 * 2))]" "
                mov $pDamage, 1
                mov [$l0 + $(($1 * 2))], [$l0]
            ")
        " "
            mov $pDamage, 1

            $(If "[$l0]" == "[$l0 + $(($1 * 2))]" "
                mov [$l0 + $1], [$l0]
            " "
                mov [$l0], [$l0 + $1]
            ")
        ")
EOF
}

cat <<EOF | filter
    jmp main
    nop
    nop
    nop

    nop

dronity:
    mov [$vStatus], $sDrone

dronify:
    $(Forever "
        $(While_ js "
            travel $cLeft
        ")

        $(While_ jns "
            getxy $x, $y
            cmp $x, 0
            je downify

            travel $cUp
            travel $cLeft
        ")
    ")

downify:
    mov r0, $cRight

    $(For $l0 0 20 "
        travel $cDown
        poke $cDown, $dDirection
    ")

    $(While_ jl "
        travel $cDown
        poke $cDown, $dDirection

        nop ; byte 72

        getxy $x, $y
        cmp $y, 39
        je readify

        nop ; byte 84
        nop ; byte 87

        cmp [$vStatus], $sReady
    ")

readify:
    cmp $y, 10
    je waitify

    $(While_ jl "
        $(While_ jns "
            $(Peek $t0 $cUp $vStatus)
        ")

        cmp $t0, $sDrone
    ")

    jne waitify
    $(Poke $sReady $cUp $vStatus)

waitify:
    mov [$vStatus], $sWaiting

downity:
    jmp downity

dedronity:

$(Proc attackify "
protectity:
    ret ;&: nop

    mov $a1, r0
    mov r0, 0

    poke $a0, $dPoke
    poke $a0, $((dWeak + 1))

    $(If_ jns "
        $(Peek $t0 $a0 0)
        $(If $t0 == $opCall "
            poke $a0, 0

            poke $a0, $((dTravel + 3))
            $(For $l0 0 70)

            poke $a0, $vGoto
            $(For $l0 dronity dedronity "
                $(Poke "[$l0]" $a0 $l0)
            ")

            $(Poke 0 $a0 $dWeak)
        ")
    ")

    mov r0, $a1
")

stage1:

main:

alignify:
    getxy $x, $y
    cmp $x, 69
    je siddle-up

    $(Forever "
        $(While_ js "
            travel $cRight
            js alignify

            travel $cDown
        ")

        getxy $x, $y
        cmp $y, 39
        je siddle-left

        $(While_ jns "
            travel $cLeft
            $(If_ js "
                $(Poke $cUp $cLeft $dDirection)
            ")

            travel $cDown
        ")
    ")

siddle-left:
    mov r0, $cUp

    $(Forever "
        getxy $x, $y
        cmp $x, 69
        je siddle-up

        $(Forever "
            $(Goto)

            travel $cRight
            js siddle-left

            poke $cRight, $dDirection
        ")
    ")

siddle-up:
    mov r0, $cLeft

    $(Forever "
        getxy $x, $y
        cmp $y, 0
        je synchrofy

        $(Forever "
            $(Goto)

            travel $cUp
            js siddle-up

            poke $cUp, $dDirection
        ")
    ")

synchrofy:
    mov [$vStatus], $sSweep

    getxy $x, $y
    cmp $y, 39
    je sweepify-39

    mov $t0, $cDown
    $(While_ jns "
        peek $t0, 0
    ")

    $(If $t0 == $opCall "
        $(Poke $cLeft $cDown $dDirection)
        jmp synchrofy
    ")

    $(Poke synchrofy $cDown $vGoto)

    jmp sweepify

clearify:
    $(Poke clearify $cLeft $vGoto)
    mov [$vStatus], $sReady
    jmp dronify

sweepify-39:
    $(Poke clearify $cLeft $vGoto)

sweepify:
    mov r0, $cLeft
    mov $a0, $cLeft

    mov $pAttack, $dDirection

sweepity:
    $(Forever "
        $(Forever "
            $(While_ jns "
                $(Peek $t0 $cDown 0)
                jns nextity

                poke $cLeft, $pAttack
            ")

            call attackify
        ")

    nextity:
        $(Forever "
            $(While_ jns "
                travel $cLeft
                js stopify

            pokity:
                poke $cLeft, $pAttack
            ")

            call attackify
        ")

    stopify:
        getxy $x, $y
        $(If $x == 4 "
            mov r0, 0
            mov $pAttack, $dTravel
            mov [$protectity], $opNop
        " "$(If $x == 1 "
            $(Forever "
                $(While_ jns "
                    $(If $y \< 10 "
                        cmp [$vStatus], $sLining
                        je liningify
                    ")

                    $(Peek $t0 $cLeft $vStatus)
                ")
                
                cmp $t0, $sWaiting
                je reprogramify

                poke $cLeft, $pAttack
                call attackify
            ")
        ")")

        cmp $y, 0
        je sweepity

        $(Peek $t0 $cUp $vStatus)
        jns stopify

        cmp $t0, 1
        jne stopify
    ")

reprogramify:
    $(If $y \>= 10 "
        linearity:
            $(Peek $t0 $cUp $vStatus)
            jns linearity

            cmp $t0, $sSweep
            jne linearity

            $(Poke $sLining $cUp $vStatus)
            jns linearity
    ")

    $(For $l0 $stage2 $LB "
        $(Poke "[$l0]" $cLeft $l0)
        add $l0, 1
        $(Poke "[$l0]" $cLeft $l0)
        add $l0, 1
        $(Poke "[$l0]" $cLeft $l0)
        add $l0, 1
        $(Poke "[$l0]" $cLeft $l0)
        add $l0, 1
        $(Poke "[$l0]" $cLeft $l0)
    ")

    $(Poke $((linify - downity)) $cLeft $((downity + 1)))

    jmp linify

stage2:
    $(Proc transfer "
        push [$vGoto]
        mov [$vGoto], 0
    ")

liningify:
    $(If $y != 0 "
        $(While_ jns "
            $(Poke $sLining $cUp $vStatus)
        ")
    ")

linify:
    charge $cRight, 200

    $(If "[0]" != 0 "
        $(Forever "
            $(While_ js "
                travel $cUp
            ")

            $(While_ jns "
                getxy $x, $y
                $(If $y == 0 "
                    jmp chargify
                ")

                travel $cRight
                travel $cUp
            ")
        ")

        $(For $l0 0 5 "
            charge $cLeft, 100
        ")
    " "$(If $y == 10 "
        $(For $l0 0 9 "
            travel $cUp
        ")

        $(For $l0 0 2 "
            $(While_ jns "
                travel $cRight
            ")

            $(Poke kickify $cUp $vGoto)
        ")

        $(While_ jns "
            travel $cUp
        ")

        energy $t0
        sub $t0, 10
        charge $cRight, $t0
    " "
        $(For $l0 0 10 "
            $(While_ jns "
                travel $cUp
            ")
        ")

        mov $l0, 0

    snarcify:
        mov $t0, $cRight
        mov $t1, $cDown

    snarfify:
        $(While_ jne "
            $(While_ jns "
                charge $t0, 1000
                $(If_ jns "
                    $(Nop 3)
                ")

                travel $t0
            ")

            eat

            getxy $x, $y
            mod $x, 69
            cmp $x, 0
        ")

        $(If $l0 == 0 "
            $(For $l1 0 20 "
                $(While_ jns "
                    travel $t1
                ")
            ")

            $(If $t0 == $cRight "
                mov $t0, $cLeft
                mov $t1, $cUp
                jmp snarfify
            ")

            mov $l0, 1
            jmp snarcify
        ")

        getxy $x, $y
        $(If $y != 19 "
            $(For $t0 $y 19 "
                $(While_ jns "
                    travel $cLeft
                ")
            ")
        ")

        $(For $t0 0 $y "
            $(While_ jns "
                travel $cUp
            ")
        ")
    ")")

    jmp chargify

kickify:
    $(While_ jns "
        travel $cLeft
    ")

    $(For $l0 0 20 "
        energy $t0
        sub $t0, 10
        charge $cLeft, $t0
    ")

chargify:
    getxy $x, $y

    $(Forever "
        energy $t0
        $(If $t0 \> 10000 "
            charge $cLeft, 3000
            $(If $x == 0 "
                jmp chargedify
            ")
        ")

        $(Goto)
    ")

chargedify:
    $(Poke chargedify $cRight $vGoto)

    $(For $l0 $repairable $LB "
        mov [$l0 + $(((LB - repairable) * 1))], [$l0]
        mov [$l0 + $(((LB - repairable) * 2))], [$l0]
    ")

    $(For $l0 0 32 "
        mov [$l0 + $vToxicLocal0], 0
        mov [$l0 + $vToxicLocal1], 0
        mov [$l0 + $vToxicLocal2], 0
    ")

    $(For $l0 0 40 "
        mov [$l0 + $vCollectLocal0], 0
        mov [$l0 + $vCollectLocal1], 0
        mov [$l0 + $vCollectLocal2], 0
    ")

    $(For $l0 $((vReturn - 70 * 3)) $vReturn 3 "
        mov [$l0], 0
    ")

    mov [$vReturn], $opRet

    jmp scanify

repairable:

stage3:

safe:
    eat
$(Proc repair "
    mov $t1, $repairable
    cksum $t1, $LB

    $(If $t1 != $pCheckCode "
        $(For $l0 $repairable $LB "
            $(Compare $((LB - repairable)))
        ")
    ")

    mov $t1, $vLocal0
    cksum $t1, $vLocalEnd

    $(If $t1 != $pCheckData "
        $(For $l0 $vLocal0 $vLocalEnd "
            $(Compare $((32 + 40)))
        ")
    ")
")

scanify:
    call sync

    mov $pCheckAll, 0
    cksum $pCheckAll, 3600

    mov $pCheckCode, $repairable
    cksum $pCheckCode, $LB

    mov $pCheckData, $vLocal0
    cksum $pCheckData, $vLocalEnd

    jmp innify

    $(While_ js "
        sense $t0
        $(If_ jns "
            $(If $t0 == 65535 "
                mov [$t0 + $vCollectLocal0], 1
                mov [$t0 + $vCollectLocal1], 1
                mov [$t0 + $vCollectLocal2], 1
            " "
                mov $pDamage, 0
                call safe

                $(If $pDamage == 0 "
                    mov $t1, 0
                    cksum $t1, 3600

                    $(If $pCheckAll != $t1 "
                        mov $pDamage, 1
                        mov $pCheckAll, $t1
                    ")
                ")

                $(If $pDamage != 0 "
                    add [$t0 + $vToxicLocal0], 1
                    add [$t0 + $vToxicLocal1], 1
                    add [$t0 + $vToxicLocal2], 1
                ")
            ")

            mov $pCheckData, $vLocal0
            cksum $pCheckData, $vLocalEnd
        ")

    innify:
        travel $cDown
    ")

    call sync

    $(For $l0 0 32 "
        mov [$l0 + $vToxicRemote], 0
    ")

    $(For $l0 0 40 "
        mov [$l0 + $vCollectRemote], 0
    ")

    mov [$vStatus], $sClear

    getxy $x, $y
    cmp $x, 0
sharity:
    jne sharity

sharify:
    $(While_ jns "
        $(Peek $t0 $cRight $vStatus)
    ")

    cmp $t0, $sClear
    jne sharify

    $(For $l0 0 32 "
        add [$l0 + $vToxicLocal0], [$l0 + $vToxicRemote]
    ")

    $(For $l0 0 32 "
        add [$l0 + $vCollectLocal0], [$l0 + $vCollectRemote]
    ")

    $(For $l0 $vToxicRemote $((vToxicRemote + 32)) "
        $(Poke "[$l0 - $((vToxicRemote - vToxicLocal0))]" $cRight $l0)
    ")

    $(For $l0 $vCollectRemote $((vCollectRemote + 40)) "
        $(Poke "[$l0 - $((vCollectRemote - vCollectLocal0))]" $cRight $l0)
    ")

    $(Poke $((sharify - sharity)) $cRight $((sharity + 1)))

    cmp $x, 69
collectity:
    jne collectity

collectify:
    $(For $l0 $vToxicLocal0 $((vToxicLocal0 + 32)) "
        $(Poke "[$l0]" $cRight $l0)
    ")

    $(For $l0 $vCollectLocal0 $((vCollectLocal0 + 32)) "
        $(Poke "[$l0]" $cRight $l0)
    ")

    $(Poke $((collectify - collectity)) $cRight $((collectity + 1)))

    jmp haltify

stage4:

$(Proc sync "
    mov r0, $((syncify1 - syncity1))

    getxy $x, $y
    cmp $x, 0
syncity1:
    jne syncity1

syncify1:
    mov [$((syncity1 + 1))], 0

    $(If $x != 69 "
        $(While_ jns "
            poke $cRight, $((syncity1 + 1))
        ")
    ")

sync_:
    mov r0, $((syncify2 - syncity2))

    cmp $x, 69
    je syncify2

syncity2:
    jmp syncity2

syncify2:
    poke $cLeft, $((syncity2 + 1))
    mov [$((syncity2 + 1))], 0

    mov $t0, $vReturn
    mult $x, 3
    sub $t0, $x
    push $t0
    
    ;mult $x, 3
    ;add $x, $((vReturn - 70 * 3))
    ;push $x
")

haltify:
    travel $cUp
    travel $cDown
    jmp haltify

    energy $t0
    sub $t0, 10
    release $t0
    jmp haltify

;call grandmother!
EOF
