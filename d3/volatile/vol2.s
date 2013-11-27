	.file	"vol2.cpp"
	.intel_syntax noprefix
	.text
	.p2align 4,,15
	.globl	_Z18volatile_example_2v
	.type	_Z18volatile_example_2v, @function
_Z18volatile_example_2v:
.LFB0:
	.cfi_startproc
	fld	QWORD PTR .LC0
	fstp	QWORD PTR regular_double
	mov	BYTE PTR volatile_bool_done, 1
	ret
	.cfi_endproc
.LFE0:
	.size	_Z18volatile_example_2v, .-_Z18volatile_example_2v
	.globl	regular_double
	.bss
	.align 8
	.type	regular_double, @object
	.size	regular_double, 8
regular_double:
	.zero	8
	.globl	volatile_bool_done
	.type	volatile_bool_done, @object
	.size	volatile_bool_done, 1
volatile_bool_done:
	.zero	1
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC0:
	.long	2061584302
	.long	1072934420
	.ident	"GCC: (Debian 4.7.2-5) 4.7.2"
	.section	.note.GNU-stack,"",@progbits
