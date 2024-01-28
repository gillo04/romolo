bits 64
section .data

section .text
global main
main:
	push rbp
	mov rbp, rsp

	mov rax, 5
	mov rsp, rbp
	pop rbp
	mov ebx, eax
	mov eax, 1
	int 0x80


