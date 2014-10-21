#! /bin/bash

Clean()
{
	echo "Cleaning up files"
	rm gcd.o lntwo.o fact.o
}

CheckContinue() {
	if [ $? -ne 0 ] ; then
		echo "There was a problem with something, stopping."
		Clean
		exit 1
	fi	
}

#as file1.s 	-o file1.o
#CheckContinue
as gcdFunc.s 	-o gcdFunc.o
CheckContinue
as lntwoFunc.s 	-o lntwoFunc.o
CheckContinue
as factFunc.s 	-o factFunc.o
CheckContinue
#ld file1.o gcd.o lntwo.o fact.o	-o file1
#CheckContinue

