#!/bin/zsh

# Clones this eth2 module hook for eth3, eth4 and eth5. (This will remove and recreate the directories for them!)
# This way the hook code only needs changing in one place (eth2).

cd ${0:h}
PATHTOCLONE=$(pwd)
cd ..
DIRTOCLONE=${PATHTOCLONE:t}

# Mapping from arbitrary old-fashioned eth<N> to consistent physical p<slot>p<port> names
typeset -A CONSISTENT_NAME
CONSISTENT_NAME=( \
    0 e1 \
    1 e2 \
    2 p1p1 \
    3 p1p2 \
    4 p2p1 \
    5 p2p2 \
    )

if [[ $( print $DIRTOCLONE | grep -c 2 ) -ne 1 ]]; then
    print "This script needs to reside in the directory of the eth2 module hook. Exiting."
    exit 1
fi

autoload zmv
for N in 3 4 5; do
    CLONEDDIR=${DIRTOCLONE/2/${N}}
    echo "rm ${CLONEDDIR}/*(.N)"
    rm ${CLONEDDIR}/*(.N)
    echo "cp ${DIRTOCLONE}/*(.) ${CLONEDDIR}"
    cp ${DIRTOCLONE}/*(.) ${CLONEDDIR}
    echo "rm $CLONEDDIR/${0:t}*"
    rm $CLONEDDIR/${0:t}*
    echo "zmv ${CLONEDDIR}/'(*)2(*)' ${CLONEDDIR}/'${1}'${N}'${2}'"
    zmv ${CLONEDDIR}/'(*)2(*)' ${CLONEDDIR}/'${1}'${N}'${2}'
    echo "( cd $CLONEDDIR && sed -i -e 's/\(schar\|SCHAR\|eth\|ETH\)\([^[:space:]]*\)2/\1\2'${N}'/g' -e 's@\([^[:space:],()/=+-]*\)_2@\1_'${N}'@g' -e 's/232/23'${N}'/g' -e 's/"2"/"'${N}'"/g' -e 's/"schar 2/"schar '${N}'/g'  -e 's/"p1p1"/"'${CONSISTENT_NAME[${N}]}'"/g' eth_hook_*.[ch] Makefile )"
    ( cd $CLONEDDIR && sed -i -e 's/\(schar\|SCHAR\|eth\|ETH\)\([^[:space:]]*\)2/\1\2'${N}'/g' -e 's@\([^[:space:],()/=+-]*\)_2@\1_'${N}'@g' -e 's/232/23'${N}'/g' -e 's/"2"/"'${N}'"/g' -e 's/"schar 2/"schar '${N}'/g' -e 's/"p1p1"/"'${CONSISTENT_NAME[${N}]}'"/g' eth_hook_*.[ch] Makefile )
done
