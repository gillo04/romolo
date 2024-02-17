bits 64

section .data

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
	mov rax, 1
	mov dword [rbp - 4], eax

	sub rsp, 8
	mov rax, 2
	mov qword [rbp - 12], rax

	sub rsp, 2
	mov rax, 3
	mov word [rbp - 14], ax

	sub rsp, 1
	mov rax, 4
	mov byte [rbp - 15], al

	sub rsp, 1
	mov rax, 5
	mov byte [rbp - 16], al

	mov rax, 0
	mov rsp, rbp
	pop rbp
	ret
	add rsp, 16


