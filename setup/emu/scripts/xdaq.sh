#!/bin/zsh
exec ${XDAQ_ROOT}/bin/xdaq.exe ${XDAQ_PROFILE:+"-e $XDAQ_PROFILE"} "$@"
