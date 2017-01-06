info: XJB11, Jay Freeman, saurik@cs.ucsb.edu, 805 895-7209, UCSB
    jmp main
    nop
    nop
    nop
    nop
dronity:
    mov [3578], 2
dronify:
        label_30833_0:
            label_30825_0:
            travel 3
        js label_30825_0
            label_30830_0:
            getxy r12, r13
            cmp r12, 0
            je downify
            travel 0
            travel 3
        jns label_30830_0
        jmp label_30833_0
downify:
    mov r0, 2
            mov r3, 0
    label_30837_0:
        travel 1
        poke 1, 3579
    label_30839_1:
        add r3, 1
        cmp r3, 20
        jl label_30837_0
    label_30841_2:
        label_30848_0:
        travel 1
        poke 1, 3579
        nop ; byte 72
        getxy r12, r13
        cmp r13, 39
        je readify
        nop ; byte 84
        nop ; byte 87
        cmp [3578], 3
        jl label_30848_0
readify:
    cmp r13, 10
    je waitify
        label_30858_0:
            label_30855_0:
                mov r5, 0
    peek r5, 3578
        jns label_30855_0
        cmp r5, 2
        jl label_30858_0
    jne waitify
        mov r0, 3
    poke 0, 3578
waitify:
    mov [3578], 4
downity:
    jmp downity
dedronity:
    attackify:
protectity:
    ret ;&: nop
    mov r2, r0
    mov r0, 0
    poke r1, 72
    poke r1, 13
                jns label_30901_0
            mov r5, r1
    peek r5, 0
                cmp r5, 0x8004
            jne label_30896_0
            poke r1, 0
            poke r1, 87
                    mov r3, 0
    label_30869_0:
    label_30871_1:
        add r3, 1
        cmp r3, 70
        jl label_30869_0
    label_30873_2:
            poke r1, 3576
                    mov r3, dronity
    label_30882_0:
                    mov r0, [r3]
    poke r1, r3
    label_30884_1:
        add r3, 1
        cmp r3, dedronity
        jl label_30882_0
    label_30886_2:
                mov r0, 0
    poke r1, 12
        label_30896_0:
        label_30901_0:
    mov r0, r2
        ret
stage1:
main:
alignify:
    getxy r12, r13
    cmp r12, 69
    je siddle_up
        label_30924_0:
            label_30909_0:
            travel 2
            js alignify
            travel 1
        js label_30909_0
        getxy r12, r13
        cmp r13, 39
        je siddle_left
            label_30921_0:
            travel 3
                        js label_30916_0
                    mov r0, 0
    poke 3, 3579
        label_30916_0:
            travel 1
        jns label_30921_0
        jmp label_30924_0
siddle_left:
    mov r0, 0
        label_30934_0:
        getxy r12, r13
        cmp r12, 69
        je siddle_up
            label_30931_0:
                    cmp [3576], 0
        jne transfer
            travel 2
            js siddle_left
            poke 2, 3579
        jmp label_30931_0
        jmp label_30934_0
siddle_up:
    mov r0, 3
        label_30944_0:
        getxy r12, r13
        cmp r13, 0
        je synchrofy
            label_30941_0:
                    cmp [3576], 0
        jne transfer
            travel 0
            js siddle_up
            poke 0, 3579
        jmp label_30941_0
        jmp label_30944_0
synchrofy:
    mov [3578], 1
    getxy r12, r13
    cmp r13, 39
    je sweepify_39
    mov r5, 1
        label_30948_0:
        peek r5, 0
        jns label_30948_0
            cmp r5, 0x8004
            jne label_30956_0
            mov r0, 3
    poke 1, 3579
        jmp synchrofy
        label_30956_0:
        mov r0, synchrofy
    poke 1, 3576
    jmp sweepify
clearify:
        mov r0, clearify
    poke 3, 3576
    mov [3578], 3
    jmp dronify
sweepify_39:
        mov r0, clearify
    poke 3, 3576
sweepify:
    mov r0, 3
    mov r1, 3
    mov r7, 3579
