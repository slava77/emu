#!/bin/zsh


# Create a dir for the output if it doesn't exist yet
OUTDIR=out
[ -d $OUTDIR ] || mkdir $OUTDIR

typeset -A sideName
sideName=( \
"P" "_sideP" \
"M" "_sideM" \
"B" "" \
)

# .duck for DQM_Display:
echo "xsltproc --stringparam SIDE B --stringparam FARM DQM_Display --stringparam NAME DAQ_side${SIDE}_writeN_buildN EmuDAQDUCKGenerator.xsl RUI-to-computer_mapping.xml > $OUTDIR/DQM_Display_side${SIDE}.duck"
xsltproc --stringparam SIDE B --stringparam FARM DQM_Display --stringparam NAME DAQ_writeN_buildN EmuDAQDUCKGenerator.xsl RUI-to-computer_mapping.xml > $OUTDIR/DQM_Display.duck

# loop over plus, minus and both sides:
for SIDE in ${(k)sideName}; do
    # .duck for DQM:
    echo "xsltproc --stringparam SIDE $SIDE --stringparam FARM DQM --stringparam NAME DAQ${sideName[$SIDE]}_writeN_buildN EmuDAQDUCKGenerator.xsl RUI-to-computer_mapping.xml > $OUTDIR/DQM${sideName[$SIDE]}.duck"
    xsltproc --stringparam SIDE $SIDE --stringparam FARM DQM --stringparam NAME DAQ${sideName[$SIDE]}_writeN_buildN EmuDAQDUCKGenerator.xsl RUI-to-computer_mapping.xml > $OUTDIR/DQM${sideName[$SIDE]}.duck
    # loop over all parameter combinations for DAQ:
    for WRITE in Y N; do
	for BUILD in Y N; do
	    NAME=DAQ${sideName[$SIDE]}_write${WRITE}_build${BUILD}
	    # .duck for DAQ:
	    echo "xsltproc --stringparam SIDE $SIDE --stringparam FARM DAQ --stringparam NAME $NAME EmuDAQDUCKGenerator.xsl RUI-to-computer_mapping.xml > $OUTDIR/$NAME.duck"
	    xsltproc --stringparam SIDE $SIDE --stringparam FARM DAQ --stringparam NAME $NAME EmuDAQDUCKGenerator.xsl RUI-to-computer_mapping.xml > $OUTDIR/$NAME.duck
	    # common .xml for DAQ and DQM and DQM_Display:
	    echo "xsltproc --stringparam SIDE $SIDE --stringparam WRITE $WRITE --stringparam BUILD $BUILD EmuDAQConfigGenerator.xsl RUI-to-computer_mapping.xml > $OUTDIR/$NAME.xml"
	    xsltproc --stringparam SIDE $SIDE --stringparam WRITE $WRITE --stringparam BUILD $BUILD EmuDAQConfigGenerator.xsl RUI-to-computer_mapping.xml > $OUTDIR/$NAME.xml
	done
    done
done
