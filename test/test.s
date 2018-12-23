	.text
	.file	"main"
	.globl	fuck                    # -- Begin function fuck
	.p2align	4, 0x90
	.type	fuck,@function
fuck:                                   # @fuck
	.cfi_startproc
# %bb.0:                                # %entry
	movl	%edi, -4(%rsp)
	movl	%edi, %eax
	retq
.Lfunc_end0:
	.size	fuck, .Lfunc_end0-fuck
	.cfi_endproc
                                        # -- End function
	.globl	shit                    # -- Begin function shit
	.p2align	4, 0x90
	.type	shit,@function
shit:                                   # @shit
	.cfi_startproc
# %bb.0:                                # %entry
	movsd	%xmm0, -8(%rsp)
	retq
.Lfunc_end1:
	.size	shit, .Lfunc_end1-shit
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst8,"aM",@progbits,8
	.p2align	3               # -- Begin function main
.LCPI2_0:
	.quad	4607182418800017408     # double 1
.LCPI2_1:
	.quad	4587366580439587226     # double 0.050000000000000003
.LCPI2_2:
	.quad	4609434218613702656     # double 1.5
.LCPI2_3:
	.quad	-4631501856787818086    # double -0.10000000000000001
.LCPI2_4:
	.quad	-4613937818241073152    # double -1.5
	.text
	.globl	main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rbp
	.cfi_def_cfa_offset 16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	subq	$40, %rsp
	.cfi_def_cfa_offset 64
	.cfi_offset %rbx, -24
	.cfi_offset %rbp, -16
	xorl	%ebx, %ebx
	movl	$.L__unnamed_1, %edi
	movl	$42, %esi
	xorl	%eax, %eax
	callq	printf
	movabsq	$4609434218613702656, %rax # imm = 0x3FF8000000000000
	movq	%rax, 8(%rsp)
	testb	%bl, %bl
	jne	.LBB2_8
# %bb.1:                                # %for_loop.preheader
	movabsq	$-4613937818241073152, %rbx # imm = 0xBFF8000000000000
	xorl	%ebp, %ebp
	movsd	.LCPI2_0(%rip), %xmm4   # xmm4 = mem[0],zero
	xorpd	%xmm5, %xmm5
	.p2align	4, 0x90
.LBB2_2:                                # %for_loop
                                        # =>This Loop Header: Depth=1
                                        #     Child Loop BB2_3 Depth 2
	movq	%rbx, 16(%rsp)
	testb	%bpl, %bpl
	jne	.LBB2_7
	.p2align	4, 0x90
.LBB2_3:                                # %for_loop2
                                        #   Parent Loop BB2_2 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movsd	8(%rsp), %xmm0          # xmm0 = mem[0],zero
	movapd	%xmm0, %xmm1
	mulsd	%xmm0, %xmm1
	movsd	16(%rsp), %xmm2         # xmm2 = mem[0],zero
	mulsd	%xmm2, %xmm2
	addsd	%xmm2, %xmm1
	subsd	%xmm4, %xmm1
	movsd	%xmm1, 24(%rsp)
	movapd	%xmm1, %xmm3
	mulsd	%xmm1, %xmm3
	mulsd	%xmm1, %xmm3
	mulsd	%xmm0, %xmm2
	mulsd	%xmm0, %xmm2
	mulsd	%xmm0, %xmm2
	subsd	%xmm2, %xmm3
	cmplesd	%xmm5, %xmm3
	andpd	%xmm4, %xmm3
	ucomisd	%xmm5, %xmm3
	je	.LBB2_5
# %bb.4:                                # %if_then
                                        #   in Loop: Header=BB2_3 Depth=2
	movl	$42, %edi
	jmp	.LBB2_6
	.p2align	4, 0x90
.LBB2_5:                                # %if_else
                                        #   in Loop: Header=BB2_3 Depth=2
	movl	$32, %edi
