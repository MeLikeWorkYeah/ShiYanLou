INITSEG = 0x9000
entry _start
_start:
	mov ah,#0x03
	xor bh,bh
	int 0x10

	mov ax,cs
	mov es,ax

	mov cx,#23
	mov bx,#0x0007
	mov bp,#msg2    !bp:str address
	mov ax,#0x1301
	int 0x10

	!---init ss:sp
	mov ax,#INITSEG
	mov ss,ax
	mov sp,#0xFF00

	!---get params
	!---cursor position to [0...1]
	mov ds,ax       !ds<-#INITSEG
	mov ah,#0x03
	xor bh,bh
	int 0x10
	mov [0],dx      !cursor position is in dx and it's word format
	!---memory size to [2...3]
	mov ah,#0x88
	int 0x15
	mov [2],ax
	!---disk param
	mov ax,#0x0000
	mov ds,ax
	lds si,[4*0x41]
	mov ax,#INITSEG
	mov es,ax
	mov di,#0x0004
	mov cx,#0x10
	rep 
	movsb

	!---Print
	!---Print Prepare
	mov ax,cs
	mov es,ax
	mov ax,#INITSEG
	mov ds,ax
	!---Cursor Position
	mov ah,#0x03
	xor bh,bh
	int 0x10
	mov cx,#18
	mov bx,#0x0007
	mov bp,#msg_cursor
	mov ax,#0x1301
	int 0x10
	mov dx,[0]
	call print_hex
	!---Memory Size
	mov ah,#0x03
	xor bh,bh
	int 0x10
	mov cx,#14
	mov bx,#0x0007
	mov bp,#msg_memory
	mov ax,#0x1301
	int 0x10
	call print_hex
	mov ah,#0x03
	xor bh,bh
	int 0x10
	mov cx,#2
	mov bx,#0x0007
	mov bp,#msg_kb
	mov ax,#0x1301
	int 0x10
	!---Cyles
	mov ah,#0x03
	xor bh,bh
	int 0x10
	mov cx,#7
	mov bx,#0x0007
	mov bp,#msg_cyles
	mov ax,#0x1301
	int 0x10
	mov dx,[4]
	call print_hex
	!---Heads
	mov ah,#0x03
	xor bh,bh
	int 0x10
	mov cx,#8
	mov bx,#0x0007
	mov bp,#msg_heads
	mov ax,#0x1301
	int 0x10
	mov dx,[6]
	call print_hex
	!---Sectors
	mov ah,#0x03
	xor bh,bh
	int 0x10
	mov cx,#10
	mov bx,#0x0007
	mov bp,#msg_sectors
	mov ax,#0x1301
	int 0x10
	mov dx,[12]
	call print_hex
inf_loop:
	jmp inf_loop
print_hex:
	mov cx,#4
print_digit:
	rol dx,#4
	mov ax,#0xe0f
	and al,dl
	add al,#0x30
	cmp al,#0x3a
	jl outp
	add al,#0x07
outp:
	int 0x10
	loop print_digit
	ret
print_nl:
	mov ax,#0xe0d
	int 0x10
	mov al,#0xa
	int 0x10
	ret

msg2:
	.ascii "Now we are in Setup"
	.byte 13,10,13,10
msg_cursor:
	.byte 13,10
	.ascii "Cursor position:"
msg_memory:
	.byte 13,10
	.ascii "Memory Size:"
msg_cyles:
	.byte 13,10
	.ascii "Cyls:"
msg_heads:
	.byte 13,10
	.ascii "Heads:"
msg_sectors:
	.byte 13,10
	.ascii "Sectors:"
msg_kb:
	.ascii "KB"

.org 510
boot_flag:
	.word 0xAA55
