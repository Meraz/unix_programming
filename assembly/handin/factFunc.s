.global fact
fact:
pushl %ebp				# push returnpointer to the stack. (This is a function)
movl %esp, %ebp			#

movl 8(%ebp), %eax		# Get the in parameter and place it in %eax
movl %eax, %ecx			# Duplicate it and place it in %ebx
decl %ecx				# Decrease the one in %ebx by on	

factStart:
	cmpl $0, %ecx		# Compare %ebx with 0
	je factEnd			# If zero, end
	mull %ecx			# else multiply so that %eax = %eax * %ecx
	decl %ecx			# Decrease %ecx with one
	jmp factStart
factEnd:

leave
ret
