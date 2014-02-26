#!/bin/bash

cd `dirname $0`
source ../cloudrc

[ $# -lt 2 ] && echo "$0 <user> <vlan>" && exit -1

owner=$1
vlan=$2
num=`sqlite3 $db_file "select count(*) from network where vlan='$vlan' and owner='$owner'"`
[ $num -lt 1 ] && die "Not the network owner!"
num=`sqlite3 $db_file "select count(*) from instance where vlan='$vlan' and status='running'"`
[ $num -ge 1 ] && die "The network is being used!"

sqlite3 $db_file "delete from network where vlan='$vlan'"
sqlite3 $db_file "delete from address where vlan='$vlan'"

hyper=`echo $sql_ret | cut -d'|' -f3`
hyper_id=`sqlite3 $db_file "select id from compute where hyper_name='$hyper'"`
/opt/cloudland/bin/sendmsg "inter $hyper_id" "/opt/cloudland/scripts/backend/`basename $0` $vlan"
echo "Vlan $vlan was deleted!"