sweepity:
        label_31022_0:
            label_30975_0:
                label_30972_0:
                    mov r5, 1
    peek r5, 0
                jns nextity
                poke 3, r7
        jns label_30972_0
            call attackify
        jmp label_30975_0
    nextity:
            label_30983_0:
                label_30980_0:
                travel 3
                js stopify
            pokity:
                poke 3, r7
        jns label_30980_0
            call attackify
        jmp label_30983_0
    stopify:
        getxy r12, r13
                cmp r12, 4
            jne label_31015_0
            mov r0, 0
            mov r7, 84
            mov [132], 0x0000
            jmp label_31017_1
        label_31015_0:
                    cmp r12, 1
            jne label_31008_0
                label_31003_0:
                    label_31000_0:
                            cmp r13, 10
            jge label_30993_0
                        cmp [3578], 5
                        je liningify
        label_30993_0:
                        mov r5, 3
    peek r5, 3578
        jns label_31000_0
                cmp r5, 4
                je reprogramify
                poke 3, r7
                call attackify
        jmp label_31003_0
        label_31008_0:
        label_31017_1:
        cmp r13, 0
        je sweepity
            mov r5, 0
    peek r5, 3578
        jns stopify
        cmp r5, 1
        jne stopify
        jmp label_31022_0
reprogramify:
            cmp r13, 10
            jl label_31032_0
        linearity:
                mov r5, 0
    peek r5, 3578
            jns linearity
            cmp r5, 1
            jne linearity
                mov r0, 5
    poke 0, 3578
            jns linearity
        label_31032_0:
            mov r3, 621
    label_31048_0:
            mov r0, [r3]
    poke 3, r3
        add r3, 1
            mov r0, [r3]
    poke 3, r3
        add r3, 1
            mov r0, [r3]
    poke 3, r3
        add r3, 1
            mov r0, [r3]
    poke 3, r3
        add r3, 1
            mov r0, [r3]
    poke 3, r3
    label_31050_1:
        add r3, 1
        cmp r3, 1299
        jl label_31048_0
    label_31052_2:
        mov r0, 516
    poke 3, 130
    jmp linify
stage2:
        transfer:
        push [3576]
        mov [3576], 0
        ret
liningify:
            cmp r13, 0
            je label_31071_0
            label_31066_0:
                mov r0, 5
    poke 0, 3578
        jns label_31066_0
        label_31071_0:
linify:
    charge 2, 200
            cmp [0], 0
            je label_31446_0
            label_31094_0:
                label_31079_0:
                travel 0
        js label_31079_0
                label_31091_0:
                getxy r12, r13
                        cmp r13, 0
            jne label_31086_0
                    jmp chargify
        label_31086_0:
                travel 2
                travel 0
        jns label_31091_0
        jmp label_31094_0
                mov r3, 0
    label_31098_0:
            charge 3, 100
    label_31100_1:
        add r3, 1
        cmp r3, 5
        jl label_31098_0
    label_31102_2:
            jmp label_31448_1
        label_31446_0:
                    cmp r13, 10
            jne label_31439_0
                mov r3, 0
    label_31110_0:
            travel 0
    label_31112_1:
        add r3, 1
        cmp r3, 9
        jl label_31110_0
    label_31114_2:
                mov r3, 0
    label_31127_0:
                label_31122_0:
                travel 2
        jns label_31122_0
                mov r0, kickify
    poke 0, 3576
    label_31129_1:
        add r3, 1
        cmp r3, 2
        jl label_31127_0
    label_31131_2:
            label_31138_0:
            travel 0
        jns label_31138_0
        energy r5
        sub r5, 10
        charge 2, r5
            jmp label_31441_1
        label_31439_0:
        ; INSERTION SEQUENCE
        getxy r12, r13
                cmp r13, 21
            jge label_31196_0
            mov r5, 21
            sub r5, r13
                    mov r3, 0
    label_31154_0:
                    label_31151_0:
                charge 0, 100
                            jns label_31146_0
                    nop
