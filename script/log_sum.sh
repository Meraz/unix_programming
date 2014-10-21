#! /bin/sh

# Output text
Text=text

# Preprocessing flags
LimitFlag=0
HourFlag=0
DayFlag=0

# Function flags
cFlag=0
twoFlag=0
rFlag=0
FFlag=0
tFlag=0

INFILE=none

ArgumentUse() {
	echo "log_sum.sh [-n N] [-h H|-d D] (-c|-2|-r|-F|-t|-f) <filename>\n"

	echo "\t-n: Limit the number of results to N\n"

	echo "\t-h: Limit the query to the last number of hours H"
	echo "\t-d: Limit the query to the last number of days D\n"

	echo "This script requires you to specify atleast one of the following five"
	echo "\t-c: Which IP address makes the most number of connection attempts?"
	echo "\t-2: Which address makes the most number of successful attempts?"
	echo "\t-r: What are the most common results codes and where do they come from?"
	echo "\t-F: What are the most common result codes that indicate failure and where do they come from?"
	echo "\t-t: Which IP number get the most bytes sent to them?\n"

	echo "\t-?: Shows this message"
}

LimitResult() {
		Text=`echo "$Text" | head -n $LimitFlag`
}

PreprocesHours() {	
	oldestAllowedDate=`tail -n 1 $INFILE | cut -d " " -f4 | sed 's/\[\([0-9]\{2\}\)\/\([A-Za-z]\{3\}\)\/\([0-9]\{4\}\)\(.*\)/\3\ \2\ \1 \4/'`
	oyear=`echo $oldestAllowedDate | cut -d " " -f 1`
	omonth=`echo $oldestAllowedDate | cut -d " " -f 2`
	oday=`echo $oldestAllowedDate | cut -d " " -f 3`
	ohour=`echo "$oldestAllowedDate" | cut -d ":" -f 2,3`	
	oldestAllowedDate=`date -d "$omonth $oday $oyear $ohour" "+%Y-%m-%d %H:%M"` 			# Create a valid date from it
	oldestAllowedDate=`date -d "-$HourFlag hours $oldestAllowedDate" "+%Y%m%d%H%M"`			# Use this valid date to subtract hours from it
	
	reversertmp=`mktemp`				# Temporary file for reverse the order on all the entries in the file
	cat $INFILE | tac > $reversertmp	# The actual reversing
	
	LimitTmp=`mktemp` # Temporary file to save entries in
	while read line
	do
		logDate=`echo "$line" | cut -d " " -f 4,5 | sed 's/\[\([0-9]\{2\}\)\/\([A-Za-z]\{3\}\)\/\([0-9]\{4\}\)\(.*\)/\3\ \2\ \1/'`
		lyear=`echo $logDate | cut -d " " -f 1`
		lmonth=`echo $logDate | cut -d " " -f 2`
		lday=`echo $logDate | cut -d " " -f 3`
		lhour=`echo "$line" | cut -d ":" -f 2,3`
		logDate=`date -d "$lmonth $lday $lyear $lhour" "+%Y%m%d%H%M"`

		if [ $logDate -gt $oldestAllowedDate ] ;
			then
				echo $line >> $LimitTmp
		else
			break
		fi	
	done < $reversertmp
	INFILE=$LimitTmp
	unlink $reversertmp
}

