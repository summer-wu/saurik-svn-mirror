info: Pac-Bot, Louis Kruger, University of Wisconsin Madison
	mov	r13, main_0         // r13 is permanent cksum register
	cksum	r13, end_0
	mov	r12, _installVirus         // r13 is permanent cksum register
	cksum	r12, main_0
	mov	r9, selfcksum0_0
	cksum	r9, end_0
	jmp	_fixup1
_end_prologue:
	
_init:
	or	sp, 0xFFFF
	or	sp, 0xFFFF
	or	sp, 0xFFFF
	add	r10, 1
	mod	r10, 4
	test	r10, 1
	je	_init2
	and	r0, selfcksum_0
	or	r0, selfcksum_0
	and	r0, selfcksum_0
	or	r0, selfcksum_0
	push	start_0
	cmp	[sp], start_0
	je	r0
	push	start_0
	jmp	r0
	
	data	{ 0xBADD 0xBADD 0xBADD }
	data	{ 0xBADD 0xBADD 0xBADD }
	data	{ 0xBADD 0xBADD 0xBADD }
	data	{ 0xBADD 0xBADD 0xBADD }
	data	{ 0xBADD 0xBADD 0xBADD }
	data	{ 0xBADD 0xBADD 0xBADD }
	data	{ 0xBADD 0xBADD 0xBADD }
	data	{ 0xBADD 0xBADD 0xBADD }
	data	{ 0xBADD 0xBADD 0xBADD }
	data	{ 0xBADD 0xBADD 0xBADD }
	data	{ 0xBADD 0xBADD 0xBADD }
	
_init2:
	and	r0, selfcksum_1
	or	r0, selfcksum_1
	and	r0, selfcksum_1
	or	r0, selfcksum_1
	push	start_0
	cmp	[sp], start_0
	je	r0
	push	start_0
	jmp	r0
	
_fixup1:
	cmp	[1], 0xBADD
	je	_init
	cmp	[2], 0xBADD
	je	_init
			//// EVAL _end_prologue-3 = 18
	mov	r1, 18
_fixup2:
	call	defense_0
	mov	[r1], 0xBADD
	mov	[r1 + 1], 0xBADD
	mov	[r1 + 2], 0xBADD
	sub	r1, 3
	cmp	r1, 0x7FFF
	jle	_fixup2
	jmp	_init
	
	
_installVirus:
	// disable travel first so we can program
	mov	r0, 0x001F
	poke	[dir], 84
	jns	_installVirusRet
	
	energy	r0		// this is a slow and intense process...
	cmp	r0, 20000
	jl	_installVirusRet
	
	call	selfcksum_0		// selfcksum_0
	cmp	r0, 0x600d
	jne	_installVirusRet
	mov	r1, [dir]
	peek	r1, magic3
	cmp	r1, 0x4444
	
	je	_installVirusRet
	test	r7, 0xF000
	jne	_specialRepair
	mov	r0, [dir]
	peek	r0, magic2
	cmp	r0, 0xC96F
	je	_specialRepair
	cmp	r1, 0x9406
	je	_specialRepair
	cmp	r1, 0x4967
	je	_specialRepair
	
	mov	r2, _virus_start
	mov	r5, end_0
	mov	r3, 1
	and	r6, 0
	
_installerMainLoop:
	// call	_defense
	mov	r0, 0x1022
	poke	r2, 72
	
	mov	r0, [r2]
	poke	[dir], r2
	add	r2, r3
	
	jns	_installVirusRet0
	
	mov	r0, [r2]
	poke	[dir], r2
	add	r2, r3
	
	mov	r0, [r2]
	poke	[dir], r2
	add	r2, r3
	
	mov	r0, [r2]
	poke	[dir], r2
	add	r2, r3
	
	mov	r0, [r2]
	poke	[dir], r2
	
	jns	_installVirusRet0
	add	r2, r3
	
	cmp	r2, r5
	jl	_installerMainLoop
	
_installerActivate:
	
	
	mov	r0, 0x8000
	poke	[dir], destX // 0x8000
	
	mov	r0, 0
	poke	[dir], 95
	jns	_installVirusRet
	cmp	[colseen], 2
	jl	_installerColseen
	mov	r0, [collocX]
	poke	[dir], collocX
	mov	r0, [collocY]
	poke	[dir], collocY
	mov	r0, 2
	
_installerColseen:
	poke	[dir], colseen // colseen
	// poke	[dir], collocX + n // copy colseen data
	//mov	r0, [94]
			//// EVAL _virus_start-93 = 387
	mov	r0, 387
	poke	[dir], 94
	call	defense_0
	
	call	selfcksum_0
	cmp	r0, 0x600d
	jne	_installVirusRet
	cmp	r0, 0x600d
	jne	_installVirusRet
	mov	r0, _installVirus
	cksum	r0, main_0
	cmp	r0, r12
	mov	r0, _installVirus
	cksum	r0, main_0
	cmp	r0, r12
	jne	_installVirusRet
	
	mov	r0, 0x0000   // make it go!
	poke	[dir], 87
	mov	r0, 0x8006   // make it go!
	poke	[dir], 93
	
	//mov	r0, 0x001B   // restore travel
	//poke	[dir], 84
	
	//mov	r0, 0x23A3
	//poke	[dir], magic // 0x23A3 -- must be LAST!!!
	
	// jmp	_installVirus	// do phase 2
	
_installVirusRet0:
_installVirusCharge:
	
	charge  [dir], 5000   // kickstart
_installVirusRet:
	ret
	ret
	
_installVirusEnd:
	
