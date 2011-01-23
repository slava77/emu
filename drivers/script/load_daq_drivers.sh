#!/bin/bash

# This script loads the local DAQ Gbit drivers provided it's run on a
# local DAQ host csc-daq{01..10}.

function load_daq_drivers(){

    # Unload the standard intel driver if it has been loaded by the system.
    # The system may load the standard intel driver if it recognizes our NICs.
    # It is however not needed as our e1000h should be used instead for the
    # spy channel readout. If the built-in NIC (for eth0) also happened to be
    # Intel, we would need the modified e1000, but now we have a Broadcom
    # built-in NIC.
    [[ $(/sbin/lsmod | grep -c '^e1000 ') -eq 0 ]] || /sbin/modprobe -r e1000

    # Bring down the interfaces
    for N in 2 3 4 5; do
	/sbin/ifconfig eth${N} down
    done

    # Unload the modules if they've been loaded already
    [[ $(/sbin/lsmod | grep -c '^e1000h ') -eq 0 ]] || /sbin/modprobe -r e1000h

    # Create schar devices if they don't yet exist
    for N in 2 3 4 5
      do
      [[ -c /dev/schar${N} ]] || ( mknod /dev/schar${N} c 23${N} 0 && chmod 777 /dev/schar${N} )
    done

    # Update module dependencies
    /sbin/depmod

    # Load new modules
    /sbin/modprobe e1000h

    # The position number inside the rack obtained from the host name
    PN=$(hostname -s | sed -e 's/[^\-]*-[^\-]*-0\?\([0-9]\)/\1/')
    # Bring up the interfaces
    for N in 2 3 4 5; do
	/sbin/ifconfig eth${N} down
	/sbin/ifconfig eth${N} 192.168.${PN}.${N} 
	/sbin/ifconfig eth${N} promisc mtu 8192 broadcast 192.168.255.255 netmask 255.255.0.0
    done

}


for ALIAS in csc-daq0{1..9} csc-daq10; do
    if [[ $(host $ALIAS | grep -i -c $(hostname -s)) -ge 1 ]]; then
	load_daq_drivers
	exit 0
    fi
done
