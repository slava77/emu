#!/bin/bash

shopt -s nullglob

current_path=`pwd -P`
if [[ $current_path == *TriDAS* ]]; then
    export BUILD_HOME=`echo $current_path | sed -r 's|(TriDAS).*$|\1|'`
elif [[ $current_path == *local/data/odmb_ucsb* ]]; then
    export BUILD_HOME=/home/cscme11/TriDAS
else
    export BUILD_HOME=`dirname ${current_path}`/TriDAS
fi

export XDAQ_ROOT=/opt/xdaq
export XDAQ_OS=linux
export XDAQ_PLATFORM=x86_64_slc5

tmp_file_build="makefile_errors_and_warnings_temporary_collision_free_long_unambiguous_name_build.log"
tmp_file_install="makefile_errors_and_warnings_temporary_collision_free_long_unambiguous_name_install.log"

rm -f core.*
make -j -k 2> >(tee $tmp_file_build >&2)
make -j -k install 2> >(tee $tmp_file_install >&2)

echo

if [[ -s $tmp_file_build || -s $tmp_file_install ]] ; then
    echo
    echo "ERRORS AND WARNINGS:"
    if [[ -s $tmp_file_build ]] ; then
	cat $tmp_file_build >&2
    fi
    if [[ -s $tmp_file_install ]] ; then
	cat $tmp_file_install >&2
    fi
    rm -rf $tmp_file_build
    rm -rf $tmp_file_install
    echo
    exit 1
else
    echo
    echo "Compiled successfully without errors or warnings!"
    echo
    rm -rf $tmp_file_build
    rm -rf $tmp_file_install
    exit 0
fi
