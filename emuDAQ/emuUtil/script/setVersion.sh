#!/bin/zsh

function printHelp(){
    cat <<EOF
This script sets the version of all XDAQ applications in the given directory and its subdirectories.
Usage:
       setVersion version TriDAS/emu/emuDAQ
Example:
       setVersion CRUZET4 .
EOF
}

[[ $# -lt 2 ]] && { printHelp; echo; echo "Argument missing. Exiting."; exit 1; }

VERSION_FILES=( $(ls $2/**/*V.h) )

for VERSION_FILE in $VERSION_FILES; do
    echo "Changing version in $VERSION_FILE"
    cp $VERSION_FILE ${VERSION_FILE}_before_setVersion
    sed -e "s/\(.*\)versions\([ ]*\)=\([ ]*\)\"\([^ ]*\)\"[ ]*;/\1versions\2=\3\"$1\";/" $VERSION_FILE > ${VERSION_FILE}_after_setVersion
    cp ${VERSION_FILE}_after_setVersion $VERSION_FILE
done
