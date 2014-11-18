#!/bin/zsh

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
	/sbin/ifconfig ${IF_NAME[$N]} down
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
    rm -f /lib/modules/$(uname -r)/kernel/drivers/net/igb/eth_hook_*.ko(N)
    rm -f /lib/modules/$(uname -r)/kernel/drivers/net/eth_hook_*.ko(N)
    cp ${DRIVERS_DIR}/igb_emu.ko /lib/modules/$(uname -r)/kernel/drivers/net/igb
    for HOOK in "$@"; do
	cp ${DRIVERS_DIR}/${HOOK}.ko /lib/modules/$(uname -r)/kernel/drivers/net/igb
    done    

    NPORTS=4
    # Module aliases. Note that, in the case of a built-in Intel NIC with module igb, it will also be replaced with igb_emu.
    if [[ $SLC_MAJOR -eq 5 ]]; then
	echo "Updating /etc/modprobe.conf"
	[[ -f /etc/modprobe.conf ]] && sed -i.bak -e 's/^alias eth\([012345]\) igb$/alias eth\1 igb_emu/g' /etc/modprobe.conf
        # Count the number of on-board ports to be served by igb_emu
	[[ -f /etc/modprobe.conf ]] && (( NPORTS+=$(grep -c '^alias eth[01] igb' /etc/modprobe.conf) ))
    elif [[ $SLC_MAJOR -eq 6 ]]; then
	echo "Updating /etc/modprobe.d"
	sed -i.bak -e 's:^\(alias p[12]p[12] [^ ]\+\)$:# \1 # commented out by '${0}':g' -e 's:^alias \(e[12]\) \(igb[^ ]*\)$:# alias \1 \2 # commented out by '${0}'\nalias \1 igb_emu:g' /etc/modprobe.d/*.conf
	rm -f /etc/modprobe.d/net-igb_emu.conf
	for N in 2 3 4 5; do
	    print "alias ${IF_NAME[$N]} igb_emu" >> /etc/modprobe.d/net-igb_emu.conf
	done
        # Count the number of on-board ports to be served by igb_emu
	(( NPORTS+=$( grep -c "^alias em[12] igb" =(cat /etc/modprobe.d/*.conf) ) ))
    fi
	
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

    # Disable automatic start on boot and network manager for the plugged-in interfaces
    for N in 2 3 4 5; do
	if [[ -f /etc/sysconfig/network-scripts/ifcfg-${IF_NAME[$N]} ]]; then
	    sed -i.bak -e '/^BOOTPROTO=/d' -e '/^ONBOOT=/d' -e '/^NM_CONTROLLED=/d' -e '/^TYPE=/d' /etc/sysconfig/network-scripts/ifcfg-${IF_NAME[$N]}
	    print "BOOTPROTO=static" >> /etc/sysconfig/network-scripts/ifcfg-${IF_NAME[$N]}
	    print "ONBOOT=no"        >> /etc/sysconfig/network-scripts/ifcfg-${IF_NAME[$N]}
	    print "NM_CONTROLLED=no" >> /etc/sysconfig/network-scripts/ifcfg-${IF_NAME[$N]}
	    print "TYPE=Ethernet"    >> /etc/sysconfig/network-scripts/ifcfg-${IF_NAME[$N]}
	fi
    done

    sleep 2;

    # Restart the built-in interfaces, too, in case they're also Intel supported by igb.
    for N in 0 1; do
	if [[ $SLC_MAJOR -eq 5 && -f /etc/modprobe.conf && $( grep -c "^alias ${IF_NAME[$N]} igb" /etc/modprobe.conf ) -gt 0 ]] || \
	   [[ $SLC_MAJOR -eq 6 && $( grep -c "^alias ${IF_NAME[$N]} igb" =(cat /etc/modprobe.d/*.conf) ) -gt 0 ]]; then
	    sleep 2
	    echo "Restarting the built-in interface ${IF_NAME[$N]}"
	    /etc/sysconfig/network-scripts/ifdown ${IF_NAME[$N]}
	    sleep 5
	    /etc/sysconfig/network-scripts/ifup-eth ${IF_NAME[$N]}
	fi
    done

    # Bring up the interfaces for the plugged-in NICs
    echo "Bringing up the interfaces for the plugged-in NICs"
    for N in 2 3 4 5; do
	print "/sbin/ifconfig ${IF_NAME[$N]} down"
	/sbin/ifconfig ${IF_NAME[$N]} down
	print "/sbin/ifconfig ${IF_NAME[$N]} 192.168.1.${N}"
	/sbin/ifconfig ${IF_NAME[$N]} 192.168.1.${N} 
	print "/sbin/ifconfig ${IF_NAME[$N]} promisc mtu 8192"
	/sbin/ifconfig ${IF_NAME[$N]} promisc mtu 8192
    done

}

####################################################################################################

# Kernel object files are installed in DRIVERS_DIR, from where we copy only those required on the current host to /lib/modules/$(uname -r)/kernel/drivers
# Let it be the same directory this script is in:
DRIVERS_DIR=${0%/*}

# Mapping from arbitrary old-fashioned eth<N> to consistent physical p<slot>p<port> interface name, if necessary
typeset -A IF_NAME
if [[ $(uname -r | grep -c '^2\.6\.32-') -gt 0 ]]; then
    IF_NAME=(
	0 em1
	1 em2
	2 p1p1
	3 p1p2
	4 p2p1
	5 p2p2
	)
    SLC_MAJOR=6
elif [[ $(uname -r | grep -c '^2\.6\.18-') -gt 0 ]]; then
    IF_NAME=(
	0 eth0
	1 eth1
	2 eth2
	3 eth3
	4 eth4
	5 eth5
	)
    SLC_MAJOR=5
else
    print "Cannot determine network interface naming scheme. Exiting."
    exit 1
fi
print "Seems to be SLC${SLC_MAJOR}. Assuming interface names ${IF_NAME}."

# Only load the drivers on hosts in this list of aliases:
for ALIAS in vmepc-e1x07-26-01 emuslice06 emuslice12 emu42fastprod01 emu-me11-step{1,2,3,4} emuupgrade1 ctrl-s2g18-{15..18}-01; do
    if [[ $(host $ALIAS | grep -i -c $(hostname -s)) -ge 1 ]]; then
	load_igb_emu eth_hook_2_vme eth_hook_3_vme eth_hook_4_vme eth_hook_5_vme
	exit 0
    fi
done
