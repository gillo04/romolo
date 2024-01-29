bits 64
extern printf

section .data
lit_0 db "Fibonacci sequence from 0 to 10:", 10, "", 0
lit_2 db "%d: %d", 10, "", 0

section .text
global _start
_start:
; align stack
	call main
	mov ebx, eax
	mov eax, 1
	int 0x80
main:
	push rbp
	mov rbp, rsp

	mov rax, lit_0
	mov rbx, 0
	mov rcx, 0
	mov rdi, rax
	mov rsi, rbx
	mov rdx, rcx
	call printf

	sub rsp, 4
	mov rax, 0
	mov dword [rbp - 4], eax


loop_1:
loop_cond_1:
	mov eax, dword [rbp - 4]
	mov rbx, 10
	cmp eax, ebx
	setle al
	and eax, 1

	cmp eax, 0
	je loop_end_1
	mov rax, lit_2
	mov ebx, dword [rbp - 4]
	mov ecx, dword [rbp - 4]
	mov edi, ecx
	mov rdx, rax
	mov eax, ecx
	push rdx
	push rbx
	sub rsp, 12
	call fib
	add rsp, 12
	pop rbx
	pop rdx
	mov rdi, rdx
	mov esi, ebx
	mov edx, eax
	sub rsp, 12
	call printf
	add rsp, 12

	mov eax, dword [rbp - 4]
	inc eax
	mov dword [rbp - 4], eax
	jmp loop_1
loop_end_1:

	add rsp, 4
	mov rax, 0
	mov rsp, rbp
	pop rbp
	ret

fib:
	push rbp
	mov rbp, rsp
	sub rsp, 4
	mov dword [rbp - 4], edi

	mov eax, dword [rbp - 4]
	mov rbx, 2
	cmp eax, ebx
	setl al
	and eax, 1
	cmp eax, 0
	je if_end_3
	mov eax, dword [rbp - 4]
	mov rsp, rbp
	pop rbp
	ret
if_end_3:
	mov eax, dword [rbp - 4]
	mov rbx, 1
	sub eax, ebx
	mov edi, eax
	sub rsp, 12
	call fib
	add rsp, 12
	mov ebx, dword [rbp - 4]
	mov rcx, 2
	sub ebx, ecx
	mov edi, ebx
	mov ecx, eax
	mov eax, ebx
	push rcx
	sub rsp, 12
	call fib
	add rsp, 12
	pop rcx
	add ecx, eax
	mov eax, ecx
	mov rsp, rbp
	pop rbp
	ret


