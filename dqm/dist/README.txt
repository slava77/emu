Instructions to build CERN emu-ROOT rpm package using pre-compiled and installed binaries 
(Currently works on SLC5 only, but rpm for SLC6 can be created using SLC5 setup as well)
==========================================================================================================================
- download ROOT binary distribution from root.cern.ch
- install it in /opt/cern (optionally rename /opt/cern/root to /opt/cern/root-vXX.YY.ZZ to keep track of ROOT version and make symlink to /opt/cern/root)
- recreate RPMBUILD tree in user folder 'mkdir -p $HOME/rpm/{BUILD,RPMS,SOURCES,SPECS,SRPMS,tmp}'
- copy supplied root.spec to $HOME/rpm/SPECS
- create target folder for ROOT package 'mkdir $HOME/rpm/BUILD/emu-ROOT'
- create symlink to instaled ROOT path  'ln -s /opt $HOME/rpm/BUILD/emu-ROOT/opt'
- edit root.spec file and adjust various version parameters for target package and also set correct path to installed ROOT binaries in %files section
- copy supplied rpmmacros file to $HOME/.rpmmacros and adjust %_topdir and %_tmppath to paths on your system
- build rpm from binaries using 'rpmbuild -bb $HOME/rpm/SPECS/root.spec' 
- check that there were no errors and rpm is placed in $HOME/rpm/RPMS/{ARCH}/ folder

