.global lntwo
lntwo:

pushl %ebp				# push returnpointer to the stack. (This is a function)
movl %esp, %ebp			#
subl $4, %esp			# I need 1(%ebx) local variables

movl %ebx, -12(%ebp)	# Must save %ebx if I use the register %ebx in function.

movl 8(%ebp), %eax		# Get the in parameter
movl $1, %ebx			# Move 1 to %ebx, leftshift comparer
movl $0, %ecx			# Move 0 to %ecx, counter

lnTwoStart:
	cmpl %eax, %ebx
		je lnTwoEnd			# Exactky right
		jg lnTwoPreEnd		# Means we passed it on last move, thus decrease with one and quit
	sall $1, %ebx			# Move one step left
	incl %ecx				# Increase counter by one
	jmp lnTwoStart			# Repeat
lnTwoPreEnd:				
	decl %ecx				# Decrease counter by one
lnTwoEnd:

movl -12(%ebp), %ebx			# Restore %ebx
movl %ecx, %eax					# Result
leave
ret
