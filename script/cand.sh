#! /bin/bash

while read loc can num
do
  if ! [ ${!can} ]
  then
    cans+=($can)
  fi
  (( $can += num ))
done < votes.txt

#echo ${cans[*]}


for can in ${cans[*]}
do
  echo $can ${!can}
done
