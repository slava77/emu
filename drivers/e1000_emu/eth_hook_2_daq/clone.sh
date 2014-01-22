#!/bin/zsh

# Clone this eth2 module hook for eth3, eth4 and eth5. (This will remove and recreate the directories for them!)
# This way the hook code only needs changing in one place (eth2).

cd ${0:h}
PATHTOCLONE=$(pwd)
cd ..
DIRTOCLONE=${PATHTOCLONE:t}

if [[ $( print $DIRTOCLONE | grep -c 2 ) -ne 1 ]]; then
    print "This script needs to reside in the directory of the eth2 module hook. Exiting."
    exit 1
fi

autoload zmv
for N in 3 4 5; do
    CLONEDDIR=${DIRTOCLONE/2/${N}}
    echo "mkdir -p ${CLONEDDIR}/{ddu,dmb}"
    mkdir -p ${CLONEDDIR}/{ddu,dmb}
    echo "rm ${CLONEDDIR}/{ddu,dmb}/*(.N)"
    rm ${CLONEDDIR}/{ddu,dmb}/*(.N)
    echo "rm ${CLONEDDIR}/*(.N)"
    rm ${CLONEDDIR}/*(.N)
    echo "cp ${DIRTOCLONE}/*(.) ${CLONEDDIR}"
    cp ${DIRTOCLONE}/*(.) ${CLONEDDIR}
    echo "cp ${DIRTOCLONE}/ddu/Makefile ${CLONEDDIR}/ddu"
    cp ${DIRTOCLONE}/ddu/Makefile ${CLONEDDIR}/ddu
    echo "cp ${DIRTOCLONE}/dmb/Makefile ${CLONEDDIR}/dmb"
    cp ${DIRTOCLONE}/dmb/Makefile ${CLONEDDIR}/dmb
    echo "rm $CLONEDDIR/${0:t}*"
    rm $CLONEDDIR/${0:t}*
    echo "zmv ${CLONEDDIR}/'(*)2(*)' ${CLONEDDIR}/'${1}'${N}'${2}'"
    zmv ${CLONEDDIR}/'(*)2(*)' ${CLONEDDIR}/'${1}'${N}'${2}'
    echo "( cd $CLONEDDIR && sed -i -e 's/\(schar\|SCHAR\|eth\|ETH\)\([^[:space:]]*\)2/\1\2'${N}'/g' -e 's@\([^[:space:],()/=+-]*\)_2@\1_'${N}'@g' -e 's/232/23'${N}'/g' -e 's/"2"/"'${N}'"/g' -e 's/"schar 2/"schar '${N}'/g' eth_hook_*.[ch] Makefile )"
    ( cd $CLONEDDIR && sed -i -e 's/\(schar\|SCHAR\|eth\|ETH\)\([^[:space:]]*\)2/\1\2'${N}'/g' -e 's@\([^[:space:],()/=+-]*\)_2@\1_'${N}'@g' -e 's/232/23'${N}'/g' -e 's/"2"/"'${N}'"/g' -e 's/"schar 2/"schar '${N}'/g' eth_hook_*.[ch] Makefile )
    echo "sed -i -e 's/\(schar\|SCHAR\|eth\|ETH\)\([^[:space:]]*\)2/\1\2'${N}'/g' -e 's@\([^[:space:],()/=+-]*\)_2@\1_'${N}'@g' -e 's/232/23'${N}'/g' -e 's/"2"/"'${N}'"/g' -e 's/"schar 2/"schar '${N}'/g' ${CLONEDDIR}/{ddu,dmb}/Makefile"
    sed -i -e 's/\(schar\|SCHAR\|eth\|ETH\)\([^[:space:]]*\)2/\1\2'${N}'/g' -e 's@\([^[:space:],()/=+-]*\)_2@\1_'${N}'@g' -e 's/232/23'${N}'/g' -e 's/"2"/"'${N}'"/g' -e 's/"schar 2/"schar '${N}'/g' ${CLONEDDIR}/{ddu,dmb}/Makefile
done
