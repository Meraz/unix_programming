#! /bin/sh

file=$1
#loopC=1
rm tmp1.file

# Puts one unique entry in the tmp1.file for every IP in the log
cat ${file} | sed 's/\([0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\)\ \(.*\)/\1/' | uniq -d >> result.file
cat ${file} | sed 's/\([0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\)\ \(.*\)/\1/' | uniq >> tmp1.file

cat ${file} | sed 's/\([0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\)\(.*\)\(\"\)\([A-Za-z]\{3,4\}\ \/.*\)\"\ \([0-9]\{3\}\)\ \([0-9|\-]\{1,7\}\)\(.*\)/\1\ \6/' | grep -v '-' >> tmp2.file


while read IPET BYTES
do
	echo $IPET
	echo $BYTES
#	cat tmp2.file | grep $line > test.1
#	echo $TEST
#	echo $line
#	echo "$TEST" | head -n 1

#	MYIP=`cat "${file}" | grep "${TEST}"`

#	echo ${MYIP}

#	TEST2=$(echo $1 | grep $TEST)
#	echo ${TEST2}
    #    echo test;
#	file=0#$(echo $1 | grep -v $TEST)
#	loopC=`expr $loopC + 1`
done < tmp2.file


#rm tmp1.file
#rm tmp2.file
