%define workingDir %(pwd)
%define _topdir %{workingDir}/rpm
%define XDAQ_ROOT /opt/xdaq

Summary: Configuration files and scripts needed for emu to run processes on csc-tf and csc-ttc.
Name: emu-trigger
Version: 1.0.0
Release: 1.cmsos10.slc4
License: none
Group: none
URL: none
Source0: cvs
BuildRoot: /tmp/%{name}-%{version}-%{release}-root

%description
Configuration files and scripts needed for emu to run processes on csc-tf and csc-ttc.
It doesn't set up xdaqd or similar as that is run in a trigger zone on the trigger nodes.
To be installed on trigger nodes exclusively.

%prep

%build

%install
[[ ${RPM_BUILD_ROOT} != "/" ]] && rm -rf $RPM_BUILD_ROOT
for SUBDIR in etc conf flash profile pulser scripts sensor xplore; do
  mkdir -p $RPM_BUILD_ROOT/%{XDAQ_ROOT}/share/emu/$SUBDIR
  cp %{workingDir}/$SUBDIR/* $RPM_BUILD_ROOT/%{XDAQ_ROOT}/share/emu/$SUBDIR &2>/dev/null
done

%clean
[[ ${RPM_BUILD_ROOT} != "/" ]] && rm -rf $RPM_BUILD_ROOT


%files
%defattr(644,root,root,755)
%{XDAQ_ROOT}/share/emu/conf
%{XDAQ_ROOT}/share/emu/etc
%{XDAQ_ROOT}/share/emu/flash
%{XDAQ_ROOT}/share/emu/profile
%{XDAQ_ROOT}/share/emu/pulser
%{XDAQ_ROOT}/share/emu/scripts
%{XDAQ_ROOT}/share/emu/sensor
%{XDAQ_ROOT}/share/emu/xplore
%doc


%changelog
* Thu Jul 7 2011 cscdaq common account <cscdaq@csc-C2D08-11.cms>
- Initial build.
