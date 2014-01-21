#!/bin/bash

function load_e1000_emu(){
    # To be invoked with the list of hook names, e.g.
    # load_e1000_emu eth_hook_2_vme eth_hook_3_ddu eth_hook_4_dmb eth_hook_5_dmb

    # Bring down the interfaces
    echo "Bringing down the interfaces"
    for N in 2 3 4 5; do
	/sbin/ifconfig eth${N} down
    done

    # Unload the modules if they've been loaded already
    echo "Unloading the modules if they've been loaded already"
    echo "lsmod | grep e1000"
    /sbin/lsmod | grep e1000
    /sbin/lspci -k | grep -A 3 Ethernet
    if [[ $(/sbin/lsmod | grep -c 'e1000h ') -gt 0 ]]; then
	echo "/sbin/modprobe -r e1000h"
	/sbin/modprobe -r e1000h
    fi
    if [[ $(/sbin/lsmod | grep -c 'e1000 ') -gt 0 ]]; then
	echo "/sbin/modprobe -r e1000"
	/sbin/modprobe -r e1000
    fi
    echo "lsmod | grep e1000"
    /sbin/lsmod | grep e1000
    /sbin/lspci -k | grep -A 3 Ethernet

    # Create schar devices if they don't yet exist
    echo "Creating schar devices if they don't yet exist"
    for N in 2 3 4 5
      do
      [[ -c /dev/schar${N} ]] || ( mknod /dev/schar${N} c 23${N} 0 && chmod 777 /dev/schar${N} )
    done

    # Copy the driver and the requested hooks
    echo "Copying the driver and the requested hooks"
    rm -f /lib/modules/$(uname -r)/kernel/drivers/net/e1000/e1000h.ko
    rm -f /lib/modules/$(uname -r)/kernel/drivers/net/e1000/eth_hook_*.ko
    rm -f /lib/modules/$(uname -r)/kernel/drivers/net/eth_hook_*.ko
    cp ${DRIVERS_DIR}/e1000h.ko /lib/modules/$(uname -r)/kernel/drivers/net
    for HOOK in "$@"; do
	cp ${DRIVERS_DIR}/${HOOK}.ko /lib/modules/$(uname -r)/kernel/drivers/net
    done    

    # Module aliases. Note that, in the case of a built-in Intel NIC with module e1000, it will also be replaced with e1000h.
    echo "Updating /etc/modprobe.conf"
    [[ -f /etc/modprobe.conf ]] && sed -i.bak -e 's/^alias eth\([012345]\) e1000$/alias eth\1 e1000h/g' /etc/modprobe.conf
    # Count the number of ports served by e1000h
    NPORTS=$(( 4 + $(grep -c '^alias eth[01] e1000' /etc/modprobe.conf) ))

    # Update module dependencies
    echo "Updating module dependencies"
    /sbin/depmod

    # Load new modules
    echo "Loading e1000h"
    echo "/sbin/modprobe e1000h"
    /sbin/modprobe e1000h
    echo "lsmod | grep e1000"
    /sbin/lsmod | grep e1000
    /sbin/lspci -k | grep -A 3 Ethernet

    # Disable automatic start on boot
    for N in 2 3 4 5; do
	if [[ -f /etc/sysconfig/network-scripts/ifcfg-eth${N} ]]; then
	    sed -i.bak -e 's/^BOOTPROTO=[[:print:]]*$/BOOTPROTO=static/g' -e 's/^ONBOOT=[[:print:]]*$/ONBOOT=no/g' /etc/sysconfig/network-scripts/ifcfg-eth${N}
	fi
    done    

    # Restart the built-in interfaces, too, in case they're also Intel supported by e1000.
    for N in 0 1; do
	if [[ $( grep -c "^alias eth${N} e1000" /etc/modprobe.conf ) -gt 0 ]]; then
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

}

####################################################################################################

# Kernel object files are installed in DRIVERS_DIR, from where we copy only those required on the current host to /lib/modules/$(uname -r)/kernel/drivers
# Let it be the same directory this script is in:
DRIVERS_DIR=${0%/*}

# Only load the drivers on hosts in this list of aliases:
for ALIAS in vmepc-e1x07-25-01; do
    if [[ $(host $ALIAS | grep -i -c $(hostname -s)) -ge 1 ]]; then
	load_e1000_emu eth_hook_2_dmb eth_hook_3_ddu eth_hook_4_ddu eth_hook_5_ddu
	exit 0
    fi
done
