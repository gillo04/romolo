.intel_syntax noprefix
.global main

.text
main:
	push rbp
	mov rbp, rsp

	sub rsp, 4
	mov eax, 1
	mov dword ptr [rbp - 4], eax

	sub rsp, 4
	mov eax, 2
	mov dword ptr [rbp - 8], eax

	sub rsp, 4
	mov eax, 3
	mov dword ptr [rbp - 12], eax

	sub rsp, 4
	mov eax, 4
	mov dword ptr [rbp - 16], eax

	sub rsp, 4
	mov eax, 5
	mov dword ptr [rbp - 20], eax

	add rsp, 4
	sub rsp, 4
	mov eax, 5
	mov dword ptr [rbp - 20], eax

	add rsp, 12
	sub rsp, 4
	mov eax, 5
	mov dword ptr [rbp - 12], eax

	mov eax, 42
	mov rsp, rbp
	pop rbp
	ret
	add rsp, 12


