info: Chronos, Bryant Brownell, Oregon State University

; General features:
; * Remembers which sludge is toxic, and ignores it in the future
; * Reprograms the drones, copying its code onto them
; * If its own program code gets mutated, it waits to be repaired before continuing further
; * Contains optimized sludge-finding search algorithm, mixing up/down and left/right movement

; Speed-based features:
; * Stores the known location of a collection point, for quicker release when energy storage becomes full
; * Remembers which sludge is safe to eat, to bypass the check-for-damage routine
; * Stops searching for drones to repair after 80 direction turn-arounds
; * Code is organized to minimize unneccessary jmp usage

; Specifics:
; * Releases all but 3,000 energy at collection points
; * Charges adjacent NANORGS with 10,000 energy each time it hits one

; This program scored about 1,750,000,000 points on average for me, when tested in 100 tanks.
; It never scored below 1,500,000,000 in that run, and its best score was 2,138,714,578.

; Time I spent on this project:
; Feb. 22nd	3 hrs
; Feb. 23rd	5 hrs
; Feb. 24th	3.5 hrs
; Feb. 25th	2 hrs
; Feb. 26th	4 hrs
; Feb. 27th	5 hrs
; Feb. 28th	5.5 hrs
; Total:	28 hrs

; r0 is the temp variable, always being reused
; r1 tells if the bot should move left or right next
; r2 tells if the bot should move up or down next
; r3 holds the current checksum of all memory space
; r4 is used in cloning, holding the next memory to poke
; r5 holds how many times the bump routine has been called
; r6-r7 are the direction bounds. (4, 2) is used for left/right movement, (2, 4) for up/down
; r8-r11 are used in FindPt to locate a collection point
; r11 is also the sludge ID that was most recently eaten
; r12-r13 holds the location of a known collection point

;;;

	jmp	start		//skip the stun protection NOP commands

stunProtection:
	//the drones get poked in this memory area to stun them
	data { 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 }
	data { 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 }
	data { 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 }
	data { 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 }

start:
	mov	[kSum],	start	//get the correct checksum of the entire program
	cksum	[kSum],	kSum	//drones don't need to do this, because they get the cksum programmed into them

droneStart:
	mov	r1,	2	//set the starting directions (right, up)
	mov	r2,	0
	mov	r12,	0xFFFF	//mark the collection location as unknown

	mov	r6,	4	//set direction bounds correctly for left/right movement
	mov	r7,	2

	jmp	mainLoop	//initialization complete, start main loop

quickEat:			//this routine is placed here to speed up the quickEat action
	eat
	jns	findPt		//if this failed, bot has hit maximum energy, so find a collection point
//	jmp	mainLoop	//keep looking for more sludge (jmp not actually needed)

mainLoop:
	travel	r1
	jns	bump		//the bot ran into something!
	mainLoopNoMove:
	sense	r11
	cmp	r11,	0	//did it find a non-empty space?
	je	mainLoop	//no, move again
	cmp	r11,	0xFFFF	//is it a collection pt?
	jl	eatIt		//no, it's a food, so eat it, and deposit if needed

	getxy	r12,	r13	//it's a collection point, so remember this location for later
//	jmp	deposit		//drop off any extra energy (jmp not actually needed)

deposit:
	energy	r0
	sub	r0,	3000	//release all energy except for this amount
	release	r0
	jmp mainLoop

keepTrying:
	rand	r0,	4	//move whackily to circumvent any possible obstacle
	travel	r0
//	jmp	findPt		//look again for that collection point (jmp not actually needed)

findPt:
	cmp	r12,	0xFFFF	//does the bot know where a collection point is?
	je	mainLoop	//no, keep looking

	getxy	r10,	r11	//where is the bot now?

	mov	r8,	100	//put 100 in the slot
	sub	r8,	r10	//subtract by current location
	add	r8,	r12	//increase by the destination
	mov	r9,	100	//put 100 in the slot
	sub	r9,	r11	//subtract by current location
	add	r9,	r13	//increase by the destination

	checkUpDown:		//move up and down the correct number of times
	cmp	r9,	100
	jl	moveUp
	jg	moveDown

	checkLeftRight:		//move left and right the correct number of times
	cmp	r8,	100
	jl	moveLeft
	jg	moveRight

	
				//did it work?
	getxy	r10,	r11	//where is the bot now?
	cmp	r10,	r12
	jne	keepTrying	//didn't make it, so try again
	cmp	r11,	r13
	jne	keepTrying	//still didn't make it
	jmp	deposit		//successful arrival, now release proper amount of energy

	moveUp:			//moving UP routine
	travel	0
	add	r9,	1
	jmp	checkUpDown

	moveDown:		//moving DOWN routine
	travel	1
	sub	r9,	1
	jmp	checkUpDown

	moveRight:		//moving RIGHT routine
	travel	2
	sub	r8,	1
	jmp	checkLeftRight

	moveLeft:		//moving LEFT routine
	travel	3
	add	r8,	1
	jmp	checkLeftRight

