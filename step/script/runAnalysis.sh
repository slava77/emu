#!/bin/zsh
# Prepares a CSC-mapping db if necessary (for tast stands) and invokes the analyzer on the specified data files.
# Invoke it as
#   analysisScriptName analysisExeName 'dataFile1 dataFile2 ...' ['crateId1 dmbSlot1 chamberLabel1' 'crateId2 dmbSlot2 chamberLabel2' ...]

function convertToDDU(){
    # Adds DDU headers and trailers if necessary
    # Argument 1: data file name
    if [[ $(od -Ad -tx8z -w8 $1 | grep -c 8000ffff80008000) -eq 0 ]]; then
	print "No DDU trailers found in $1. Adding them and the headers."
	# First rename the original file.
	print "mv ${1} ${1:r}.odmb"
	mv ${1} ${1:r}.odmb
	# Add the DDU headers and trailers.
	print "Adding DDU headers and trailers:\n${ANALYZER:h}/add_ddu_hdr.exe ${1:r}.odmb ${1:r}.raw"
	${ANALYZER:h}/add_ddu_hdr.exe ${1:r}.odmb ${1:r}.raw
    fi
}

ulimit -c unlimited

print
print "Environment variables:"
print "    BUILD_HOME         = $BUILD_HOME"
print "    XDAQ_ROOT          = $XDAQ_ROOT"
print "    XDAQ_OS            = $XDAQ_OS"
print "    XDAQ_PLATFORM      = $XDAQ_PLATFORM"
print "    XDAQ_DOCUMENT_ROOT = $XDAQ_DOCUMENT_ROOT"
print "    ROOTSYS            = $ROOTSYS"
print "    LD_LIBRARY_PATH    = $LD_LIBRARY_PATH"
print "    CMSSW              = $CMSSW"
print "    DQMCONFIG          = $DQMCONFIG"
print

print "Invoked as"
print $0 "$@"


#
# Sanity check
#

