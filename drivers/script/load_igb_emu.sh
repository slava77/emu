#!/bin/bash

function module_parameters(){
    # Print module parameters (same value for all ports) in a comma separated list ready to be passed to modprobe.
    #   argument 1: number of ports
    #   argument 2: value
    PARAMS=$2
    for ((I=1;I<$1;I++)); do
	PARAMS=${PARAMS}",$2"
    done
    echo -n $PARAMS
}

function load_igb_emu(){
    # To be invoked with the list of hook names, e.g.
    # load_igb_emu eth_hook_2_vme eth_hook_3_ddu eth_hook_4_dmb eth_hook_5_dmb

    # Bring down the interfaces
    echo "Bringing down the interfaces"
    for N in 2 3 4 5; do
	/sbin/ifconfig eth${N} down
    done

    # Unload the modules if they've been loaded already
    echo "Unloading the modules if they've been loaded already"
    echo "lsmod | grep igb"
    /sbin/lsmod | grep igb
    /sbin/lspci -k | grep -A 3 Ethernet
    if [[ $(/sbin/lsmod | grep -c 'igb_emu ') -gt 0 ]]; then
	echo "/sbin/modprobe -r igb_emu"
	/sbin/modprobe -r igb_emu
    fi
    if [[ $(/sbin/lsmod | grep -c 'igb ') -gt 0 ]]; then
	echo "/sbin/modprobe -r igb"
	/sbin/modprobe -r igb
    fi
    echo "lsmod | grep igb"
    /sbin/lsmod | grep igb
    /sbin/lspci -k | grep -A 3 Ethernet

    # Create schar devices if they don't yet exist
    echo "Creating schar devices if they don't yet exist"
    for N in 2 3 4 5
      do
      [[ -c /dev/schar${N} ]] || ( mknod /dev/schar${N} c 23${N} 0 && chmod 777 /dev/schar${N} )
    done

    # Copy the driver and the requested hooks
    echo "Copying the driver and the requested hooks"
    rm -f /lib/modules/$(uname -r)/kernel/drivers/net/igb/igb_emu.ko
    rm -f /lib/modules/$(uname -r)/kernel/drivers/net/igb/eth_hook_*.ko
    rm -f /lib/modules/$(uname -r)/kernel/drivers/net/eth_hook_*.ko
    cp ${DRIVERS_DIR}/igb_emu.ko /lib/modules/$(uname -r)/kernel/drivers/net/igb
    for HOOK in "$@"; do
	cp ${DRIVERS_DIR}/${HOOK}.ko /lib/modules/$(uname -r)/kernel/drivers/net/igb
    done    

    # Module aliases. Note that, in the case of a built-in Intel NIC with module igb, it will also be replaced with igb_emu.
    echo "Updating /etc/modprobe.conf"
    [[ -f /etc/modprobe.conf ]] && sed -i.bak -e 's/^alias eth\([012345]\) igb$/alias eth\1 igb_emu/g' /etc/modprobe.conf
    # Count the number of ports served by igb_emu
    NPORTS=$(( 4 + $(grep -c '^alias eth[01] igb' /etc/modprobe.conf) ))

    # Update module dependencies
    echo "Updating module dependencies"
    /sbin/depmod

    # Load new modules
    echo "Loading igb_emu"
    echo "/sbin/modprobe igb_emu InterruptThrottleRate=$(module_parameters $NPORTS 0) QueuePairs=$(module_parameters $NPORTS 0) EEE=$(module_parameters $NPORTS 0) MDD=$(module_parameters $NPORTS 0)"
    /sbin/modprobe igb_emu InterruptThrottleRate=$(module_parameters $NPORTS 0) QueuePairs=$(module_parameters $NPORTS 0) EEE=$(module_parameters $NPORTS 0) MDD=$(module_parameters $NPORTS 0)
    echo "lsmod | grep igb"
    /sbin/lsmod | grep igb
    /sbin/lspci -k | grep -A 3 Ethernet

    # Disable automatic start on boot
    for N in 2 3 4 5; do
	if [[ -f /etc/sysconfig/network-scripts/ifcfg-eth${N} ]]; then
	    sed -i.bak -e 's/^BOOTPROTO=[[:print:]]*$/BOOTPROTO=static/g' -e 's/^ONBOOT=[[:print:]]*$/ONBOOT=no/g' /etc/sysconfig/network-scripts/ifcfg-eth${N}
	fi
    done    

    # Restart the built-in interfaces, too, in case they're also Intel supported by igb.
    for N in 0 1; do
	if [[ $( grep -c "^alias eth${N} igb" /etc/modprobe.conf ) -gt 0 ]]; then
	    sleep 2
	    echo "Restarting the built-in interface eth${N}"
	    /etc/sysconfig/network-scripts/ifdown eth${N}
	    sleep 5
	    /etc/sysconfig/network-scripts/ifup-eth eth${N}
	fi
    done

    # Bring up the interfaces for the plugged-in NICs
    echo "Bringing up the interfaces for the plugged-in NICs"
    for N in 2 3 4 5; do
	/sbin/ifconfig eth${N} down
	/sbin/ifconfig eth${N} 192.168.1.${N} 
	/sbin/ifconfig eth${N} promisc mtu 8192 broadcast 192.168.255.255 netmask 255.255.0.0
    done

    # Disable checks that could cause some packets to be dropped
    echo "Disabling checks that could cause some packets to be dropped"
    for N in 2 3 4 5; do
	/sbin/ethtool -K eth${N} rx off
	/sbin/ethtool -K eth${N} sg off
    done    

}

####################################################################################################

# Kernel object files are installed in DRIVERS_DIR, from where we copy only those required on the current host to /lib/modules/$(uname -r)/kernel/drivers
# Let it be the same directory this script is in:
DRIVERS_DIR=${0%/*}

# Only load the drivers on hosts in this list of aliases:
for ALIAS in vmepc-e1x07-26-01 emuslice06 emuslice12 emu42fastprod01 emu-me11-step{1,2,3,4}; do
    if [[ $(host $ALIAS | grep -i -c $(hostname -s)) -ge 1 ]]; then
	load_igb_emu eth_hook_2_dmb eth_hook_3_dmb eth_hook_4_ddu eth_hook_5_ddu
	exit 0
    fi
done
