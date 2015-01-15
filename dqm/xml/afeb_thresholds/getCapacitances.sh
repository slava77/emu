#!/bin/bash

ips=csc_pos_map_tab.txt
TYPE=1
NUM=1
CSC=ME+1.1.01
while read line; do
#echo "$line"
TYPE=$(echo "$line" | gawk '{ printf("%s",$1) }')
TYPE=${TYPE/#ME/}
NUM=$(echo "$line" | gawk '{ printf("%s",$3) }')
CSC=$(echo "$line" | gawk '{ printf("%s.%s.%s",$4,$5,$6) }')
echo "$CSC $TYPE $NUM"

wget -q https://oraweb.cern.ch/pls/cms_emu_cern.pro/csc_afeb.search${TYPE}?v_csc_number=${NUM} -O AFEB_${CSC}.html
html2txt AFEB_${CSC}.html
rm AFEB_${CSC}.html
done < $ips
