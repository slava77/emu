%define workingDir %(pwd)
%define _topdir %{workingDir}/rpm
%define kernel_version %(uname -r)

Summary: CMS Emu local DAQ Gbit drivers for kernel %{kernel_version}
Name: emu-daqdriver
Version: 1.2.0
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
cp %{workingDir}/script/load_daq_drivers.sh $RPM_BUILD_ROOT/usr/local/bin


%clean
[[ ${RPM_BUILD_ROOT} != "/" ]] && rm -rf $RPM_BUILD_ROOT


%files
%defattr(755,root,root,-)
/lib/modules/%{kernel_version}/kernel/drivers/net/e1000h.ko
/lib/modules/%{kernel_version}/kernel/drivers/net/eth_hook_2.ko
/lib/modules/%{kernel_version}/kernel/drivers/net/eth_hook_3.ko
/lib/modules/%{kernel_version}/kernel/drivers/net/eth_hook_4.ko
/lib/modules/%{kernel_version}/kernel/drivers/net/eth_hook_5.ko
/usr/local/bin/load_daq_drivers.sh


%post
NWSDIR=/etc/sysconfig/network-scripts/

# The postion number inside the rack from the host name
HN=$(hostname -s | sed -e 's/[^\-]*-[^\-]*-0\?\([0-9]\)/\1/')

for N in 2 3 4 5; do
    # Create character devices
    [ -c /dev/schar${N} ] || ( mknod /dev/schar${N} c 23${N} 0 && chmod 777 /dev/schar${N} )
    # Associate device name with driver
    [[ $(grep -c "alias eth${N}" /etc/modprobe.conf) -eq 0 ]] && echo "alias eth${N} e1000h" >> /etc/modprobe.conf || sed -i -e "s/^alias eth${N}.*$/alias eth${N} e1000h/" /etc/modprobe.conf
done

# Have load_daq_drivers.sh invoked on booting
sed -i -e "/\/usr\/local\/bin\/load_daq_drivers.sh/d" /etc/rc.d/rc.local
echo "[[ -x /usr/local/bin/load_daq_drivers.sh ]] && /usr/local/bin/load_daq_drivers.sh" >> /etc/rc.d/rc.local

# Update module dependencies
/sbin/depmod

# Load new modules
/usr/local/bin/load_daq_drivers.sh


%preun
# Bring down the interfaces
for N in 2 3 4 5; do
    /sbin/ifconfig eth${N} down
done

# Unload modules
[[ $(/sbin/lsmod | grep -c e1000h) -eq 0 ]] || /sbin/modprobe -r e1000h

# Stop loading daq drivers at boot time.
sed -i -e "/\/usr\/local\/bin\/load_daq_drivers.sh/d" /etc/rc.d/rc.local


%postun
# Update module dependencies
/sbin/depmod


%changelog
* Mon Jul 13 2009 cscdaq common account <cscdaq@srv-C2C04-22.cms> - emu-daqdriver
- Initial build.
