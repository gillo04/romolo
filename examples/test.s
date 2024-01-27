.intel_syntax noprefix
.global main

.text
add:
	push rbp
	mov rbp, rsp
	sub rsp, 4
	mov dword ptr [rbp - 4], edi
	sub rsp, 4
	mov dword ptr [rbp - 8], esi

	mov eax, dword ptr [rbp - 4]
	mov ebx, dword ptr [rbp - 8]
	add eax, ebx
	mov rsp, rbp
	pop rbp
	ret

main:
	push rbp
	mov rbp, rsp

	mov eax, 40
	mov edi, eax
	mov eax, 2
	mov esi, eax
	call add
	mov rsp, rbp
	pop rbp
	ret
	add rsp, 4


