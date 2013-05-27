#!/bin/zsh
# Prepares a CSC-mapping db if necessary (for tast stands) and invokes the analyzer on the specified data files.
# Invoke it as
#   analysisScriptName analysisExeName 'dataFile1 dataFile2 ...' ['crateId1 dmbSlot1 chamberLabel1' 'crateId2 dmbSlot2 chamberLabel2' ...]


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
[[ -x $ANALYZER ]] || { print "*** Error: Analyzer \"$ANALYZER\" not found or not executable. Exiting."; exit 1 }

[[ -d $DQMCONFIG ]] || { print "*** Error: DQM config dir \"$DQMCONFIG\" not found. Exiting."; exit 1 }


#
# Prepare CSC-mapping database
#

# If we have more than two arguments, those beyond the second one are mappings of the form 'crateId dmbSlot chamberLabel'
if [[ $# -gt 2 ]]; then

    cd $DQMCONFIG

    P5_DB_FILE=csc_map_P5.db
    TESTSTAND_DB_FILE=csc_map.db

    [[ -e $P5_DB_FILE ]] || { print "*** Error: P5 db file $P5_DB_FILE not found. Exiting."; exit 1 }

    [[ -e $TESTSTAND_DB_FILE ]] && { print "Removing old $TESTSTAND_DB_FILE"; rm $TESTSTAND_DB_FILE }

    # Copy csc_map table definition from P5 db into test stand db
    # LIMIT 0 prevents the records from being copied in sqlite3 3.7.13. It prevents the table from being created altogether in sqlite3 v3.3.6 ...
    # sqlite3 -line $TESTSTAND_DB_FILE "ATTACH ${(qq)P5_DB_FILE} AS P5; CREATE TABLE main.csc_map AS SELECT * FROM P5.csc_map LIMIT 0;"
    sqlite3 -line $TESTSTAND_DB_FILE "ATTACH ${(qq)P5_DB_FILE} AS P5; CREATE TABLE main.csc_map AS SELECT * FROM P5.csc_map WHERE chamberLabel='nonexistent';"

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

    # Loop over the arguments beyond the second one
    for I in {3..$#}; do
	MAPPING=( $(print ${(P)I}) )
	CRATEID=${MAPPING[1]}
	DMB=${dmbSlotToId[${MAPPING[2]}]}
	CHAMBER=${MAPPING[3]}
	print $CRATEID $DMB $CHAMBER
	case $CHAMBER in

	    ( ME[+-]1/1/<37-99> | ME[+-]4/2/<37-99> )

	    print "Dummy chamber label ${(qq)CHAMBER} is added to test stand mapping db."
	    # Copy from the P5 CSC mapping db a row that corresponds to an existing chamber in this ring (say, chamber 01)
	    EXISTINGCHAMBER="${CHAMBER[1,-3]}01"
	    sqlite3 -line $TESTSTAND_DB_FILE "ATTACH ${(qq)P5_DB_FILE} AS P5; INSERT INTO main.csc_map SELECT * FROM P5.csc_map WHERE chamberLabel=${(qq)EXISTINGCHAMBER};"
	    # Change the crate id and DMB id to those of the test stand in order for the analysis program to know to what chamber the data containing these ids belong to. Change the chamber label, too (to the dummy one).
	    sqlite3 -line $TESTSTAND_DB_FILE "UPDATE csc_map SET crateid=${(qq)CRATEID}, dmb=${(qq)DMB}, chamberLabel=${(qq)CHAMBER} WHERE chamberLabel=${(qq)EXISTINGCHAMBER};"
	    ;;


	    ( ME[+-]1/<1-3>/<01-36> | ME[+-]<2-4>/1/<01-18> | ME[+-]<2-4>/2/<01-36> )

	    print "Canonical chamber label ${(qq)CHAMBER} is added to test stand mapping db."
	    # Copy this chamber's row from the P5 CSC mapping db
	    sqlite3 -line $TESTSTAND_DB_FILE "ATTACH ${(qq)P5_DB_FILE} AS P5; INSERT INTO main.csc_map SELECT * FROM P5.csc_map WHERE chamberLabel=${(qq)CHAMBER};"
	    # Change the crate id and DMB id to those of the test stand in order for the analysis program to know to what chamber the data containing these ids belong to
	    sqlite3 -line $TESTSTAND_DB_FILE "UPDATE csc_map SET crateid=${(qq)CRATEID}, dmb=${(qq)DMB} WHERE chamberLabel=${(qq)CHAMBER};"
	    ;;


	    ( * )

	    print "Non-canonical chamber label ${(qq)CHAMBER}. Adding it to test stand mapping db assuming the properties of ME+4/2/01"
	    # ME+4/2/01 implies chamberId=142010
	    # Copy ME+4/2/01's row from the P5 CSC mapping db
	    sqlite3 -line $TESTSTAND_DB_FILE "ATTACH ${(qq)P5_DB_FILE} AS P5; INSERT INTO main.csc_map SELECT * FROM P5.csc_map WHERE chamberId=142010;"
	    # Change the crate id and DMB id to those of the test stand in order for the analysis program to know to what chamber the data containing these ids belong to
	    sqlite3 -line $TESTSTAND_DB_FILE "UPDATE csc_map SET crateid=${(qq)CRATEID}, dmb=${(qq)DMB}, chamberLabel=${(qq)CHAMBER} WHERE chamberId=142010;"
	    ;;

	esac
    done

fi


#
# Run analyzer
#

# The first argument is a space-separated list of data files given in the format host:file
DATAFILES=( $(print $2) )
for DATAFILE in $DATAFILES; do
    DATAHOSTNAME=${DATAFILE%:*}
    DATAPATHNAME=${DATAFILE#*:}
    RESULTSTOPDIR=${DATAPATHNAME:h}/Tests_results
    [[ -d $RESULTSTOPDIR ]] || mkdir -p $RESULTSTOPDIR
    print "cd $RESULTSTOPDIR && $ANALYZER $DATAPATHNAME"
    cd $RESULTSTOPDIR && $ANALYZER $DATAPATHNAME
    RESULTSDIR=$( print $RESULTSTOPDIR/Test_*/${DATAPATHNAME:t:r}.plots(/om[1]) )
    if [[ -x ${0:h}/generateIndexHTML.sh ]]; then
	print "Generating web page with ${0:h}/generateIndexHTML.sh $RESULTSDIR"
	${0:h}/generateIndexHTML.sh $RESULTSDIR
    else
	print "** Warning: Web page generator script ${0:h}/generateIndexHTML.sh not found."
    fi
done
