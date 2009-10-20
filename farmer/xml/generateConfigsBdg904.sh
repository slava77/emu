#!/bin/zsh


# Create a dir for the output if it doesn't exist yet
OUTDIR=$(pwd)/out
[ -d $OUTDIR ] || mkdir $OUTDIR

# .duck for DQM_Display:
echo "xsltproc --stringparam FARM DQM_Display --stringparam DIR $OUTDIR --stringparam NAME DAQ_writeN_buildN EmuDAQ904DUCKGenerator.xsl RUI-to-computer_mapping_bdg904.xml > $OUTDIR/DQM_Display.duck"
xsltproc --stringparam FARM DQM_Display --stringparam DIR $OUTDIR --stringparam NAME DAQ_writeN_buildN EmuDAQ904DUCKGenerator.xsl RUI-to-computer_mapping_bdg904.xml > $OUTDIR/DQM_Display.duck


# .duck for DQM:
echo "xsltproc --stringparam FARM DQM --stringparam DIR $OUTDIR --stringparam NAME DAQ_writeN_buildN EmuDAQ904DUCKGenerator.xsl RUI-to-computer_mapping_bdg904.xml > $OUTDIR/DQM.duck"
xsltproc --stringparam FARM DQM --stringparam DIR $OUTDIR --stringparam NAME DAQ_writeN_buildN EmuDAQ904DUCKGenerator.xsl RUI-to-computer_mapping_bdg904.xml > $OUTDIR/DQM.duck
# loop over all parameter combinations for DAQ:
for WRITE in Y N; do
    for BUILD in N; do
	NAME=DAQ_write${WRITE}_build${BUILD}
	# .duck for DAQ:
	echo "xsltproc --stringparam FARM DAQ --stringparam DIR $OUTDIR --stringparam NAME $NAME EmuDAQ904DUCKGenerator.xsl RUI-to-computer_mapping_bdg904.xml > $OUTDIR/$NAME.duck"
	xsltproc --stringparam FARM DAQ --stringparam DIR $OUTDIR --stringparam NAME $NAME EmuDAQ904DUCKGenerator.xsl RUI-to-computer_mapping_bdg904.xml > $OUTDIR/$NAME.duck
        # common .xml for DAQ and DQM and DQM_Display:
	echo "xsltproc --stringparam WRITE $WRITE --stringparam BUILD $BUILD EmuDAQ904ConfigGenerator.xsl RUI-to-computer_mapping_bdg904.xml > $OUTDIR/$NAME.xml"
	xsltproc --stringparam WRITE $WRITE --stringparam BUILD $BUILD EmuDAQ904ConfigGenerator.xsl RUI-to-computer_mapping_bdg904.xml > $OUTDIR/$NAME.xml
    done
done
