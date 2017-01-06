info: LineDanceV2, Matthew Menke, MIT
// Not really a line dance, exactly, and it only exists for a couple thousand cycles,
// I suppose, but I'm too lazy to think of a better name.  Most of the time is spent
// running around in pairs of rows each with 3-4 bots each...Not very inspiring.
// Assigning 7 bots per 4 lines might have done better, as when the faster drones are off
// looking for a dump spot, food builds up on their rows, but I don't have time to try it.

// Note:  Two complete copies of most of my, written so that they compile
// to identical code.  I only run the repair code of my copy, but both should
// work.  Underscore prefixes for memory locations that have only one cope.

// Designed to mirror drone code.  Attack strategy:  Kill the two annoying lines
// (travel and poke).  Everything else is harmless.  Make them all go to one side,
// and then have a specific one of my bots reprogram them and tell them where to go.

// Generalized repair stuff:  Damaged bots tend to end up near start, as a jump to a location
// greater than 3599 ends up here, as do jumps to very numbers (And both drones memory and mine
// have a lot of low numbers).
// Therefore, I try and keep a useful pointer on the stack, so when I end up back here,
// I can just try to repair myself and then jump to the pointer on the stack.  Not
// sure if this helps much in practice, but I have seen the code end up here more than once.

// Other than one time use code and emergency stuff, no overlap between drones' and my code.
_Start:
	// On jump/call corruption, often end up here, as I use lotsa registers, so making a command
	// a call/jump often brings use here.  Also, modifying a jump to location randomly, to something
	// over 3599 seems to bring us here, too, so nop it out later.  This code isn't backed up, sadly.
	// Still, reduces the number of single mutations that almost always mean instant death, slightly.
	jmp _Init
	nop // A bit of leeway for my emergency repair stuff.
	nop
	// If ended up here, odds are main copy is corrupt, so try backup repair.
	call CheckAndFix_1
	// If we're lucky enough to have a pointer on the stack, use it.  Often, call
	// CheckAndFix_0 and end up here, so we return to the calling proc, after fixing
	// the problem.  Otherwise, just gather cash.
	cmp sp, 3600
	je GatherCashStart_0
	ret
_DroneInit:
_Init:
	mov r13, CodeStart_0
	cksum r13, CodeEnd_0
	mov r11, 0xFFFF
	// Nop out a line, so if we end up at start again due to a bad jump,
	// at least we're not stuck.
	mov [_Start], 0
	// Prepare my battle word.  Gets drones out of the way.
	mov r0, 103
_DroneSense:
	travel 3
	// emergency recover location
	push _DroneSense
	getxy r3, r4
	cmp r3, 3
	mov r3, 3
	jg MoveLeftSemifast_0
	je AtRow3New_0
	mov r3, 2
	jmp NearRow3_0
_DroneAttack:

// Messages from friends.  More efficient to use push than poll.
// Can run off to attack after messaging.  Also, peeking requires
// 2 ops to transfer date (As does poking), but checking local
// memory just requires 1, so poke once, check memory as needed
// works best.  The only check on other bots is checking the checksum.
// Sticking them here, they're safe from me, and under no more threat than
// usual from the bots.  Modifying any of these could kill me, unfortunately,
// but change too much to put in the checksum.  There is some security in that
// neighbors repeatedly overwrite them, it's not that great, and if the values of
// the top or bottom one are modified, I'm in trouble.  Could fix that, but takes
// at least 5 cycles per loop iteration for everyone.
_messageFromAbove:
	data { 0 }
_messageFromBelow:
	data { 0 }
// Just used at the end of an attack to communicate information on
// drop off spots.  A lot of code on it, but it's one of my few changes that
// actually seemed to improved my average harvest...if barely.
_messageFromAbove2:
	data { 0x0F00 }
_messageFromBelow2:
	data { 0x0F00 }
_messageFromAbove3:
	data { 0x0F00 }
_messageFromBelow3:
	data { 0x0F00 }
// Used to identify bots in my line of attack.  Needed along with _messageFromAbove
// and _messageFromBelow to keep my line together.  Less vulnerable than either, as I
// keep a backup in a register.  Without a backup, it would be the most vulnerable by far.
_attackStatus:
	data { 0 }

	nop // One of my primary attack targets on the drones.  Poke command.
	nop
_DroneMove:
	nop
	nop
_DroneMove2:
	nop // Another attack target.  Travel.
SlaveActivate_0: // overwrite with nop to make a drone be reborn as my eternal slave.
SlaveStart_0:
	nop // Jump statment.  I make it loop back to _DroneMove2.

CodeStart_0:
SlaveInit_0:
	mov [HarvestProc_0], SlaveHarvest_0
	mov r11, [_randomData1]
	mov r12, [_randomData2]
	mov r5, [_randomData3]

GatherCashStart2_0:
	// Reduces chances of getting stuck on the mass of dead drones.
	mov r9, 10
	call GotoColumnR9_0
	mov r9, r5
	call GotoRowR9_0
GatherCashStart_0:
	push GatherCashStart_0
	getxy r0, r2
	and r2, 1
	xor r2, 1
	mov r1, 2
GatherCash_0:
GatherCashLoop_0:
	travel r2
	jns GatherCashSkip_0
	xor r2, 1
GatherCashSkip_0:
	travel r1
	js SameRow_0
	xor r1, 1
SameRow_0:
	call [HarvestProc_0]
	energy r3
	cmp r3, 55000
	jle GatherCash_0
	getxy r0, r5
	call LookForDumpStart_0
	mov r9, r5
	call GotoRowR9_0

	jmp SameRow_0


// Note:  May change column, uses r6-r8
// When done, new position is in r6, r7
GotoRowR9_0:
	getxy r6, r7
	sub r7, r9
	shr r7, 15
GotoRowR92_0:
	getxy r8, r6
	cmp r9, r6
	je Return_0
	sub r6, r9
	mult r6, [MTable_0 + r7]
	// Don't sit at destination as long this way
	push GotoRowR92_0
	jmp MoveToLoopStart_0

// Note:  May change row, uses r6-r8
// When done, new position is in r6, r7
GotoColumnR9_0:
	getxy r7, r8
	sub r7, r9
	shr r7, 15
	xor r7, 3
GotoColumnR92_0:
	getxy r6, r8
	cmp r9, r6
	je Return_0
	sub r6, r9
	mult r6, [MTable2_0 + r7]
	push GotoColumnR92_0
	//call MoveToLoopStart
	//jmp GotoColumnR92

// sub r6, r9
// mov r7, r6
// shr r7, 15
// xor r6, [table + r7]
// add r6, r7

// Tries to move r6 times in the r7 direction.
// Success not guaranteed, moves randomly perpendicularly on collision

MoveToLoopStart_0:
	mov r8, r6
	and r8, 0x7
	shr r6, 3
	jmp [ jmpTable_0 + r8 ]


MoveToLoop8_0:
MoveToLoop_0:
	sub r6, 1
	travel r7
MoveToLoop7_0:
	travel r7
MoveToLoop6_0:
	travel r7
MoveToLoop5_0:
	travel r7
MoveToLoop4_0:
	travel r7
