.intel_syntax noprefix
.global main

.text
main:
	push rbp
	mov rbp, rsp

	sub rsp, 8
	mov rax, 5
	mov qword ptr [rbp - 8], rax

	mov rax, qword ptr [rbp - 8]
	mov rsp, rbp
	pop rbp
	ret
	add rsp, 8


