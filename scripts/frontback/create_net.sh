#!/bin/bash

cd `dirname $0`
source ../cloudrc

vlan=$1
hyper=$2

sqlite3 $db_file "update network set router='$hyper' where vlan='$vlan'"

