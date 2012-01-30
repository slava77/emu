#!/bin/bash

# This script loads the local DAQ Gbit drivers provided it's run on
TARGET_HOSTS=( emu42fastprod01 )

function load_daq_pcvme_drivers(){

    # Make sure eth{2,3} are alias for dl2k
    [[ -f /etc/modprobe.conf ]] && sed -i.bak -e '/^alias eth\([23]\) [^ ]*$/d' /etc/modprobe.conf
    for N in 2 3; do
	echo "alias eth${N} dl2k" >> /etc/modprobe.conf
    done

    # Bring down the interfaces
    for N in 2 3; do
	/sbin/ifconfig eth${N} down
    done

    # Unload the modules if they've been loaded already
    [[ $(/sbin/lsmod | grep -c '^dl2k ') -eq 0 ]] || /sbin/modprobe -r dl2k

    # Create schar devices if they don't yet exist
    for N in 2 3; do
      [[ -c /dev/schar${N} ]] || ( mknod /dev/schar${N} c 23${N} 0 && chmod 777 /dev/schar${N} )
    done

    # Update module dependencies
    /sbin/depmod

    # Load new modules
    /sbin/modprobe dl2k

    # Bring up the interfaces
    for N in 2 3; do
	/sbin/ifconfig eth${N} down
	/sbin/ifconfig eth${N} 192.168.1.${N} 
	/sbin/ifconfig eth${N} -arp promisc mtu 8192 broadcast 192.168.255.255 netmask 255.255.0.0
    done

}

# Load drivers on these nodes only
for ALIAS in ${TARGET_HOSTS[*]}; do
    if [[ $(host $ALIAS | grep -i -c $(hostname -s)) -ge 1 ]]; then
	load_daq_pcvme_drivers
	exit 0
    fi
done