_specialRepair:
_specialRepairDelay:
	mov	r0, dir
	peek	r0, magic3
	jns	_installVirusRet
	sub	r2, 6
	cmp	r2, 0x7FFF
	jle	_specialRepairDelay
	
	mov	r0, 0x4444
	poke	[dir], magic3
	jns	_installVirusRet
	
			//// EVAL end_0-2 = 1984
	mov	r2, 1984
	mov	r0, 0x4006
_specialRepairLoop:
	// program "jmp r_invalid" into every location
	// if it is unrecoverably looped, this will
	// probably send it to address 0000 before we start the real upload
	
	poke	[dir], r2
	jns	_installVirusRet
	sub	r2, 1
	poke	[dir], r2
	sub	r2, 2
	cmp	r2, 0
	jg	_specialRepairLoop
	mov	r3, 0xFFFF
	mov	r5, 0x8000
	jmp	_installerMainLoop
	
	
_virus_start:
	// virus header
	or	sp, 0xFFFF
	mov	[magic],  0x23A3		// I.. am....  ALIVE!
	mov	[magic2], 0x416F
	mov	[magic3], 0x9406
	
	jmp	start_0
	jmp	start_0
	
_droneloop:
	eat
	travel  [dir]
	jns	blockade_0
	eat
	travel  [dir]
	eat
	travel  [dir]
	eat
	travel  [dir]
	eat
	travel  [dir]
	jns	blockade_0
	eat
	travel  [dir]
	eat
	travel  [dir]
	eat
	travel  [dir]
	eat
	sense	r2
	js	mainsensed_0
	travel  [dir]
	sub     [count], 8
	energy  r0
	cmp     r0, 63000
	jge     gotonearestcol_0
	jmp     [r11]
	
	main_0:			// start redundant code section
	
	////////////////////////////  Self-repairing code  /////
	
// this is a vulnerable area:
	
selfcksum_0:
	cmp	[magic2], 0x416F
	jne	selfcksumnotdrone_0
	cmp	[magic2], 0x416F
	jne	selfcksumnotdrone_0
	ret
	
virusEnd_0:
	
selfcksumnotdrone_0:
	mov	[0], 0x8006
	mov	[1], _init
	mov	r0, selfcksum0_0
	cksum	r0, end_0
	cmp	r0, r9
	je	selfcksum1ok_0
	mov	r0, start_0
	cksum	r0, end_0
	cmp	r0, r12
	jne	selfcksum2_0
selfcksum1ok_0:
	mov	r0, selfcksum0_0
	push	selfcksum0_1
	cmp	r0, selfcksum0_0
	je	r0
	mov	r0, selfcksum0_0
	jmp	r0
	
selfcksum2_0:
	mov	r0, selfcksum0_1
	cksum	r0, end_1
	cmp	r0, r9
	je	selfcksum2ok_0
	mov	r0, start_1
	cksum	r0, end_1
	cmp	r0, r12
	jne	selfcksum3_0
selfcksum2ok_0:
	mov	r0, selfcksum0_1
	push	selfcksum0_1
	cmp	r0, selfcksum0_1
	je	r0
	mov	r0, selfcksum0_1
	jmp	r0
	
selfcksum3_0:
	// we're pretty screwed up here...
	or	[magic], 0xFFFF
	or	[magic], 0xFFFF
	or	[magic], 0xFFFF
	xor	r10, 2
	test	r10, 2
	// it's not really ok...
	je	selfcksum1ok_0
	test	r10, 2
	jne	selfcksum2ok_0
	test	r10, 2
	je	selfcksum1ok_0
	test	r10, 2
	jne	selfcksum2ok_0
	
selfcksum0_0:
	
	mov	r0, main_0
	cksum	r0, end_0
	mov	r1, main_1
	cksum	r1, end_1
	cmp	r0, r13
	je	chkok0_0
	cmp	r1, r13
	je	copy1to0_0
chkfatal_0:
	// give up, we have no stable copy
	mov	[selfcksum_0], 0x0005	// "ret"
	mov	[selfcksum_1], 0x0005	// "ret"
	or	[magic], 0xFFFF
	or	[magic], 0xFFFF
	or	[magic], 0xFFFF
	or	sp, 0xFFFF
	mov	r0, 0
	ret
	
chkok0_0:
	cmp	r1, r13
	jne	copy0to1_0
	mov	[magic], 0x23A3
	mov	r0, 0x600d
	ret			// both OK
	ret
	
	// r0-r1: source, r2-r3: dest
	// r4 cksum1      r5 cksum2
	// r6 scratch
binaryheal_0:
	mov	r6, r0
	mov	r0, 0x1022
	poke	0, 72
	poke	1, 72
	poke	2, 72
	poke	3, 72
	mov	r0, r6
	
	mov	r4, r0
	cksum	r4, r1
	mov	r5, r2
	cksum	r5, r3
	cmp	r4, r5
	jne	binaryheal1_0
	ret
	
binaryheal1_0:
	mov	r6, r1
	sub	r6, r0
	cmp	r6, 63
	jle	datacopy_0
	
	push	r3
	push	r1
	push	r2
	push	r0
	add	r0, r1
	shr	r0, 1
	add	r2, r3
	shr	r2, 1
	push	r0
	push	r2
	call	binaryheal_0
	pop	r3
	pop	r1
	mov	r0, [sp]
	mov	r2, [sp+1]
	call	binaryheal_0
	pop	r0
	pop	r2
	pop	r1
	pop	r3
	
	mov	r4, r0
	cksum	r4, r1
	mov	r5, r2
	cksum	r5, r3
	cmp	r4, r5
	jne	datacopy_0
	ret
	
datacopy_0:
	mov	[r2], [r0]
	mov	[r2 +1], [r0 +1]
	mov	[r2 +2], [r0 +2]
	mov	[r2 +3], [r0 +3]
	mov	[r2 +4], [r0 +4]
	mov	[r2 +5], [r0 +5]
	add	r0, 6
	add	r2, 6
	cmp	r0, r1
	jl	datacopy_0
	ret
	
