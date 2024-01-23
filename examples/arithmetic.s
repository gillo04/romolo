.intel_syntax noprefix
.global main
.text

main:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov rax, 5
	mov [rbp - 8], rax
	sub rsp, 16
	mov rax, 10
	mov [rbp - 16], rax
	mov rax, [rbp - 8]
	mov rbx, [rbp - 16]
	add rax, rbx
	mov rsp, rbp
	pop rbp
	ret


