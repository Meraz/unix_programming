.global gcd
gcd:

pushl %ebp				# push returnpointer to the stack. (This is a function)
movl %esp, %ebp			#
subl $4, %esp			# I need 1(%ebx) local variables

movl %ebx, -4(%ebp)		# Must save %ebx if I use the register %ebx in function.

movl 8(%ebp), %eax		# Get one of the parameters, +4 because I pushed %ebp on the stack before this
movl 12(%ebp), %ebx		# Get the other parameter

gcdStart:
	cmpl	%eax, %ebx
	je gcdEnd		
	jl gcdElse		
	subl	%eax, %ebx
	jmp	L003		
gcdElse:			
	subl	%ebx, %eax
L003:
jmp gcdStart
gcdEnd:

movl -12(%ebp), %ebx			# Restore %ebx
#movl $0, %eax					# Result
leave
ret