copy0to1_0:
	mov	r0, main_0
	mov	r1, end_0
	mov	r2, main_1
	mov	r3, end_1
	
	call	binaryheal1_0
	jmp	selfcksumnotdrone_0
	
copy1to0_0:
	mov	r0, 0x1022
	// mov	[magic], 0x4EA1		// maybe a friend can help
	mov	r0, main_1
	mov	r1, end_1
	mov	r2, main_0
	mov	r3, end_0
	
	call	binaryheal_0
	
	cksum	r0, end_0
	cmp	r0, r13
	je	copydone_0
	jmp	selfcksum0_0
	
copydone_0:
	// we're in an interminate state, lets restart
	mov	r0, start_0
	or	sp, 0xFFFF
	jmp	r0
	
	////////////////////////////  End Self-repairing code  /////
	
	
mainloop_0:
go_0:
	// check if I'm on top of food and eat if so
	sense   r2
	js	mainsensed_0
move_0:
	// move me
	cmp	[count], 0x8000	// who says no signed arithmetic?
	jge	newDir_0
	
moverightnow_0:
	travel	[dir]
	jns	blockade_0
	sense	r2
	js	mainsensed_0
	travel	[dir]
	jns	blockade_0
	sense	r2
	js	mainsensed_0
	travel	[dir]
	jns	blockade_0
	sense	r2
	js	mainsensed_0
	travel	[dir]
	jns	blockade_0
	sense	r2
	js	mainsensed_0
	travel	[dir]
	jns	blockade_0
	sense	r2
	js	mainsensed_0
	travel	[dir]
	jns	blockade_0
	sense	r2
	js	mainsensed_0
	travel	[dir]
	jns	blockade_0
	sub	[count], 7
	jmp	[r11]
	
	////////////////////////////
	
mainsensed_0:
	cmp     r2, 0xFFFF      // are we on a colleciton point?
	jne	foundfood_0
	
	cmp	[colseen], 20
	jge	seen20_0
	getxy	r0, r1
	call	collection_0
	
seen20_0:
	// nop
releasenow_0:
	energy  r0
	cmp     r0, 8000
	jl      move_0
	// too little
	sub	r0, 4000
	release r0
	//rand	[destX], 70
	//rand	[destY], 40
	//rand	[count], 100
	jmp	[r11]
	
	
defense_0:
	mov	r0, 0x1022
defense1_0:
	poke	0, 72
	poke	1, 72
	poke	2, 72
	poke	3, 72
	ret
	
	
start_0:
	or	sp, 0xFFFF
	and	r1, 0
	call	defense_0
setloop_0:
	cmp	r1, 0
	jne	setloop2_0
	mov	r1, mainloop_0
	
	cmp	[magic2], 0x416F
	jne	setloop2_0
	
	//cmp	[magic2], 0xBEEF     // special test, 48879 in base10
	//jne	setloop2_0
	
	mov	r1, _droneloop  // OK because drone only executes main_0
setloop2_0:
	mov	[r11], r1
	jmp	[r11]
	
	call	defense_0
	ret
	
gotonearestcol_0:
	mod	[colseen], 21
	cmp	[colseen], 2
	jl	move_0
	
	rand	r4, [colseen]
	and	r4, 0xfffe
	mov	r6, 110
	
	
	
	
	mov	[destX], [collocX + r4]
	mov	[destY], [collocY + r4]
	mov	[destIDX], r4
	mod	[destX], 70		// just in case...
	mod	[destY], 40
	// mult	r6, 100			// DEBUG
	// add	r6, 6
	mov	[count], r6
	mov	r1, mainloopHunt_0
	jmp	setloop_0
	
	// expect sense value in r2
foundfood_0:
	cmp	[badfood+r2], 0xBADF
	// not hungry...
	je	toxicfood_0
checkfood_0:
	cmp	[badfood+r2], 0xFFFF
	je	quickEat_0
	call	selfcksum_0
	mov	r0, 0
	cksum	r0, 3600
	
	/// DEBUG
	//cmp	[magic2], 0xC96F
	//jne	debugSkipCheck
	//cmp	[colseen], 10
	//jl	debugSkipCheck
	//mov	[2], main_0
	//cksum   [2], end_0
	//cmp	[2], r13
	//jne	actBizarre
	//mov	[2], main_1
	//cksum   [2], end_1
	//cmp	[2], r13
	//jne	actBizarre
// debugSkipCheck:
	/// END DEBUG
	
	eat
	jns	gotonearestcol_0
	mov	r1, 0
	cksum	r1, 3600
	cmp	r0, r1
	jne     badf_0
	
	sub	[badfood+r2], 1
	jmp	goodGulp_0
	
toxicfood_0:
	cmp	[magic2], 0x416F
	jne	move_0
	//	jmp droneToxic_0
	
droneToxic_0:
	mov	[badfood+r2], 0xFFFF
quickEat_0:
	eat
	jns	gotonearestcol_0
	jmp	goodGulp_0
	
	badf_0:				// something changed, its bad food
	mov	[badfood+r2], 0xBADF
	call	selfcksum_0
	jmp	move_0
	
collection_0:
	// store it in table
	// assume x in r0, y in r1
	mov	r5, 0xfffe
	mod	[colseen], 21
findcolloop_0:
	add	r5, 2
	cmp	r0, [collocX + r5]
	jne	findcolnotit_0
	cmp	r1, [collocY + r5]
	jne	findcolnotit_0
	ret			// already seen it.  :(
	ret			// already seen it.  :(