nop
        label_31146_0:
                    travel 0
        jns label_31151_0
    label_31156_1:
        add r3, 1
        cmp r3, r5
        jl label_31154_0
    label_31158_2:
            jmp label_31198_1
        label_31196_0:
                    cmp r13, 21
            jle label_31189_0
            mov r5, r13
            sub r5, 21
                    mov r3, 0
    label_31177_0:
                    label_31174_0:
                charge 1, 100
                            jns label_31169_0
                    nop
nop
        label_31169_0:
                    travel 1
        jns label_31174_0
    label_31179_1:
        add r3, 1
        cmp r3, r5
        jl label_31177_0
    label_31181_2:
        label_31189_0:
        label_31198_1:
        mov r6, 69
    godify:
                mov r3, 0
    label_31229_0:
                label_31226_0:
                eat
                charge 2, 100
                            jns label_31205_0
                    nop
nop
        label_31205_0:
                sense r5
                jns bark
                        cmp r5, 0xffff
            jne label_31221_0
                    energy r5
                            cmp r5, 6000
            jle label_31214_0
                        sub r5, 6000
                        release r5
        label_31214_0:
        label_31221_0:
bark:
                travel 2
        jns label_31226_0
    label_31231_1:
        add r3, 1
        cmp r3, r6
        jl label_31229_0
    label_31233_2:
            label_31247_0:
                charge 1, 100
                            jns label_31242_0
                    nop
nop
        label_31242_0:
            travel 1
        jns label_31247_0
                mov r3, 0
    label_31278_0:
                label_31275_0:
                eat
                charge 3, 100
                            jns label_31254_0
                    nop
nop
        label_31254_0:
                sense r5
                jns bob
                        cmp r5, 0xffff
            jne label_31270_0
                    energy r5
                            cmp r5, 6000
            jle label_31263_0
                        sub r5, 6000
                        release r5
        label_31263_0:
        label_31270_0:
bob:
                travel 3
        jns label_31275_0
    label_31280_1:
        add r3, 1
        cmp r3, 68
        jl label_31278_0
    label_31282_2:
        getxy r12, r13
        cmp r13, 38
                    jl label_31322_0
                label_31297_0:
                charge 3, 100
                            jns label_31292_0
                    nop
nop
        label_31292_0:
                travel 3
        jns label_31297_0
                    mov r3, 0
    label_31312_0:
                    label_31309_0:
                    charge 0, 100
                                jns label_31304_0
                        nop
nop
        label_31304_0:
                    travel 0
        jns label_31309_0
    label_31314_1:
        add r3, 1
        cmp r3, 37
        jl label_31312_0
    label_31316_2:
            mov r6, 69
            jmp godify
        label_31322_0:
            label_31335_0:
                charge 1, 100
                            jns label_31330_0
                    nop
nop
        label_31330_0:
            travel 1
        jns label_31335_0
        mov r6, 68
        jmp godify
        ; INSERTION SEQUENCE
                mov r3, 0
    label_31343_0:
                label_31340_0:
                travel 0
        jns label_31340_0
    label_31345_1:
        add r3, 1
        cmp r3, 10
        jl label_31343_0
    label_31347_2:
        mov r3, 0
    snarcify:
        mov r5, 2
        mov r6, 1
    snarfify:
            label_31365_0:
                label_31362_0:
                charge r5, 1000
                            jns label_31357_0
                    nop
nop
nop
        label_31357_0:
                travel r5
        jns label_31362_0
            eat
            getxy r12, r13
            mod r12, 69
            cmp r12, 0
        jne label_31365_0
                cmp r3, 0
            jne label_31394_0
                    mov r4, 0
    label_31374_0:
                    label_31371_0:
                    travel r6
        jns label_31371_0
    label_31376_1:
        add r4, 1
        cmp r4, 20
        jl label_31374_0
    label_31378_2:
                    cmp r5, 2
            jne label_31387_0
                mov r5, 3
                mov r6, 0
                jmp snarfify
        label_31387_0:
            mov r3, 1
            jmp snarcify
        label_31394_0:
        getxy r12, r13
                cmp r13, 19
            je label_31417_0
                    mov r5, r13
    label_31405_0:
                    label_31402_0:
                    travel 3
        jns label_31402_0
    label_31407_1:
        add r5, 1
        cmp r5, 19
        jl label_31405_0
    label_31409_2:
        label_31417_0:
                mov r5, 0
    label_31427_0:
                label_31424_0:
                travel 0
        jns label_31424_0
    label_31429_1:
        add r5, 1
        cmp r5, r13
        jl label_31427_0
    label_31431_2:
        label_31441_1:
        label_31448_1:
    jmp chargify
