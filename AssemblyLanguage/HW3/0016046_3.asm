TITLE ASM_HW2_3 (main.asm)

;f(n)=0,if n<=3
;f(n)=f(n-3)+f(n-4)+7,if n>3

INCLUDE Irvine32.inc

.code

func PROC

    push    ebp
	mov     ebp,esp
	mov     eax,[ebp+8]
	cmp     eax,3
	ja      L1
	jmp     L2

L1: add     ebx,7
    sub     eax,3
    push    eax
	call    func
	mov     eax,[ebp+8]
	sub     eax,4
	push    eax
	call    func
	
L2: pop     ebp
    ret     4
		
func ENDP

main PROC

    call    ReadInt
	mov     n,eax
	mov     ebx,0
	push    eax
	call    func
	mov     eax,ebx
	call    WriteInt
	exit

main ENDP

END main