findcolnotit_0:
	cmp	r5, [colseen]
	jl	findcolloop_0
	// its new
	mov	[collocX + r5], r0
	mov	[collocY + r5], r1
	add	[colseen], 2
	call	selfcksum_0
	ret
	ret
	
goodGulp_0:
	cmp	[destX], 70
	jge	goodGulp2_0
	jmp	[r11]	// go back to the hunt_0
	
	
goodGulp2_0:
	energy	r0
	cmp	r0, 63000
	jge	gotonearestcol_0
	jmp	move_0
	
newDir_0:
	// moved enough in this direction, try a new one
	mov	[destX], 0x8000
	
	rand	r0, 3
	xor	r0, 3
	xor	[dir], r0
	rand	[count], 300
	cmp	count, 10
	jg	moverightnow_0
	rand	[bias], 4
	jmp	moverightnow_0
	
blockade_0:
	mov	r0, 0x1022
	poke	[dir], 72
	js	otherbot_0
	
blockadewall_0:
	mov	r1, [bias]
	mov	r2, [dir]
	xor	[dir], 2
	and	[dir], 2
	test	[dir], 2
	jne	wall2_0
	shr     r1, 1
wall2_0:
	and     r1, 1
	or      [dir], r1
wallmove_0:
	travel  [dir]
	jns     wallturnblocked_0
	xor	r2, 1
	mov     [dir], r2
	jmp     [r11]
wallturnblocked_0:
	mov     r0, 0x1022
	poke    [dir], 72
	js      otherbot_0
	rand    r2, 3
	add     r2, 1
	xor     [bias], r2
	jmp     blockadewall_0
	
	
otherbot_0:
	mov	r7, [dir]
	peek	r7, magic
	cmp	r7, 0x23A3
	je	friendlybot1_0
	mov	r0, 0x001F
	poke	[dir], 84
	jns	runaway_0
	
	mov	r0, _installVirus
	cksum	r0, main_0
	cmp	r0, r12
	jne	runaway_0
	mov	r1, _installVirus
	call	r1
	jmp	runaway_0
	
friendlybot1_0:
	// its friendly, but in the way
	cmp	[colseen], 20
	jge	friendlybot2_0
	mov	r0, [dir]
	peek	r0, magic2
	cmp	r0, 0xC96F
	jne	friendlybot2_0
	
	
	// now share the love
			//// EVAL end_0+1 = 1987
	// rand	r1, [1987/3]
	mov	r0, [dir]
	mov	r1, [dir]
	mov	r2, [dir]
	peek	r2, colseen
	jns	runaway_0
	peek	r0, collocX
	jns	runaway_0
	peek	r1, collocY
	jns	runaway_0
	cmp	r2, 2
	jl	friendlybot2_0
	cmp	r0, 70
	jge	friendlybot2_0
	cmp	r1, 40
	jge	friendlybot2_0
	call	collection_0
	
friendlybot2_0:
	energy	r0
	cmp	r0, 6000
	jl	othernohelp_0
	charge	[dir], 60		// "a friend in need..."
	mov	r1, [dir]
	//peek	r1, prio
	
othernohelp_0:
runaway_0:
	// get away from other bot
	rand	r0, 3
	add	r0, 1
	xor	[dir], r0
	travel  [dir]
	travel  [dir]
	mov	r0, 0x1022
	poke	[dir], 72
	rand	r0, 3
	xor	r0, 2			// 00, 10, or 11
	xor	[dir], r0
	jmp	[r11]
	
	
	////////////////////////////  Hunting sub //////////////////
hunt_0:
	rand	r4, 21
	cmp	r4, 0
	jne	huntcol1_0
	//rand	r2, 21
	//cmp	r2, 0
	
	//cmp	r2, 10
	//jge	huntcol1_0
	//cmp	r2, 0
	//je	huntrand_0
	//jmp	[r11]
	
huntrand_0:
	call	selfcksum_0
	//getxy   r0, r1
	//sub	r0, [destX]
	//sub	r1, [destY]
	//call	distance
	//mov	[destDIST], r0
	sub	[count], 1
	jmp	hunt_0
	
	rand	r2, 2
	add	r2, 2
	xor	[dir], r2
	
	travel	[dir]
	jns	huntblockade_0
	travel	[dir]
	jmp	[r11]
	
	jns	huntblockade_0
	jmp	[r11]
huntcol1_0:
	getxy	r0, r1
	cmp	r1, [destY]
	je	huntgoX_0
	cmp	r4, 10
	jge	huntgoY_0
	cmp	r0, [destX]
	je	huntgoY_0
	jmp	huntgoX1_0
huntgoY_0:
	cmp	r1, [destY]
	jl	huntgoSOUTH_0
	mov	[dir],  0
	jmp	huntsetYdist_0
huntgoSOUTH_0:
	mov	[dir],  1
huntsetYdist_0:
	sub	r1, [destY]
	mov	r0, r1
	cmp	r0, 0x7fff
	jle	huntmove_0
	xor	r0, 0xffff
	add	r0, 1
	jmp	huntmove_0
huntgoX_0:
	cmp	r0, [destX]
	jne	huntgoX1_0
	jmp	huntremovespot_0
huntgoX1_0:
	jg	huntgoWEST_0
	mov	[dir],  2
	jmp	huntsetXdist_0
huntgoWEST_0:
	mov	[dir],  3
huntsetXdist_0:
	sub	r0, [destX]
	cmp	r0, 0x7fff
	jle	huntmove_0
	xor	r0, 0xffff
	add	r0, 1
	jmp	huntmove_0
huntblockade_0:
	rand	[dir], 4
	travel  [dir]
	jns	huntblockade_0
	travel  [dir]
	xor	[dir], 3
	rand	[dir], 4
	travel	[dir]
	jmp	[r11]
	
