info: SampleBot, John Doe, youremail@your-domain.com, 310 555-1212, UCLA

main:
  L00:  call randomize      ; (8004 000F 0000)

begin:
  L03:  call consume        ; (8004 0018 0000)
  L06:  call attack         ; (8004 002A 0000)
  L09:  call move           ; (8004 0045 0000)
  L12:  jmp begin           ; (8006 FFF7 0000)

randomize:
  L15:  rand [3579], 4      ; (2020 0DFB 0004)
  L18:  rand [3580], 10     ; (2020 0DFC 000A)
  L21:  add [3580], 1       ; (200F 0DFC 0001)
  L24:  ret                 ; (0005 0000 0000)

consume:
  L27:  sense r2            ; (401E 0002 0000)
  L30:  jns L45             ; (800E 000F 0000)
  L33:    energy r2         ; (401A 0002 0000)
  L36:    cmp r2, 10000     ; (6017 0002 2710)
  L39:    jg L45            ; (8009 0006 0000)
  L42:      eat             ; (001F 0000 0000)
  L45:  ret                 ; (0005 0000 0000)

attack:
  L48:  mov r0, [3579]      ; (4001 0000 0DFB)
  L51:  rand r1, 3576       ; (6020 0001 0DF8)
  L54:  peek r0, r1         ; (5024 0000 0001)
  L57:  jns L75             ; (800E 0012 0000)
  L60:    cmp r0, [r1]      ; (7017 0000 1000)
  L63:    je L75            ; (800B 000C 0000)
  L66:      rand r1, 3600   ; (6020 0001 0E10)
  L69:      mov r0, [r1]    ; (7001 0000 1000)
  L72:      poke [3579], r1 ; (1023 0DFB 0001)
  L75:  ret                 ; (0005 0000 0000)

move:
  L78:  cmp [3580], 0       ; (2017 0DFC 0000)
  L81:  je L96              ; (800B 000F 0000)
  L84:    travel [3579]     ; (001B 0DFB 0000)
  L87:    jns L96           ; (800E 0009 0000)
  L90:      sub [3580], 1   ; (2010 0DFC 0001)
  L93:      ret             ; (0005 0000 0000)
  L96:  call randomize      ; (8004 FFAF 0000)
  L99:  jmp move            ; (8006 FFEB 0000)
