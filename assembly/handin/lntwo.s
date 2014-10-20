.global lntwo
lntwo:

pushl %ebp				# push returnpointer to the stack. (This is a function)
movl %esp, %ebp			#
subl $4, %esp			# I need 1(%ebx) local variables

movl %ebx, -12(%ebp)	# Must save %ebx if I use the register %ebx in function.

movl 8(%ebp), %eax		# Get the in parameter
movl $1, %ebx			# Move 1 to %ebx, leftshift comparer
movl $0, %ecx			# Move 0 to %ecx, counter

begin:
cmpl %eax, %ebx
jae end
sall $1, %ebx			# Move one step left
incl %ecx				# Increase counter by one
jmp begin
end:

movl -12(%ebp), %ebx			# Restore %ebx
movl %ecx, %eax					# Result
leave
ret