huntmove_0:
	sub	r0, 1
huntmoveloop_0:
	cmp	r0, 0x8000
	jge	[r11]
	cmp	r0, 7
	jge	huntmovefar_0
	cmp	r0, 1
	jge	huntmovenear_0
	
	travel  [dir]
	sub	r0, 1
	jmp	hunthasmoved_0
	
huntmovenear_0:
			//// EVAL huntmovefar_0/3 = 617
			//// EVAL 617+7 = 624
	mov	r2, 624
	sub	r2, r0
	mult	r2, 3
	jmp	r2
	
huntmovefar_0:
	travel	[dir]
	travel	[dir]
	travel	[dir]
	travel	[dir]
	travel	[dir]
	travel	[dir]
	travel	[dir]
	travel	[dir]
	sub	r0, 8
hunthasmoved_0:
	jns	huntblockade_0
	sense	r2
	js	huntsensed_0
	jmp	huntmoveloop_0
	
	
mainloopHunt_0:
	energy	r2
	cmp	r2, 40000
	jl	nomorehunt_0
	sub     [count], 1
	cmp     [count], 0x8000	// who says no signed arithmetic?
	jge     huntremovespot_0
	// if we couldn't get there, it may be blocked
	
	sense   r2
	jns     hunt_0
huntsensed_0:
	cmp     r2, 0xFFFF      // are we on a colleciton point?
	je	huntfoundcollection_0
	energy	r3
	cmp	r3, 62000	// avoid infinite hang on food point
	jle 	foundfood_0
	jmp	hunt_0
	
huntfoundcollection_0:
	energy	r0
	cmp	r0, 8000
	jl	nomorehunt_0
	// too little
	sub	r0, 4000
	release	r0
huntpostcollection_0:
	rand	[dir], 4
	travel	[dir]
	jns	huntpostcollection_0
	
	jmp	nomorehunt_0
huntremovespot_0:
	// no collection point here
	//	cmp huntIDX, 0xffff
	//	for use if I ever need non-collection hunts
	
	//	huntIDX is bogus
	sub	[colseen], 2
	mov	r0,	[colseen]
	mov	r1,	[destIDX]
	mov	[collocX + r1], [collocX + r0]
	mov	[collocY + r1], [collocY + r0]
	
	//cmp	[colseen], 0x8000
	//jl	huntremovecol
	//jmp	actBizarre
nomorehunt_0:
	rand	[dir], 4
	mov	[destX], 0x8000
	and	r1, 0
	jmp	setloop_0
	
	//////////////////////////// End of Hunting sub //////////////////
	
end_0:
	
	jmp	0
	jmp	0
	jmp	0
	
dir:
	data { 0 }       // our initial direction
	
bias:
	data { 2 }	 // // 0-3, specifies corner preference
	
	count:                   // our initial count of how far to move in the cur dir
	data { 1 }
	
	// Big Mike typed here:  ccccccccccccccccccccccccccccc
	// I think it means something like "Go Louis!" in cat language
	
badfood:
	data { 0xBADF 6 6 6 6 6 6 6 }
	data { 6 6 6 6 6 6 6 6 }
	data { 6 6 6 6 6 6 6 6 }
	data { 6 6 6 6 6 6 6 6 }
	data { 6 }
	
	
repair_end:
	
	data { 0xF00F 0xF00F 0xF00F }
	data { 0xF00F 0xF00F 0xF00F }
	data { 0xF00F 0xF00F 0xF00F }
	data { 0xF00F 0xF00F 0xF00F }
	
	
destX:
	data { 0x8000 }
destY:
	data { 0x8000 }
destIDX:
	data { 0x8000 }
	
foodseen:
	// data { 0 }
	
colseen:
	data { 0 }	// how many seen?
	
	collocX:		// locations
	data { 0x7FFF }
collocY:
	data { 0x7FFF }
	data { 0x7FFF 0x7FFF }
	data { 0x7FFF 0x7FFF }
	data { 0x7FFF 0x7FFF }
	data { 0x7FFF 0x7FFF }
	data { 0x7FFF 0x7FFF }
	data { 0x7FFF 0x7FFF }
	data { 0x7FFF 0x7FFF }
	data { 0x7FFF 0x7FFF }
collocX18:
	data { 0x7FFF 0x7FFF }
// prio:
	// data { 0 }
	
magic:
	data { 0x23A3 }
	magic2:			// distinguishes drones
	data { 0xC96F }
magic3:
	data { 0x4967 } // indicate if drone has been programmed before
	
	// alignment padding
	
	data { 0xF00F 0xF00F 0xF00F }
	
	/////////////////////////// redundant copy of main code //////////////////////////////////
	
	main_1:			// start redundant code section
	
	////////////////////////////  Self-repairing code  /////
	
// this is a vulnerable area:
	
selfcksum_1:
	cmp	[magic2], 0x416F
	jne	selfcksumnotdrone_1
	cmp	[magic2], 0x416F
	jne	selfcksumnotdrone_1
	ret
	
virusEnd_1:
	
selfcksumnotdrone_1:
	mov	[0], 0x8006
	mov	[1], _init
	mov	r0, selfcksum0_0
	cksum	r0, end_0
	cmp	r0, r9
	je	selfcksum1ok_1
	mov	r0, start_0
	cksum	r0, end_0
	cmp	r0, r12
	jne	selfcksum2_1
selfcksum1ok_1:
	mov	r0, selfcksum0_0
	push	selfcksum0_1
	cmp	r0, selfcksum0_0
	je	r0
	mov	r0, selfcksum0_0
	jmp	r0
	
