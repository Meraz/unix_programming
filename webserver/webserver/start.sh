#! /bin/bash
clear
id=`ps aux | grep wserver | grep -v grep | cut -d " "  -f4`

if ! [ -z "$id" ]; then
#	echo KILL
	kill $id
fi

./wserver -p $1