MoveToLoop3_0:
	travel r7
MoveToLoop2_0:
	travel r7
MoveToLoop1_0:
	travel r7
	js [ jmpTable2_0 + r6 ]
	rand r8, 2
	xor r8, r7
	travel [xorTwoTable_0 + r8]
MoveToLoop0_0:
	jmp [ jmpTable2_0 + r6 ]

xorTwoTable_0:
	data { 2 3 0 1 }

jmpTable_0:
	data { MoveToLoop0_0 MoveToLoop1_0 MoveToLoop2_0 }
	data { MoveToLoop3_0 MoveToLoop4_0 MoveToLoop5_0 }
	data { MoveToLoop6_0 MoveToLoop7_0 }
jmpTable2_0:
	data { Return_0 MoveToLoop8_0 MoveToLoop8_0 }
	data { MoveToLoop8_0 MoveToLoop8_0 MoveToLoop8_0 }
	data { MoveToLoop8_0 MoveToLoop8_0 MoveToLoop8_0 }
	data { MoveToLoop8_0 }

SlaveHarvest_0:
	eat
	travel r1
	eat
	travel r1
	eat
	travel r1
	eat
	travel r1
	eat
	travel r1
	eat
	travel r1
	js SlaveHarvest_0
Mtable2_0:
	// ret
	data { 5 }


MTable_0:
	data {1 0xFFFF 1}


LookForDumpStart_0:
	cmp r11, 69
	jle GoToKnownDump_0

GoToKnownDumpFail_0: // Aka look slowly for new dump
	rand r8, 2
	rand r9, 2
	// add r8, 2

LookForDumpCheck_0:
	travel [ xorTwoTable_0 + r8 ]
	jns LookForDumpNextLine_0
LookForDumpCheck2_0:
	sense r3
	jns LookForDumpCheck_0
	cmp r3, 65535
	jl  LookForDumpCheck_0
	jmp DumpFound_0

LookForDumpNextLine_0:
	energy r3
	cmp r3, 12000
	jl GiveupDumpSearch_0
	xor r8, 1
	travel r9
	js LookForDumpCheck2_0
	xor r9, 1
	jmp LookForDumpNextLine_0


GotoKnownDump_0:
	mov r0, 0
GotoKnownDump2_0:
	add r0, 1
	cmp r0, 2
	jg GotoKnownDumpFail_0
	mov r9, r11
	call GotoColumnR9_0
	mov r9, r12
	call GotoRowR9_0

	sense r3
	jns GotoKnownDump2_0
	cmp r3, 65535
	jl GotoKnownDump2_0 // jl is more robust than je, in case
	                  // of corruption.  Seen the above number modified.

DumpFound_0:
	energy r3
	sub r3, 7000
	release r3
	getxy r11, r12
	travel 1
	travel 2
GiveupDumpSearch_0:

SlaveCodeEnd_0:
Return_0: // General purpose return.
	ret

AtRow3New_0:
	// A little extra security.
	// Means if we ever jump somewhere near the start, can get back.
	//jmp SkipDroneTest
	pop r1
DroneTest_0:
	push DroneTest_0
	getxy r4, r1
	div r1, 20

	// Needed here to prevent carrying messages prematurely
MoveOver_0:
	travel r1
	js MoveOver_0
	// one line to clear both.  No, it doesn't really matter.
	getxy [_messageFromAbove], [_messageFromBelow]

	getxy r4, r5
	mov r12, 0

	cmp r5, 0
	jne MiniSkip1_0
	or r12, 1
	mov [_messageFromAbove], 0xF00
MiniSkip1_0:
	cmp r5, 39
	jne MiniSkip2_0
	or r12, 2
	mov [_messageFromBelow], 0xF00
MiniSkip2_0:

	sub r5, 15
	cmp r5, 10
	jge Stay_0
	travel 3
	js Wait2_0
	or r12, 4
	Stay_0:

	// Putting this here is a minor optimization.
	// Slows down most bots, but means the second row
	// fills in a bit faster.


AtRow3Loop_0:
	test r12, 4
	je StayStill_0
	travel 3
	js Wait2_0
StayStill_0:
	cmp [_messageFromAbove], 0xF00
	jl SkipAttackCheck_0 // Don't need to poke, either...Minor speedup
	cmp [_messageFromBelow], 0xF00
	mov r0, [_messageFromAbove]
	poke 1, _messageFromAbove
SkipAttackCheck_0:
	mov r0, [_messageFromBelow]
	poke 0, _messageFromBelow
	// Have to poke before attack.
	jge Attack_0
	test r12, 1 // 2 steps when have a neighbor, 8 steps when don't.  better than always 6.
	jne SkipAboveDroneTest_0

	mov r0, 0
	peek r0, IFF_0
	jns SkipAboveDroneTest_0
	or r12, 1
	cmp r0, [IFF_0]
	je SkipAboveDroneTest_0
	mov r9, 0
	call DisableR9_0
	xor r12, 1

SkipAboveDroneTest_0:
	test r12, 2
	jne SkipDroneTest_0

	mov r0, 1
	peek r0, IFF_0
	jns SkipDroneTest_0
	or r12, 2
	cmp r0, [IFF_0]
	je SkipDroneTest_0
	mov r9, 1
	call DisableR9_0
	xor r12, 2

SkipDroneTest_0:
	mov r9, 2
	call CheckAndDisableR9_0
	mov r0, 103
	poke 2, 85
SkipDroneTest2_0:
	mov r9, 3
	call CheckAndDisableR9_0
	cmp [_messageFromAbove+r1], 0xF00
	travel r1
	js MoveOver_0
	jmp AtRow3Loop_0

AttackRecover_0:
	push AttackRecover_0
	jmp AttackTryMove_0

Attack_0:
	mov [sp], AttackRecover_0
	call CheckAndFix_0
	mov r10, 0xF01
	mov r0, r10
	poke 3, _MessageFromAbove
	// Just in case.
	poke 1, _MessageFromAbove
	poke 0, _MessageFromBelow

	getxy r0, r1
	cmp r1, 0
	jne AttackMiniSkip1_0
	mov [_messageFromAbove], 0xFFFF
	mov [_messageFromAbove3], 0x0300
AttackMiniSkip1_0:
	cmp r1, 39
	jne AttackMiniSkip2_0
	mov [_messageFromBelow], 0xFFFF
	mov [_messageFromBelow3], 0x00F0
AttackMiniSkip2_0:
AttackMoved_0:
	add r10, 1
	sense r0
	jns AttackTryMove_0
	cmp r0, 0xFFFF
	jne AttackTryMove2_0
	getxy r11, r12

AttackTryMove2_0:
AttackTryMove_0:
	mov [_attackStatus], r10
	// Fastest way to check for something.  NearRow3 is harmless to
	// friendly and enemy bots.  Trying to do damage without knowing
	// what's in r0 is a *bad* idea.  Accidentally using part of a
	// jump command left over from attacking another drone, for example,
	// which I would never, ever do, even by accident, could be
	// interesting.  Completely random example, without any personal
	// experience in the matter of course.
	poke 2, 74
	js AttackDisable_0

	cmp [_messageFromAbove], r10
	jge AttackAboveHappy_0
	mov r0, 0
	// Message prevents getting confused because
	// of harvesters in back.
	peek r0, _attackStatus
	jns AttackUnhappy_0
	cmp r0, r10
	je AttackAboveHappy2_0
	mov r9, 0
	call DisableR9_0
	jmp AttackTryMove_0
