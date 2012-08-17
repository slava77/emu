#!/bin/bash

# This script loads the local DAQ Gbit drivers provided it's run on a
# local DAQ host csc-daq{01..10}.

function load_daq_drivers(){

    # Unload the standard intel driver if it has been loaded by the system.
    # The system may load the standard intel driver if it recognizes our NICs.
    # It is however not needed as our e1000h should be used instead for the
    # spy channel readout. If the built-in NIC (for eth0) also happens to be
    # Intel, we must unload the standard driver first, load e1000h and set up 
    # the corresponding interfaces, then reload the standard driver.
    # Also, make sure eth* are alias for e1000h and not e1000e.
    # It's important that the built-in NICs' eth* also be alias for e1000h and not e1000e,
    # or else e1000e will be automatically reloaded immediately after when we unload it, 
    # and it will take charge of _all_ Intel NICs, preventing e1000h from doing its job.
    [[ -f /etc/modprobe.conf ]] && sed -i.bak -e 's/^alias eth\([2345]\) e1000e$/alias eth\1 e1000h/g' /etc/modprobe.conf
    [[ $(/sbin/lsmod | grep -c '^e1000e ') -eq 0 ]] || /sbin/modprobe -r e1000e

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

    # Bring up the interfaces
    for N in 2 3 4 5; do
	/sbin/ifconfig eth${N} down
	/sbin/ifconfig eth${N} 192.168.1.${N} 
	/sbin/ifconfig eth${N} promisc mtu 8192 broadcast 192.168.255.255 netmask 255.255.0.0
    done

}

# Execute on hosts with non-Intel built-in NIC(s)
for ALIAS in csc-daq0{1..9} csc-daq10; do
    if [[ $(host $ALIAS | grep -i -c $(hostname -s)) -ge 1 ]]; then
	load_daq_drivers
	exit 0
    fi
done

# Execute on hosts with Intel built-in NIC(s)
for ALIAS in emume11; do
    if [[ $(host $ALIAS | grep -i -c $(hostname -s)) -ge 1 ]]; then
	load_daq_drivers
        # Now that all other interfaces have been set up, reload e1000e for the built-in NIC. 
	# Then the interfaces to the built-in NIC(s) (eth[01]) should be brought up automatically.
        /sbin/modprobe e1000e
	exit 0
    fi
done
