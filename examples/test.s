.intel_syntax noprefix
.global main

.text
function:
	push rbp
	mov rbp, rsp

	mov rax, 40
	mov rsp, rbp
	pop rbp
	ret

main:
	push rbp
	mov rbp, rsp