AttackAboveHappy2_0:
	mov [_messageFromAbove], r10
AttackAboveHappy_0:
	cmp [_messageFromBelow], r10
	jge AttackBelowHappy_0
	mov r0, 1
	peek r0, _attackStatus
	jns AttackUnhappy_0
	cmp r0, r10
	je AttackBelowHappy2_0
	mov r9, 1
	call DisableR9_0
	jmp AttackTryMove_0
AttackBelowHappy2_0:
	mov [_messageFromBelow], r10
AttackBelowHappy_0:
	mov r0, r10
	poke 0, _MessageFromBelow
	poke 1, _MessageFromAbove

	travel 2
	js AttackMoved_0
CheckIfAttackDone_0:
	getxy r4, r5
	cmp r4, 69
	je AttackDone_0

AttackUnhappy_0:
AttackDisable_0:
	mov r0, 104
	poke 2, 85
	jns AttackTryMove_0 // Drone moves left only, so he's stuck.  Nana.
	poke 2, 72
	mov r0, 0xFFFD
	poke 2, 88
	mov r0, 0x8006
	poke 2, 87
	call CheckAndFix_0

AttackTryDodge_0:
	mov r0, r10
	poke 0,_MessageFromBelow
	poke 1,_MessageFromAbove
AttackTryDodge2_0:
	// Wait until I've received the ok from
	// both neighbors.  Otherwise, dodging
	// can confuse me, or my neighbors' neighbors
	// (Which incldues me, I suppose).
	cmp [_MessageFromAbove], r10
	jl AttackTryDodge_0
	poke 0, _MessageFromBelow
AttackTryDodge4_0:
	cmp [_MessageFromBelow], r10
	jl AttackTryDodge4_0
	poke 1, _MessageFromAbove

AttackTryDodge3_0:
	travel 1
	mov r4, 0
	js AttackDodged2_0
	travel 0
	mov r4, 1
	jns AttackTryDodge3_0
AttackDodged2_0:
AttackDodged_0:
	travel r4
	js AttackTryMove_0
	jmp AttackDodged_0

AttackDone_0:
	cmp r11, 80
	jl PassData_0
WaitForData_0:
	mov r0, [_MessageFromBelow2]
	poke 0, _MessageFromBelow2
	mov r0, [_MessageFromBelow3]
	poke 0, _MessageFromBelow3
	mov r0, [_MessageFromAbove2]
	poke 1, _MessageFromAbove2
	mov r0, [_MessageFromAbove3]
	poke 1, _MessageFromAbove3

	add r0, [_MessageFromBelow3]
	cmp r0, 0x0400
	jge WaitForData_0

	// Note:  Location still in r4, r5

	// If ClosestAbove + ClosestBelow - 2 * y is > 0,
	// then above is closest.  Otherwise, below is.
	// Note that I'm treating numbers as 10-bit signed numbers.
	// Therefore, 0x300 is quit negative and farther away than
	// any valid row but still avoids overly.  Conveniently, 
	// I can also use my sums to 0x400 test above.
	// A lot of effort to save 2 or 3 lines of code, but I'm getting
	// low on space.
	sub r0, r5
	sub r0, r5
	shr r0, 9
	and r0, 1

	mov r11, [_MessageFromAbove2+r0]
	mov r12, [_MessageFromAbove3+r0]

PassData_0:
	mov r0, r11
	poke 0, _MessageFromBelow2
	poke 1, _MessageFromAbove2
	mov r0, r12
	poke 0, _MessageFromBelow3
	poke 1, _MessageFromAbove3
MessagingDone_0:
	call CheckAndFix_0
	// Location still in r4, r5
	test r5, 3
	jne GatherCashStart_0
	jmp AssymilateStart_0
Wait2_0:
	getxy r4, r5
	// r5 is our ID.  Now *EVERYONE* has a unique position.
	// This means that I can assign them final harvesting
	// spots based solely on their location in this dance.
	// Downside:  If anything goes wrong, entire operation
	// fails.
Wait3_0:
	call CheckAndFix_0
	mov r0, 104
	poke 0, 85
	poke 0, 72
	poke 1, 85
	poke 1, 72
	poke 3, 85
	poke 3, 72
	mov r0, 103
	poke 2, 85
	poke 2, 72
	cmp [_MessageFromAbove], 0xF00
	jle Wait3_0

// All I want is to disable any stragglers.
// Can't think of a better way to do this without
// a lot more code, possibly having my 10 stragglers form
// up in the middle and doing an attack run just like
// everyone else....Ick.
Cleanup_0:
	// Location still in r4, r5
	mov r1, r5
	div r1, 20

	// Column r9 is just to spread them out to disable anything
	// that might have been hiding.  Table is smaller and does better
	// than the code to generate something comparable.

	mov r9, [spreadTablePrefix_0 + r5]
	call GoToColumnR9_0
	
	// r5 is 4 * i + 2, where i ranges from 0 to 9.  It's the column where
	// our slave will end up.

	mult r5, 4
	sub r5, 58

CleanupDisable_0:
	mov r9, 2
	call CheckAndDisableR9_0
	mov r9, 3
	call CheckAndDisableR9_0
	travel r1
	js CleanupDisable_0
	mov r9, r1
	call CheckAndDisableR9_0
	getxy r4, r3

	// Saves a line to do it this way.
	sub r3, 2
	cmp r3, 36
	jge AssymilateStart2_0
	rand r9, 3
	call GotoColumnR9_0
	jmp CleanupDisable_0

AssymilateStart_0:
AssymilateStart2_0:
	// This causes the first drones to take rows which only have one other bot, initially,
	// at least it does in theory.
	xor r5, 2
	mov r3, 1
	jmp AssymilateFlip_0
Assymilate_0:
	travel 3
	js Assymilate_0
	mov r9, 3
	call TryAssimilateR9_0
	cmp r0, 1
	je GatherCashStart2_0
AssymilateSkip_0:
	travel r3
	js Assymilate_0
	mov r9, r3
	call TryAssimilateR9_0
	cmp r0, 1
	je GatherCashStart2_0
AssymilateSkip2_0:
	getxy r6, r7
	travel 2
	jns AssymilateFlip_0
	cmp r6, 2
	jle AssymilateSkip_0
AssymilateFlip_0:
	xor r3, 1
	// Assymilation takes tons of energy.
	// Worth putting it off a bit to be sure of survival.
	// Give some energy to new slave, too, just in case.
	energy r6
	cmp r6, 8000
	jg Flip2_0
	mov r1, 3
	call DefaultHarvest2_0
	mov r1, 2
	call DefaultHarvest2_0
	travel r3
	travel r1
	travel r3
	jmp AssymilateFlip_0
Flip2_0:
	jmp Assymilate_0

