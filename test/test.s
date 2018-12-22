	.text
	.file	"main"
	.section	.rodata.cst8,"aM",@progbits,8
	.p2align	3               # -- Begin function main
.LCPI0_0:
	.quad	4607182418800017408     # double 1
.LCPI0_1:
	.quad	4587366580439587226     # double 0.050000000000000003
.LCPI0_2:
	.quad	4609434218613702656     # double 1.5
.LCPI0_3:
	.quad	-4631501856787818086    # double -0.10000000000000001
.LCPI0_4:
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
	subq	$24, %rsp
	.cfi_def_cfa_offset 48
	.cfi_offset %rbx, -24
	.cfi_offset %rbp, -16
	movabsq	$4609434218613702656, %rax # imm = 0x3FF8000000000000
	movq	%rax, (%rsp)
	xorl	%eax, %eax
	testb	%al, %al
	jne	.LBB0_8
# %bb.1:                                # %for_loop.preheader
	movabsq	$-4613937818241073152, %rbx # imm = 0xBFF8000000000000
	xorl	%ebp, %ebp
	movsd	.LCPI0_0(%rip), %xmm4   # xmm4 = mem[0],zero
	xorpd	%xmm5, %xmm5
	.p2align	4, 0x90
.LBB0_2:                                # %for_loop
                                        # =>This Loop Header: Depth=1
                                        #     Child Loop BB0_3 Depth 2
	movq	%rbx, 8(%rsp)
	testb	%bpl, %bpl
	jne	.LBB0_7
	.p2align	4, 0x90
.LBB0_3:                                # %for_loop2
                                        #   Parent Loop BB0_2 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movsd	(%rsp), %xmm0           # xmm0 = mem[0],zero
	movapd	%xmm0, %xmm1
	mulsd	%xmm0, %xmm1
	movsd	8(%rsp), %xmm2          # xmm2 = mem[0],zero
	mulsd	%xmm2, %xmm2
	addsd	%xmm2, %xmm1
	subsd	%xmm4, %xmm1
	movsd	%xmm1, 16(%rsp)
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
	je	.LBB0_5
# %bb.4:                                # %if_then
                                        #   in Loop: Header=BB0_3 Depth=2
	movl	$42, %edi
	jmp	.LBB0_6
	.p2align	4, 0x90
.LBB0_5:                                # %if_else
                                        #   in Loop: Header=BB0_3 Depth=2
	movl	$32, %edi
.LBB0_6:                                # %if_after
                                        #   in Loop: Header=BB0_3 Depth=2
	callq	putchar
	xorpd	%xmm5, %xmm5
	movsd	.LCPI0_0(%rip), %xmm4   # xmm4 = mem[0],zero
	movsd	8(%rsp), %xmm0          # xmm0 = mem[0],zero
	addsd	.LCPI0_1(%rip), %xmm0
	movsd	%xmm0, 8(%rsp)
	movsd	.LCPI0_2(%rip), %xmm1   # xmm1 = mem[0],zero
	cmpnlesd	%xmm0, %xmm1
	andpd	%xmm4, %xmm1
	ucomisd	%xmm5, %xmm1
	jne	.LBB0_3
.LBB0_7:                                # %for_after
                                        #   in Loop: Header=BB0_2 Depth=1
	movl	$10, %edi
	callq	putchar
	xorpd	%xmm5, %xmm5
	movsd	.LCPI0_0(%rip), %xmm4   # xmm4 = mem[0],zero
	movsd	(%rsp), %xmm0           # xmm0 = mem[0],zero
	addsd	.LCPI0_3(%rip), %xmm0
	movsd	%xmm0, (%rsp)
	movsd	.LCPI0_4(%rip), %xmm1   # xmm1 = mem[0],zero
	cmpltsd	%xmm0, %xmm1
	andpd	%xmm4, %xmm1
	ucomisd	%xmm5, %xmm1
	jne	.LBB0_2
.LBB0_8:                                # %for_after22
	xorl	%eax, %eax
	addq	$24, %rsp
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function

	.section	".note.GNU-stack","",@progbits
