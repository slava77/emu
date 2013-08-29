%define workingDir %(pwd)
%define _topdir %{workingDir}/rpm
%define kernel_version %(uname -r)

Summary: CMS Emu local DAQ Gbit and peripheral crate VME drivers for kernel %{kernel_version} based on the igb module for the Intel dual port NIC model I350-F2
Name: emu-igb_emu
Version: 1.2.0
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
mkdir -p $RPM_BUILD_ROOT/usr/local/bin
cp %{workingDir}/igb_emu/eth_hook_2_daq/ddu/eth_hook_2_ddu.ko $RPM_BUILD_ROOT/usr/local/bin
cp %{workingDir}/igb_emu/eth_hook_3_daq/ddu/eth_hook_3_ddu.ko $RPM_BUILD_ROOT/usr/local/bin
cp %{workingDir}/igb_emu/eth_hook_4_daq/ddu/eth_hook_4_ddu.ko $RPM_BUILD_ROOT/usr/local/bin
cp %{workingDir}/igb_emu/eth_hook_5_daq/ddu/eth_hook_5_ddu.ko $RPM_BUILD_ROOT/usr/local/bin
cp %{workingDir}/igb_emu/eth_hook_2_daq/dmb/eth_hook_2_dmb.ko $RPM_BUILD_ROOT/usr/local/bin
cp %{workingDir}/igb_emu/eth_hook_3_daq/dmb/eth_hook_3_dmb.ko $RPM_BUILD_ROOT/usr/local/bin
cp %{workingDir}/igb_emu/eth_hook_4_daq/dmb/eth_hook_4_dmb.ko $RPM_BUILD_ROOT/usr/local/bin
cp %{workingDir}/igb_emu/eth_hook_5_daq/dmb/eth_hook_5_dmb.ko $RPM_BUILD_ROOT/usr/local/bin
cp %{workingDir}/igb_emu/eth_hook_2_vme/eth_hook_2_vme.ko $RPM_BUILD_ROOT/usr/local/bin
cp %{workingDir}/igb_emu/eth_hook_3_vme/eth_hook_3_vme.ko $RPM_BUILD_ROOT/usr/local/bin
cp %{workingDir}/igb_emu/eth_hook_4_vme/eth_hook_4_vme.ko $RPM_BUILD_ROOT/usr/local/bin
cp %{workingDir}/igb_emu/eth_hook_5_vme/eth_hook_5_vme.ko $RPM_BUILD_ROOT/usr/local/bin
cp %{workingDir}/igb_emu/igb-4.1.2/src/igb_emu.ko         $RPM_BUILD_ROOT/usr/local/bin
cp %{workingDir}/script/load_igb_emu.sh                   $RPM_BUILD_ROOT/usr/local/bin
touch %{_topdir}/BUILD/ChangeLog
touch %{_topdir}/BUILD/README
touch %{_topdir}/BUILD/MAINTAINER

%clean
[[ ${RPM_BUILD_ROOT} != "/" ]] && rm -rf $RPM_BUILD_ROOT


%files
%defattr(744,root,root,-)
/usr/local/bin/eth_hook_2_ddu.ko
/usr/local/bin/eth_hook_3_ddu.ko
/usr/local/bin/eth_hook_4_ddu.ko
/usr/local/bin/eth_hook_5_ddu.ko
/usr/local/bin/eth_hook_2_dmb.ko
/usr/local/bin/eth_hook_3_dmb.ko
/usr/local/bin/eth_hook_4_dmb.ko
/usr/local/bin/eth_hook_5_dmb.ko
/usr/local/bin/eth_hook_2_vme.ko
/usr/local/bin/eth_hook_3_vme.ko
/usr/local/bin/eth_hook_4_vme.ko
/usr/local/bin/eth_hook_5_vme.ko
/usr/local/bin/igb_emu.ko
/usr/local/bin/load_igb_emu.sh
# Files required by Quattor
%defattr(644,root,root,755)
%doc MAINTAINER ChangeLog README

%post
# Have load_daq_pcvme_drivers.sh invoked on booting
#sed -i -e "/\/usr\/local\/bin\/load_igb_emu.sh/d" /etc/rc.d/rc.local
#echo "[[ -x /usr/local/bin/load_igb_emu.sh ]] && /usr/local/bin/load_igb_emu.sh > /var/log/load_igb_emu.log 2>&1" >> /etc/rc.d/rc.local

# Load new modules
/usr/local/bin/load_igb_emu.sh

%preun
# Unload modules
[[ $(/sbin/lsmod | grep -c igb_emu) -eq 0 ]] || /sbin/modprobe -r igb_emu

# Stop loading daq drivers at boot time.
sed -i -e "/\/usr\/local\/bin\/load_igb_emu.sh/d" /etc/rc.d/rc.local

%postun
# Remove modules from /lib/modules
rm -f /lib/modules/%{kernel_version}/kernel/drivers/net/igb_emu.ko
rm -f /lib/modules/%{kernel_version}/kernel/drivers/net/eth_hook_*.ko
# Update module dependencies
/sbin/depmod