PreprocessDays() {
	oldestAllowedDate=`tail -n 1 $INFILE | cut -d " " -f4 | sed 's/\[\([0-9]\{2\}\)\/\([A-Za-z]\{3\}\)\/\([0-9]\{4\}\)\(.*\)/\3\ \2\ \1 \4/'`
	oyear=`echo $oldestAllowedDate | cut -d " " -f 1`
	omonth=`echo $oldestAllowedDate | cut -d " " -f 2`
	oday=`echo $oldestAllowedDate | cut -d " " -f 3`
	ohour=`echo "$oldestAllowedDate" | cut -d ":" -f 2,3`	
	oldestAllowedDate=`date -d "$omonth $oday $oyear $ohour" "+%Y-%m-%d %H:%M"` 			# Create a valid date from it
	oldestAllowedDate=`date -d "-$DayFlag days $oldestAllowedDate" "+%Y%m%d%H%M"`			# Use this valid date to subtract hours from it
	
	reversertmp=`mktemp`				# Temporary file for reverse the order on all the entries in the file
	cat $INFILE | tac > $reversertmp	# The actual reversing
	
	LimitTmp=`mktemp` # Temporary file to save entries in
	while read line
	do
		logDate=`echo "$line" | cut -d " " -f 4,5 | sed 's/\[\([0-9]\{2\}\)\/\([A-Za-z]\{3\}\)\/\([0-9]\{4\}\)\(.*\)/\3\ \2\ \1/'`
		lyear=`echo $logDate | cut -d " " -f 1`
		lmonth=`echo $logDate | cut -d " " -f 2`
		lday=`echo $logDate | cut -d " " -f 3`
		lhour=`echo "$line" | cut -d ":" -f 2,3`
		logDate=`date -d "$lmonth $lday $lyear $lhour" "+%Y%m%d%H%M"`

		if [ $logDate -gt $oldestAllowedDate ] ;
			then
				echo $line >> $LimitTmp
		else
			break
		fi	
	done < $reversertmp
	INFILE=$LimitTmp
	cat $LimitTmp
	exit 1
	unlink $reversertmp
}

PostProcessing() {
	if [ $LimitFlag -gt 0 ] || [ -z "$INFILE" ] ;
	then
		LimitResult
	fi
}

Preprocessing() {

test1="-"
	if [ "$INFILE" = "$test1" ] ;
	then
		INFILE=/dev/stdin 		# Read standard in
	fi

	if [ $HourFlag -gt 0 ] ;
	then
		PreprocesHours
	fi

	if [ $DayFlag -gt 0 ] ;
	then
		PreprocessDays
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
while getopts n:h:d:c2rFt option
do
	case $option in
	n)
#		echo "-n was triggered, Paramenter: $OPTARG"
		LimitFlag=$OPTARG ;;
	h)
#		echo "-h was triggered, Paramenter: $OPTARG"
		HourFlag=$OPTARG;;
	d)
#		echo "-d was triggered, Paramenter: $OPTARG"
		DayFlag=$OPTARG;;
	c)
#		echo "-c was triggered"
		cFlag=1;;
	2)
#		echo "-2 was triggered"
		twoFlag=1;;
	r)
#		echo "-r was triggered"
		rFlag=1;;
	F)
#		echo "-F was triggered"
		FFlag=1;;
	t)
#		echo "-t was triggered"
		tFlag=1;;
	:)
#		echo no # TODO, when do I enter here?
		;;
	\?)
		ArgumentUse; exit 0;;
	*)
		ArgumentUse; exit 1;;
	esac
done

shift $(($OPTIND - 1)) 	# This shifts all the getopts arguments that has been used away from the argumentlist
INFILE=$1		# This should now correspond to the last
#echo "File: $INFILE" 	# Test to verify file
#echo "\n"

# If no of the mandatory arguments exist, print error message and exit
if [ $cFlag -eq 0 ] && [ $twoFlag -eq 0 ] && [ $rFlag -eq 0 ] && [ $FFlag -eq 0 ] && [ $tFlag -eq 0 ] ; then
	echo "\n\tAtleast one of the following options must be specified\n\t -c | -2 | -r | -F | -t \n"
	exit 1
fi

if [ $cFlag -gt 0 ] ;
then
	# Check to see if any preprocessing needs to be done on the text, also do it if true
	Preprocessing

	Text=`cut -d " " -f1 $INFILE | sort -rn -k1,1 | uniq -c | sort -nr -k1,1 | sed 's/\(.*\)\ \([0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\)/\2\\1/'`
		# Cut all fields on space and select first field (ips)
		# Combine all uniq fields and print occurence afterwards
		# sort after numerical values and reverse order 
		# switch places on the fields, IP first

	PostProcessing	# Check to see if any postprocessing needs to be done on the text, also do it if true
	echo "$Text"	# Print text
fi

if [ $twoFlag -gt 0 ] ;
then
	Preprocessing	# Check to see if any preprocessing needs to be done on the text, also do it if true

	Text=`grep ' 200 \| 201 \| 202 \| 204' $INFILE | cut -d " " -f1 | uniq -c | sort -rn | sed 's/\(.*\)\ \([0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\)/\2\1/'`
		# Get all rows that has a space followed by an 200/201/202/204 status code AND an space efter
		# Cut all lines on space and save first field (ips)
		# Combine all unique fields and print occurence afterwards
		# switch places on the fields, IP first

	PostProcessing	# Check to see if any postprocessing needs to be done on the text, also do it if true
	echo "$Text"	# Print text
fi

if [ $rFlag -gt 0 ] ;
then
	Preprocessing	# Check to see if any preprocessing needs to be done on the text, also do it if true

	Text=`cut -d " " -f1,9 $INFILE | uniq -c | sort -rn -k1,1 | rev | cut -d " " -f1,2,3 | rev | cut -d " " -f 2,3`

	PostProcessing	# Check to see if any postprocessing needs to be done on the text, also do it if true
	echo "$Text"	# Print text
fi

if [ $FFlag -gt 0 ] ;
then
	Preprocessing	# Check to see if any preprocessing needs to be done on the text, also do it if true

	Text=`grep -v ' 200 \| 201 \| 202 \| 204 ' $INFILE | cut -d " " -f1,9 | sed 's/\([0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\)\ \(.*\)/\2\ \1/' | sort -rk1,1 | uniq -c | sort -k2,2nr -k1,1nr | sort -u -k2,2 -r`
		# Grep all successful resultcodes, inverse result.
		# cut everything on space and select field one and nine, ip and status code
		# Find ip and "something" else, and switch place on these two
		# Sort every row on their first field (status code) # (This dosen't exist anymore) and make it an stable sort (-s)
		# Combine all identical fields and print occurence
		# This is an two staged sort
			# first sort everything on second field (ips) numerical and inverse result
			# then sort on firest field (status code) numerical and inverse result
		# Then sort again on second field (ips)
	Text=`echo "$Text" | sed 's/^[ \t]*//' | cut -d " " -f 2,3`
	Text=`echo "$Text" | sed 's/\(.*\)\ \([0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\)/\2\ \1/'`

	PostProcessing	# Check to see if any postprocessing needs to be done on the text, also do it if true
	echo "$Text"	# Print text
fi

if [ $tFlag -gt 0 ] ;
then
	Preprocessing	# Check to see if any preprocessing needs to be done on the text, also do it if true

	singleOccurence=`mktemp`        # Alla dubletter
	result=`mktemp`                 # End result
	file=`mktemp`                   # Input filen

	cat $INFILE | cut -d " " -f 1,10 | grep -v - > $file                 # Kopiera orginalfilen till en tempfil, spara endast fält 1 och 10. IP och BYTE's
	cat $INFILE | cut -d " " -f 1 | sort | uniq > $singleOccurence       # Saves one entry of all unique

	while read ip # For each ip in $file
	do
	        tmp=`mktemp`                                    # Creates an temp fil
	        grep $ip $file | cut -d " " -f 2 > $tmp         # Greps on the ip in the file with all untaken entries, only saves second field, bytes

	        totalSize=0
	        while read size
	        do
	                totalSize=`expr $totalSize + $size`
	        done < $tmp

	        if [ ! $totalSize -eq 0 ]; then
	                echo "$ip $totalSize" >> $result                        # saves ip and result in the result file
	        fi
	        cat $file | grep -v $ip > $tmp
	        cat $tmp > $file
	unlink $tmp                                     # unlinks tmp filen

	done < $singleOccurence

	unlink $file            # unlink file
	unlink $singleOccurence # unlink file

	Text=`cat $result | sort -rn -k2,2`     # Print resultat
	unlink $result  # unlink file

	PostProcessing	# Check to see if any postprocessing needs to be done on the text, also do it if true
	echo "$Text"	# Print text
fi

# If any of the preprocessing options was enabled, unlink the file.
if [ $HourFlag -gt 0 ] || [ $DayFlag -gt 0 ];
then
	unlink $LimitTmp
fi