selfcksum2_1:
	mov	r0, selfcksum0_1
	cksum	r0, end_1
	cmp	r0, r9
	je	selfcksum2ok_1
	mov	r0, start_1
	cksum	r0, end_1
	cmp	r0, r12
	jne	selfcksum3_1
selfcksum2ok_1:
	mov	r0, selfcksum0_1
	push	selfcksum0_1
	cmp	r0, selfcksum0_1
	je	r0
	mov	r0, selfcksum0_1
	jmp	r0
	
selfcksum3_1:
	// we're pretty screwed up here...
	or	[magic], 0xFFFF
	or	[magic], 0xFFFF
	or	[magic], 0xFFFF
	xor	r10, 2
	test	r10, 2
	// it's not really ok...
	je	selfcksum1ok_1
	test	r10, 2
	jne	selfcksum2ok_1
	test	r10, 2
	je	selfcksum1ok_1
	test	r10, 2
	jne	selfcksum2ok_1
	
selfcksum0_1:
	
	mov	r0, main_0
	cksum	r0, end_0
	mov	r1, main_1
	cksum	r1, end_1
	cmp	r0, r13
	je	chkok0_1
	cmp	r1, r13
	je	copy1to0_1
chkfatal_1:
	// give up, we have no stable copy
	mov	[selfcksum_0], 0x0005	// "ret"
	mov	[selfcksum_1], 0x0005	// "ret"
	or	[magic], 0xFFFF
	or	[magic], 0xFFFF
	or	[magic], 0xFFFF
	or	sp, 0xFFFF
	mov	r0, 0
	ret
	
chkok0_1:
	cmp	r1, r13
	jne	copy0to1_1
	mov	[magic], 0x23A3
	mov	r0, 0x600d
	ret			// both OK
	ret
	
	// r0-r1: source, r2-r3: dest
	// r4 cksum1      r5 cksum2
	// r6 scratch
binaryheal_1:
	mov	r6, r0
	mov	r0, 0x1022
	poke	0, 72
	poke	1, 72
	poke	2, 72
	poke	3, 72
	mov	r0, r6
	
	mov	r4, r0
	cksum	r4, r1
	mov	r5, r2
	cksum	r5, r3
	cmp	r4, r5
	jne	binaryheal1_1
	ret
	
binaryheal1_1:
	mov	r6, r1
	sub	r6, r0
	cmp	r6, 63
	jle	datacopy_1
	
	push	r3
	push	r1
	push	r2
	push	r0
	add	r0, r1
	shr	r0, 1
	add	r2, r3
	shr	r2, 1
	push	r0
	push	r2
	call	binaryheal_1
	pop	r3
	pop	r1
	mov	r0, [sp]
	mov	r2, [sp+1]
	call	binaryheal_1
	pop	r0
	pop	r2
	pop	r1
	pop	r3
	
	mov	r4, r0
	cksum	r4, r1
	mov	r5, r2
	cksum	r5, r3
	cmp	r4, r5
	jne	datacopy_1
	ret
	
datacopy_1:
	mov	[r2], [r0]
	mov	[r2 +1], [r0 +1]
	mov	[r2 +2], [r0 +2]
	mov	[r2 +3], [r0 +3]
	mov	[r2 +4], [r0 +4]
	mov	[r2 +5], [r0 +5]
	add	r0, 6
	add	r2, 6
	cmp	r0, r1
	jl	datacopy_1
	ret
	
copy0to1_1:
	mov	r0, main_0
	mov	r1, end_0
	mov	r2, main_1
	mov	r3, end_1
	
	call	binaryheal1_1
	jmp	selfcksumnotdrone_1
	
copy1to0_1:
	mov	r0, 0x1022
	// mov	[magic], 0x4EA1		// maybe a friend can help
	mov	r0, main_1
	mov	r1, end_1
	mov	r2, main_0
	mov	r3, end_0
	
	call	binaryheal_1
	
	cksum	r0, end_0
	cmp	r0, r13
	je	copydone_1
	jmp	selfcksum0_1
	
copydone_1:
	// we're in an interminate state, lets restart
	mov	r0, start_0
	or	sp, 0xFFFF
	jmp	r0
	
	////////////////////////////  End Self-repairing code  /////
	
	
mainloop_1:
go_1:
	// check if I'm on top of food and eat if so
	sense   r2
	js	mainsensed_1
move_1:
	// move me
	cmp	[count], 0x8000	// who says no signed arithmetic?
	jge	newDir_1
	
moverightnow_1:
	travel	[dir]
	jns	blockade_1
	sense	r2
	js	mainsensed_1
	travel	[dir]
	jns	blockade_1
	sense	r2
	js	mainsensed_1
	travel	[dir]
	jns	blockade_1
	sense	r2
	js	mainsensed_1
	travel	[dir]
	jns	blockade_1
	sense	r2
	js	mainsensed_1
	travel	[dir]
	jns	blockade_1
	sense	r2
	js	mainsensed_1
	travel	[dir]
	jns	blockade_1
	sense	r2
	js	mainsensed_1
	travel	[dir]
	jns	blockade_1
	sub	[count], 7
	jmp	[r11]
	
	////////////////////////////
	
mainsensed_1:
	cmp     r2, 0xFFFF      // are we on a colleciton point?
	jne	foundfood_1
	
	cmp	[colseen], 20
	jge	seen20_1
	getxy	r0, r1
	call	collection_1
	
seen20_1:
	// nop
releasenow_1:
	energy  r0
	cmp     r0, 8000
	jl      move_1
	// too little
	sub	r0, 4000
	release r0
	//rand	[destX], 70
	//rand	[destY], 40
	//rand	[count], 100
	jmp	[r11]
	
	
defense_1:
	mov	r0, 0x1022
defense1_1:
	poke	0, 72
	poke	1, 72
	poke	2, 72
	poke	3, 72
	ret
	
	
