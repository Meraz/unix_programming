#! /bin/bash

ASM_OUT_FILE='asm_placeholder'
EXECUTABLE_OUT_FILE='executable_placeholder'
USE_CURRENT_ASM_FLAG=0


Clean()
{
	echo "Cleaning up files"
	rm $EXECUTABLE_OUT_FILE
}

ArgumentUse() {
	echo "i836-driver.sh <filename>\n"

	echo "\t-c: Cleans files up"
	echo "\t-h: Shows this message"
}

CheckContinue() {
	if [ $? -ne 0 ] ; then
		echo "There was a problem with something, stopping."
		Clean
		exit 1
	fi	
}

## 	"ENTRYPOINT"
# check if enough flags exists
if [ $# -lt 1 ] ; # Minimum amount of flags is one
	then
		ArgumentUse;
		exit 0;
fi

# Check and set flags
while getopts nh option
do
	case $option in
	n)
		USE_CURRENT_ASM_FLAG=1;;
	h)
		ArgumentUse; exit 0;;
	\?)
		ArgumentUse; exit 1;;
	*)
		ArgumentUse; exit 1;;
	esac
done

shift $(($OPTIND - 1)) 	# This shifts all the getopts arguments that has been used away from the argumentlist
INFILE=$1				# This should now correspond to the last

EXECUTABLE_OUT_FILE=`echo $INFILE | cut -d "." -f1`
ASM_OUT_FILE=`echo "$EXECUTABLE_OUT_FILE.s"`

if [ $USE_CURRENT_ASM_FLAG -eq 0 ] ; then
	echo ".section .data" 	> $ASM_OUT_FILE
	echo "output: .long 0" 	>> $ASM_OUT_FILE
	echo "a: .long 0" 		>> $ASM_OUT_FILE
	echo "b: .long 0" 		>> $ASM_OUT_FILE
	echo "c: .long 0" 		>> $ASM_OUT_FILE
	echo "d: .long 0" 		>> $ASM_OUT_FILE
	echo "e: .long 0" 		>> $ASM_OUT_FILE
	echo "f: .long 0" 		>> $ASM_OUT_FILE
	echo "g: .long 0" 		>> $ASM_OUT_FILE
	echo "h: .long 0" 		>> $ASM_OUT_FILE
	echo "i: .long 0" 		>> $ASM_OUT_FILE
	echo "j: .long 0" 		>> $ASM_OUT_FILE
	echo "k: .long 0" 		>> $ASM_OUT_FILE
	echo "l: .long 0" 		>> $ASM_OUT_FILE
	echo "m: .long 0" 		>> $ASM_OUT_FILE
	echo "n: .long 0" 		>> $ASM_OUT_FILE
	echo "o: .long 0" 		>> $ASM_OUT_FILE
	echo "p: .long 0" 		>> $ASM_OUT_FILE
	echo "q: .long 0" 		>> $ASM_OUT_FILE
	echo "r: .long 0" 		>> $ASM_OUT_FILE
	echo "s: .long 0" 		>> $ASM_OUT_FILE
	echo "t: .long 0" 		>> $ASM_OUT_FILE
	echo "u: .long 0" 		>> $ASM_OUT_FILE
	echo "v: .long 0" 		>> $ASM_OUT_FILE
	echo "w: .long 0" 		>> $ASM_OUT_FILE
	echo "x: .long 0" 		>> $ASM_OUT_FILE
	echo "y: .long 0" 		>> $ASM_OUT_FILE
	echo "z: .long 0" 		>> $ASM_OUT_FILE
	echo "" 				>> $ASM_OUT_FILE		# Empty row

	echo '.section .text' 	>> $ASM_OUT_FILE
	echo '.globl _start'	>> $ASM_OUT_FILE
	echo '_start:' 			>> $ASM_OUT_FILE

	cat $INFILE | ./calc3b.exe >> $ASM_OUT_FILE

	echo "" 					>> $ASM_OUT_FILE		# Empty row
	echo 'exit:' 				>> $ASM_OUT_FILE
	echo 'movl $1,%eax' 		>> $ASM_OUT_FILE
	echo 'int $0x80' 			>> $ASM_OUT_FILE
	echo "" 					>> $ASM_OUT_FILE		# Empty row
	cat onlyThePrintFunction.s 	>> $ASM_OUT_FILE
	echo "" 					>> $ASM_OUT_FILE		# Empty row in the end, needed
fi

as $ASM_OUT_FILE	-o $EXECUTABLE_OUT_FILE.o
CheckContinue
ld gcdFunc.o lntwoFunc.o factFunc.o $EXECUTABLE_OUT_FILE.o -o $EXECUTABLE_OUT_FILE
CheckContinue
rm $EXECUTABLE_OUT_FILE.o