TryAssimilateR9_0:
	mov r2, r9
	mov r0, 0
	poke r9, _DroneMove2
	jns Return_0
	peek r2, IFF_0
	jns Return_0
	cmp r2, [IFF_0]
	jge Return_0

	mov r0, 0xCAFE
	poke r9, IFF_0
	mov r0, r5
	poke r9, _randomData3

	call DisableR9_0

	mov r0, r11
	poke r9, _randomData1
	mov r0, r12
	poke r9, _randomData2

	// Check if someone else had the temerity to claim the
	// bot just as we did.  If so, and he won, fail.
	// 6 cycles from check IFF to write, 8 to overwrite r5,
	// and over 10 for disable + writing release spot, so
	// shouldn't be a problem.
	mov r0, r9
	peek r0, _randomData3
	cmp r0, r5
	mov r0, 0
	jne Return_0




	// Reduces chance of food problems from
	// 4 buddies sharing a row too early.
	// I switch because for the first 10, this helps
	// fill in the pairs of rows with only one bot.
	xor r5, 2
	mov [SlaveStart_0], 0

	mov r2, SlaveCodeEnd_0

CopyLoop_0:
	mov r0, [r2]
	poke r9, r2
	sub r2, 1
	mov r0, [r2]
	poke r9, r2
	sub r2, 1
	cmp r2, SlaveStart_0
	jge CopyLoop_0

	charge r9, 1300

	mov r0, 1
	// ret.  Saves me a couple words to stick in with some data.
	data { 5 }

FoodTablePrefix_0:
	data { EnergyRelease_0 }
FoodTable_0: // 32 words, not counting prefix
	data { UnknownFood_0 UnknownFood_0 UnknownFood_0 UnknownFood_0 }
	data { UnknownFood_0 UnknownFood_0 UnknownFood_0 UnknownFood_0 }
	data { UnknownFood_0 UnknownFood_0 UnknownFood_0 UnknownFood_0 }
	data { UnknownFood_0 UnknownFood_0 UnknownFood_0 UnknownFood_0 }
	data { UnknownFood_0 }
// 15 before actual table
SpreadTablePrefix_0:
	data { UnknownFood_0 UnknownFood_0 UnknownFood_0 }
	data { UnknownFood_0 UnknownFood_0 UnknownFood_0 UnknownFood_0 }
	data { UnknownFood_0 UnknownFood_0 UnknownFood_0 UnknownFood_0 }
	data { UnknownFood_0 UnknownFood_0 UnknownFood_0 UnknownFood_0 }

SpreadTable_0:
	data { 1 2 0 2 1 }
	data { 1 2 0 2 1 }

EndTable0_0:
	data { CodeStart_0 CodeEnd_0 }
EndTable1_0:
	data { CodeStart_1 CodeEnd_1 }
	data { CodeStart_0 CodeEnd_0 }




CheckAndDisableR9_0:
	mov r8, r9
	peek r8, IFF_0
	jns Return_0
	cmp r8, [IFF_0]
	je Return_0

DisableR9_0:
	mov r0, 104
	poke r9, 85
	poke r9, 72
	mov r0, 0xFFFD
	poke r9, 88
	mov r0, 0x8006
	poke r9, 87
	// Go on into repair code, just in case.



// Not as fast as it could be, but the goal is to jump to
// the duplicate function ASAP if its checksum isn't right.
CheckAndFix_0:
	mov r6, CodeStart_0
CheckAndFix2_0: // marginally safer, if r6 is already set
	// Fewest possible number of steps to get out of here if
	// something's wrong.
	cksum r6, CodeEnd_0
	cmp r6, r13
	jne [BackupGoodRepair_1]

	// If we're running our backup repair code, and
	// then notice it's been modified...This might help, though
	// at the moment, I don't save the checksum I decide I like
	// until the end, so it's not too likely.  Still, can end here
	// in emergencies.  Best to be prepared.
	jmp [OriginalGoodRepair_0]

	// Used to have two functions for this.  By having 1,
	// I save about 3 lines of code.  Should require quite
	// so many lines, really.  Oh well, if I get desperate,
	// I can hide data in rets or something.

BackupRepair_0: // Saves an op in some very few cases. Pretty
	          // pointless, actually
	mov r8, EndTable1_1 // Pun Intended
Repair_0:
	mov r7, CodeStart_1
	cksum r7, CodeEnd_1
	cmp r6, r7
	je CheckHappy_0

	cmp r6, r13
	jne BackupGood_0
	mov r8, EndTable0_0
BackupGood_0:

	mov r0, [r8]
	mov [_randomData3], [r8+1]
	mov r6, [r8+2]
	mov r7, [r8+3]
BeginFix_0:
	mov r8, r0
	add r8, [_randomData3]
	shr r8, 1

	mov r9, r6
	add r9, r7
	shr r9, 1

	mov [_randomData1], r8
	cksum [_randomData1], [_randomData3]
	mov [_randomData2], r9
	cksum [_randomData2], r7
	cmp [_randomData1], [_randomData2]
	// This fall through method works best.  If for some
	// reason r10 < r0 or r7 < r6, we should still be able to
	// exit the loop, as opposed to getting stuck, even
	// if our code has already been fixed.
	jne SecondHalfBad_0
FirstHalfBad_0:
	mov [_randomData3], r8
	mov r7, r9
	jmp ZeroIn_0
SecondHalfBad_0:
	mov r0, r8
	mov r6, r9
	mov r8, [_randomData3]
ZeroIn_0:
	sub r8, r0
	cmp r8, 4
	jg BeginFix_0
	mov [r6], [r0]
	mov [r6+1], [r0+1]
	mov [r6+2], [r0+2]
	mov [r6+3], [r0+3]
	jmp CheckAndFix_0

CheckHappy_0:
	mov r13, r7
	// ret.  Making this shorter saves a little memory, as I can store
	// some values here.
	data { 5 }

IFF_0:  // Cool secret word...The drones will never figure it out!
      // Used to use checksum (Stored in r13), but corrupted bots could
      // cause problems...Still can, but now there's less chance.
	data { 0xCAFE }

HarvestProc_0:
	data { DefaultHarvest2_0 }

SafeFood_0:
	eat
	jns Return_0
DefaultHarvest_0:
	travel r1
	jns Return_0
DefaultHarvest2_0:
	sense r10
	js [FoodTable_0 + r10]
	travel r1
	sense r10
	js [FoodTable_0 + r10]
	travel r1
	sense r10
	js [FoodTable_0 + r10]
	travel r1
	sense r10
	js [FoodTable_0 + r10]
DangerousFood_0:
	travel r1
	js DefaultHarvest2_0
	// ret.  Save a bit if I stick some data after it.  I need space
	// to play with.
	data { 5 }

OriginalGoodRepair_0:
	data { Repair_0 }

BackupGoodRepair_0:
	data { BackupRepair_1 }

// If there's a release point on a bot's row, no reason to release too much
EnergyRelease_0:
	energy r0
	sub r0, 15000
	release r0
	jmp DefaultHarvest_0

UnknownFood_0:
	nop