eatIt:
	cmp	[toxic+r11],	0xFFFE
	jg	quickEat	//it's a known safe food, quickly snap it up and continue hunting
	je	mainLoop	//if this is a known toxin, ignore it!

	mov	r3,	0	//what's the current checksum?
	cksum	r3,	3599

	eat
	jns	findPt
	mov	r0,	0
	cksum	r0,	3599	//how's the checksum after that meal?
	cmp	r0,	r3
	je	safeFood	//if it's unchanged, mark it as safe for later

	mov	[toxic+r11],	0xFFFE	//that was toxic! Remember it
	jmp	checkDamage	//make sure my progam code wasn't affected

safeFood:
	mov	[toxic+r11],	0xFFFF	//this is a known safe sludge ID to eat
	jmp	mainLoop

randDirection:
	add	r5,	1	//add one last bump so this routine gets called only once
	rand	r0,	2	//get a random result...
	cmp	r0,	1
	je	continueBump	//half the time, don't do anything
	mov	r1,	0	//the other half of the time, switch to up/down movement
	mov	r2,	2
	mov	r6,	2	//change the direction bounds to match this new up/down movement
	mov	r7,	4	
//	jmp	bump		//direction switch complete! (jmp not actually needed)

bump:
	cmp	r5,	80	//allow a certain number of bumps, then stop trying to repair/clone
	jg	continueBump	//this is to speed up the bump routine
	je	randDirection	//once the bot hits 80 bumps, possibly switch it to up/down movement
	add	r5,	1	//if the limit hasn't been reached, keep counting bumps

	mov	r0,	0	//stun enemy bots in direction r1
	poke	r1,	13

	mov	r0,	r1
	peek	r0,	myID	//is this a robot?
	jns	continueBump	//no, continue bump routine
	cmp	r0,	1	//yes, now does it need to be reprogrammed?
	jne	clone		//yes, start cloning
				//otherwise, continue bump routine

	continueBump:
	charge	r1,	10000	//share energy in case bumped bot is hibernating

	add	r1,	1	//swap the main r1 direction
	cmp	r1,	r6	//if this went beyond the direction bounds...
	jne	skipR1Sub
	sub	r1,	2	//recalibrate r1 direction
	skipR1Sub:

	travel	r2		//get out of the way of the obstacle
	js	mainLoopNoMove	//if that didn't work...

	add	r2,	1	//swap the secondary r2 direction moving direction
	cmp	r2,	r7	//if this went beyond the direction bounds...
	jne	skipR2Sub
	sub	r2,	2	//recalibrate r2 direction
	SkipR2Sub:
	travel	r2		//get out of the way of the obstacle in the other direction
	
	jmp	mainLoopNoMove	//bump is complete!

clone:
	mov	r4,	start	//get starting location to poke
	mov	r0,	[start]	//get starting value to poke

	cloneLoop:		//loop...

	poke	r1,	r4	//poke the value at the location
	jns	checkDamage	//if it didn't work, the victim moved, so give up
	add	r4,	1
	mov	r0,	[r4]

	cmp	r4,	toxic	//is this the end of the program?
	jl	cloneLoop	//if not, keep poking

	mov	r0,	1	//set the drone's ID to say "don't need cloning"
	poke	r1,	myId
	
	mov	r0,	droneStart
	sub	r0,	13	//offset correctly, because it's relative memory addressing
	poke	r1,	13	//start the victim!
//	jmp	checkDamage	//bot has been sitting in one place, so see if it's been damaged by a drone (jmp not actually needed)

checkDamage:
	mov	r0,	start	//is the program code affected?
	cksum	r0,	kSum
	cmp	r0,	[kSum]
	je	mainLoop	//nope, keep going as normal
//	jmp	waitForRepair	//oops, it's broken, so stop and wait to get fixed (jmp not actually needed)

waitForRepair:
	mov	[myId],	0	//tell everyone that I need repair
	waitingForRepairLoop:	//loop...
	mov	r0,	start	//get the current checksum
	cksum	r0,	kSum
	sub	r0,	[kSum]	//find the difference between it and the "correct" checksum
	sub	r0,	1	//wrap the number to 0xFFFF
	cmp	r0,	0xFFFE	//if it is within 2, the bot is repaired! (allowing margin of error)
	jle	waitingForRepairLoop	//otherwise, keep waiting to be repaired

	jmp	mainLoop	//repair complete

kSum:
	data { 0 }		//this stores the correct checksum of the in-use program space

toxic:
	data { 0 }		//safety buffer
	data { 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 }	//32 slots, one for each sludge ID

myId:
	data { 1 }	//0 means "needs repair or programming"
			//1 means "ignore me"

end:
	data { 0 }