#!/bin/bash

cd `dirname $0`
source ../cloudrc

[ $# -lt 4 ] && echo "$0 <vm_ID> <image> <vlan> <mac> [name] [ip] [cpu] [memory]"

vm_ID=$1
img_name=$2
vlan=$3
vm_mac=$4
vm_name=$5
vm_ip=$6 
vm_cpu=$7
vm_mem=$8
vm_stat=error
vm_vnc=""

vm_img=/var/lib/libvirt/images/$vm_ID.img
if [ ! -f "$cache_dir/$img_name" ]; then
    swift -A $swift_url -U $swift_user -K $swift_pass download images $img_name -o $cache_dir/$img_name
fi
if [ ! -f "$cache_dir/$img_name" ]; then
    echo "Image $img_name downlaod failed!"
    echo "|:-COMMAND-:| /opt/cloudland/scripts/frontback/`basename $0` $vm_ID $vm_stat `hostname -s` $vm_mac"
    exit -1
fi

qemu-img convert -f qcow2 -O raw $cache_dir/$img_name $vm_img

vm_br=br$vlan
cat /proc/net/dev | grep -q "^\<$vm_br\>"
if [ $? -ne 0 ]; then
    brctl addbr $vm_br
    ip link set $vm_br up
    if [ $vlan -ge 4095 ]; then
        ip link add vxlan-$vlan type vxlan id $vlan group ${vxlan_mcast_addr} dev ${vxlan_interface}
        ip link set vxlan-$vlan up
        brctl addif $vm_br vxlan-$vlan
    fi
fi
[ -z "$vm_mem" ] && vm_mem='1024m'
[ -z "$vm_cpu" ] && vm_cpu=1
let vm_mem=${vm_mem%[m|M]}*1024
vnc_pass=`date | sum | cut -d' ' -f1`
vm_xml=$xml_dir/$vm_ID.xml
cp $xml_dir/template.xml $vm_xml
sed -i "s/VM_ID/$vm_ID/g; s/VM_MEM/$vm_mem/g; s/VM_CPU/$vm_cpu/g; s#VM_IMG#$vm_img#g; s/VM_MAC/$vm_mac/g; s/VM_BRIDGE/$vm_br/g; s/VNC_PASS/$vnc_pass/g;" $vm_xml
virsh create $vm_xml
if [ $? -eq 0 ]; then 
    vm_stat=running
    vnc_port=`cat /var/run/libvirt/qemu/$vm_ID.xml | grep "graphics type='vnc'" | sed "s/.*port='\([0-9]*\)' .*/\1/"` 
    vm_vnc="$vnc_port:$vnc_pass"
fi
echo "|:-COMMAND-:| /opt/cloudland/scripts/frontback/`basename $0` $vm_ID $vm_stat `hostname -s` $vm_mac '$vm_vnc'"
