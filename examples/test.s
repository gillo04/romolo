bits 64
extern puts
extern printf

section .data
lit_0 db "num: %d", 10, "", 0

section .text
global _start
_start:
; align stack
	call main
	mov ebx, eax
	mov eax, 1
	int 0x80
main:
	push rbp
	mov rbp, rsp

	mov rax, lit_0
	mov rdi, rax
	mov rax, 123
	mov rsi, rax
	call printf

	mov rax, 0
	mov rsp, rbp
	pop rbp
	ret


