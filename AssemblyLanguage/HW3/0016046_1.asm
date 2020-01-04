TITLE ASM_HW2_1 (main.asm)

;prime

INCLUDE Irvine32.inc

.data

m    DWORD ?
yes  BYTE  "Yes",0
no   BYTE  "No",0

.code

main PROC

    call    ReadInt
	mov     m,eax
	mov     ebx,1

L1: inc     ebx
    cmp     ebx,m
    jae     P
    mov     eax,m
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