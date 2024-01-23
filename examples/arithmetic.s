.intel_syntax noprefix
.global main
.text

main:
	push rbp
	mov rbp, rsp
	mov rax, 1
	mov rbx, 2
	mov rcx, 3
	mov rdx, 4
	mov rsi, 5
	mov rdi, 6
	mov r8, 7
	mov r9, 8
	mov r10, 9
	mov r11, 10
	mov r12, 11
	mov r13, 12
	mov r14, 13
	mov r15, 14
	sub rsp, 8
	sub rsp, 8
	mov [rbp - 16], r15
	mov r15, [rbp - 8]
	mov r15, 15
	sub rsp, 8
	mov [rbp - 24], r14
	mov r14, [rbp - 16]
	add r14, r15
	mov r15, [rbp - 24]
	add rsp, 24
	add r15, r14
	add r13, r15
	add r12, r13
	add r11, r12
	add r10, r11
	add r9, r10
	add r8, r9
	add rdi, r8
	add rsi, rdi
	add rdx, rsi
	add rcx, rdx
	add rbx, rcx
	add rax, rbx
	mov rsp, rbp
	pop rbp
	ret


