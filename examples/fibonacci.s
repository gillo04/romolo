.intel_syntax noprefix
.global main

.text
fib:
	push rbp
	mov rbp, rsp
	sub rsp, 4
	mov dword ptr [rbp - 4], edi

	mov eax, dword ptr [rbp - 4]
	mov ebx, 2
	cmp eax, ebx
	setl al
	and eax, 1
	cmp eax, 0
	je if_end_0
	mov eax, dword ptr [rbp - 4]
	mov rsp, rbp
	pop rbp
	ret
if_end_0:
	mov eax, dword ptr [rbp - 4]
	mov ebx, 1
	sub eax, ebx
	mov edi, eax
	call fib
	mov ebx, dword ptr [rbp - 4]
	mov ecx, 2
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

main:
	push rbp
	mov rbp, rsp

	mov eax, 10
	mov edi, eax
	call fib
	mov rsp, rbp
	pop rbp
	ret


