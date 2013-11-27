	.file	"vol.cpp"
	.intel_syntax noprefix
	.text
	.p2align 4,,15
	.globl	_Z17volatile_store_64x
	.type	_Z17volatile_store_64x, @function
_Z17volatile_store_64x:
.LFB0:
	.cfi_startproc
	mov	eax, DWORD PTR [esp+4]
	mov	edx, DWORD PTR [esp+8]
	mov	DWORD PTR vlt64, eax
	mov	DWORD PTR vlt64+4, edx
	ret
	.cfi_endproc
.LFE0:
	.size	_Z17volatile_store_64x, .-_Z17volatile_store_64x
	.globl	vlt64
	.bss
	.align 8
	.type	vlt64, @object
	.size	vlt64, 8
vlt64:
	.zero	8
	.ident	"GCC: (Debian 4.7.2-5) 4.7.2"
	.section	.note.GNU-stack,"",@progbits
