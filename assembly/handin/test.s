.section .data
output: .long 0


.section .text
.globl _start
_start:

#movl $2147483648, %eax
#movl $2147483647, %ebx

#movl $-2147483649, %eax
#movl $101, %ebx

movl $100, %eax
movl $0, %ebx


xorl %edx, %edx
movl $0, %ecx
divl %ecx

pushl %eax
call print
jmp exit



cmpl %eax, %ebx
je equal			# %eax == %eax
jg positive			# %eax < %ebx == true
jl negative1		# %eax > %ebx == true
jmp exit

negative1:
movl $4,%eax				# sys_write
movl $1,%ebx				# Filedescriptor, 1 is standard out
movl $output, %ecx			# Get the adress of the working memory
movl $45, (%ecx)			# -
movl $4,%edx				# Length of memory location in bytes. A long, 4 bytes
int  $0x80					# System Call
jmp exit

positive:
movl $4,%eax				# sys_write
movl $1,%ebx				# Filedescriptor, 1 is standard out
movl $output, %ecx			# Get the adress of the working memory
movl $43, (%ecx)			# +
movl $4,%edx				# Length of memory location in bytes. A long, 4 bytes
int  $0x80					# System Call
jmp exit

equal:
movl $4,%eax				# sys_write
movl $1,%ebx				# Filedescriptor, 1 is standard out
movl $output, %ecx			# Get the adress of the working memory
movl $46, (%ecx)			# +
movl $4,%edx				# Length of memory location in bytes. A long, 4 bytes
int  $0x80					# System Cal
jmp exit



exit:

						# Print a newline aswell
movl $4,%eax				# sys_write
movl $1,%ebx				# Filedescriptor, 1 is standard out
movl $output, %ecx			# Get the adress of the working memory
movl $10, (%ecx)			# +
movl $4,%edx				# Length of memory location in bytes. A long, 4 bytes
int  $0x80					# System Cal

						# Exit call
movl $1,%eax
int $0x80


print:

pushl %ebp				# push returnpointer to the stack. (This is a function)
movl %esp, %ebp
subl $8, %esp			# I need 2 local variables. %ebx when dividing

movl 8(%ebp),%eax		# Get the first parameter
movl %ebx, -8(%ebp)		# Must save this



movl %eax, -4(%ebp) 	# in parameter this 

#if parameter MSB is set, negative, print negative sign, then jump loop
#else if MSB is not set, positive, jump to loop
test %eax, %eax
js negative
movl -4(%ebp), %eax
jmp preloop

negative:
movl $4,%eax                            # sys_write
movl $1,%ebx                            # Filedescriptor, 1 is standard out
movl $output, %ecx                      # Get the adress of the working memory
movl $45, (%ecx)                        # -
movl $4,%edx                            # Length of memory location in bytes. A long, 4 bytes
int  $0x80                              # System Call

movl -4(%ebp), %eax
movl $4294967296, %ebx					# This row gives the warning
subl %eax, %ebx
movl %ebx, %eax

preloop:
movl $0, -4(%ebp)			# Zero %counter. %counter = 0

loop:
xorl %edx,%edx			# Zero %edx. %edx = 0
movl $10,%ecx			# %ebx = 10
divl %ecx				# dividing, result in %eax (123) and remainder in %edx (4)
addl $48,%edx			# +48. Convert into ascii
pushl %edx				# on stack
movl -4(%ebp), %ecx		# Get the current counter from memory
incl %ecx				# Increase the counter of digits
movl %ecx, -4(%ebp)		# Return the counter to memory
cmpl $0,%eax			# If eax is zero after division, no more digits	
jz   next				# If zero, then all individual digits are found.
jmp loop				# Otherwise, keep the loop going

next:						#
movl -4(%ebp), %ecx		# Get the current counter from memory
testl %ecx,%ecx				# if(%ecx == 0)
jz   functionReturn			#  return()
decl %ecx					# Decrease the counter of digits
movl %ecx, -4(%ebp)		# Return the counter to memory
movl $4,%eax				# sys_write
movl $1,%ebx				# Filedescriptor, 1 is standard out
movl $output, %ecx			# Get the adress of the working memory
popl (%ecx)						# Get the value on the stack and overwrite the value of the adress that %ecx is pointing to
movl $4,%edx				# Length of memory location in bytes. A long, 4 bytes
int  $0x80					# System Call 

jmp  next
functionReturn:
							# Newline
movl $4,%eax				# sys_write
movl $1,%ebx				# Filedescriptor, 1 is standard out
movl $output, %ecx			# Get the adress of the working memory
movl $10, (%ecx)			# Newline
movl $4,%edx				# Length of memory location in bytes. A long, 4 bytes
int  $0x80					# System Call 

movl -8(%ebp), %ebx			# Restore this
movl $0, %eax				# Result

leave
ret
