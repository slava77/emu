#!/bin/zsh
# Creates web pages and symbolic links to analysis results in a chamber-oriented directory structure
# Usage:
#   linkToChambers <results_dir> <dir_for_symlinks>
# where
#   results_dir      : contains the Test_* directories created by the analyzer
#   dir_for_symlinks : is where the chamber-oriented directory structure should be created for the symlinks

# Example for analysis results directory structure:
#   Test_CFEB04/csc_00000001_EmuRUI01_Test_17b_000_130531_092346_UTC.plots/ME+2.1.03
# Symlink to it in a chamber-oriented directory structure:
#   ME+2.1.03/Test_CFEB04/csc_00000001_EmuRUI01_Test_17b_000_130531_092346_UTC/plots
# Web page in a chamber-oriented directory structure:
#   ME+2.1.03/Test_CFEB04/csc_00000001_EmuRUI01_Test_17b_000_130531_092346_UTC/index.html

function generateWebPage(){
    # argument 1: directory
    cd $1
    {
	print "<!DOCTYPE html>"
	print "<html>"
	print "<head><title>${1:t}</title><head>"
	print "<style>"
	print "body { background-color:#ffffff; }"
	print "table { background-color:#ffffff; border-collapse: separate; }"
	print "td, th { color:#000000; font-size:90%; }"
	print "th { background-color:#dddddd; }"
	print "td { background-color:#eeeeee; text-align:left; padding-left: 10px; padding-right: 10px; }"
	print "</style>"
	print "<body>"
	print "<a id=\"top\"/>"
	print "<table style=\"font-size:small;\"><tr><th>${1:t}</th></tr>"
	print "<tr><td>"
	for IMAGEFILE in ./plots/*.png(N); do
	    PLOTCODE=${${IMAGEFILE:r}##*_}
	    print "<a href=\"#csc_$PLOTCODE\">$PLOTCODE</a> |"
	done
	print "</td></tr>"
	for IMAGEFILE in ./plots/*.png(N); do
	    PLOTCODE=${${IMAGEFILE:r}##*_}
	    print "<tr><td><a id=\"csc_$PLOTCODE\" href=\"#top\"><img src=\"$IMAGEFILE\" alt=\"$IMAGEFILE\"></a></td></tr>"
	done
	print "<table>"
	print "</body>"
	print "</html>"
    } > index.html
    cd -
}

if [[ $# -ne 2 || ${1[1]} != "/" || ${2[1]} != "/" ]]; then
    print "Need two full paths as arguments. Exiting.\nUsage:\n  $0 <results_dir> <dir_for_symlinks>"
    exit 1
fi

# Remove old chamber-oriented directory structure...
OLDDIRS=( $2/(EMU|ME(+|-)(1|2|3|4)\.(1|2|3)\.)*(/N) )
[[ ${#OLDDIRS} -gt 0 ]] && rm -rf $OLDDIRS

# ...and recreate it
IFS='/'
for DIR in $1/Test_*/csc_*.plots/*(/); do
    DIRARRAY=( ${=DIR} )
    mkdir -p $2/${DIRARRAY[-1]}/${DIRARRAY[-3]}/${DIRARRAY[-2]%.plots}
    [[ -L $2/${DIRARRAY[-1]}/${DIRARRAY[-3]}/${DIRARRAY[-2]%.plots}/plots ]] || ln -s $DIR $2/${DIRARRAY[-1]}/${DIRARRAY[-3]}/${DIRARRAY[-2]%.plots}/plots
    generateWebPage $2/${DIRARRAY[-1]}/${DIRARRAY[-3]}/${DIRARRAY[-2]%.plots}
done
