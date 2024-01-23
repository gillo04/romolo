.intel_syntax noprefix
.global main
.text

main:
	push rbp
	mov rbp, rsp
	sub rsp, 4
	mov rax, 5
	mov [rbp - 4], rax
	mov rax, 0
	mov rsp, rbp
	pop rbp
	ret


