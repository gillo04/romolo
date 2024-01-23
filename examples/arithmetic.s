.intel_syntax noprefix
.global main
.text

main:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	mov rax, 0
	mov [rbp - 8], rax
	sub rsp, 16
	mov rax, 0
	mov [rbp - 16], rax
while_0:
	mov rax, [rbp - 8]
	mov rbx, 5
	cmp rax, rbx
	setl al
	and rax, 1
	cmp rax, 0
	je while_end_0
	mov rax, [rbp - 8]
	inc rax
	mov [rbp - 8], rax
	mov rax, [rbp - 16]
	mov rbx, 5
	add rax, rbx
	mov [rbp - 16], rax
	jmp while_0
while_end_0:
	mov rax, [rbp - 16]
	mov rsp, rbp
	pop rbp
	ret


