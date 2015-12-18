#!/bin/bash

set -e

if [ $# -lt 4 ]
then
echo "Usage: "$(basename $0)" VOL_NAME IMG_FILE IMG_SIZE IN_DIR"
echo "IMG_SIZE must be expressed in Megabyte"
exit -1
fi

#Some distributions put tools like mkdosfs in sbin, which isn't always in the default PATH
PATH="${PATH}:/sbin:/usr/sbin"

VOL_NAME=$1
IMG_FILE=$2
IMG_SIZE=$3
BIN_DIR=$4

#Create filesystem image
rm -f $IMG_FILE
echo "Create FAT filesystem of size $IMG_SIZE"
mkdosfs -F 16 -n $VOL_NAME -C $IMG_FILE $(($IMG_SIZE * 1024))

for f in $BIN_DIR/*; do
    if [[ ! "$f" =~ ".sh" ]] && [[ ! "$f" =~ ".part" ]]; then
        echo "Copying $f in $IMG_FILE"
        mcopy -s -Q -i $IMG_FILE $f ::$(basename $f)
    fi
done

#Cut the IMG_SIZE to the lower size as possible
cat $IMG_FILE | hd > tmp
LOWEST_SIZE=`tail -n 3 tmp | head -n 1 | cut -d ' ' -f 1`

#Convert in upper case
UPPER=`echo $LOWEST_SIZE | tr '[:lower:]' '[:upper:]'`
#Then in decimal
LOWEST_SIZE=`echo "ibase=16; $UPPER" | bc`

#Cut the image
dd if=$IMG_FILE of=$IMG_FILE"2" bs=1 count=$LOWEST_SIZE

#Delete temporary files
rm -f tmp
mv $IMG_FILE"2" $IMG_FILE
