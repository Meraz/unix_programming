.section .bss
output: .long
a: .long
b: .long
c: .long
d: .long
e: .long
f: .long
g: .long
h: .long
i: .long
j: .long
k: .long
l: .long
m: .long
n: .long
o: .long
p: .long
q: .long
r: .long
s: .long
t: .long
u: .long
v: .long
w: .long
x: .long
y: .long
z: .long

.section .data
.globl main
main:
	pushl	$5
	popl	i
L000:
	pushl	i
	pushl	$0
	popl	%eax
	popl	%ebx
	cmpl	 %eax, %ebx
	jz	L001
	pushl	i
	call print
		jmp exit
	ADDL $4, %esp
	jmp exit
	pushl	i
	pushl	$1
	popl	%eax
	popl	%ebx
	subl	 %eax, %ebx
	pushl	 %ebx
	popl	i
	jmp	L000
L001:

exit:
movl $1,%eax
int $0x80

print:
movl 4(%esp),%eax		# Get the first parameter
movl $output, %ecx		# Get the adress of the working memory
#pushl %ebp				# push returnpointer to the stack. (This is a function)

xorl %esi,%esi			# Zero %esi. %esi = 0

loop:
xorl %edx,%edx			# Zero %edx. %edx = 0
movl $10,%ebx			#
divl %ebx				# dividing, result in %eax (123) and remainder in %edx (4)
addl $48,%edx			# +48. Convert into ascii
pushl %edx				# on stack
incl %esi				# counter of digits
cmpl $0,%eax			# If eax is zero after division, no more digits	
jz   next				# If zero, then all individual digits are found.
jmp loop				# Otherwise, keep the loop going

next:				#
popl (%ecx)			# Get the value on the stack and overwrite the adress that %ecx is pointing to
testl %esi,%esi		# if(%esi == 0)
jz   functionReturn	#  return()
decl %esi			# Decrease %esi. %esi = %esi - 1
movl $4,%eax		# sys_write
movl $1,%ebx		# Filedescriptor, 1 is standard out
movl $2,%edx		# 
int  $0x80			# System Call 
jmp  next
functionReturn:

movl $0, %eax
ret
