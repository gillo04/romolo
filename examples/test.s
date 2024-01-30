bits 64
extern printf

section .data
lit_0 db "num: %d", 10, "", 0

section .text
global _start
_start:
	call main
	mov ebx, eax
	mov eax, 1
	int 0x80
main:
	push rbp
	mov rbp, rsp

	sub rsp, 4
	mov rax, 1234
	mov dword [rbp - 4], eax

	sub rsp, 8
	mov rax, rbp
	sub rax, 4
	mov qword [rbp - 12], rax

	sub rsp, 8

	mov rax, lit_0
	mov rbx, qword [rbp - 12]
	mov ecx, dword [rbx]
	mov rdi, rax
	mov esi, ecx
	sub rsp, 12
	call printf
	add rsp, 12

	mov rax, 0
	mov rsp, rbp
	pop rbp
	ret
	add rsp, 20


