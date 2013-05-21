#!/bin/zsh
# Generates a web page (index.html) for the results of the STEP analyzer (runEmuCalibAnalysis.exe).
# Invoke it as
#    generateIndexHTML.sh <results_directory>
# where <results_directory> is the directory containing the Test_AFEB*, Test_CFEB* directories.

DATAPATH=$1
DATADIR=${DATAPATH:h}
DATAFILE=${DATAPATH:t}

# Go to the corresponding result directory
RESULTSDIR=$( print $DATADIR/Test_[AC]FEB??/${DATAFILE:r}.plots(/om[1]) )
[[ -d $RESULTSDIR ]] || { print "Directory of results \"${RESULTSDIR}\" not found. Exiting."; exit 1 }
cd $RESULTSDIR

print "Generating $RESULTSDIR/index.html"
{
    SUMIMAGEFILES=( $(print ./EMU/sum*.png) )
    CSCS=( $(print ME*(/) ) )
    print "<!DOCTYPE html>"
    print "<html>"
    print "<head><title>${RESULTSDIR:t}</title><head>"
    print "<style>"
    print "body { background-color:#ffffff; }"
    print "table { background-color:#ffffff; border-collapse: separate; }"
    print "td, th { color:#000000; font-size:90%; }"
    print "th { background-color:#dddddd; }"
    print "td { background-color:#eeeeee; text-align:left; padding-left: 10px; padding-right: 10px; }"
    print "</style>"
    print "<body>"
    print "<a id=\"top\"/>"
    print "<table style=\"font-size:small;\"><tr><th colspan=\"2\">${RESULTSDIR:t}</th></tr>"
    print "<tr><th>Summaries</th><td>"
    for IMAGEFILE in $SUMIMAGEFILES; do
	PLOTCODE=${${IMAGEFILE:r}##*_}
	print "<a href=\"#sum_$PLOTCODE\">$PLOTCODE</a> |"
    done
    print "</td></tr>"
    for CSC in ${CSCS}; do
	CSCIMAGEFILES=( $(print ./$CSC/*.png) )
	print "<tr><th>$CSC</th><td>"
	for IMAGEFILE in $CSCIMAGEFILES; do
	    PLOTCODE=${${IMAGEFILE:r}##*_}
	    print "<a href=\"#csc_$CSC_$PLOTCODE\">$PLOTCODE</a> |"
	done
	print "</td></tr>"
    done
    for IMAGEFILE in $SUMIMAGEFILES; do
	PLOTCODE=${${IMAGEFILE:r}##*_}
	print "<tr><td colspan=\"2\"><a id=\"sum_$PLOTCODE\" href=\"#top\"><img src=\"$IMAGEFILE\" alt=\"$IMAGEFILE\"></a></td></tr>"
    done
    for CSC in ${CSCS}; do
	CSCIMAGEFILES=( $(print ./$CSC/*.png) )
	for IMAGEFILE in $CSCIMAGEFILES; do
	    PLOTCODE=${${IMAGEFILE:r}##*_}
	    print "<tr><td colspan=\"2\"><a id=\"csc_$CSC_$PLOTCODE\" href=\"#top\"><img src=\"$IMAGEFILE\" alt=\"$IMAGEFILE\"></a></td></tr>"
	done
    done
    print "<table>"
    print "</body>"
    print "</html>"
} > index.html
