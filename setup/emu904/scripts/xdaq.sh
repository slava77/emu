#!/bin/zsh
# The xdaq process is started with the profile specified by the environment variable XDAQ_PROFILE.
# If XDAQ_PROFILE is not set, the profile is selected by the standard rules.

echo "This script was invoked by the command:"
echo $0 "$@"
echo
echo "The shell environment:"
limit
set
echo
echo Invoking command:
echo ${XDAQ_ROOT}/bin/xdaq.exe ${XDAQ_PROFILE:+"-e $XDAQ_PROFILE"} "$@"
echo
${XDAQ_ROOT}/bin/xdaq.exe ${XDAQ_PROFILE:+"-e $XDAQ_PROFILE"} "$@"
