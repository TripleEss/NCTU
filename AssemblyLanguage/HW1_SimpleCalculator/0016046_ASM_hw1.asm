TITLE ASM_HW1 (main.asm)

;Simple Calculator

INCLUDE Irvine32.inc

.data
m	DWORD  ?
n	DWORD  ?
sum	DWORD  0
dif DWORD  0
pro DWORD  0
quo DWORD  0

_sum DWORD ' + '
_dif DWORD ' - '
_pro DWORD ' * '
_quo DWORD ' / '
_eq  DWORD ' = '

.code

main PROC

	call	ReadDec
	mov		m,eax
	call	ReadDec
	mov		n,eax

	;addition: m+n=sum
	mov		eax,m
	add		eax,n
	mov		sum,eax

	;substraction: m-n=dif
	mov		eax,m
	sub		eax,n
	mov		dif,eax
	
	;multiplication: m*n=pro
	mov		eax,0
	mov		ecx,n

m1:	add		eax,m
	loop m1

	mov		pro,eax

	;division: m/n=quo
	mov		eax,0
	mov		ecx,m
	mov		ebx,n
	sub		ebx,1

d1:	sub		ecx,ebx
	inc		eax
	loop d1

	mov		quo,eax

	;representation
	mov		eax,m
	call	WriteDec
	mov		edx,OFFSET _sum
	call	WriteString
	mov		eax,n
	call	WriteDec
	mov		edx,OFFSET _eq
	call	WriteString
	mov		eax,sum
	call	WriteDec
	call	Crlf

	mov		eax,m
	call	WriteDec
	mov		edx,OFFSET _dif
	call	WriteString
	mov		eax,n
	call	WriteDec
	mov		edx,OFFSET _eq
	call	WriteString
	mov		eax,dif
	call	WriteDec
	call	Crlf

	mov		eax,m
	call	WriteDec
	mov		edx,OFFSET _pro
	call	WriteString
	mov		eax,n
	call	WriteDec
	mov		edx,OFFSET _eq
	call	WriteString
	mov		eax,pro
	call	WriteDec
	call	Crlf

	mov		eax,m
	call	WriteDec
	mov		edx,OFFSET _quo
	call	WriteString
	mov		eax,n
	call	WriteDec
	mov		edx,OFFSET _eq
	call	WriteString
	mov		eax,quo
	call	WriteDec
	call	Crlf

	exit

main ENDP

END main 