.LBB2_6:                                # %if_after
                                        #   in Loop: Header=BB2_3 Depth=2
	callq	putchar
	xorpd	%xmm5, %xmm5
	movsd	.LCPI2_0(%rip), %xmm4   # xmm4 = mem[0],zero
	movsd	16(%rsp), %xmm0         # xmm0 = mem[0],zero
	addsd	.LCPI2_1(%rip), %xmm0
	movsd	%xmm0, 16(%rsp)
	movsd	.LCPI2_2(%rip), %xmm1   # xmm1 = mem[0],zero
	cmpnlesd	%xmm0, %xmm1
	andpd	%xmm4, %xmm1
	ucomisd	%xmm5, %xmm1
	jne	.LBB2_3
.LBB2_7:                                # %for_after
                                        #   in Loop: Header=BB2_2 Depth=1
	movl	$10, %edi
	callq	putchar
	xorpd	%xmm5, %xmm5
	movsd	.LCPI2_0(%rip), %xmm4   # xmm4 = mem[0],zero
	movsd	8(%rsp), %xmm0          # xmm0 = mem[0],zero
	addsd	.LCPI2_3(%rip), %xmm0
	movsd	%xmm0, 8(%rsp)
	movsd	.LCPI2_4(%rip), %xmm1   # xmm1 = mem[0],zero
	cmpltsd	%xmm0, %xmm1
	andpd	%xmm4, %xmm1
	ucomisd	%xmm5, %xmm1
	jne	.LBB2_2
.LBB2_8:                                # %for_after26
	xorl	%edi, %edi
	callq	fuck
	testl	%eax, %eax
	jne	.LBB2_10
# %bb.9:                                # %if_then27
	movl	$48, %edi
	callq	putchar
.LBB2_10:                               # %if_after28
	movl	$2, %edi
	callq	fuck
	testl	%eax, %eax
	je	.LBB2_12
# %bb.11:                               # %if_then29
	movl	$49, %edi
	callq	putchar
.LBB2_12:                               # %if_after30
	movl	$2, %edi
	callq	fuck
	cmpl	$-1, %eax
	je	.LBB2_14
# %bb.13:                               # %if_then31
	movl	$50, %edi
	callq	putchar
.LBB2_14:                               # %if_after32
	movl	$48, %edi
	callq	fuck
	movl	%eax, %edi
	callq	putchar
	movl	$-3, %edi
	callq	putchar
	movl	$2, %edi
	callq	putchar
	xorl	%ebx, %ebx
	xorl	%edi, %edi
	callq	putchar
	xorl	%edi, %edi
	callq	putchar
	movl	$10, %edi
	callq	putchar
	movl	$9, 4(%rsp)
	testb	%bl, %bl
	jne	.LBB2_16
	.p2align	4, 0x90
.LBB2_15:                               # %while_loop
                                        # =>This Inner Loop Header: Depth=1
	movl	4(%rsp), %edi
	addl	$48, %edi
	callq	putchar
	movl	4(%rsp), %eax
	decl	%eax
	movl	%eax, 4(%rsp)
	testl	%eax, %eax
	jg	.LBB2_15
.LBB2_16:                               # %while_after
	movabsq	$4607182418800017408, %rax # imm = 0x3FF0000000000000
	movq	%rax, 32(%rsp)
	movsd	.LCPI2_0(%rip), %xmm0   # xmm0 = mem[0],zero
	movl	$.L__unnamed_2, %edi
	movb	$1, %al
	callq	printf
	movl	$48, %edi
	callq	putchar
	xorl	%eax, %eax
	addq	$40, %rsp
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end2:
	.size	main, .Lfunc_end2-main
	.cfi_endproc
                                        # -- End function
	.type	.L__unnamed_1,@object   # @0
	.section	.rodata.str1.16,"aMS",@progbits,1
	.p2align	4
.L__unnamed_1:
	.asciz	"Hello World! %d\n"
	.size	.L__unnamed_1, 17

	.type	.L__unnamed_2,@object   # @1
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_2:
	.asciz	"%.2f\n"
	.size	.L__unnamed_2, 6


	.section	".note.GNU-stack","",@progbits
