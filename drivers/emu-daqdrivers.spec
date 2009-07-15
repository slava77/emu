%define workingDir %(pwd)
%define _topdir %{workingDir}/rpm
%define kernel_version %(uname -r)

Summary: CMS Emu local DAQ Gbit drivers for kernel %{kernel_version}
Name: emu-daqdrivers
Version: 1.0.0
Release: 1
License: none
Group: none
URL: none
Source0: cvs
BuildRoot: /tmp/%{name}-%{version}-%{release}-root


%description


%build

%pre
# Unload old modules
/sbin/modprobe -r e1000h

%install
[[ ${RPM_BUILD_ROOT} != "/" ]] && rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/lib/modules/$(uname -r)/kernel/drivers/net
cp %{workingDir}/e1000hook/e1000-7.0.39/src/e1000h.ko $RPM_BUILD_ROOT/lib/modules/$(uname -r)/kernel/drivers/net/
for N in 2 3 4 5; do
  cp %{workingDir}/e1000hook/eth_hook_${N}_nobigphysxxx/eth_hook_${N}.ko $RPM_BUILD_ROOT/lib/modules/$(uname -r)/kernel/drivers/net
done


%clean
[[ ${RPM_BUILD_ROOT} != "/" ]] && rm -rf $RPM_BUILD_ROOT


%files
%defattr(744,root,root,-)
/lib/modules/%{kernel_version}/kernel/drivers/net/e1000h.ko
/lib/modules/%{kernel_version}/kernel/drivers/net/eth_hook_2.ko
/lib/modules/%{kernel_version}/kernel/drivers/net/eth_hook_3.ko
/lib/modules/%{kernel_version}/kernel/drivers/net/eth_hook_4.ko
/lib/modules/%{kernel_version}/kernel/drivers/net/eth_hook_5.ko


%post
NWSDIR=/etc/sysconfig/network-scripts/

H=$(hostname -s)

for N in 2 3 4 5; do
    # Back up network config files
    [[ -f ${NWSDIR}/ifcfg-eth${N} ]] && mv ${NWSDIR}/ifcfg-eth${N} ${NWSDIR}/ifcfg-eth${N}_old
    # Create new network config files
    echo "DEVICE=eth${N}"               >> ${NWSDIR}/ifcfg-eth${N}
    echo "ONBOOT=yes"                   >> ${NWSDIR}/ifcfg-eth${N}
    echo "BOOTPROTO=none"               >> ${NWSDIR}/ifcfg-eth${N}
    echo "IPADDR=192.168.${H##*-}.${N}" >> ${NWSDIR}/ifcfg-eth${N}
    echo "NETMASK=255.255.0.0"          >> ${NWSDIR}/ifcfg-eth${N}
    echo "USERCTL=no"                   >> ${NWSDIR}/ifcfg-eth${N}
    echo "IPV6INIT=no"                  >> ${NWSDIR}/ifcfg-eth${N}
    echo "PEERDNS=yes"                  >> ${NWSDIR}/ifcfg-eth${N}
    echo "TYPE=Ethernet"                >> ${NWSDIR}/ifcfg-eth${N}
    echo "MTU=8192"                     >> ${NWSDIR}/ifcfg-eth${N}
    echo "PROMISC=yes"                  >> ${NWSDIR}/ifcfg-eth${N}
    echo "SCHARDEV=yes"                 >> ${NWSDIR}/ifcfg-eth${N}
    [[ -f /sys/class/net/eth${N}/address ]] && echo "HWADDR=$(cat /sys/class/net/eth${N}/address)" >> ${NWSDIR}/ifcfg-eth${N}
    # Create character devices
    [ -c /dev/schar${N} ] || ( mknod /dev/schar${N} c 232 0 && chmod 777 /dev/schar${N} )
done

# Load new modules
/sbin/modprobe e1000h


%changelog
* Mon Jul 13 2009 cscdaq common account <cscdaq@srv-C2C04-22.cms> - emu-daqdrivers
- Initial build.
