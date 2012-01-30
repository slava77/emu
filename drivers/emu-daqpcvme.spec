%define workingDir %(pwd)
%define _topdir %{workingDir}/rpm
%define kernel_version %(uname -r)

Summary: CMS Emu local DAQ Gbit and peripheral crate VME drivers for kernel %{kernel_version} based on the dl2k module for D-Link NIC model DGE-550SX
Name: emu-daqpcvme
Version: 1.0.0
Release: 1.slc4
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
cp %{workingDir}/dlk2-daq-pcvme/dl2k_driver/dl2k.ko $RPM_BUILD_ROOT/lib/modules/$(uname -r)/kernel/drivers/net/
cp %{workingDir}/dlk2-daq-pcvme/eth_hook_jtag_simple/eth_hook_2.ko $RPM_BUILD_ROOT/lib/modules/$(uname -r)/kernel/drivers/net
cp %{workingDir}/dlk2-daq-pcvme/eth_hook_3_nobigphysxxx/eth_hook_3.ko $RPM_BUILD_ROOT/lib/modules/$(uname -r)/kernel/drivers/net
mkdir -p $RPM_BUILD_ROOT/usr/local/bin
cp %{workingDir}/script/load_daq_pcvme_drivers.sh $RPM_BUILD_ROOT/usr/local/bin
touch %{_topdir}/BUILD/ChangeLog
touch %{_topdir}/BUILD/README
touch %{_topdir}/BUILD/MAINTAINER

%clean
[[ ${RPM_BUILD_ROOT} != "/" ]] && rm -rf $RPM_BUILD_ROOT


%files
%defattr(744,root,root,-)
/lib/modules/%{kernel_version}/kernel/drivers/net/dl2k.ko
/lib/modules/%{kernel_version}/kernel/drivers/net/eth_hook_2.ko
/lib/modules/%{kernel_version}/kernel/drivers/net/eth_hook_3.ko
/usr/local/bin/load_daq_pcvme_drivers.sh
# Files required by Quattor
%defattr(644,root,root,755)
%doc MAINTAINER ChangeLog README

%post
# Have load_daq_pcvme_drivers.sh invoked on booting
sed -i -e "/\/usr\/local\/bin\/load_daq_pcvme_drivers.sh/d" /etc/rc.d/rc.local
echo "[[ -x /usr/local/bin/load_daq_pcvme_drivers.sh ]] && /usr/local/bin/load_daq_pcvme_drivers.sh" >> /etc/rc.d/rc.local

# Load new modules
/usr/local/bin/load_daq_pcvme_drivers.sh

%preun
# Unload modules
[[ $(/sbin/lsmod | grep -c dl2k) -eq 0 ]] || /sbin/modprobe -r dl2k

# Stop loading daq drivers at boot time.
sed -i -e "/\/usr\/local\/bin\/load_daq_pcvme_drivers.sh/d" /etc/rc.d/rc.local

%postun
# Update module dependencies
/sbin/depmod

#%changelog
#* Mon Jul 13 2009 cscdaq common account <cscdaq@srv-C2C04-22.cms> - emu-daqpcvme
#- Initial build.
