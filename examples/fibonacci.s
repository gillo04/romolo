.intel_syntax noprefix
.global main

.text
main:
	push rbp
	mov rbp, rsp

	sub rsp, 4
	mov eax, 0
	mov dword [rbp - 4], eax

	sub rsp, 4
	mov eax, 1
	mov dword [rbp - 8], eax

	sub rsp, 4
	mov eax, 2
	mov dword [rbp - 12], eax

loop_0:
loop_cond_0:
	mov eax, 1

	cmp eax, 0
	je loop_end_0
	sub rsp, 4
	mov eax, dword [rbp - 4]
	mov ebx, dword [rbp - 8]
	add eax, ebx
	mov dword [rbp - 16], eax

	mov eax, dword [rbp - 8]
	mov dword [rbp - 4], eax

	mov eax, dword [rbp - 16]
	mov dword [rbp - 8], eax

	mov eax, dword [rbp - 12]
	inc eax
	mov dword [rbp - 12], eax

	mov eax, dword [rbp - 12]
	mov ebx, 10
	cmp eax, ebx
	setg al
	and eax, 1
	cmp eax, 0
	je if_else_1
	jmp loop_end_0
	jmp if_end_1
if_else_1:
	jmp loop_cond_0
if_end_1:
	add rsp, 4
	jmp loop_0
loop_end_0:

	mov eax, dword [rbp - 8]
	mov rsp, rbp
	pop rbp
	ret


