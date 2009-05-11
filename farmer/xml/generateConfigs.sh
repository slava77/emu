#!/bin/zsh

echo "Executing"
echo $0 $@

if [[ $#@ -ne 2 ]]; then
    echo "Need exactly two arguments. Exiting."
    echo "Usage:"
    echo "$0 RUI-to-computer_mapping.xml /directory/for/the/output"
    exit 1
fi

RUIMAP=$1
# Create a dir for the output if it doesn't exist yet
OUTDIR=$2
[ -d $OUTDIR ] || mkdir $OUTDIR

typeset -A sideName
sideName=( \
"P" "_sideP" \
"M" "_sideM" \
"B" "" \
)

# .duck for DQM_Display:
echo "xsltproc --stringparam FARM DQM_Display EmuDAQDUCKGenerator.xsl $RUIMAP > $OUTDIR/DQM_Display.duck"
xsltproc --stringparam FARM DQM_Display --stringparam NAME DAQ_writeN_buildN EmuDAQDUCKGenerator.xsl $RUIMAP > $OUTDIR/DQM_Display.duck

# loop over plus, minus and both sides:
for SIDE in ${(k)sideName}; do
    # .duck for DQM:
    echo "xsltproc --stringparam SIDE $SIDE --stringparam FARM DQM --stringparam DIR $OUTDIR --stringparam NAME DAQ${sideName[$SIDE]}_writeN_buildN EmuDAQDUCKGenerator.xsl $RUIMAP > $OUTDIR/DQM${sideName[$SIDE]}.duck"
    xsltproc --stringparam SIDE $SIDE --stringparam FARM DQM --stringparam DIR $OUTDIR --stringparam NAME DAQ${sideName[$SIDE]}_writeN_buildN EmuDAQDUCKGenerator.xsl $RUIMAP > $OUTDIR/DQM${sideName[$SIDE]}.duck
    # loop over all parameter combinations for DAQ:
    for WRITE in Y N; do
	for BUILD in Y N; do
	    NAME=DAQ${sideName[$SIDE]}_write${WRITE}_build${BUILD}
	    # .duck for DAQ:
	    echo "xsltproc --stringparam SIDE $SIDE --stringparam FARM DAQ --stringparam DIR $OUTDIR --stringparam NAME $NAME EmuDAQDUCKGenerator.xsl $RUIMAP > $OUTDIR/$NAME.duck"
	    xsltproc --stringparam SIDE $SIDE --stringparam FARM DAQ --stringparam DIR $OUTDIR --stringparam NAME $NAME EmuDAQDUCKGenerator.xsl $RUIMAP > $OUTDIR/$NAME.duck
	    # common .xml for DAQ and DQM and DQM_Display:
	    echo "xsltproc --stringparam SIDE $SIDE --stringparam WRITE $WRITE --stringparam BUILD $BUILD EmuDAQConfigGenerator.xsl $RUIMAP > $OUTDIR/$NAME.xml"
	    xsltproc --stringparam SIDE $SIDE --stringparam WRITE $WRITE --stringparam BUILD $BUILD EmuDAQConfigGenerator.xsl $RUIMAP > $OUTDIR/$NAME.xml
	done
    done
done