UnknownFood2_0:
	mov r8, 0
	mov r9, 0
	mov r6, CodeStart_0
	cksum r8, 3600
	eat
	cksum r9, 3600
	call CheckAndFix2_0
	cmp r8, r9
	je UpdateSafe_0
	mov [FoodTable_0 + r10], DangerousFood_0
	mov [FoodTable_1 + r10], DangerousFood_0
	jmp UpdateChecksum_0
UpdateSafe_0:
	cmp [FoodTable_0 + r10], UnknownFood_0
	sub [FoodTable_0 + r10], 1
	sub [FoodTable_1 + r10], 1
	jg UpdateChecksum_0 // Not sure yet
	mov [FoodTable_0 + r10], SafeFood_0
	mov [FoodTable_1 + r10], SafeFood_0
UpdateChecksum_0:
	call CheckAndFix_0
	jmp DefaultHarvest_0

InvertTable_0:
	data {1 0}

// Basic idea is to move a lot, to make it less likely that a drone will damage us.
// This code used to not be backed up, so error recovery was unlikely.
MoveLeft3_0:
	travel r3
	travel r3
 	travel r3
	js MoveLeftSemifast_0
Dodge_0:
	// If near two drones, should always be able to get around them
	rand r1, 2
	travel r1
	travel r3
	travel r1
	travel [ InvertTable_0 + r1 ]
	travel r3
 	call CheckAndFix_0
MoveLeftSemifast_0:
	getxy r1, r2
	cmp r1, 7
	jg MoveLeft3_0
NearRow3_0:
	getxy r4, r5
	cmp r4, 3
	je AtRow3New_0
	travel r3
	js NearRow3_0
	poke 3, 72
	poke 3, 85

	poke 0, 72
	poke 1, 72
	poke 2, 72
	travel r3
	js NearRow3_0
	div r5, 20
	// xor r5, 1
	travel [ invertTable_0 + r5 ]
	travel r3
	travel [ invertTable_0 + r5 ]
	js NearRow3_0
	// Get enemies out of the way
	poke [ invertTable_0 + r5 ], 85
 	call CheckAndFix_0
	jmp NearRow3_0

CodeEnd_0:

_slack:
// overflow for when copying data.  Can also be used as general purpose
// short term storage.
_randomData1:
	data { 0 }
_randomData2:
	data { 0 }
_randomData3:
	data { 0 }


// Backup code should never run, except for the checksum part, though
// it is functional.
CodeStart_1:
SlaveInit_1:
	mov [HarvestProc_0], SlaveHarvest_0
	mov r11, [_randomData1]
	mov r12, [_randomData2]
	mov r5, [_randomData3]

GatherCashStart2_1:
	// Reduces chances of getting stuck on the mass of dead drones.
	mov r9, 10
	call GotoColumnR9_1
	mov r9, r5
	call GotoRowR9_1
GatherCashStart_1:
	push GatherCashStart_0
	getxy r0, r2
	and r2, 1
	xor r2, 1
	mov r1, 2
GatherCash_1:
GatherCashLoop_1:
	travel r2
	jns GatherCashSkip_1
	xor r2, 1
GatherCashSkip_1:
	travel r1
	js SameRow_1
	xor r1, 1
SameRow_1:
	call [HarvestProc_0]
	energy r3
	cmp r3, 55000
	jle GatherCash_1
	getxy r0, r5
	call LookForDumpStart_1
	mov r9, r5
	call GotoRowR9_1

	jmp SameRow_1


// Note:  May change column, uses r6-r8
// When done, new position is in r6, r7
GotoRowR9_1:
	getxy r6, r7
	sub r7, r9
	shr r7, 15
GotoRowR92_1:
	getxy r8, r6
	cmp r9, r6
	je Return_1
	sub r6, r9
	mult r6, [MTable_0 + r7]
	// Don't sit at destination as long this way
	push GotoRowR92_0
	jmp MoveToLoopStart_1

// Note:  May change row, uses r6-r8
// When done, new position is in r6, r7
GotoColumnR9_1:
	getxy r7, r8
	sub r7, r9
	shr r7, 15
	xor r7, 3
GotoColumnR92_1:
	getxy r6, r8
	cmp r9, r6
	je Return_1
	sub r6, r9
	mult r6, [MTable2_0 + r7]
	push GotoColumnR92_0
	//call MoveToLoopStart
	//jmp GotoColumnR92

// sub r6, r9
// mov r7, r6
// shr r7, 15
// xor r6, [table + r7]
// add r6, r7

// Tries to move r6 times in the r7 direction.
// Success not guaranteed, moves randomly perpendicularly on collision

MoveToLoopStart_1:
	mov r8, r6
	and r8, 0x7
	shr r6, 3
	jmp [ jmpTable_0 + r8 ]


MoveToLoop8_1:
MoveToLoop_1:
	sub r6, 1
	travel r7
MoveToLoop7_1:
	travel r7
MoveToLoop6_1:
	travel r7
MoveToLoop5_1:
	travel r7
MoveToLoop4_1:
	travel r7
MoveToLoop3_1:
	travel r7
MoveToLoop2_1:
	travel r7
MoveToLoop1_1:
	travel r7
	js [ jmpTable2_0 + r6 ]
	rand r8, 2
	xor r8, r7
	travel [xorTwoTable_0 + r8]
MoveToLoop0_1:
	jmp [ jmpTable2_0 + r6 ]

xorTwoTable_1:
	data { 2 3 0 1 }

jmpTable_1:
	data { MoveToLoop0_0 MoveToLoop1_0 MoveToLoop2_0 }
	data { MoveToLoop3_0 MoveToLoop4_0 MoveToLoop5_0 }
	data { MoveToLoop6_0 MoveToLoop7_0 }
jmpTable2_1:
	data { Return_0 MoveToLoop8_0 MoveToLoop8_0 }
	data { MoveToLoop8_0 MoveToLoop8_0 MoveToLoop8_0 }
	data { MoveToLoop8_0 MoveToLoop8_0 MoveToLoop8_0 }
	data { MoveToLoop8_0 }

SlaveHarvest_1:
	eat
	travel r1
	eat
	travel r1
	eat
	travel r1
	eat
	travel r1
	eat
	travel r1
	eat
	travel r1
	js SlaveHarvest_1
Mtable2_1:
	// ret
	data { 5 }


MTable_1:
	data {1 0xFFFF 1}


LookForDumpStart_1:
	cmp r11, 69
	jle GoToKnownDump_1

GoToKnownDumpFail_1: // Aka look slowly for new dump
	rand r8, 2
	rand r9, 2
	// add r8, 2

LookForDumpCheck_1:
	travel [ xorTwoTable_0 + r8 ]
	jns LookForDumpNextLine_1
LookForDumpCheck2_1:
	sense r3
	jns LookForDumpCheck_1
	cmp r3, 65535
	jl  LookForDumpCheck_1
	jmp DumpFound_1

LookForDumpNextLine_1:
	energy r3
	cmp r3, 12000
	jl GiveupDumpSearch_1
	xor r8, 1
	travel r9
	js LookForDumpCheck2_1
	xor r9, 1
	jmp LookForDumpNextLine_1


GotoKnownDump_1:
	mov r0, 0
