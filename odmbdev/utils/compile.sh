#! /bin/bash

shopt -s nullglob

tmp_file="makefile_errors_and_warnings_temporary_collision_free_long_unambiguous_name.log"

make -j 4 -k -r -R 2> >(tee $tmp_file >&2)

echo

if [[ -s $tmp_file ]] ; then
    echo "ERRORS AND WARNINGS:"
    cat $tmp_file >&2
    rm -rf $tmp_file
    exit 1
else
    echo "Compiled successfully without errors or warnings!"
    rm -rf $tmp_file
    exit 0
fi

