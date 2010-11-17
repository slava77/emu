#!/bin/zsh
# This script restarts a xdaq process after it crashes. If crashes occur too frequently, it exits.
# The xdaq process is started with the profile specified by the environment variable XDAQ_PROFILE.
# If XDAQ_PROFILE is not set, the profile is selected by the standard rules rules.

#                            I M P O R T A N T
#                            =================
# In order for auto-restart to work properly in this script, 
# xdaqexec.startmode=1
# must be set in $CATALINA_HOME/webapps/rcms/WEB-INF/conf/RCMSExpert.properties
# so that, after crashing, the restarted xdaq::executive can be configured.

# Wait SLEEP seconds after crash before relaunching process
SLEEP=3

# N_STARTS starts should not happen within a time span of MIN_TIMESPAN seconds
N_STARTS=5
MIN_TIMESPAN=1800

NOW=$(date +"%s")
# The array (ring buffer) of the last few invocation times:
TIMES=( $(for (( I=1; I<$N_STARTS; I++ )); do print $NOW; done) )

echo "This script was invoked by the command:"
echo $0 "$@"
echo
echo "The shell environment:"
limit
set
echo

for (( I=1 ;; I++ )); do
    echo "***********"
    echo "* Start $I *"
    echo "***********"
    TIMES[$I%5+1]=$(date +"%s")
    ORDERED_TIMES=( $(echo ${(on)TIMES}) )
    ((TIMESPAN=${ORDERED_TIMES[-1]}-${ORDERED_TIMES[1]}))
    echo "Time span of last "$( [[ $I -lt ${#TIMES} ]] && echo $I || echo ${#TIMES} )" (re)starts: " $TIMESPAN " seconds"
    if [[ $TIMESPAN -lt $MIN_TIMESPAN && $I -ge ${#TIMES} ]]; then
	echo "******************************************"
	echo "* Been crashing too frequently. Exiting. *"
	echo "******************************************"
	exit 1
    fi
    echo Invoking command:
    echo ${XDAQ_ROOT}/bin/xdaq.exe ${XDAQ_PROFILE:+"-e $XDAQ_PROFILE"} "$@"
    echo
    ${XDAQ_ROOT}/bin/xdaq.exe ${XDAQ_PROFILE:+"-e $XDAQ_PROFILE"} "$@"
    /bin/sleep $SLEEP
done
