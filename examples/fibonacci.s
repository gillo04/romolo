.intel_syntax noprefix
.global main
.text

main:
	push rbp
	mov rbp, rsp

	sub rsp, 8
	mov rax, 0
	mov [rbp - 8], rax
	sub rsp, 8
	mov rax, 1
	mov [rbp - 16], rax
	sub rsp, 8
	mov rax, 2
	mov [rbp - 24], rax
while_0:
	mov rax, [rbp - 24]
	mov rbx, 10
	cmp rax, rbx
	setle al
	and rax, 1

	cmp rax, 0
	je while_end_0
	sub rsp, 8
	mov rax, [rbp - 8]
	mov rbx, [rbp - 16]
	add rax, rbx
	mov [rbp - 32], rax
	mov rax, [rbp - 16]
	mov [rbp - 8], rax

	mov rax, [rbp - 32]
	mov [rbp - 16], rax

	mov rax, [rbp - 24]
	inc rax
	mov [rbp - 24], rax

	add rsp, 8
	jmp while_0
while_end_0:
	sub rsp, 8
	mov rax, 0
	mov [rbp - 40], rax
	mov rax, [rbp - 16]
	mov rsp, rbp
	pop rbp
	ret
	add rsp, 0

