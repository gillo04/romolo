bits 64
extern printf

section .data
lit_0 db "Fibonacci sequence from 0 to 10:", 10, "", 0
lit_2 db "%hu: %d", 10, "", 0

section .text
global _start
_start:
	call main
	mov ebx, eax
	mov eax, 1
	int 0x80
main:
	push rbp
	mov rbp, rsp

	mov rax, lit_0
	mov ebx, 0
	mov ecx, 0
	mov rdi, rax
	mov esi, ebx
	mov edx, ecx
	call printf

	sub rsp, 2
	mov eax, 0
	mov word [rbp - 2], ax


loop_1:
loop_cond_1:
	mov ax, word [rbp - 2]
	mov ebx, 10
	cmp ax, bx
	setle al
	and ax, 1

	cmp ax, 0
	je loop_end_1
	mov rax, lit_2
	mov bx, word [rbp - 2]
	mov cx, word [rbp - 2]
	mov di, cx
	mov rdx, rax
	mov eax, ecx
	push rdx
	push rbx
	sub rsp, 14
	call fib
	add rsp, 14
	pop rbx
	pop rdx
	mov rdi, rdx
	mov si, bx
	mov edx, eax
	sub rsp, 14
	call printf
	add rsp, 14

	mov ax, word [rbp - 2]
	inc ax
	mov word [rbp - 2], ax
	jmp loop_1
loop_end_1:

	add rsp, 2
	mov eax, 0
	mov rsp, rbp
	pop rbp
	ret

fib:
	push rbp
	mov rbp, rsp
	sub rsp, 2
	mov word [rbp - 2], di

	mov ax, word [rbp - 2]
	mov ebx, 2
	cmp ax, bx
	setl al
	and ax, 1
	cmp ax, 0
	je if_end_3
	mov ax, word [rbp - 2]
	mov rsp, rbp
	pop rbp
	ret
if_end_3:
	mov ax, word [rbp - 2]
	movsx eax, ax
	mov ebx, 1
	sub eax, ebx
	mov edi, eax
	sub rsp, 14
	call fib
	add rsp, 14
	mov bx, word [rbp - 2]
	movsx ebx, bx
	mov ecx, 2
	sub ebx, ecx
	mov edi, ebx
	mov ecx, eax
	mov eax, ebx
	push rcx
	sub rsp, 14
	call fib
	add rsp, 14
	pop rcx
	add ecx, eax
	mov eax, ecx
	mov rsp, rbp
	pop rbp
	ret


