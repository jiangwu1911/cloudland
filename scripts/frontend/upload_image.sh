#!/bin/bash

cd `dirname $0`
source ../cloudrc

[ $# -lt 2 ] && echo "$0 <user> <file> [shared(yes|no)] [description]" && exit -1

owner=$1
img_file=$2
shared=$3
img_desc=$4

[ ! -f "$img_file" ] && die "File does not exist!"
img_dir=`dirname $img_file`
img_name=`basename $img_file`
img_size=`ls -l $img_file | cut -d' ' -f 5`
[ -z "$shared" ] && shared=no

swift -A $swift_url -U $swift_user -K $swift_pass stat images $img_name >/dev/null 2>&1
[ $? -eq 0 ] && die "Image already exists!"

cd $img_dir
swift -A $swift_url -U $swift_user -K $swift_pass upload images $img_name >/dev/null 2>&1
[ $? -ne 0 ] && die "Image upload failed!"

sqlite3 $db_file "insert into image (name, size, description, owner, shared) values ('$img_name', '$img_size', '$img_desc', '$owner', '$shared')"
[ $? -eq 0 ] && echo "Image $img_file uploaded successfully!"