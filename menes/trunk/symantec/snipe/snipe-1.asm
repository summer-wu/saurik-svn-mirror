info: SampleBot, John Doe, youremail@your-domain.com, 310 555-1212, UCLA

; r0: loop control variable
; r1: output register
; r2: checksum cache

main:
	mov r2, 39
	cksum r2, 3600

check:
	mov r3, 39
	cksum r3, 3600
	cmp r2, r3
	je check
        mov r2, r3

	mov r0, 39
loop:
	mov r1, [r0]
	add r0, 1
	cmp r0, 3600
	je check
	jmp loop
