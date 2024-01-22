.intel_syntax noprefix
.global main
.text

main:
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
	add r14, r15
	add r13, r14
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
	ret


