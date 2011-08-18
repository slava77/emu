%define workingDir %(pwd)
%define _topdir %{workingDir}/rpm
%define kernel_version %(uname -r)

Summary: CMS Emu Peripheral Controller Gbit driver for kernel %{kernel_version}
Name: emu-pcvme
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
[[ -x /etc/rc.d/init.d/pcratedriver ]] && /etc/rc.d/init.d/pcratedriver stop

%install
[[ ${RPM_BUILD_ROOT} != "/" ]] && rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/lib/modules/$(uname -r)/kernel/drivers/net
mkdir -p $RPM_BUILD_ROOT/etc/rc.d/init.d
cp %{workingDir}/gbit-vme/dl2k_driver/dl2k.ko $RPM_BUILD_ROOT/lib/modules/$(uname -r)/kernel/drivers/net/
cp %{workingDir}/gbit-vme/eth_hook_jtag_simple/eth_hook.ko $RPM_BUILD_ROOT/lib/modules/$(uname -r)/kernel/drivers/net
cp %{workingDir}/script/pcratedriver $RPM_BUILD_ROOT/etc/rc.d/init.d

%clean
[[ ${RPM_BUILD_ROOT} != "/" ]] && rm -rf $RPM_BUILD_ROOT


%files
%defattr(744,root,root,-)
/lib/modules/%{kernel_version}/kernel/drivers/net/dl2k.ko
/lib/modules/%{kernel_version}/kernel/drivers/net/eth_hook.ko
%attr(755, root, root) /etc/rc.d/init.d/pcratedriver

%post
/sbin/chkconfig --level 35 pcratedriver on
/etc/rc.d/init.d/pcratedriver start

%preun
[[ -x /etc/rc.d/init.d/pcratedriver ]] && /etc/rc.d/init.d/pcratedriver stop

%postun
/sbin/chkconfig --del pcratedriver 

%changelog
* Sun Aug 16 2009 --
- Initial build.

