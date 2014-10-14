#! /bin/sh
singleOccurence=`mktemp` 	# Alla dubletter
result=`mktemp`			# End result
file=`mktemp`  			# Input filen

cat $1 | cut -d " " -f 1,10 | grep -v - > $file 		# Kopiera orginalfilen till en tempfil, spara endast fält 1 och 10. IP och BYTE's
cat $1 | cut -d " " -f 1 | sort | uniq > $singleOccurence 	# Saves one entry of all unique

#cat $singleOccurence

while read ip # For each ip in $file
do
	tmp=`mktemp`					# Creates an temp fil
        grep $ip $file | cut -d " " -f 2 > $tmp		# Greps on the ip in the file with all untaken entries, only saves second field, bytes

	totalSize=0
	while read size
	do
		totalSize=`expr $totalSize + $size`
	done < $tmp

	if [ ! $totalSize -eq 0 ]; then
		echo "$ip $totalSize" >> $result			# saves ip and result in the result file
	fi
	cat $file | grep -v $ip > $tmp
	cat $tmp > $file
unlink $tmp					# unlinks tmp filen
#exit

#	echo "`cat "$file" | grep -v $ip`"
done < $singleOccurence

unlink $file		# unlink file
unlink $singleOccurence	# unlink file

Text=`cat $result | sort -rn -k2,2`	# Print resultat
echo "$Text"
unlink $result	# unlink file
