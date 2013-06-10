#!/bin/zsh

[[ $(hostname -f | grep -c '.cms904') -gt 0 ]] && DATADIR=/csc/daq || DATADIR=/local/data/$USERNAME

# RAWDATADIR, ARCHIVEDIR and CONDEMNDIR should be set. Their default values are:
# RAWDATADIR should have the raw data files and the Tests_results dir
RAWDATADIR=${RAWDATADIR:-$DATADIR/current}
# The results to be kept will be moved to ARCHIVEDIR. It should be completely disjoint from RAWDATADIR
ARCHIVEDIR=${ARCHIVEDIR:-$DATADIR/archive}
# The results to be deleted will be moved to CONDEMNDIR. It should be completely disjoint from RAWDATADIR
CONDEMNDIR=${CONDEMNDIR:-$DATADIR/condemn}

[[ ${#RAWDATADIR} -eq 0 ]] && { print "RAWDATADIR is not set. Exiting."; exit 1 }
[[ ${#ARCHIVEDIR} -eq 0 ]] && { print "ARCHIVEDIR is not set. Exiting."; exit 1 }
[[ ${#CONDEMNDIR} -eq 0 ]] && { print "CONDEMNDIR is not set. Exiting."; exit 1 }

mkdir -p $ARCHIVEDIR
mkdir -p $CONDEMNDIR

CONDEMNED="to be deleted"

autoload -U colors && colors

#
# List runs with no README file in the result plots' dir
#
((INDEX=0))
typeset -a INDICES
typeset -a RUNTYPES
typeset -a RUNNUMBERS
typeset -a RUNTIMES
PLOTDIRS=( $RAWDATADIR/Tests_results/Test_*/csc_*plots(/Ne{'[[ ! -f $REPLY/README ]]'}) )
for PLOTDIR in $PLOTDIRS; do
    ((INDEX++))
    INDICES+=( $INDEX )
    RUNTYPES+=( $(print $PLOTDIR | sed 's/.*_\(Test_[^_]\+\).*/\1/') )
    RUNNUMBERS+=( $(print $PLOTDIR | sed 's/.*csc_\([0-9]\{8\}\)_.*/\1/') )
    RUNTIMES+=( $(print $PLOTDIR | sed 's/.*_\([0-9]\{6\}_[0-9]\{6\}_UTC\).*/\1/') )
done
print ${fg_bold[black]}"Runs in "${fg_no_bold[black]} $RAWDATADIR/Tests_results
print ${fg_bold[black]}"index  run type     run number start time"${fg_no_bold[black]}
for INDEX in $INDICES; do
    print "${(l:5:)INDEX}  ${(r:12:)RUNTYPES[$INDEX]} ${(r:10:)RUNNUMBERS[$INDEX]} ${(r:22:)RUNTIMES[$INDEX]}"
done

#
# Prompt user to select runs to comment from the above list
#
print -n ${fg[blue]}'Select runs as a space-separated list of indices or index ranges (e.g., 1 3,7). \nUse * for all, nothing for none: '${fg[black]}
read SELECTION
SUBSCRIPTARRAY=( $( print $SELECTION ) )
# Collect into INDICESARRAY all elements specified by indices or index arrays
typeset -a INDICESARRAY
for SUBSCRIPT in $SUBSCRIPTARRAY; do
    eval 'INDICESARRAY+=( ${INDICES['$SUBSCRIPT']} )'
done
print ${fg_bold[black]}"index  run type     run number start time"${fg_no_bold[black]}
for I in $INDICESARRAY; do
    print "${(l:5:)I}  ${(r:12:)RUNTYPES[$I]} ${(r:10:)RUNNUMBERS[$I]} ${(r:22:)RUNTIMES[$I]}"
done
# Prompt user for comment (same for all the selected runs)
print -n ${fg[blue]}"Enter comment in one line for the selected runs [${(qq)CONDEMNED}]: "${fg[black]}
read COMMENT
if [[ ${#COMMENT} -eq 0 ]]; then
    COMMENT=$CONDEMNED
fi
# Ask user for confirmation
print ${fg_bold[black]}"index  run type     run number start time             comment"${fg_no_bold[black]}
for I in $INDICESARRAY; do
    print "${(l:5:)I}  ${(r:12:)RUNTYPES[$I]} ${(r:10:)RUNNUMBERS[$I]} ${(r:22:)RUNTIMES[$I]} ${(qq)COMMENT}"
done
print -n ${fg[blue]}"Is this OK [y|n]? "${fg[black]}
read -q || exit 0

#
# Write README files with comment, and move the non-condemned runs ARCHIVEDIR, the condemned ones to the CONDEMNDIR
#
for I in $INDICESARRAY; do
    print "print \"$COMMENT\" > ${PLOTDIRS[$I]}/README"
    print $COMMENT > ${PLOTDIRS[$I]}/README
    [[ $COMMENT == $CONDEMNED ]] && DESTDIR=$CONDEMNDIR || DESTDIR=$ARCHIVEDIR
    # Move the raw data files
    DATAFILES=( $RAWDATADIR/${PLOTDIRS[$I]:t:r}*(.N) )
    if [[ ${#DATAFILES} -gt 0 ]]; then
	print "mv $DATAFILES $DESTDIR"
	mv $DATAFILES $DESTDIR
    fi
    # Move the time stamp files of test 25
    if [[ ${RUNTYPES[$I]} == "Test_25" ]]; then
	TIMESTAPMFILES=( $RAWDATADIR/Test25_*_${RUNNUMBERS[$I]}_${RUNTIMES[$I]}.txt(N) )
	if [[ ${#TIMESTAPMFILES} -gt 0 ]]; then
	    print "mv $TIMESTAPMFILES $DESTDIR"
	    mv $TIMESTAPMFILES $DESTDIR
	fi
    fi
    # Move the analysis plots
    print "mkdir -p ${${PLOTDIRS[$I]/$RAWDATADIR/$DESTDIR}:h}"
    mkdir -p ${${PLOTDIRS[$I]/$RAWDATADIR/$DESTDIR}:h}
    print "mv ${PLOTDIRS[$I]} ${PLOTDIRS[$I]/$RAWDATADIR/$DESTDIR}"
    mv ${PLOTDIRS[$I]} ${PLOTDIRS[$I]/$RAWDATADIR/$DESTDIR}
    # Move the analysis ROOT files (and any other files with the same file name)
    ROOTFILES=( ${PLOTDIRS[$I]:r}*(.N) )
    for ROOTFILE in $ROOTFILES; do
	mv $ROOTFILE ${PLOTDIRS/$RAWDATADIR/$DESTDIR}
    done
done

#
# Recreate the chamber-oriented directory structures
#
if [[ -x ${0:h}/linkToChambers.sh ]]; then
    print "Generating chamber-oriented directory structure in $RAWDATADIR/Tests_results"
    ${0:h}/linkToChambers.sh $RAWDATADIR/Tests_results $RAWDATADIR/Tests_results
    print "Generating chamber-oriented directory structure in $ARCHIVEDIR/Tests_results"
    ${0:h}/linkToChambers.sh $ARCHIVEDIR/Tests_results $ARCHIVEDIR/Tests_results
    print "Generating chamber-oriented directory structure in $CONDEMNDIR/Tests_results"
    ${0:h}/linkToChambers.sh $CONDEMNDIR/Tests_results $CONDEMNDIR/Tests_results
else
    print "** Warning: Chamber-oriented directory generator script ${0:h}/linkToChambers.sh not found."
fi
