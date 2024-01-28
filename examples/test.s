bits 64
extern puts

section .data
lit_0 db "Hello world", 0

section .text
global main
main:
	push rbp
	mov rbp, rsp

	sub rsp, 8
	mov rax, lit_0
	mov qword [rbp - 8], rax

	mov rax, qword [rbp - 8]
	mov rdi, rax
	call puts

	mov rax, 0
	mov rsp, rbp
	pop rbp
	mov ebx, eax
	mov eax, 1
	int 0x80
	add rsp, 8


