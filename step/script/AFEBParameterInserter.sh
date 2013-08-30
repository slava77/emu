#!/bin/zsh
# Inserts delays or thresholds from an analysis result file into a VME config XML file.
# Examples:
#  AFEBParameterInserter.sh ME+1.1.28_AFEB07_R04.results VME.xml
#  AFEBParameterInserter.sh ME+1.1.28_AFEB06_R13.results VME.xml

if [[ $# -ne 2 ]]; then
    print "*** Error: Need exactly two arguments. Exiting."
    print "    Usage:"
    print "    $0 <AFEB_delay_or_threshold_results> <VME_config_XML>"
    exit 1
fi

RESULTSFILE=$1
VMECONFIGFILE=$2

NOW=$( date +"%Y%m%d_%H%M%S" )

CHAMBERLABEL=$( sed -n -e 's/^#[[:print:]]*ME\([+-]\)\([1-4]\)[.\/]\([1-3]\)[.\/]\([^ \/]\+\)[[:print:]]*/ME\1\2\/\3\/\4/p' $RESULTSFILE )
if [[ ${#CHAMBERLABEL} -lt 9 ]]; then
    print "*** Error: Chamber label \'${CHAMBERLABEL}\' found in ${RESULTSFILE} is not well formed. Exiting."
    exit 1
fi

if [[ $( grep -c -i threshold $RESULTSFILE ) -gt 0 ]]; then

    print "Backing up $VMECONFIGFILE as ${VMECONFIGFILE}_before_inserting_thresholds_${NOW}"
    cp $VMECONFIGFILE ${VMECONFIGFILE}_before_inserting_thresholds_${NOW}

    typeset -A THRESHOLDS
    THRESHOLDS=( $( grep '^[ ]*[0-9]\+[ ]*[0-9]\+[ ]*$' $RESULTSFILE ) )
    print "AFEB  threshold    for $CHAMBERLABEL"
    for I in ${(onk)THRESHOLDS}; do
	print ${(l:4:)I} ${(l:10:)THRESHOLDS[$I]}
	mv $VMECONFIGFILE ${VMECONFIGFILE}_tmp
	xsltproc --stringparam CHAMBERLABEL $CHAMBERLABEL --stringparam AFEBNUMBER $I --stringparam AFEBTHRESHOLD ${THRESHOLDS[$I]} ${0:h}/AFEBParameterInserter.xsl ${VMECONFIGFILE}_tmp > ${VMECONFIGFILE}
    done

    rm ${VMECONFIGFILE}_tmp
    
elif [[ $( grep -c -i delay $RESULTSFILE ) -gt 0 ]]; then

    print "Backing up $VMECONFIGFILE as ${VMECONFIGFILE}_before_inserting_delays_${NOW}"
    cp $VMECONFIGFILE ${VMECONFIGFILE}_before_inserting_delays_${NOW}

    print "AFEB      delay    for $CHAMBERLABEL"
    grep '^[ ]*[0-9]\+[ ]*[0-9]\+[ ]*[0-9.]\+[ ]*[0-9.]\+[ ]*[0-9]\+[ ]*[0-9.]\+[ ]*$' $RESULTSFILE | while read LINE; do
	PARAMETERS=( $( print $LINE ) )
	print ${(l:4:)PARAMETERS[1]} ${(l:10:)PARAMETERS[5]}
	mv $VMECONFIGFILE ${VMECONFIGFILE}_tmp
	xsltproc --stringparam CHAMBERLABEL $CHAMBERLABEL --stringparam AFEBNUMBER ${PARAMETERS[1]} --stringparam AFEBFINEDELAY ${PARAMETERS[5]} ${0:h}/AFEBParameterInserter.xsl ${VMECONFIGFILE}_tmp > ${VMECONFIGFILE}
    done

    rm ${VMECONFIGFILE}_tmp

else

    print "*** Error: Cannot figure out if file \"$RESULTSFILE\" is a threshold or delay file. Exiting."
    exit 1

fi
