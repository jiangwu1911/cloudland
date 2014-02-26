#!/bin/bash

cd `dirname $0`
source ../cloudrc

[ $# -lt 2 ] && echo "$0 <user> <url> [shared(yes|no)] [description]" && exit -1

owner=$1
url=$2
shared=$3
desc=$4

file=/tmp/`basename $url`
wget -q $url -O $file
[ -f "$file" ] || die "Failed to download file!"

size=`ls -l $file | cut -d' ' -f 5`
[ $size -gt 0 ] || die "Invalid File"

./upload_image.sh $owner $file $shared $desc
