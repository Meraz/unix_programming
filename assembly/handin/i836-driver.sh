#! /bin/bash

OUTFILE='file1.run'

ArgumentUse() {
	echo "i836-driver.sh <filename>\n"

	echo "\t-h: Shows this message"
}

## 	"ENTRYPOINT"
# check if enough flags exists
if [ $# -lt 1 ] ; # Minimum amount of flags is one
	then
		ArgumentUse;
		exit 0;
fi

# Check and set flags
while getopts n:h:d:c2rFt option
do
	case $option in
	\?)
		ArgumentUse; exit 1;;
	h)
		ArgumentUse; exit 0;;
	*)
		ArgumentUse; exit 1;;
	esac
done

shift $(($OPTIND - 1)) 	# This shifts all the getopts arguments that has been used away from the argumentlist
INFILE=$1				# This should now correspond to the last



echo ".section .data" 	> $OUTFILE
echo "output: .long 0" 	>> $OUTFILE
echo "a: .long 0" 		>> $OUTFILE
echo "b: .long 0" 		>> $OUTFILE
echo "c: .long 0" 		>> $OUTFILE
echo "d: .long 0" 		>> $OUTFILE
echo "e: .long 0" 		>> $OUTFILE
echo "f: .long 0" 		>> $OUTFILE
echo "g: .long 0" 		>> $OUTFILE
echo "h: .long 0" 		>> $OUTFILE
echo "i: .long 0" 		>> $OUTFILE
echo "j: .long 0" 		>> $OUTFILE
echo "k: .long 0" 		>> $OUTFILE
echo "l: .long 0" 		>> $OUTFILE
echo "m: .long 0" 		>> $OUTFILE
echo "n: .long 0" 		>> $OUTFILE
echo "o: .long 0" 		>> $OUTFILE
echo "p: .long 0" 		>> $OUTFILE
echo "q: .long 0" 		>> $OUTFILE
echo "r: .long 0" 		>> $OUTFILE
echo "s: .long 0" 		>> $OUTFILE
echo "t: .long 0" 		>> $OUTFILE
echo "u: .long 0" 		>> $OUTFILE
echo "v: .long 0" 		>> $OUTFILE
echo "w: .long 0" 		>> $OUTFILE
echo "x: .long 0" 		>> $OUTFILE
echo "y: .long 0" 		>> $OUTFILE
echo "z: .long 0" 		>> $OUTFILE
echo "" 				>> $OUTFILE		# Empty row

echo '.section .text' >> $OUTFILE
echo '.globl _start' >> $OUTFILE
echo '_start:' >> $OUTFILE

cat $INFILE | ./calc3b.exe >> $OUTFILE

echo "" 					>> $OUTFILE		# Empty row
echo 'exit:' 		>> $OUTFILE
echo 'movl $1,%eax' 		>> $OUTFILE
echo 'int $0x80' 			>> $OUTFILE
echo "" 					>> $OUTFILE		# Empty row
cat onlyThePrintFunction.txt >> $OUTFILE
echo "" 					>> $OUTFILE		# Empty row in the end, needed


chmod 755 file1.run
