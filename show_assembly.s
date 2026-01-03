	.file	"show_assembly.c"
	.text
	.globl	sum
	.type	sum, @function
sum:
.LFB0:
	.cfi_startproc
	endbr64
	subq	$88, %rsp
	.cfi_def_cfa_offset 96
	movq	%rsi, 40(%rsp)
	movq	%rdx, 48(%rsp)
	movq	%rcx, 56(%rsp)
	movq	%r8, 64(%rsp)
	movq	%r9, 72(%rsp)
	movq	%fs:40, %rax
	movq	%rax, 24(%rsp)
	xorl	%eax, %eax
	movl	$8, (%rsp)
	leaq	96(%rsp), %rax
	movq	%rax, 8(%rsp)
	leaq	32(%rsp), %rax
	movq	%rax, 16(%rsp)
	testl	%edi, %edi
	jle	.L7
	movq	%rax, %r8
	movl	$0, %ecx
	movl	$0, %esi
	jmp	.L5
.L3:
	movq	8(%rsp), %rdx
	leaq	8(%rdx), %rax
	movq	%rax, 8(%rsp)
.L4:
	addl	(%rdx), %esi
	addl	$1, %ecx
	cmpl	%ecx, %edi
	je	.L1
.L5:
	movl	(%rsp), %eax
	cmpl	$47, %eax
	ja	.L3
	movl	%eax, %edx
	addq	%r8, %rdx
	addl	$8, %eax
	movl	%eax, (%rsp)
	jmp	.L4
.L7:
	movl	$0, %esi
.L1:
	movq	24(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L10
	movl	%esi, %eax
	addq	$88, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
.L10:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE0:
	.size	sum, .-sum
	.globl	sum_fixed
	.type	sum_fixed, @function
sum_fixed:
.LFB1:
	.cfi_startproc
	endbr64
	addl	%esi, %edi
	leal	(%rdi,%rdx), %eax
	ret
	.cfi_endproc
.LFE1:
	.size	sum_fixed, .-sum_fixed
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
