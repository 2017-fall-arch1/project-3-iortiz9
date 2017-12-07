	.file	"assemblyMain.c"
	.text
	.p2align 4,,15
	.globl	toggle
	.type	toggle, @function
toggle:
.LFB0:
	.cfi_startproc
	subl	$12, %esp
	.cfi_def_cfa_offset 16
	movl	16(%esp), %eax
	testb	%al, %al
	je	.L3
	cmpb	$1, %al
	jne	.L2
	subl	$12, %esp
	.cfi_def_cfa_offset 28
	pushl	$300
	.cfi_def_cfa_offset 32
	call	buzzer_set_period
	addl	$16, %esp
	.cfi_def_cfa_offset 16
.L2:
	movl	$1, %eax
	addl	$12, %esp
	.cfi_remember_state
	.cfi_def_cfa_offset 4
	ret
	.p2align 4,,10
	.p2align 3
.L3:
	.cfi_restore_state
	subl	$12, %esp
	.cfi_def_cfa_offset 28
	pushl	$1000
	.cfi_def_cfa_offset 32
	call	buzzer_set_period
	addl	$16, %esp
	.cfi_def_cfa_offset 16
	movl	$1, %eax
	addl	$12, %esp
	.cfi_def_cfa_offset 4
	ret
	.cfi_endproc
.LFE0:
	.size	toggle, .-toggle
	.ident	"GCC: (GNU) 7.1.1 20170622 (Red Hat 7.1.1-3)"
	.section	.note.GNU-stack,"",@progbits
