.intel_syntax noprefix
.global main
.text

main:
	push rbp
	mov rbp, rsp
	mov rax, 200
	mov rbx, 50
	mov rcx, 10
	mov rdx, rax
	mov rax, rbx
	mov rsi, rdx
	mov rdx, rbx
	xor rdx, rdx
	idiv rcx
	mov rbx, rax
	mov rax, rsi
	mov rdx, rcx
	xor rdx, rdx
	idiv rbx
	mov rbx, 10
	mov rdx, rcx
	xor rdx, rdx
	idiv rbx
	mov rbx, 3
	mov rdx, rcx
	xor rdx, rdx
	idiv rbx
	mov rax, rdx
	mov rsp, rbp
	pop rbp
	ret