[[ $# -lt 2 ]] && { print "*** Error: At least 2 arguments are needed. Exiting."; exit 1 }

ANALYZER=$1

if [[ $2 == *STEP_27* ]]; then
    DQMANALYZER=${1:h}/runEmuCSCAnalyzer.exe
    print "Looks like test 27 (high-statistics cosmics and gas gain). Will try to analyze it with $ANALYZER and $DQMANALYZER"
    [[ -x $DQMANALYZER ]] || { print "*** Error: DQM analyzer \"$DQMANALYZER\" not found or not executable. Exiting."; exit 1 }
elif [[ $2 == *STEP_40* ]]; then
    DQMANALYZER=${1:h}/runEmuCSCAnalyzer.exe
    print "Looks like test 40 (beam trigger). Will try to analyze it with $ANALYZER and $DQMANALYZER"
    [[ -x $DQMANALYZER ]] || { print "*** Error: DQM analyzer \"$DQMANALYZER\" not found or not executable. Exiting."; exit 1 }

fi

[[ -x $ANALYZER ]] || { print "*** Error: Analyzer \"$ANALYZER\" not found or not executable. Exiting."; exit 1 }

[[ -d $DQMCONFIG ]] || { print "*** Error: DQM config dir \"$DQMCONFIG\" not found. Exiting."; exit 1 }


#
# Prepare CSC-mapping database
#

P5_DB_FILE=csc_map_P5.db
DB_FILE=csc_map.db

# If we have more than two arguments, those beyond the second one are mappings of the form 'crateId dmbSlot chamberLabel'
if [[ $# -gt 2 ]]; then

    cd $DQMCONFIG

    [[ -e $P5_DB_FILE ]] || { print "*** Error: P5 db file $P5_DB_FILE not found. Exiting."; exit 1 }

    [[ -e $DB_FILE ]] && { print "Removing old $DQMCONFIG/$DB_FILE"; rm $DB_FILE }

    # Copy csc_map table definition from P5 db into test stand db
    # LIMIT 0 prevents the records from being copied in sqlite3 3.7.13. It prevents the table from being created altogether in sqlite3 v3.3.6 ...
    # sqlite3 -line $DB_FILE "ATTACH ${(qq)P5_DB_FILE} AS P5; CREATE TABLE main.csc_map AS SELECT * FROM P5.csc_map LIMIT 0;"
    sqlite3 -line $DB_FILE "ATTACH ${(qq)P5_DB_FILE} AS P5; CREATE TABLE main.csc_map AS SELECT * FROM P5.csc_map WHERE chamberLabel='nonexistent';"

    # DMB slot --> DMB id associative array
    typeset -A dmbSlotToId 
    dmbSlotToId=(
	3 1
	5 2
	7 3
	9 4
	11 5
	15 7
	17 8
	19 9
	21 10
    )

    # Create a chamber list
    CHAMBERARRAY=()

    # Loop over the arguments beyond the second one
    for I in {3..$#}; do
	MAPPING=( $(print ${(P)I}) )
	CRATEID=${MAPPING[1]}
	DMB=${dmbSlotToId[${MAPPING[2]}]}
	(( $DMB<6 )) && (( CSCID=$DMB )) || (( CSCID=$DMB-1 ))
	CHAMBER=${MAPPING[3]}
	CHAMBERARRAY+=$CHAMBER
	print $CRATEID $DMB $CHAMBER
	case $CHAMBER in

	    ( ME[+-]1/<1-3>/<01-36> | ME[+-]<2-4>/1/<01-18> | ME[+-]<2-4>/2/<01-36> )

	    print "Canonical chamber label ${(qq)CHAMBER} is added to test stand mapping db."
	    # Copy this chamber's row from the P5 CSC mapping db
	    sqlite3 -line $DB_FILE "ATTACH ${(qq)P5_DB_FILE} AS P5; INSERT INTO main.csc_map SELECT * FROM P5.csc_map WHERE chamberLabel=${(qq)CHAMBER};"
	    # Change the crate id, csc id, csc index and DMB id to those of the test stand in order for the analysis program to know to what chamber the data containing these ids belong to
	    sqlite3 -line $DB_FILE "UPDATE csc_map SET crateid=${(qq)CRATEID}, cscid=${(qq)CSCID}, dmb=${(qq)DMB}, cscIndex=${(qq)$(( 10*$CRATEID+$CSCID ))} WHERE chamberLabel=${(qq)CHAMBER};"
	    ;;


	    ( ME[+-]1/<1-3>/* | ME[+-]<2-4>/1/* | ME[+-]<2-4>/2/* )

	    print "Non-canonical chamber label ${(qq)CHAMBER} is added to test stand mapping db."
	    # Copy from the P5 CSC mapping db a row that corresponds to an existing chamber in this ring (say, chamber 01)
	    EXISTINGCHAMBER="${CHAMBER[1,7]}01"
	    sqlite3 -line $DB_FILE "ATTACH ${(qq)P5_DB_FILE} AS P5; INSERT INTO main.csc_map SELECT * FROM P5.csc_map WHERE chamberLabel=${(qq)EXISTINGCHAMBER};"
	    # Change the crate id, csc id, csc index and DMB id to those of the test stand in order for the analysis program to know to what chamber the data containing these ids belong to. Change the chamber label, too (to the dummy one).
	    sqlite3 -line $DB_FILE "UPDATE csc_map SET crateid=${(qq)CRATEID}, cscid=${(qq)CSCID}, dmb=${(qq)DMB}, cscIndex=${(qq)$(( 10*$CRATEID+$CSCID ))}, chamberLabel=${(qq)CHAMBER} WHERE chamberLabel=${(qq)EXISTINGCHAMBER};"
	    ;;


	    ( * )

	    print "*** Error: Malformed chamber label ${(qq)CHAMBER}. Exiting."
	    print '           Note that all chamber labels must be canonical up to the ring. A well-formed non-canonical chamber label example: ME+4/2/_239_'
	    exit 1
	    ;;

	esac
    done

else
    # No explicit chamber mappings specified. Use the canonical one of P5.
    [[ -e $DQMCONFIG/$P5_DB_FILE ]] && cp $DQMCONFIG/$P5_DB_FILE $DQMCONFIG/$DB_FILE
fi


#
# Run analyzer
#

# The first argument is a space-separated list of data files given in the format host:file
DATAFILES=( $(print $2) )
for DATAFILE in $DATAFILES; do
    DATAFQHOSTNAME=${DATAFILE%:*}
    DATAHOSTNAME=${DATAFQHOSTNAME%.*}
    DATANETNAME=${DATAFQHOSTNAME#*.}
    DATAPATHNAME=${DATAFILE#*:}
    print "DATAFQHOSTNAME =$DATAFQHOSTNAME"
    print "DATAHOSTNAME   =$DATAHOSTNAME"
    print "DATANETNAME    =$DATANETNAME"
    print "DATAPATHNAME   =$DATAPATHNAME"
    if [[ $DATANETNAME == "cms" ]]; then
	# In the .cms network, convert 
	# csc-daq03.cms:/data/csc_00000001_EmuRUI09_Test_13_000_130924_171005_UTC.raw to
	# /cmscsc/csc-daq03/csc_00000001_EmuRUI09_Test_13_000_130924_171005_UTC.raw
	DATAPATHNAME=/cmscsc/${DATAHOSTNAME}/${DATAPATHNAME#/data/}
    fi
    print "DATAPATHNAME   =$DATAPATHNAME"

    convertToDDU $DATAPATHNAME

    RESULTSTOPDIR=${DATAPATHNAME:h}/Tests_results
    [[ -d $RESULTSTOPDIR ]] || mkdir -p $RESULTSTOPDIR

    if [[ $DATAPATHNAME == *STEP_27* ]]; then
	# For test 27, the high-stat cosmics, first produce the .root file, then the plots. Finally, list the chambers in chambers.txt for linkToChambers.sh to know which chambers' data these results contain.
	print "cd $RESULTSTOPDIR && mkdir -p Test_27_Cosmics && cd Test_27_Cosmics && $DQMANALYZER $DATAPATHNAME && $DQMANALYZER ${DATAPATHNAME:t:r}.root && cd ${DATAPATHNAME:t:r}.plots && { print $CHAMBERARRAY > chambers.txt }"
	cd $RESULTSTOPDIR && mkdir -p Test_27_Cosmics && cd Test_27_Cosmics && $DQMANALYZER $DATAPATHNAME && $DQMANALYZER ${DATAPATHNAME:t:r}.root && cd ${DATAPATHNAME:t:r}.plots && { print $CHAMBERARRAY > chambers.txt }
    elif [[ $DATAPATHNAME == *STEP_40* ]]; then
	# For test 40, the beam trigger, first produce the .root file, then the plots. Finally, list the chambers in chambers.txt for linkToChambers.sh to know which chambers' data these results contain.
	print "cd $RESULTSTOPDIR && mkdir -p Test_40_Beam && cd Test_40_Beam && $DQMANALYZER $DATAPATHNAME && $DQMANALYZER ${DATAPATHNAME:t:r}.root && cd ${DATAPATHNAME:t:r}.plots && { print $CHAMBERARRAY > chambers.txt }"
	cd $RESULTSTOPDIR && mkdir -p Test_40_Beam && cd Test_40_Beam && $DQMANALYZER $DATAPATHNAME && $DQMANALYZER ${DATAPATHNAME:t:r}.root && cd ${DATAPATHNAME:t:r}.plots && { print $CHAMBERARRAY > chambers.txt }
    fi

    print "cd $RESULTSTOPDIR && $ANALYZER $DATAPATHNAME"
    cd $RESULTSTOPDIR && $ANALYZER $DATAPATHNAME

    # Generate web page for easier browsing. Not for tests 27 and 40, though, for those the DQM analyzer does it already:
    print "Results' dir:"
    print $RESULTSTOPDIR/Test_*/${DATAPATHNAME:t:r}.plots(/Nom[1])
    RESULTSDIR=$( print $RESULTSTOPDIR/Test_*/${DATAPATHNAME:t:r}.plots(/Nom[1]) )
    if [[ ${#RESULTSDIR} -gt 0 && $RESULTSDIR != *Test_27_* && $RESULTSDIR != *Test_40_* ]]; then
	if [[ -x ${0:h}/generateIndexHTML.sh ]]; then
	    print "Generating web page with ${0:h}/generateIndexHTML.sh $RESULTSDIR"
	    ${0:h}/generateIndexHTML.sh $RESULTSDIR
	else
	    print "** Warning: Web page generator script ${0:h}/generateIndexHTML.sh not found."
	fi
    fi

    if [[ -x ${0:h}/linkToChambers.sh ]]; then
	print "Generating chamber-oriented directory structure in $RESULTSTOPDIR"
	${0:h}/linkToChambers.sh $RESULTSTOPDIR $RESULTSTOPDIR
    else
	print "** Warning: Chamber-oriented directory generator script ${0:h}/linkToChambers.sh not found."
    fi
done
