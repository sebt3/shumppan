#!/bin/bash
if [ $# -eq 1 ];then
	SRC="data/${1}.tmx"
	DST="data/${1}.map"
else
	SRC=$1
	DST=$2
fi

if [ ! -f $SRC ];then
	echo source $SRC file not found
	exit
fi
IMG=$(awk '/image source/{sub(/.*="/,"",$2);sub(/"/,"",$2);print $2}'< $SRC)
WIDTH=$(awk '/map /{sub(/.*="/,"",$4);sub(/"/,"",$4);print $4}'< $SRC)
HEIGHT=$(awk '/map /{sub(/.*="/,"",$5);sub(/"/,"",$5);print $5}'< $SRC)

{
echo "$IMG $WIDTH:$HEIGHT"
awk 'BEGIN{p=0}/\/data/{p=0}p==1{gsub(/,/," ");print}/data /{p=1}'<$SRC
}>$DST