GotoKnownDump2_1:
	add r0, 1
	cmp r0, 2
	jg GotoKnownDumpFail_1
	mov r9, r11
	call GotoColumnR9_1
	mov r9, r12
	call GotoRowR9_1

	sense r3
	jns GotoKnownDump2_1
	cmp r3, 65535
	jl GotoKnownDump2_1 // jl is more robust than je, in case
	                  // of corruption.  Seen the above number modified.

DumpFound_1:
	energy r3
	sub r3, 7000
	release r3
	getxy r11, r12
	travel 1
	travel 2
GiveupDumpSearch_1:

SlaveCodeEnd_1:
Return_1: // General purpose return.
	ret

AtRow3New_1:
	// A little extra security.
	// Means if we ever jump somewhere near the start, can get back.
	//jmp SkipDroneTest
	pop r1
DroneTest_1:
	push DroneTest_0
	getxy r4, r1
	div r1, 20

	// Needed here to prevent carrying messages prematurely
MoveOver_1:
	travel r1
	js MoveOver_1
	// one line to clear both.  No, it doesn't really matter.
	getxy [_messageFromAbove], [_messageFromBelow]

	getxy r4, r5
	mov r12, 0

	cmp r5, 0
	jne MiniSkip1_1
	or r12, 1
	mov [_messageFromAbove], 0xF00
MiniSkip1_1:
	cmp r5, 39
	jne MiniSkip2_1
	or r12, 2
	mov [_messageFromBelow], 0xF00
MiniSkip2_1:

	sub r5, 15
	cmp r5, 10
	jge Stay_1
	travel 3
	js Wait2_1
	or r12, 4
	Stay_1:

	// Putting this here is a minor optimization.
	// Slows down most bots, but means the second row
	// fills in a bit faster.


AtRow3Loop_1:
	test r12, 4
	je StayStill_1
	travel 3
	js Wait2_1
StayStill_1:
	cmp [_messageFromAbove], 0xF00
	jl SkipAttackCheck_1 // Don't need to poke, either...Minor speedup
	cmp [_messageFromBelow], 0xF00
	mov r0, [_messageFromAbove]
	poke 1, _messageFromAbove
SkipAttackCheck_1:
	mov r0, [_messageFromBelow]
	poke 0, _messageFromBelow
	// Have to poke before attack.
	jge Attack_1
	test r12, 1 // 2 steps when have a neighbor, 8 steps when don't.  better than always 6.
	jne SkipAboveDroneTest_1

	mov r0, 0
	peek r0, IFF_0
	jns SkipAboveDroneTest_1
	or r12, 1
	cmp r0, [IFF_0]
	je SkipAboveDroneTest_1
	mov r9, 0
	call DisableR9_1
	xor r12, 1

SkipAboveDroneTest_1:
	test r12, 2
	jne SkipDroneTest_1

	mov r0, 1
	peek r0, IFF_0
	jns SkipDroneTest_1
	or r12, 2
	cmp r0, [IFF_0]
	je SkipDroneTest_1
	mov r9, 1
	call DisableR9_1
	xor r12, 2

SkipDroneTest_1:
	mov r9, 2
	call CheckAndDisableR9_1
	mov r0, 103
	poke 2, 85
SkipDroneTest2_1:
	mov r9, 3
	call CheckAndDisableR9_1
	cmp [_messageFromAbove+r1], 0xF00
	travel r1
	js MoveOver_1
	jmp AtRow3Loop_1

AttackRecover_1:
	push AttackRecover_0
	jmp AttackTryMove_1

Attack_1:
	mov [sp], AttackRecover_0
	call CheckAndFix_1
	mov r10, 0xF01
	mov r0, r10
	poke 3, _MessageFromAbove
	// Just in case.
	poke 1, _MessageFromAbove
	poke 0, _MessageFromBelow

	getxy r0, r1
	cmp r1, 0
	jne AttackMiniSkip1_1
	mov [_messageFromAbove], 0xFFFF
	mov [_messageFromAbove3], 0x0300
AttackMiniSkip1_1:
	cmp r1, 39
	jne AttackMiniSkip2_1
	mov [_messageFromBelow], 0xFFFF
	mov [_messageFromBelow3], 0x00F0
AttackMiniSkip2_1:
AttackMoved_1:
	add r10, 1
	sense r0
	jns AttackTryMove_1
	cmp r0, 0xFFFF
	jne AttackTryMove2_1
	getxy r11, r12

AttackTryMove2_1:
AttackTryMove_1:
	mov [_attackStatus], r10
	// Fastest way to check for something.  NearRow3 is harmless to
	// friendly and enemy bots.  Trying to do damage without knowing
	// what's in r0 is a *bad* idea.  Accidentally using part of a
	// jump command left over from attacking another drone, for example,
	// which I would never, ever do, even by accident, could be
	// interesting.  Completely random example, without any personal
	// experience in the matter of course.
	poke 2, 74
	js AttackDisable_1

	cmp [_messageFromAbove], r10
	jge AttackAboveHappy_1
	mov r0, 0
	// Message prevents getting confused because
	// of harvesters in back.
	peek r0, _attackStatus
	jns AttackUnhappy_1
	cmp r0, r10
	je AttackAboveHappy2_1
	mov r9, 0
	call DisableR9_1
	jmp AttackTryMove_1
AttackAboveHappy2_1:
	mov [_messageFromAbove], r10
AttackAboveHappy_1:
	cmp [_messageFromBelow], r10
	jge AttackBelowHappy_1
	mov r0, 1
	peek r0, _attackStatus
	jns AttackUnhappy_1
	cmp r0, r10
	je AttackBelowHappy2_1
	mov r9, 1
	call DisableR9_1
	jmp AttackTryMove_1
AttackBelowHappy2_1:
	mov [_messageFromBelow], r10
AttackBelowHappy_1:
	mov r0, r10
	poke 0, _MessageFromBelow
	poke 1, _MessageFromAbove

	travel 2
	js AttackMoved_1
CheckIfAttackDone_1:
	getxy r4, r5
	cmp r4, 69
	je AttackDone_1

AttackUnhappy_1:
AttackDisable_1:
	mov r0, 104
	poke 2, 85
	jns AttackTryMove_1 // Drone moves left only, so he's stuck.  Nana.
	poke 2, 72
	mov r0, 0xFFFD
	poke 2, 88
	mov r0, 0x8006
	poke 2, 87
	call CheckAndFix_1

AttackTryDodge_1:
	mov r0, r10
	poke 0,_MessageFromBelow
	poke 1,_MessageFromAbove
AttackTryDodge2_1:
	// Wait until I've received the ok from
	// both neighbors.  Otherwise, dodging
	// can confuse me, or my neighbors' neighbors
	// (Which incldues me, I suppose).
	cmp [_MessageFromAbove], r10
	jl AttackTryDodge_1
	poke 0, _MessageFromBelow
AttackTryDodge4_1:
	cmp [_MessageFromBelow], r10
	jl AttackTryDodge4_1
	poke 1, _MessageFromAbove

AttackTryDodge3_1:
	travel 1
	mov r4, 0
	js AttackDodged2_1
	travel 0
	mov r4, 1
	jns AttackTryDodge3_1
