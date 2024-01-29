bits 64

section .data

section .text
global main
push byte 8
main:
	push rbp
	mov rbp, rsp

	sub rsp, 4
	mov rax, 10
	mov dword [rbp - 4], eax

	mov eax, dword [rbp - 4]
	mov edi, eax
	call fib
	mov rsp, rbp
	pop rbp
	mov ebx, eax
	mov eax, 1
	int 0x80
	add rsp, 4

fib:
	push rbp
	mov rbp, rsp
	sub rsp, 4
	mov dword [rbp - 4], edi

	mov eax, dword [rbp - 4]
	mov rbx, 2
	cmp eax, ebx
	setl al
	and eax, 1
	cmp eax, 0
	je if_end_0
	mov eax, dword [rbp - 4]
	mov rsp, rbp
	pop rbp
	ret
if_end_0:
	mov eax, dword [rbp - 4]
	mov rbx, 1
	sub eax, ebx
	mov edi, eax
	call fib
	mov ebx, dword [rbp - 4]
	mov rcx, 2
	sub ebx, ecx
	mov edi, ebx
	mov ecx, eax
	mov eax, ebx
	push rcx
	call fib
	pop rcx
	add ecx, eax
	mov eax, ecx
	mov rsp, rbp
	pop rbp
	ret


