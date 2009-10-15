%define workingDir %(pwd)
%define _topdir %{workingDir}/rpm
%define kernel_version %(uname -r)

Summary: CMS Emu local DAQ Gbit drivers for kernel %{kernel_version}
Name: emu-daqdriver
Version: 1.1.1
Release: 1
License: none
Group: none
URL: none
Source0: cvs
BuildRoot: /tmp/%{name}-%{version}-%{release}-root


%description


%build


%pre
# Bring down the interfaces
for N in 2 3 4 5; do
    /sbin/ifconfig eth${N} down
done

# Unload old modules
[[ $(/sbin/lsmod | grep -c '^e1000h ') -eq 0 ]] || /sbin/modprobe -r e1000h

# Unload the standard Intel driver.
# The standard Intel driver seems to be loaded by the system if it recognizes _any_ Intel NIC.
[[ $(/sbin/lsmod | grep -c '^e1000 ') -eq 0 ]] || /sbin/modprobe -r e1000


%install
[[ ${RPM_BUILD_ROOT} != "/" ]] && rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/lib/modules/$(uname -r)/kernel/drivers/net
cp %{workingDir}/e1000hook/e1000-7.0.39/src/e1000h.ko $RPM_BUILD_ROOT/lib/modules/$(uname -r)/kernel/drivers/net/
for N in 2 3 4 5; do
  cp %{workingDir}/e1000hook/eth_hook_${N}_nobigphysxxx/eth_hook_${N}.ko $RPM_BUILD_ROOT/lib/modules/$(uname -r)/kernel/drivers/net
done
mkdir -p $RPM_BUILD_ROOT/usr/local/bin
cp %{workingDir}/script/create_char_devices.sh $RPM_BUILD_ROOT/usr/local/bin
# cp %{workingDir}/script/unload_intel_driver.sh $RPM_BUILD_ROOT/usr/local/bin


%clean
[[ ${RPM_BUILD_ROOT} != "/" ]] && rm -rf $RPM_BUILD_ROOT


%files
%defattr(755,root,root,-)
/lib/modules/%{kernel_version}/kernel/drivers/net/e1000h.ko
/lib/modules/%{kernel_version}/kernel/drivers/net/eth_hook_2.ko
/lib/modules/%{kernel_version}/kernel/drivers/net/eth_hook_3.ko
/lib/modules/%{kernel_version}/kernel/drivers/net/eth_hook_4.ko
/lib/modules/%{kernel_version}/kernel/drivers/net/eth_hook_5.ko
/usr/local/bin/create_char_devices.sh
# /usr/local/bin/unload_intel_driver.sh


%post
NWSDIR=/etc/sysconfig/network-scripts/

# The postion number inside the rack from the host name
HN=$(hostname -s | sed -e 's/[^\-]*-[^\-]*-0\?\([0-9]\)/\1/')

for N in 2 3 4 5; do
    # Back up network config files. Or rather not as at boot time it would bring up eth*_old, too...
    # [[ -f ${NWSDIR}/ifcfg-eth${N} ]] && mv ${NWSDIR}/ifcfg-eth${N} ${NWSDIR}/ifcfg-eth${N}_old
    # Create new network config files
    echo "DEVICE=eth${N}"             > ${NWSDIR}/ifcfg-eth${N}
    echo "ONBOOT=yes"                >> ${NWSDIR}/ifcfg-eth${N}
    echo "BOOTPROTO=none"            >> ${NWSDIR}/ifcfg-eth${N}
    echo "IPADDR=192.168.${HN}.${N}" >> ${NWSDIR}/ifcfg-eth${N}
    echo "NETMASK=255.255.0.0"       >> ${NWSDIR}/ifcfg-eth${N}
    echo "USERCTL=no"                >> ${NWSDIR}/ifcfg-eth${N}
    echo "IPV6INIT=no"               >> ${NWSDIR}/ifcfg-eth${N}
    echo "PEERDNS=yes"               >> ${NWSDIR}/ifcfg-eth${N}
    echo "TYPE=Ethernet"             >> ${NWSDIR}/ifcfg-eth${N}
    echo "MTU=8192"                  >> ${NWSDIR}/ifcfg-eth${N}
    echo "PROMISC=yes"               >> ${NWSDIR}/ifcfg-eth${N}
    echo "SCHARDEV=yes"              >> ${NWSDIR}/ifcfg-eth${N}
    [[ -f /sys/class/net/eth${N}/address ]] && echo "HWADDR=$(cat /sys/class/net/eth${N}/address)" >> ${NWSDIR}/ifcfg-eth${N}
    # Create character devices
    [ -c /dev/schar${N} ] || ( mknod /dev/schar${N} c 23${N} 0 && chmod 777 /dev/schar${N} )
    # Associate device name with driver
    [[ $(grep -c "alias eth${N}" /etc/modprobe.conf) -eq 0 ]] && echo "alias eth${N} e1000h" >> /etc/modprobe.conf || sed -i -e "s/^alias eth${N}.*$/alias eth${N} e1000h/" /etc/modprobe.conf
done

# Have create_char_devices.sh invoked on booting
sed -i -e "/\/usr\/local\/bin\/create_char_devices.sh/d" /etc/rc.d/rc.local
echo "[[ -x /usr/local/bin/create_char_devices.sh ]] && /usr/local/bin/create_char_devices.sh 2 3 4 5" >> /etc/rc.d/rc.local

# Have unload_intel_driver.sh invoked on booting (see comment in the script)
# sed -i -e "/\/usr\/local\/bin\/unload_intel_driver.sh/d" /etc/rc.d/rc.local
# echo "[[ -x /usr/local/bin/unload_intel_driver.sh ]] && /usr/local/bin/unload_intel_driver.sh" >> /etc/rc.d/rc.local

# Update module dependencies
/sbin/depmod

# Load new modules
/sbin/modprobe e1000h

# Bring up the interfaces
for N in 2 3 4 5; do
    /sbin/ifconfig eth${N} down
    /sbin/ifconfig eth${N} 192.168.${HN}.${N} 
    /sbin/ifconfig eth${N} promisc mtu 8192 broadcast 192.168.255.255 netmask 255.255.0.0
done

%preun
# Bring down the interfaces
for N in 2 3 4 5; do
    /sbin/ifconfig eth${N} down
done

# Unload modules
[[ $(/sbin/lsmod | grep -c e1000h) -eq 0 ]] || /sbin/modprobe -r e1000h

%postun
# Update module dependencies
/sbin/depmod


%changelog
* Mon Jul 13 2009 cscdaq common account <cscdaq@srv-C2C04-22.cms> - emu-daqdriver
- Initial build.
