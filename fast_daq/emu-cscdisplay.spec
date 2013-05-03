%define workingDir %(pwd)
%define _topdir %{workingDir}/rpm

Summary: Low-level CSC event display
Name: emu-cscdisplay
Version: 1.0.0
Release: 1.slc5
License: none
Group: none
URL: none
Source0: svn
BuildRoot: /tmp/%{name}-%{version}-%{release}-root

%description

%build

%pre

%install
INSTALL_PATH=/opt/xdaq
mkdir -p $RPM_BUILD_ROOT/$INSTALL_PATH/{bin,lib}
mkdir -p $RPM_BUILD_ROOT/$INSTALL_PATH/htdocs/emu/cscdisplay/images
cp %{workingDir}/csc_display/csc_display.sh $RPM_BUILD_ROOT/$INSTALL_PATH/bin
cp %{workingDir}/csc_display/lib/csc_display $RPM_BUILD_ROOT/$INSTALL_PATH/bin
cp %{workingDir}/csc_display/src/icons/*.xpm $RPM_BUILD_ROOT/$INSTALL_PATH/htdocs/emu/cscdisplay/images

%clean
[[ ${RPM_BUILD_ROOT} != "/" ]] && rm -rf $RPM_BUILD_ROOT

%files
%defattr(755,root,root,-)
/opt/xdaq/bin/csc_display.sh
/opt/xdaq/bin/csc_display
%attr(644, root, root) /opt/xdaq/htdocs/emu/cscdisplay/images/pause.xpm 
%attr(644, root, root) /opt/xdaq/htdocs/emu/cscdisplay/images/play.xpm  
%attr(644, root, root) /opt/xdaq/htdocs/emu/cscdisplay/images/print.xpm 
%attr(644, root, root) /opt/xdaq/htdocs/emu/cscdisplay/images/rewind.xpm