start_1:
	or	sp, 0xFFFF
	and	r1, 0
	call	defense_1
setloop_1:
	cmp	r1, 0
	jne	setloop2_1
	mov	r1, mainloop_0
	
	cmp	[magic2], 0x416F
	jne	setloop2_1
	
	//cmp	[magic2], 0xBEEF     // special test, 48879 in base10
	//jne	setloop2_1
	
	mov	r1, _droneloop  // OK because drone only executes main_0
setloop2_1:
	mov	[r11], r1
	jmp	[r11]
	
	call	defense_1
	ret
	
gotonearestcol_1:
	mod	[colseen], 21
	cmp	[colseen], 2
	jl	move_1
	
	rand	r4, [colseen]
	and	r4, 0xfffe
	mov	r6, 110
	
	
	
	
	mov	[destX], [collocX + r4]
	mov	[destY], [collocY + r4]
	mov	[destIDX], r4
	mod	[destX], 70		// just in case...
	mod	[destY], 40
	// mult	r6, 100			// DEBUG
	// add	r6, 6
	mov	[count], r6
	mov	r1, mainloopHunt_0
	jmp	setloop_1
	
	// expect sense value in r2
foundfood_1:
	cmp	[badfood+r2], 0xBADF
	// not hungry...
	je	toxicfood_1
checkfood_1:
	cmp	[badfood+r2], 0xFFFF
	je	quickEat_1
	call	selfcksum_1
	mov	r0, 0
	cksum	r0, 3600
	
	/// DEBUG
	//cmp	[magic2], 0xC96F
	//jne	debugSkipCheck
	//cmp	[colseen], 10
	//jl	debugSkipCheck
	//mov	[2], main_0
	//cksum   [2], end_0
	//cmp	[2], r13
	//jne	actBizarre
	//mov	[2], main_1
	//cksum   [2], end_1
	//cmp	[2], r13
	//jne	actBizarre
// debugSkipCheck:
	/// END DEBUG
	
	eat
	jns	gotonearestcol_1
	mov	r1, 0
	cksum	r1, 3600
	cmp	r0, r1
	jne     badf_1
	
	sub	[badfood+r2], 1
	jmp	goodGulp_1
	
toxicfood_1:
	cmp	[magic2], 0x416F
	jne	move_1
	//	jmp droneToxic_1
	
droneToxic_1:
	mov	[badfood+r2], 0xFFFF
quickEat_1:
	eat
	jns	gotonearestcol_1
	jmp	goodGulp_1
	
	badf_1:				// something changed, its bad food
	mov	[badfood+r2], 0xBADF
	call	selfcksum_1
	jmp	move_1
	
collection_1:
	// store it in table
	// assume x in r0, y in r1
	mov	r5, 0xfffe
	mod	[colseen], 21
findcolloop_1:
	add	r5, 2
	cmp	r0, [collocX + r5]
	jne	findcolnotit_1
	cmp	r1, [collocY + r5]
	jne	findcolnotit_1
	ret			// already seen it.  :(
	ret			// already seen it.  :(
findcolnotit_1:
	cmp	r5, [colseen]
	jl	findcolloop_1
	// its new
	mov	[collocX + r5], r0
	mov	[collocY + r5], r1
	add	[colseen], 2
	call	selfcksum_1
	ret
	ret
	
goodGulp_1:
	cmp	[destX], 70
	jge	goodGulp2_1
	jmp	[r11]	// go back to the hunt_1
	
	
goodGulp2_1:
	energy	r0
	cmp	r0, 63000
	jge	gotonearestcol_1
	jmp	move_1
	
newDir_1:
	// moved enough in this direction, try a new one
	mov	[destX], 0x8000
	
	rand	r0, 3
	xor	r0, 3
	xor	[dir], r0
	rand	[count], 300
	cmp	count, 10
	jg	moverightnow_1
	rand	[bias], 4
	jmp	moverightnow_1
	
blockade_1:
	mov	r0, 0x1022
	poke	[dir], 72
	js	otherbot_1
	
blockadewall_1:
	mov	r1, [bias]
	mov	r2, [dir]
	xor	[dir], 2
	and	[dir], 2
	test	[dir], 2
	jne	wall2_1
	shr     r1, 1
wall2_1:
	and     r1, 1
	or      [dir], r1
wallmove_1:
	travel  [dir]
	jns     wallturnblocked_1
	xor	r2, 1
	mov     [dir], r2
	jmp     [r11]
wallturnblocked_1:
	mov     r0, 0x1022
	poke    [dir], 72
	js      otherbot_1
	rand    r2, 3
	add     r2, 1
	xor     [bias], r2
	jmp     blockadewall_1
	
	
otherbot_1:
	mov	r7, [dir]
	peek	r7, magic
	cmp	r7, 0x23A3
	je	friendlybot1_1
	mov	r0, 0x001F
	poke	[dir], 84
	jns	runaway_1
	
	mov	r0, _installVirus
	cksum	r0, main_0
	cmp	r0, r12
	jne	runaway_1
	mov	r1, _installVirus
	call	r1
	jmp	runaway_1
	
friendlybot1_1:
	// its friendly, but in the way
	cmp	[colseen], 20
	jge	friendlybot2_1
	mov	r0, [dir]
	peek	r0, magic2
	cmp	r0, 0xC96F
	jne	friendlybot2_1
	
	
	// now share the love
			//// EVAL end_0+1 = 1987
	// rand	r1, [1987/3]
	mov	r0, [dir]
	mov	r1, [dir]
	mov	r2, [dir]
	peek	r2, colseen
	jns	runaway_1
	peek	r0, collocX
	jns	runaway_1
	peek	r1, collocY
	jns	runaway_1
	cmp	r2, 2
	jl	friendlybot2_1
	cmp	r0, 70
	jge	friendlybot2_1
	cmp	r1, 40
	jge	friendlybot2_1
	call	collection_1
	
