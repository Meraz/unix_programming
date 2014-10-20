.global gcd
gcd:

pushl %ebp				# push returnpointer to the stack. (This is a function)
movl %esp, %ebp			#
subl $4, %esp			# I need 1(%ebx) local variables

movl %ebx, -4(%ebp)		# Must save %ebx if I use the register %ebx in function.

movl 8(%ebp), %eax		# Get the 
movl 12(%ebp), %ebx

start:
	cmpl	%eax, %ebx			# %eax = 2684, %ebx = 732
	je end						# if(%eax == %ebx) jump return			# 2684 == 732
	jl else						# if(%eax > %ebx) jump L002				# 2684 >  732
	subl	%eax, %ebx			# else %ebx = %ebx - %eax				# 2684 <= 732
	jmp	L003					# 					
else:							#
	subl	%ebx, %eax			# %eax = %eax - %ebx			
L003:
jmp start
end:

movl -12(%ebp), %ebx			# Restore %ebx
#movl $0, %eax					# Result
leave
ret
