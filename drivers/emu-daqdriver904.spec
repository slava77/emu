%define workingDir %(pwd)
%define _topdir %{workingDir}/rpm
%define kernel_version %(uname -r)

Summary: CMS Emu local DAQ Gbit drivers for kernel %{kernel_version} to be used in bdg 904
Name: emu-daqdriver904
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
/sbin/modprobe -r dl2k

%install
[[ ${RPM_BUILD_ROOT} != "/" ]] && rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/lib/modules/$(uname -r)/kernel/drivers/net
cp %{workingDir}/dl2khook/dl2k_driver/dl2k.ko $RPM_BUILD_ROOT/lib/modules/$(uname -r)/kernel/drivers/net/
cp %{workingDir}/dl2khook/eth_hook_2_nobigphysxxx/eth_hook_2.ko $RPM_BUILD_ROOT/lib/modules/$(uname -r)/kernel/drivers/net


%clean
[[ ${RPM_BUILD_ROOT} != "/" ]] && rm -rf $RPM_BUILD_ROOT


%files
%defattr(744,root,root,-)
/lib/modules/%{kernel_version}/kernel/drivers/net/dl2k.ko
/lib/modules/%{kernel_version}/kernel/drivers/net/eth_hook_2.ko


%post
NWSDIR=/etc/sysconfig/network-scripts/

H=$(hostname -s)

# Back up network config file
[[ -f ${NWSDIR}/ifcfg-eth2 ]] && mv ${NWSDIR}/ifcfg-eth2 ${NWSDIR}/ifcfg-eth2_old
# Create new network config file
echo "DEVICE=eth2"                      >> ${NWSDIR}/ifcfg-eth2
echo "ONBOOT=yes"                       >> ${NWSDIR}/ifcfg-eth2
echo "BOOTPROTO=none"                   >> ${NWSDIR}/ifcfg-eth2
echo "IPADDR=192.168.${H##*[A-Za-z]}.2" >> ${NWSDIR}/ifcfg-eth2
echo "NETMASK=255.255.255.0"            >> ${NWSDIR}/ifcfg-eth2
echo "USERCTL=no"                       >> ${NWSDIR}/ifcfg-eth2
echo "IPV6INIT=no"                      >> ${NWSDIR}/ifcfg-eth2
echo "PEERDNS=yes"                      >> ${NWSDIR}/ifcfg-eth2
echo "TYPE=Ethernet"                    >> ${NWSDIR}/ifcfg-eth2
echo "MTU=8192"                         >> ${NWSDIR}/ifcfg-eth2
echo "PROMISC=yes"                      >> ${NWSDIR}/ifcfg-eth2
echo "SCHARDEV=yes"                     >> ${NWSDIR}/ifcfg-eth2
[[ -f /sys/class/net/eth2/address ]] && echo "HWADDR=$(cat /sys/class/net/eth2/address)" >> ${NWSDIR}/ifcfg-eth2
# Create character device
[ -c /dev/schar2 ] || ( mknod /dev/schar2 c 232 0 && chmod 777 /dev/schar2 )

# Load new modules
/sbin/modprobe dl2k


%changelog
* Mon Jul 15 2009 cscdaq common account <cscdaq@srv-C2C04-22.cms> - emu-daqdriver904
- Initial build.
