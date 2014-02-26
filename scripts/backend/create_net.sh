#!/bin/bash

cd `dirname $0`
source ../cloudrc

[ $# -lt 6 ] && echo "$0 <vlan> <network> <netmask> <gateway> <start_ip> <end_ip>" && exit -1

vlan=$1
network=$2
netmask=$3
gateway=$4
start_ip=$5
end_ip=$6
hyper=$7

vm_br=br$vlan
ip netns add vlan$vlan
brctl addbr $vm_br
ip link add ns-$vlan type veth peer name tap-$vlan
brctl addif $vm_br tap-$vlan
brctl setfd $vm_br 0
ip link set tap-$vlan up
ip link set $vm_br up
ip link set ns-$vlan netns vlan$vlan
ip netns exec vlan$vlan ip link set ns-$vlan up
ip netns exec vlan$vlan ip link set lo up
ip netns exec vlan$vlan ifconfig ns-$vlan $start_ip netmask $netmask
if [ $vlan -ge 4095 ]; then
    ip link add vxlan-$vlan type vxlan id $vlan group ${vxlan_mcast_addr} dev ${vxlan_interface}
    ip link set vxlan-$vlan up
    brctl addif $vm_br vxlan-$vlan
fi

dns_host=$dmasq_dir/vlan$vlan.host
dns_opt=$dmasq_dir/vlan$vlan.opts
ipcalc -c $gateway >/dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "tag:tag$vlan,option:router,$gateway" >> $dns_opt
else
    echo "tag:tag$vlan,option:router" >> $dns_opt
fi
[ -n "$dns_server" ] && echo "tag:tag$vlan,option:dns-server,$dns_server" >> $dns_opt
dns_pid=`ps -ef | grep dnsmasq | grep "\<interface=ns-$vlan\>" | awk '{print $2}'`
if [ -z "$dns_pid" ]; then
    pid_file=$dmasq_dir/vlan$vlan.pid
    ip netns exec vlan$vlan /usr/sbin/dnsmasq --no-hosts --no-resolv --strict-order --bind-interfaces --interface=ns-$vlan --except-interface=lo --dhcp-range=set:tag$vlan,$network,static,86400s --pid-file=$pid_file --dhcp-hostsfile=$dns_host --dhcp-optsfile=$dns_opt --leasefile-ro --dhcp-ignore='tag:!known'
fi
echo "|:-COMMAND-:| /opt/cloudland/scripts/frontback/`basename $0` $vlan `hostname -s`"
