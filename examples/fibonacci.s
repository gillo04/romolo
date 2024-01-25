.intel_syntax noprefix
.global main

.text
main:
	push rbp
	mov rbp, rsp

	sub rsp, 8
	mov rax, 0
	mov qword [rbp - 8], rax

	sub rsp, 8
	mov rax, 1
	mov qword [rbp - 16], rax

	sub rsp, 8
	mov rax, 2
	mov qword [rbp - 24], rax

loop_0:
loop_cond_0:
	mov rax, 1

	cmp rax, 0
	je loop_end_0
	sub rsp, 8
	mov rax, qword [rbp - 8]
	mov rbx, qword [rbp - 16]
	add rax, rbx
	mov qword [rbp - 32], rax

	mov rax, qword [rbp - 16]
	mov qword [rbp - 8], rax

	mov rax, qword [rbp - 32]
	mov qword [rbp - 16], rax

	mov rax, qword [rbp - 24]
	inc rax
	mov qword [rbp - 24], rax

	mov rax, qword [rbp - 24]
	mov rbx, 10
	cmp rax, rbx
	setg al
	and rax, 1
	cmp rax, 0
	je if_else_1
	jmp loop_end_0
	jmp if_end_1
if_else_1:
	jmp loop_cond_0
if_end_1:
	add rsp, 8
	jmp loop_0
loop_end_0:

	mov rax, qword [rbp - 16]
	mov rsp, rbp
	pop rbp
	ret