AttackDodged2_1:
AttackDodged_1:
	travel r4
	js AttackTryMove_1
	jmp AttackDodged_1

AttackDone_1:
	cmp r11, 80
	jl PassData_1
WaitForData_1:
	mov r0, [_MessageFromBelow2]
	poke 0, _MessageFromBelow2
	mov r0, [_MessageFromBelow3]
	poke 0, _MessageFromBelow3
	mov r0, [_MessageFromAbove2]
	poke 1, _MessageFromAbove2
	mov r0, [_MessageFromAbove3]
	poke 1, _MessageFromAbove3

	add r0, [_MessageFromBelow3]
	cmp r0, 0x0400
	jge WaitForData_1

	// Note:  Location still in r4, r5

	// If ClosestAbove + ClosestBelow - 2 * y is > 0,
	// then above is closest.  Otherwise, below is.
	// Note that I'm treating numbers as 10-bit signed numbers.
	// Therefore, 0x300 is quit negative and farther away than
	// any valid row but still avoids overly.  Conveniently, 
	// I can also use my sums to 0x400 test above.
	// A lot of effort to save 2 or 3 lines of code, but I'm getting
	// low on space.
	sub r0, r5
	sub r0, r5
	shr r0, 9
	and r0, 1

	mov r11, [_MessageFromAbove2+r0]
	mov r12, [_MessageFromAbove3+r0]

PassData_1:
	mov r0, r11
	poke 0, _MessageFromBelow2
	poke 1, _MessageFromAbove2
	mov r0, r12
	poke 0, _MessageFromBelow3
	poke 1, _MessageFromAbove3
MessagingDone_1:
	call CheckAndFix_1
	// Location still in r4, r5
	test r5, 3
	jne GatherCashStart_1
	jmp AssymilateStart_1
Wait2_1:
	getxy r4, r5
	// r5 is our ID.  Now *EVERYONE* has a unique position.
	// This means that I can assign them final harvesting
	// spots based solely on their location in this dance.
	// Downside:  If anything goes wrong, entire operation
	// fails.
Wait3_1:
	call CheckAndFix_1
	mov r0, 104
	poke 0, 85
	poke 0, 72
	poke 1, 85
	poke 1, 72
	poke 3, 85
	poke 3, 72
	mov r0, 103
	poke 2, 85
	poke 2, 72
	cmp [_MessageFromAbove], 0xF00
	jle Wait3_1

// All I want is to disable any stragglers.
// Can't think of a better way to do this without
// a lot more code, possibly having my 10 stragglers form
// up in the middle and doing an attack run just like
// everyone else....Ick.
Cleanup_1:
	// Location still in r4, r5
	mov r1, r5
	div r1, 20

	// Column r9 is just to spread them out to disable anything
	// that might have been hiding.  Table is smaller and does better
	// than the code to generate something comparable.

	mov r9, [spreadTablePrefix_0 + r5]
	call GoToColumnR9_1
	
	// r5 is 4 * i + 2, where i ranges from 0 to 9.  It's the column where
	// our slave will end up.

	mult r5, 4
	sub r5, 58

CleanupDisable_1:
	mov r9, 2
	call CheckAndDisableR9_1
	mov r9, 3
	call CheckAndDisableR9_1
	travel r1
	js CleanupDisable_1
	mov r9, r1
	call CheckAndDisableR9_1
	getxy r4, r3

	// Saves a line to do it this way.
	sub r3, 2
	cmp r3, 36
	jge AssymilateStart2_1
	rand r9, 3
	call GotoColumnR9_1
	jmp CleanupDisable_1

AssymilateStart_1:
AssymilateStart2_1:
	// This causes the first drones to take rows which only have one other bot, initially,
	// at least it does in theory.
	xor r5, 2
	mov r3, 1
	jmp AssymilateFlip_1
Assymilate_1:
	travel 3
	js Assymilate_1
	mov r9, 3
	call TryAssimilateR9_1
	cmp r0, 1
	je GatherCashStart2_1
AssymilateSkip_1:
	travel r3
	js Assymilate_1
	mov r9, r3
	call TryAssimilateR9_1
	cmp r0, 1
	je GatherCashStart2_1
AssymilateSkip2_1:
	getxy r6, r7
	travel 2
	jns AssymilateFlip_1
	cmp r6, 2
	jle AssymilateSkip_1
AssymilateFlip_1:
	xor r3, 1
	// Assymilation takes tons of energy.
	// Worth putting it off a bit to be sure of survival.
	// Give some energy to new slave, too, just in case.
	energy r6
	cmp r6, 8000
	jg Flip2_1
	mov r1, 3
	call DefaultHarvest2_1
	mov r1, 2
	call DefaultHarvest2_1
	travel r3
	travel r1
	travel r3
	jmp AssymilateFlip_1
Flip2_1:
	jmp Assymilate_1

TryAssimilateR9_1:
	mov r2, r9
	mov r0, 0
	poke r9, _DroneMove2
	jns Return_1
	peek r2, IFF_0
	jns Return_1
	cmp r2, [IFF_0]
	jge Return_1

	mov r0, 0xCAFE
	poke r9, IFF_0
	mov r0, r5
	poke r9, _randomData3

	call DisableR9_1

	mov r0, r11
	poke r9, _randomData1
	mov r0, r12
	poke r9, _randomData2

	// Check if someone else had the temerity to claim the
	// bot just as we did.  If so, and he won, fail.
	// 6 cycles from check IFF to write, 8 to overwrite r5,
	// and over 10 for disable + writing release spot, so
	// shouldn't be a problem.
	mov r0, r9
	peek r0, _randomData3
	cmp r0, r5
	mov r0, 0
	jne Return_1




	// Reduces chance of food problems from
	// 4 buddies sharing a row too early.
	// I switch because for the first 10, this helps
	// fill in the pairs of rows with only one bot.
	xor r5, 2
	mov [SlaveStart_0], 0

	mov r2, SlaveCodeEnd_0

CopyLoop_1:
	mov r0, [r2]
	poke r9, r2
	sub r2, 1
	mov r0, [r2]
	poke r9, r2
	sub r2, 1
	cmp r2, SlaveStart_0
	jge CopyLoop_1

	charge r9, 1300

	mov r0, 1
	// ret.  Saves me a couple words to stick in with some data.
	data { 5 }

FoodTablePrefix_1:
	data { EnergyRelease_0 }
FoodTable_1: // 32 words, not counting prefix
	data { UnknownFood_0 UnknownFood_0 UnknownFood_0 UnknownFood_0 }
	data { UnknownFood_0 UnknownFood_0 UnknownFood_0 UnknownFood_0 }
	data { UnknownFood_0 UnknownFood_0 UnknownFood_0 UnknownFood_0 }
	data { UnknownFood_0 UnknownFood_0 UnknownFood_0 UnknownFood_0 }
	data { UnknownFood_0 }
// 15 before actual table
SpreadTablePrefix_1:
	data { UnknownFood_0 UnknownFood_0 UnknownFood_0 }
	data { UnknownFood_0 UnknownFood_0 UnknownFood_0 UnknownFood_0 }
	data { UnknownFood_0 UnknownFood_0 UnknownFood_0 UnknownFood_0 }
	data { UnknownFood_0 UnknownFood_0 UnknownFood_0 UnknownFood_0 }

