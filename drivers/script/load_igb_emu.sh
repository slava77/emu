#!/bin/bash

function load_igb_emu(){
    # To be invoked with the list of hook names, e.g.
    # load_igb_emu eth_hook_2_vme eth_hook_3_daq eth_hook_4_daq eth_hook_5_daq

    # Bring down the interfaces
    for N in 2 3 4 5; do
	/sbin/ifconfig eth${N} down
    done

    # Module aliases
    [[ -f /etc/modprobe.conf ]] && sed -i.bak -e '/^alias eth\([2345]\) [[:print:]]*/d' /etc/modprobe.conf
#    for N in 2 3 4 5; do
#	echo "alias eth${N} igb_emu" >> /etc/modprobe.conf
#    done

    # Unload the modules if they've been loaded already
    [[ $(/sbin/lsmod | grep -c 'igb ') -eq 0 ]] || /sbin/modprobe -r igb
    [[ $(/sbin/lsmod | grep -c 'igb_emu ') -eq 0 ]] || /sbin/modprobe -r igb_emu

    # Create schar devices if they don't yet exist
    for N in 2 3 4 5
      do
      [[ -c /dev/schar${N} ]] || ( mknod /dev/schar${N} c 23${N} 0 && chmod 777 /dev/schar${N} )
    done

    # Copy the driver and the requested hooks
    rm /lib/modules/$(uname -r)/kernel/drivers/net/igb/igb_emu.ko
    rm /lib/modules/$(uname -r)/kernel/drivers/net/eth_hook_*.ko
    cp ${DRIVERS_DIR}/igb_emu.ko /lib/modules/$(uname -r)/kernel/drivers/net/igb
    for HOOK in "$@"; do
	cp ${DRIVERS_DIR}/${HOOK}.ko /lib/modules/$(uname -r)/kernel/drivers/net/
    done    

    # Update module dependencies
    /sbin/depmod

    # Load new modules
    /sbin/modprobe igb_emu InterruptThrottleRate=0,0,0,0 QueuePairs=0,0,0,0 EEE=0,0,0,0 MDD=0,0,0,0

    # Bring up the interfaces
    for N in 2 3 4 5; do
	/sbin/ifconfig eth${N} down
	/sbin/ifconfig eth${N} 192.168.1.${N} 
	/sbin/ifconfig eth${N} promisc mtu 8192 broadcast 192.168.255.255 netmask 255.255.0.0
    done

}

####################################################################################################

# Kernel object files are installed in DRIVERS_DIR, from where we copy only those required on the current host to /lib/modules/$(uname -r)/kernel/drivers
# Let it be the same directory this script is in:
DRIVERS_DIR=${0%/*}

# Only load the drivers on hosts in this list of aliases:
for ALIAS in vmepc-e1x07-26-01; do
    if [[ $(host $ALIAS | grep -i -c $(hostname -s)) -ge 1 ]]; then
	load_igb_emu eth_hook_2_vme eth_hook_3_daq eth_hook_4_daq eth_hook_5_daq
	exit 0
    fi
done
