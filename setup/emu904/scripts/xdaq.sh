#!/bin/zsh
# This script restarts a xdaq process after it crashes. If crashes occur too frequently, it exits.
# The xdaq process is started with the profile specified by the environment variable XDAQ_PROFILE.
# If XDAQ_PROFILE is not set, the profile is selected by the standard rules rules.

# Wait SLEEP seconds after crash before restarting process
SLEEP=5

# <Length of TIME array> invocations should not happen within a time span of MIN_TIMESPAN seconds
MIN_TIMESPAN=3600

NOW=$(date +"%s")
# The array (ring buffer) of the last few invocation times:
TIMES=( $NOW $NOW $NOW $NOW $NOW )

for (( I=1 ;; I++ )); do
    echo "****************"
    echo "* Invocation $I"
    echo "****************"
    TIMES[$I%5+1]=$(date +"%s")
    ORDERED_TIMES=( $(echo ${(on)TIMES}) )
    ((TIMESPAN=${ORDERED_TIMES[-1]}-${ORDERED_TIMES[1]}))
    echo "Time span of last "$( [[ $I -lt ${#TIMES} ]] && echo $I || echo ${#TIMES} )" invocations: " $TIMESPAN " seconds"
    if [[ $TIMESPAN -lt $MIN_TIMESPAN && $I -ge ${#TIMES} ]]; then
	echo "******************************************"
	echo "* Been crashing too frequently. Exiting. *"
	echo "******************************************"
	exit 1
    fi
    ${XDAQ_ROOT}/bin/xdaq.exe ${XDAQ_PROFILE:+"-e $XDAQ_PROFILE"} "$@"
    /bin/sleep $SLEEP
done
