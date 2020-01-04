TITLE ASM_HW2_2 (main.asm)

;coprime

INCLUDE Irvine32.inc

.data

m    DWORD ?
n    DWORD ?
min  DWORD ?
yes  BYTE  "Yes",0
no   BYTE  "No",0

.code

main PROC

    call    ReadInt
	mov     m,eax
	call    ReadInt
	mov     n,eax
	mov     ebx,1

	cmp     m,eax
	jbe     minm
	mov     min,eax
	jmp     L1
minm:
    mov     eax,m
    mov     min,eax

L1: inc     ebx
    cmp     ebx,min
    ja      P
	mov     eax,m
    mov     edx,0
	div     ebx
	cmp     edx,0
	jne     L1
	mov     eax,n
	mov     edx,0
	div     ebx
	cmp     edx,0
	je      NP
	loop    L1

P:  mov     edx,OFFSET yes
    call    WriteString
	exit

NP: mov     edx,OFFSET no
    call    WriteString
	exit

main ENDP

END main