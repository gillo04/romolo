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
	mov rax, 1
	cmp rax, 0
	je if_end_0
loop_1:
loop_cond_1:
	mov rax, 1

	cmp rax, 0
	je loop_end_1
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

	mov rax, [rbp - 24]
	mov rbx, 10
	cmp rax, rbx
	setg al
	and rax, 1
	cmp rax, 0
	je if_else_2
	jmp loop_end_1
	jmp if_end_2
if_else_2:
	jmp loop_cond_1
if_end_2:
	add rsp, 8
	jmp loop_1
loop_end_1:

if_end_0:
	mov rax, [rbp - 16]
	mov rsp, rbp
	pop rbp
	ret


