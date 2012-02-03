%define workingDir %(pwd)
%define _topdir %{workingDir}/rpm
%define kernel_version %(uname -r)

Summary: CMS Emu local DAQ Gbit drivers for kernel %{kernel_version}
Name: emu-daqdriver
Version: 2.0.0
Release: 1.slc5
License: none
Group: none
URL: none
Source0: cvs
BuildRoot: /tmp/%{name}-%{version}-%{release}-root


%description


%build


%pre

%install
mkdir -p $RPM_BUILD_ROOT/lib/modules/$(uname -r)/kernel/drivers/net
cp %{workingDir}/e1000hook/e1000-7.0.39/src/e1000h.ko $RPM_BUILD_ROOT/lib/modules/$(uname -r)/kernel/drivers/net/
for N in 2 3 4 5; do
  cp %{workingDir}/e1000hook/eth_hook_${N}_nobigphysxxx/eth_hook_${N}.ko $RPM_BUILD_ROOT/lib/modules/$(uname -r)/kernel/drivers/net
done
mkdir -p $RPM_BUILD_ROOT/usr/local/bin
cp %{workingDir}/script/load_daq_drivers.sh $RPM_BUILD_ROOT/usr/local/bin
touch %{_topdir}/BUILD/ChangeLog
touch %{_topdir}/BUILD/README
touch %{_topdir}/BUILD/MAINTAINER

%clean
[[ ${RPM_BUILD_ROOT} != "/" ]] && rm -rf $RPM_BUILD_ROOT


%files
%defattr(744,root,root,-)
/lib/modules/%{kernel_version}/kernel/drivers/net/e1000h.ko
/lib/modules/%{kernel_version}/kernel/drivers/net/eth_hook_2.ko
/lib/modules/%{kernel_version}/kernel/drivers/net/eth_hook_3.ko
/lib/modules/%{kernel_version}/kernel/drivers/net/eth_hook_4.ko
/lib/modules/%{kernel_version}/kernel/drivers/net/eth_hook_5.ko
/usr/local/bin/load_daq_drivers.sh
# Files required by Quattor
%defattr(644,root,root,755)
%doc MAINTAINER ChangeLog README

%post
# Have load_daq_drivers.sh invoked on booting
sed -i -e "/\/usr\/local\/bin\/load_daq_drivers.sh/d" /etc/rc.d/rc.local
echo "[[ -x /usr/local/bin/load_daq_drivers.sh ]] && /usr/local/bin/load_daq_drivers.sh" >> /etc/rc.d/rc.local

# Load new modules
/usr/local/bin/load_daq_drivers.sh

%preun
# Unload modules
[[ $(/sbin/lsmod | grep -c e1000h) -eq 0 ]] || /sbin/modprobe -r e1000h

# Stop loading daq drivers at boot time.
sed -i -e "/\/usr\/local\/bin\/load_daq_drivers.sh/d" /etc/rc.d/rc.local

%postun
# Update module dependencies
/sbin/depmod

#%changelog
#* Mon Jul 13 2009 cscdaq common account <cscdaq@srv-C2C04-22.cms> - emu-daqdriver
#- Initial build.
