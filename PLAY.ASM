	.MODEL  LARGE,PASCAL
	.CODE
	.radix	16
	.286
;	File	Play.asm
;	author	Paul van Keep (CIS id: 75170,1045)
;	(NC)Not Copyrighted by the Frobozz Magic Software Company
;
;	Sound sample play code
;	Works by programming timer 2 in mode 2
;	Generates 22000Hz carrier with square wave
;


PLAY	PROC	uses ds si, dat:PTR, len:WORD, frq:BYTE
	lds	si, dat
	mov	cx, len
	mov	bl, frq
	mov	dl, 1
	in	al, 21
	or	al, 1
	out	21, al
	mov	al, 98
	out	43, al
	xor	al, al
	out	42, al
	in	al, 61
	mov	dh, al			; save contents of 61
	or	al, 1
	out	61, al			; open speaker
	jmp	next_cycle
close_speaker:
	mov	bh, ah
	mov	al, 1
	out	61, al
wait_timer:
	in	al, 42
	cmp	al, 1A
	jb	next_cycle
	cmp	al, bh
	ja	wait_timer
	xor	bh, bh
	mov	al, 3
	out	61, al
	jmp	wait_timer
next_cycle:
	mov	al, 50
	out	42, al			; restart timer
	dec	dl
	jnz	close_speaker
	mov	dl, bl
	lodsb
	add	al, 4
	shr	al, 1
	shr	al, 1
	mov	ah, 50
	sub	ah, al
	loop	close_speaker
	in	al, 21
	and	al, 0fe
	out	21, al
	mov	al, dh
	out	61, al
	ret
PLAY	ENDP
	end