kickify:
        label_31452_0:
        travel 3
        jns label_31452_0
            mov r3, 0
    label_31456_0:
        energy r5
        sub r5, 10
        charge 3, r5
    label_31458_1:
        add r3, 1
        cmp r3, 20
        jl label_31456_0
    label_31460_2:
chargify:
    getxy r12, r13
        label_31485_0:
        energy r5
                cmp r5, 10000
            jle label_31478_0
            charge 3, 3000
                    cmp r12, 0
            jne label_31471_0
                jmp chargedify
        label_31471_0:
        label_31478_0:
                cmp [3576], 0
        jne transfer
        jmp label_31485_0
chargedify:
        mov r0, chargedify
    poke 2, 3576
            mov r3, 1386
    label_31491_0:
        mov [r3 + 501], [r3]
        mov [r3 + 1002], [r3]
    label_31493_1:
        add r3, 1
        cmp r3, 1887
        jl label_31491_0
    label_31495_2:
            mov r3, 0
    label_31502_0:
        mov [r3 + 3100], 0
        mov [r3 + 3172], 0
        mov [r3 + 3244], 0
    label_31504_1:
        add r3, 1
        cmp r3, 32
        jl label_31502_0
    label_31506_2:
            mov r3, 0
    label_31513_0:
        mov [r3 + 3132], 0
        mov [r3 + 3204], 0
        mov [r3 + 3276], 0
    label_31515_1:
        add r3, 1
        cmp r3, 40
        jl label_31513_0
    label_31517_2:
            mov r3, 2885
    label_31524_0:
        mov [r3], 0
    label_31526_1:
        add r3, 3
        cmp r3, 3095
        jl label_31524_0
    label_31528_2:
    mov [3095], 0x0005
    jmp scanify
repairable:
stage3:
safe:
    eat
    repair:
    mov r6, 1386
    cksum r6, 1887
            cmp r6, r9
            je label_31575_0
                mov r3, 1386
    label_31563_0:
                            cmp [r3], [r3 + 501]
            jne label_31558_0
                    cmp [r3], [r3 + 1002]
            je label_31543_0
                mov r11, 1
                mov [r3 + 1002], [r3]
        label_31543_0:
            jmp label_31560_1
        label_31558_0:
            mov r11, 1
                    cmp [r3], [r3 + 1002]
            jne label_31551_0
                mov [r3 + 501], [r3]
            jmp label_31553_1
        label_31551_0:
                mov [r3], [r3 + 501]
        label_31553_1:
        label_31560_1:
    label_31565_1:
        add r3, 1
        cmp r3, 1887
        jl label_31563_0
    label_31567_2:
        label_31575_0:
    mov r6, 3100
    cksum r6, 3316
            cmp r6, r10
            je label_31620_0
                mov r3, 3100
    label_31608_0:
                            cmp [r3], [r3 + 72]
            jne label_31603_0
                    cmp [r3], [r3 + 144]
            je label_31588_0
                mov r11, 1
                mov [r3 + 144], [r3]
        label_31588_0:
            jmp label_31605_1
        label_31603_0:
            mov r11, 1
                    cmp [r3], [r3 + 144]
            jne label_31596_0
                mov [r3 + 72], [r3]
            jmp label_31598_1
        label_31596_0:
                mov [r3], [r3 + 72]
        label_31598_1:
        label_31605_1:
    label_31610_1:
        add r3, 1
        cmp r3, 3316
        jl label_31608_0
    label_31612_2:
        label_31620_0:
        ret
