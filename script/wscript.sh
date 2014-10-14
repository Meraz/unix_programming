#! /bin/sh
dubble=`mktemp`
result=`mktemp`
filen=`mktemp`

cat $1 > $filen
cat $1 | cut -d " " -f 1 | sort | uniq -d > $dubble

while read line
do
        ip=`echo $line | cut -d " " -f 1`

        grep -q $ip $dubble
        if [ $? -eq 0 ]
        then
                tmp=`mktemp`
                grep $ip $filen | cut -d " " -f 2 > $tmp
                sum=`paste -s -d"+" $tmp | bc`
                echo "$ip $sum" >> $result
                unlink $tmp
                #cat $dubble | grep -v $ip > $dubble
                cat $filen | grep -v $ip > $filen
        else
                echo $line >> $result
        fi
done < $filen

unlink $filen
unlink $dubble
cat $result
unlink $result
