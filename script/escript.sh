#! /bin/sh
dubble=`mktemp` # Alla dubletter
result=`mktemp`	# Resultat
filen=`mktemp`  # Input filen

cat $1 | grep -v - > $filen # Kopiera orginalfilen till en tempfil
cat $1 | cut -d " " -f 1 | sort | uniq -d > $dubble # Ta ut alla dubletter av första kolumnen (IP adresser)

#echo $filen
#exit

while read line	# For each line in $filen
do
        ip=`echo $line | cut -d " " -f 1`	# Rad för rad, endast första fältet (IP adressen)

        grep -q $ip $dubble			# greppar dubblettfilen på specific IP, gör det quit för det endast är intresserad av success/fail
	if [ $? -eq 0 ]				# Kolla of resultat av förra kommandot är 0
        then
                tmp=`mktemp`					# Skapa en temp fil
                grep $ip $filen | cut -d " " -f 2 > $tmp	# Grep på alla ip i filen, cuttar bort ipadresserna och sparar fält två (byte)
                sum=`paste -s -d"+" $tmp | bc`			# Lägger ett + mellan alla raderna, summerar ihop allting
                echo "$ip $sum" >> $result			# Spara det ipet vi greppade och dem sammanslagna bytsen
                unlink $tmp					# unlinkar tmp filen
				cat $dubble | grep -v $ip > $dubble		# Tar bort ipet ur filen, genom att inverta grep resultatet och sedan skriva det till filen igen
                cat $filen | grep -v $ip > $filen		# Samma som över fast annan fil
        else							# Om du kommer hit så är det ingen dublett
                echo $line >> $result				# 	därför kan vi spara det direkt i filen
        fi
done < $filen	# 

unlink $filen	#
unlink $dubble	#
cat $result	# Print resultat
unlink $result	#