scanify:
    call sync
    mov r8, 0
    cksum r8, 3600
    mov r9, 1386
    cksum r9, 1887
    mov r10, 3100
    cksum r10, 3316
    jmp innify
        label_31665_0:
        sense r5
                    jns label_31660_0
                    cmp r5, 65535
            jne label_31655_0
                mov [r5 + 3132], 1
                mov [r5 + 3204], 1
                mov [r5 + 3276], 1
            jmp label_31657_1
        label_31655_0:
                mov r11, 0
                call safe
                        cmp r11, 0
            jne label_31640_0
                    mov r6, 0
                    cksum r6, 3600
                            cmp r8, r6
            je label_31633_0
                        mov r11, 1
                        mov r8, r6
        label_31633_0:
        label_31640_0:
                        cmp r11, 0
            je label_31648_0
                    add [r5 + 3100], 1
                    add [r5 + 3172], 1
                    add [r5 + 3244], 1
        label_31648_0:
        label_31657_1:
            mov r10, 3100
            cksum r10, 3316
        label_31660_0:
    innify:
        travel 1
        js label_31665_0
    call sync
            mov r3, 0
    label_31669_0:
        mov [r3 + 3400], 0
    label_31671_1:
        add r3, 1
        cmp r3, 32
        jl label_31669_0
    label_31673_2:
            mov r3, 0
    label_31680_0:
        mov [r3 + 3432], 0
    label_31682_1:
        add r3, 1
        cmp r3, 40
        jl label_31680_0
    label_31684_2:
    mov [3578], 6
    getxy r12, r13
    cmp r12, 0
sharity:
    jne sharity
sharify:
        label_31693_0:
            mov r5, 2
    peek r5, 3578
        jns label_31693_0
    cmp r5, 6
    jne sharify
            mov r3, 0
    label_31697_0:
        add [r3 + 3100], [r3 + 3400]
    label_31699_1:
        add r3, 1
        cmp r3, 32
        jl label_31697_0
    label_31701_2:
            mov r3, 0
    label_31708_0:
        add [r3 + 3132], [r3 + 3432]
    label_31710_1:
        add r3, 1
        cmp r3, 32
        jl label_31708_0
    label_31712_2:
            mov r3, 3400
    label_31721_0:
            mov r0, [r3 - 300]
    poke 2, r3
    label_31723_1:
        add r3, 1
        cmp r3, 3432
        jl label_31721_0
    label_31725_2:
            mov r3, 3432
    label_31734_0:
            mov r0, [r3 - 300]
    poke 2, r3
    label_31736_1:
        add r3, 1
        cmp r3, 3472
        jl label_31734_0
    label_31738_2:
        mov r0, 3
    poke 2, 1666
    cmp r12, 69
collectity:
    jne collectity
collectify:
            mov r3, 3100
    label_31750_0:
            mov r0, [r3]
    poke 2, r3
    label_31752_1:
        add r3, 1
        cmp r3, 3132
        jl label_31750_0
    label_31754_2:
            mov r3, 3132
    label_31763_0:
            mov r0, [r3]
    poke 2, r3
    label_31765_1:
        add r3, 1
        cmp r3, 3164
        jl label_31763_0
    label_31767_2:
        mov r0, 3
    poke 2, 1759
    jmp haltify
stage4:
    sync:
    mov r0, 3
    getxy r12, r13
    cmp r12, 0
syncity1:
    jne syncity1
syncify1:
    mov [1816], 0
            cmp r12, 69
            je label_31783_0
            label_31778_0:
            poke 2, 1816
        jns label_31778_0
        label_31783_0:
sync_:
    mov r0, 3
    cmp r12, 69
    je syncify2
syncity2:
    jmp syncity2
syncify2:
    poke 3, 1843
    mov [1843], 0
    mov r5, 3095
    mult r12, 3
    sub r5, r12
    push r5
    ;mult r12, 3
    ;add r12, 2885
    ;push r12
        ret
haltify:
    travel 0
    travel 1
    jmp haltify
    energy r5
    sub r5, 10
    release r5
    jmp haltify
;call grandmother!

data { 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 }
