.intel_syntax noprefix
.global main
.text

main:
	mov rax, 4
	cmp rax, 0
	je if_end_0
	mov rax, 29
	ret
if_end_0:
	mov rax, 69
	ret