SpreadTable_1:
	data { 1 2 0 2 1 }
	data { 1 2 0 2 1 }

EndTable0_1:
	data { CodeStart_0 CodeEnd_0 }
EndTable1_1:
	data { CodeStart_1 CodeEnd_1 }
	data { CodeStart_0 CodeEnd_0 }




CheckAndDisableR9_1:
	mov r8, r9
	peek r8, IFF_0
	jns Return_1
	cmp r8, [IFF_0]
	je Return_1

DisableR9_1:
	mov r0, 104
	poke r9, 85
	poke r9, 72
	mov r0, 0xFFFD
	poke r9, 88
	mov r0, 0x8006
	poke r9, 87
	// Go on into repair code, just in case.



// Not as fast as it could be, but the goal is to jump to
// the duplicate function ASAP if its checksum isn't right.
CheckAndFix_1:
	mov r6, CodeStart_0
CheckAndFix2_1: // marginally safer, if r6 is already set
	// Fewest possible number of steps to get out of here if
	// something's wrong.
	cksum r6, CodeEnd_0
	cmp r6, r13
	jne [BackupGoodRepair_1]

	// If we're running our backup repair code, and
	// then notice it's been modified...This might help, though
	// at the moment, I don't save the checksum I decide I like
	// until the end, so it's not too likely.  Still, can end here
	// in emergencies.  Best to be prepared.
	jmp [OriginalGoodRepair_0]

	// Used to have two functions for this.  By having 1,
	// I save about 3 lines of code.  Should require quite
	// so many lines, really.  Oh well, if I get desperate,
	// I can hide data in rets or something.

BackupRepair_1: // Saves an op in some very few cases. Pretty
	          // pointless, actually
	mov r8, EndTable1_1 // Pun Intended
Repair_1:
	mov r7, CodeStart_1
	cksum r7, CodeEnd_1
	cmp r6, r7
	je CheckHappy_1

	cmp r6, r13
	jne BackupGood_1
	mov r8, EndTable0_0
BackupGood_1:

	mov r0, [r8]
	mov [_randomData3], [r8+1]
	mov r6, [r8+2]
	mov r7, [r8+3]
BeginFix_1:
	mov r8, r0
	add r8, [_randomData3]
	shr r8, 1

	mov r9, r6
	add r9, r7
	shr r9, 1

	mov [_randomData1], r8
	cksum [_randomData1], [_randomData3]
	mov [_randomData2], r9
	cksum [_randomData2], r7
	cmp [_randomData1], [_randomData2]
	// This fall through method works best.  If for some
	// reason r10 < r0 or r7 < r6, we should still be able to
	// exit the loop, as opposed to getting stuck, even
	// if our code has already been fixed.
	jne SecondHalfBad_1
FirstHalfBad_1:
	mov [_randomData3], r8
	mov r7, r9
	jmp ZeroIn_1
SecondHalfBad_1:
	mov r0, r8
	mov r6, r9
	mov r8, [_randomData3]
ZeroIn_1:
	sub r8, r0
	cmp r8, 4
	jg BeginFix_1
	mov [r6], [r0]
	mov [r6+1], [r0+1]
	mov [r6+2], [r0+2]
	mov [r6+3], [r0+3]
	jmp CheckAndFix_1

CheckHappy_1:
	mov r13, r7
	// ret.  Making this shorter saves a little memory, as I can store
	// some values here.
	data { 5 }

IFF_1:  // Cool secret word...The drones will never figure it out!
      // Used to use checksum (Stored in r13), but corrupted bots could
      // cause problems...Still can, but now there's less chance.
	data { 0xCAFE }

HarvestProc_1:
	data { DefaultHarvest2_0 }

SafeFood_1:
	eat
	jns Return_1
DefaultHarvest_1:
	travel r1
	jns Return_1
DefaultHarvest2_1:
	sense r10
	js [FoodTable_0 + r10]
	travel r1
	sense r10
	js [FoodTable_0 + r10]
	travel r1
	sense r10
	js [FoodTable_0 + r10]
	travel r1
	sense r10
	js [FoodTable_0 + r10]
DangerousFood_1:
	travel r1
	js DefaultHarvest2_1
	// ret.  Save a bit if I stick some data after it.  I need space
	// to play with.
	data { 5 }

OriginalGoodRepair_1:
	data { Repair_0 }

BackupGoodRepair_1:
	data { BackupRepair_1 }

// If there's a release point on a bot's row, no reason to release too much
EnergyRelease_1:
	energy r0
	sub r0, 15000
	release r0
	jmp DefaultHarvest_1

UnknownFood_1:
	nop
UnknownFood2_1:
	mov r8, 0
	mov r9, 0
	mov r6, CodeStart_0
	cksum r8, 3600
	eat
	cksum r9, 3600
	call CheckAndFix2_1
	cmp r8, r9
	je UpdateSafe_1
	mov [FoodTable_0 + r10], DangerousFood_0
	mov [FoodTable_1 + r10], DangerousFood_0
	jmp UpdateChecksum_1
UpdateSafe_1:
	cmp [FoodTable_0 + r10], UnknownFood_0
	sub [FoodTable_0 + r10], 1
	sub [FoodTable_1 + r10], 1
	jg UpdateChecksum_1 // Not sure yet
	mov [FoodTable_0 + r10], SafeFood_0
	mov [FoodTable_1 + r10], SafeFood_0
UpdateChecksum_1:
	call CheckAndFix_1
	jmp DefaultHarvest_1

InvertTable_1:
	data {1 0}

// Basic idea is to move a lot, to make it less likely that a drone will damage us.
// This code used to not be backed up, so error recovery was unlikely.
MoveLeft3_1:
	travel r3
	travel r3
 	travel r3
	js MoveLeftSemifast_1
Dodge_1:
	// If near two drones, should always be able to get around them
	rand r1, 2
	travel r1
	travel r3
	travel r1
	travel [ InvertTable_0 + r1 ]
	travel r3
 	call CheckAndFix_1
MoveLeftSemifast_1:
	getxy r1, r2
	cmp r1, 7
	jg MoveLeft3_1
NearRow3_1:
	getxy r4, r5
	cmp r4, 3
	je AtRow3New_1
	travel r3
	js NearRow3_1
	poke 3, 72
	poke 3, 85

	poke 0, 72
	poke 1, 72
	poke 2, 72
	travel r3
	js NearRow3_1
	div r5, 20
	// xor r5, 1
	travel [ invertTable_0 + r5 ]
	travel r3
	travel [ invertTable_0 + r5 ]
	js NearRow3_1
	// Get enemies out of the way
	poke [ invertTable_0 + r5 ], 85
 	call CheckAndFix_1
	jmp NearRow3_1

CodeEnd_1:

// For fixing data, need slack as I copy 4 words at a time
_slack2:
	data { 1 1 1 }
	//Space for my call stack.  Shouldn't go too deep, unless I'm in deep.
_StackSlack:
	data { 1 1 1 }
	data { 1 1 1 }
