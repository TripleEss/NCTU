TITLE ASM_HW2 (main.asm)

;Advanced Calculator

INCLUDE Irvine32.inc

.data

m   SDWORD ?
n   SDWORD ?
op  BYTE   2 DUP(0)

err BYTE "divided by zero",0

.code

main PROC

    call    ReadInt
	mov     m,eax

	mov     edx,OFFSET op
	mov     ecx,SIZEOF op
	call    ReadString
	
	call    ReadInt
	mov     n,eax

	cmp     op,"+"
	jz      addition
	cmp     op,"-"
	jz      substraction
	cmp     op,"*"
	jz      multiplication
	cmp     op,"/"
	jz      division
	cmp     op,"%"
    jz      modulo

    
addition:
    mov		eax,m
	add		eax,n
	call    WriteInt
	exit
    
substraction:
    mov		eax,m
	sub		eax,n
	call    WriteInt
	exit

multiplication:
    mov     eax,0
	mov     ecx,n

	cmp     m,0
	jz      mulout
	cmp     n,0
	jz      mulout
	jg      mul1
	neg     ecx

mul1:
    add     eax,m
    loop    mul1
	cmp     n,0
	jge     mulout
	neg     eax

mulout:
	call    WriteInt
	exit

division:
    cmp     n,0
	jz      div0

	mov     eax,0
	mov		ebx,0

	cmp     m,0
	jg      dposem
	jmp     dnegm
dposem:
	cmp     n,0
	jg      d1             ;+/+
	jmp     d2             ;+/-
dnegm:
    cmp     n,0
	jg      d3             ;-/+
	jmp     d4             ;-/-
	
 d1:add     ebx,n
    inc     eax
    cmp     ebx,m
	jbe     d1
	dec     eax
	jmp     dout
    
 d2:sub     ebx,n
    dec     eax
    cmp     ebx,m
	jbe     d2
	inc     eax
	jmp     dout

 d3:sub     ebx,n
    dec     eax
    cmp     ebx,m
	jge     d3
	inc     eax
	jmp     dout

 d4:add     ebx,n
    inc     eax
    cmp     ebx,m
	jge     d4
	dec     eax
	jmp     dout

dout:
    call    WriteInt
	exit

modulo:
    cmp     n,0
	jz      div0

	mov     eax,n
	mov     ebx,0

	cmp     m,0
	jg      mposem
	jmp     mnegm
mposem:
	cmp     n,0
	jg      m1              ;+%+
	jmp     m2              ;+%-
mnegm:
    cmp     n,0
	jg      m3              ;-%+
	jmp     m4              ;-%-


 m1:add     ebx,n
    cmp     ebx,m
	jbe     m1
	sub     ebx,m
	sub     eax,ebx
	jmp     mout
    
 m2:sub     ebx,n
    cmp     ebx,m
	jbe     m2
	sub     ebx,m
	neg     eax
	sub     eax,ebx
	jmp     mout

 m3:sub     ebx,n
    cmp     ebx,m
	jge     m3
	sub     ebx,m
	neg     eax
    sub     eax,ebx
	jmp     mout

 m4:add     ebx,n
    cmp     ebx,m
	jge     m4
	sub     ebx,m
	sub     eax,ebx
	jmp     mout

mout:
    call    WriteInt
	exit

div0:
    mov     edx,OFFSET err
	call    WriteString
    
	exit

main ENDP


END main