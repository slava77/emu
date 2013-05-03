#!/bin/zsh

BUILD_HOME=${BUILD_HOME:-${1:h}/../..}
print "BUILD_HOME=$BUILD_HOME"

if [[ ${+ROOTSYS} -eq 1 ]]; then
    print "ROOTSYS=$ROOTSYS"
    cd $ROOTSYS
    source $ROOTSYS/bin/thisroot.sh
else
    print "ROOTSYS is not defined. Exiting."
    exit 1
fi

export DAQHOME=$BUILD_HOME/emu/fast_daq
print "DAQHOME=$DAQHOME"

# If no CMSSW symlink exists, create one to the highest installed CMSSW version
if [[ ! -e $HOME/CMSSW ]]; then
    CMSSWINST=( $(print $HOME/CMSSW_*(/) ) )
    if [[ ${#CMSSWINST} -gt 0 ]]; then
	ln -s ${CMSSWINST[-1]} $HOME/CMSSW
	print "Created symlink $HOME/CMSSW"
    else
	print "Found no CMSSW installation in $HOME\n Exiting."
	exit 1
    fi
fi
print "Using CMSSW installation $HOME/CMSSW"
print

cd $BUILD_HOME/emu/fast_daq/daq_common && make clean && make -j4

cd $BUILD_HOME/emu/fast_daq/csc_display && make clean && make -j4

cd $BUILD_HOME/emu/fast_daq && mkdir -p rpm/{BUILD,RPMS} && rpmbuild -bb emu-cscdisplay.spec
