.intel_syntax noprefix
.global main

.text
	sub rsp, -1

main:
	push rbp
	mov rbp, rsp


function:
	push rbp
	mov rbp, rsp

	mov eax, 40
	mov rsp, rbp
	pop rbp
	ret


