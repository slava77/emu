#!/bin/bash

COLLECTION=calibration

if [ $1 ]; then
COLLECTION=$1
fi

# hack for CERN's: need to remove .crt file 
rm $HOME/.sitecopy/$COLLECTION.crt

sitecopy -u $COLLECTION
