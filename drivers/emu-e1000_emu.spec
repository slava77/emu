%define workingDir %(pwd)
%define _topdir %{workingDir}/rpm
%define kernel_version %(uname -r)

Summary: CMS Emu local DAQ Gbit and peripheral crate VME drivers for kernel %{kernel_version} based on the e1000 module for the Intel dual port NIC model PRO/1000 PF
Name: emu-e1000_emu
Version: 1.1.0
Release: 1.slc6
License: none
Group: none
URL: none
Source0: cvs
BuildRoot: /tmp/%{name}-%{version}-%{release}-root


%description


%build


%pre

%install
mkdir -p $RPM_BUILD_ROOT/usr/local/bin/e1000_emu
cp %{workingDir}/e1000_emu/eth_hook_2_daq/ddu/eth_hook_2_ddu.ko $RPM_BUILD_ROOT/usr/local/bin/e1000_emu
cp %{workingDir}/e1000_emu/eth_hook_3_daq/ddu/eth_hook_3_ddu.ko $RPM_BUILD_ROOT/usr/local/bin/e1000_emu
cp %{workingDir}/e1000_emu/eth_hook_4_daq/ddu/eth_hook_4_ddu.ko $RPM_BUILD_ROOT/usr/local/bin/e1000_emu
cp %{workingDir}/e1000_emu/eth_hook_5_daq/ddu/eth_hook_5_ddu.ko $RPM_BUILD_ROOT/usr/local/bin/e1000_emu
cp %{workingDir}/e1000_emu/eth_hook_2_daq/dmb/eth_hook_2_dmb.ko $RPM_BUILD_ROOT/usr/local/bin/e1000_emu
cp %{workingDir}/e1000_emu/eth_hook_3_daq/dmb/eth_hook_3_dmb.ko $RPM_BUILD_ROOT/usr/local/bin/e1000_emu
cp %{workingDir}/e1000_emu/eth_hook_4_daq/dmb/eth_hook_4_dmb.ko $RPM_BUILD_ROOT/usr/local/bin/e1000_emu
cp %{workingDir}/e1000_emu/eth_hook_5_daq/dmb/eth_hook_5_dmb.ko $RPM_BUILD_ROOT/usr/local/bin/e1000_emu
cp %{workingDir}/e1000_emu/eth_hook_2_vme/eth_hook_2_vme.ko $RPM_BUILD_ROOT/usr/local/bin/e1000_emu
cp %{workingDir}/e1000_emu/eth_hook_3_vme/eth_hook_3_vme.ko $RPM_BUILD_ROOT/usr/local/bin/e1000_emu
cp %{workingDir}/e1000_emu/eth_hook_4_vme/eth_hook_4_vme.ko $RPM_BUILD_ROOT/usr/local/bin/e1000_emu
cp %{workingDir}/e1000_emu/eth_hook_5_vme/eth_hook_5_vme.ko $RPM_BUILD_ROOT/usr/local/bin/e1000_emu
cp %{workingDir}/e1000_emu/e1000-7.0.39/src/e1000_emu.ko       $RPM_BUILD_ROOT/usr/local/bin/e1000_emu
cp %{workingDir}/script/load_e1000_emu.sh                   $RPM_BUILD_ROOT/usr/local/bin/e1000_emu
touch %{_topdir}/BUILD/ChangeLog
touch %{_topdir}/BUILD/README
touch %{_topdir}/BUILD/MAINTAINER

%clean
[[ ${RPM_BUILD_ROOT} != "/" ]] && rm -rf $RPM_BUILD_ROOT


%files
%defattr(744,root,root,-)
/usr/local/bin/e1000_emu/eth_hook_2_ddu.ko
/usr/local/bin/e1000_emu/eth_hook_3_ddu.ko
/usr/local/bin/e1000_emu/eth_hook_4_ddu.ko
/usr/local/bin/e1000_emu/eth_hook_5_ddu.ko
/usr/local/bin/e1000_emu/eth_hook_2_dmb.ko
/usr/local/bin/e1000_emu/eth_hook_3_dmb.ko
/usr/local/bin/e1000_emu/eth_hook_4_dmb.ko
/usr/local/bin/e1000_emu/eth_hook_5_dmb.ko
/usr/local/bin/e1000_emu/eth_hook_2_vme.ko
/usr/local/bin/e1000_emu/eth_hook_3_vme.ko
/usr/local/bin/e1000_emu/eth_hook_4_vme.ko
/usr/local/bin/e1000_emu/eth_hook_5_vme.ko
/usr/local/bin/e1000_emu/e1000_emu.ko
/usr/local/bin/e1000_emu/load_e1000_emu.sh
# Files required by Quattor
%defattr(644,root,root,755)
%doc MAINTAINER ChangeLog README

%post
# Have load_e1000_emu.sh invoked on booting
#[[ -f /etc/rc.d/rc.local ]] && sed -i -e "/\/usr\/local\/bin\/e1000_emu\/load_e1000_emu.sh/d" /etc/rc.d/rc.local || true
#echo "[[ -x /usr/local/bin/e1000_emu/load_e1000_emu.sh ]] && /usr/local/bin/e1000_emu/load_e1000_emu.sh > /var/log/load_e1000_emu.log 2>&1" >> /etc/rc.d/rc.local

# Load new modules
/usr/local/bin/e1000_emu/load_e1000_emu.sh || true

%preun
# Unload modules
[[ $(/sbin/lsmod | grep -c e1000_emu) -eq 0 ]] || /sbin/modprobe -r e1000_emu || true

# Stop loading daq drivers at boot time.
[[ -f /etc/rc.d/rc.local ]] && sed -i -e "/\/usr\/local\/bin\/e1000_emu\/load_e1000_emu.sh/d" /etc/rc.d/rc.local

%postun
# Remove modules from /lib/modules
rm -f /lib/modules/%{kernel_version}/kernel/drivers/net/e1000/e1000_emu.ko
rm -f /lib/modules/%{kernel_version}/kernel/drivers/net/e1000/eth_hook_*.ko
rm -rf /usr/local/bin/e1000_emu
# Update module dependencies
/sbin/depmod