friendlybot2_1:
	energy	r0
	cmp	r0, 6000
	jl	othernohelp_1
	charge	[dir], 60		// "a friend in need..."
	mov	r1, [dir]
	//peek	r1, prio
	
othernohelp_1:
runaway_1:
	// get away from other bot
	rand	r0, 3
	add	r0, 1
	xor	[dir], r0
	travel  [dir]
	travel  [dir]
	mov	r0, 0x1022
	poke	[dir], 72
	rand	r0, 3
	xor	r0, 2			// 00, 10, or 11
	xor	[dir], r0
	jmp	[r11]
	
	
	////////////////////////////  Hunting sub //////////////////
hunt_1:
	rand	r4, 21
	cmp	r4, 0
	jne	huntcol1_1
	//rand	r2, 21
	//cmp	r2, 0
	
	//cmp	r2, 10
	//jge	huntcol1_1
	//cmp	r2, 0
	//je	huntrand_1
	//jmp	[r11]
	
huntrand_1:
	call	selfcksum_1
	//getxy   r0, r1
	//sub	r0, [destX]
	//sub	r1, [destY]
	//call	distance
	//mov	[destDIST], r0
	sub	[count], 1
	jmp	hunt_1
	
	rand	r2, 2
	add	r2, 2
	xor	[dir], r2
	
	travel	[dir]
	jns	huntblockade_1
	travel	[dir]
	jmp	[r11]
	
	jns	huntblockade_1
	jmp	[r11]
huntcol1_1:
	getxy	r0, r1
	cmp	r1, [destY]
	je	huntgoX_1
	cmp	r4, 10
	jge	huntgoY_1
	cmp	r0, [destX]
	je	huntgoY_1
	jmp	huntgoX1_1
huntgoY_1:
	cmp	r1, [destY]
	jl	huntgoSOUTH_1
	mov	[dir],  0
	jmp	huntsetYdist_1
huntgoSOUTH_1:
	mov	[dir],  1
huntsetYdist_1:
	sub	r1, [destY]
	mov	r0, r1
	cmp	r0, 0x7fff
	jle	huntmove_1
	xor	r0, 0xffff
	add	r0, 1
	jmp	huntmove_1
huntgoX_1:
	cmp	r0, [destX]
	jne	huntgoX1_1
	jmp	huntremovespot_1
huntgoX1_1:
	jg	huntgoWEST_1
	mov	[dir],  2
	jmp	huntsetXdist_1
huntgoWEST_1:
	mov	[dir],  3
huntsetXdist_1:
	sub	r0, [destX]
	cmp	r0, 0x7fff
	jle	huntmove_1
	xor	r0, 0xffff
	add	r0, 1
	jmp	huntmove_1
huntblockade_1:
	rand	[dir], 4
	travel  [dir]
	jns	huntblockade_1
	travel  [dir]
	xor	[dir], 3
	rand	[dir], 4
	travel	[dir]
	jmp	[r11]
	
huntmove_1:
	sub	r0, 1
huntmoveloop_1:
	cmp	r0, 0x8000
	jge	[r11]
	cmp	r0, 7
	jge	huntmovefar_1
	cmp	r0, 1
	jge	huntmovenear_1
	
	travel  [dir]
	sub	r0, 1
	jmp	hunthasmoved_1
	
huntmovenear_1:
			//// EVAL huntmovefar_0/3 = 617
			//// EVAL 617+7 = 624
	mov	r2, 624
	sub	r2, r0
	mult	r2, 3
	jmp	r2
	
huntmovefar_1:
	travel	[dir]
	travel	[dir]
	travel	[dir]
	travel	[dir]
	travel	[dir]
	travel	[dir]
	travel	[dir]
	travel	[dir]
	sub	r0, 8
hunthasmoved_1:
	jns	huntblockade_1
	sense	r2
	js	huntsensed_1
	jmp	huntmoveloop_1
	
	
mainloopHunt_1:
	energy	r2
	cmp	r2, 40000
	jl	nomorehunt_1
	sub     [count], 1
	cmp     [count], 0x8000	// who says no signed arithmetic?
	jge     huntremovespot_1
	// if we couldn't get there, it may be blocked
	
	sense   r2
	jns     hunt_1
huntsensed_1:
	cmp     r2, 0xFFFF      // are we on a colleciton point?
	je	huntfoundcollection_1
	energy	r3
	cmp	r3, 62000	// avoid infinite hang on food point
	jle 	foundfood_1
	jmp	hunt_1
	
huntfoundcollection_1:
	energy	r0
	cmp	r0, 8000
	jl	nomorehunt_1
	// too little
	sub	r0, 4000
	release	r0
huntpostcollection_1:
	rand	[dir], 4
	travel	[dir]
	jns	huntpostcollection_1
	
	jmp	nomorehunt_1
huntremovespot_1:
	// no collection point here
	//	cmp huntIDX, 0xffff
	//	for use if I ever need non-collection hunts
	
	//	huntIDX is bogus
	sub	[colseen], 2
	mov	r0,	[colseen]
	mov	r1,	[destIDX]
	mov	[collocX + r1], [collocX + r0]
	mov	[collocY + r1], [collocY + r0]
	
	//cmp	[colseen], 0x8000
	//jl	huntremovecol
	//jmp	actBizarre
nomorehunt_1:
	rand	[dir], 4
	mov	[destX], 0x8000
	and	r1, 0
	jmp	setloop_1
	
	//////////////////////////// End of Hunting sub //////////////////
	
end_1:
