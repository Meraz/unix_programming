#! /bin/bash
clear

# Function flags
pFlag=0
lFlag=""
dFlag=0

# Out
out='./wserver'

ArgumentUse() {
	echo "This is not how you use this file!"
}

# Check and set flags
while getopts p:l:d option
do
	case $option in
	p)
#		echo "-n was triggered, Paramenter: $OPTARG"
		pFlag=$OPTARG ;;
	l)
#		echo "-h was triggered, Paramenter: $OPTARG"
		lFlag=$OPTARG;;
	d)
#		echo "-d was triggered"
		dFlag=1;;
	\?)
		ArgumentUse; exit 0;;
	*)
		ArgumentUse; exit 1;;
	esac
done


if [ ! $pFlag -eq 0 ] ; 
	then
	out="$out -p $pFlag"
fi

if [ ! -z "$lFlag"  ] ; 
	then
	out="$out -l $lFlag"
fi

if [ ! $dFlag -eq 0 ] ; 
	then
	out="$out -d"
fi

id=`ps aux | grep wserver | grep -v grep | cut -d " "  -f4`

if ! [ -z "$id" ]; then
#	echo KILL
	kill $id
fi

echo $out
`$out`
