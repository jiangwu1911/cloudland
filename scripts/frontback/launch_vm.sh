#!/bin/bash

cd `dirname $0`
source ../cloudrc

vm_id=$1
vm_stat=$2
hyper=$3
vm_mac=$4
vm_vnc=$5

hyper_ip=`sqlite3 $db_file "select ip_addr from compute where hyper_name='$hyper'"`
vm_vnc=$hyper_ip:$vm_vnc
sqlite3 $db_file "update instance set inst_id='$vm_id', status='$vm_stat', hyper_name='$hyper', vnc='$vm_vnc' where mac_addr='$vm_mac'"
