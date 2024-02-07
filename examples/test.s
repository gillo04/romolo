bits 64
extern printf

section .data
lit_0 db "%d", 10, "", 0
lit_1 db "%d", 10, "", 0

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

	mov rax, lit_0
	mov ebx, dword [rbp - 4]
	mov rdi, rax
	mov esi, ebx
	sub rsp, 4
	call printf
	add rsp, 4

	mov rax, lit_1
	mov rbx, qword [rbp - 12]
	mov ecx, dword [rbx]
	mov rdi, rax
	mov esi, ecx
	sub rsp, 4
	call printf
	add rsp, 4

	mov rax, 0
	mov rsp, rbp
	pop rbp
	ret
	add rsp, 12


