%define workingDir %(pwd)
%define _topdir %{workingDir}/rpm
%define kernel_version %(uname -r)

Summary: CMS Emu local DAQ Gbit drivers for kernel %{kernel_version} to be used in bdg 904
Name: emu-daqdriver904
Version: 2.0.0
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
[[ -x /etc/rc.d/init.d/K20cosmicdaq ]] && /etc/rc.d/init.d/K20cosmicdaq stop || /sbin/modprobe -r dl2k

%install
[[ ${RPM_BUILD_ROOT} != "/" ]] && rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/lib/modules/$(uname -r)/kernel/drivers/cosmicdaq
cp %{workingDir}/dl2khook/dl2k_driver/dl2k.ko $RPM_BUILD_ROOT/lib/modules/$(uname -r)/kernel/drivers/cosmicdaq
cp %{workingDir}/dl2khook/eth_hook_2_nobigphysxxx/eth_hook_2.ko $RPM_BUILD_ROOT/lib/modules/$(uname -r)/kernel/drivers/cosmicdaq
mkdir -p $RPM_BUILD_ROOT/etc/rc.d/init.d
cp %{workingDir}/script/K20cosmicdaq $RPM_BUILD_ROOT/etc/rc.d/init.d

%clean
[[ ${RPM_BUILD_ROOT} != "/" ]] && rm -rf $RPM_BUILD_ROOT


%files
%defattr(744,root,root,-)
/lib/modules/%{kernel_version}/kernel/drivers/cosmicdaq/dl2k.ko
/lib/modules/%{kernel_version}/kernel/drivers/cosmicdaq/eth_hook_2.ko
/etc/rc.d/init.d/K20cosmicdaq

%post
# Load new modules
/etc/rc.d/init.d/K20cosmicdaq start


%changelog
* Mon Jul 15 2009 cscdaq common account <cscdaq@srv-C2C04-22.cms> - emu-daqdriver904
- Initial build.
