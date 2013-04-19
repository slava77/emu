cd $BUILD_HOME/emu/daq/reader
make clean
make Standalone=yes
mv lib/linux/x86_64_slc5/libemudaqreader_sa.a $BUILD_HOME/emu/fast_daq/csc_display/lib/

cd $BUILD_HOME/emu/dqm/cmssw
make clean
make

cd $BUILD_HOME/emu/dqm/common
make clean
make Standalone=yes
mv lib/linux/x86_64_slc5/libemudqmcommon_sa.a $BUILD_HOME/emu/fast_daq/csc_display/lib/

cd $BUILD_HOME/emu/fast_daq/daq_common
make clean
make

mkdir -p ~/bin
cd $BUILD_HOME/emu/fast_daq/csc_display
make clean
make
