#!/bin/bash

TEST_RESULTS_DIR=/csc_data/results/EmuTests
#RUN_LIST=runs_list.js

echo "==> Generate Run Lists"

cd $TEST_RESULTS_DIR
TESTS=`ls -d Test_*`
#cd $TEST_RESULTS_DIR

for dir in $TESTS; do
	if [ -d $dir ]; then
		RUN_LIST=$dir/runs_list.js
		SUMMARY_RES=$dir/Summary.plots
		SUM_CSC_LIST=$SUMMARY_RES/csc_list.js
		SUM_RES_LIST=$SUMMARY_RES/test_results.js
		RUNS=`ls -drt $dir/csc*.plots`
		echo $dir

		# Create Summary Results Dir
		if [ ! -e $SUMMARY_RES ]; then 
			mkdir $SUMMARY_RES
		fi
		touch $SUMMARY_RES
		cp /dev/null > $SUM_RES_LIST

		
		# Generate Runs list for 
		echo "var RUNS = [" > $RUN_LIST
		ls -gGt --time-style=long-iso $dir/ |grep .plots | gawk '{ printf("['\''%s (%s %s)'\''],\n",$6, $5, $4) }' >> $RUN_LIST
		echo "];" >> $RUN_LIST
	
		for run_dir in $RUNS; do
			echo $(basename $run_dir)
			CHAMBERS=`ls -d $run_dir/ME*`
			for csc_dir in $CHAMBERS; do
				csc=$(basename $csc_dir)
				echo $csc
				if [ -L $SUMMARY_RES/$csc ]; then
					unlink $SUMMARY_RES/$csc
				fi
				ln -s $(pwd)/$csc_dir $SUMMARY_RES/$csc
			done
		done
		CHAMBERS=`ls -d $SUMMARY_RES/ME*`
		
		echo "var CSC_LIST=[" > $SUM_CSC_LIST
		echo "['$dir Summary',''," >> $SUM_CSC_LIST
		for csc in $CHAMBERS; do
			echo "'$(basename $csc)'," >> $SUM_CSC_LIST
			if [ -e $csc/test_results.js ]; then
				cat $csc/test_results.js >> $SUM_RES_LIST
			fi
		done
		echo "]]" >> $SUM_CSC_LIST
	fi
done

#echo "var RUNS = [" > $RUN_LIST
#ls -gGt --time-style=long-iso $HOME/results/gren/ |grep .plots | gawk '{ printf("['\''%s (%s %s)'\''],\n",$6, $5, $4) }' >> $RUN_LIST
#echo "];" >> $RUN_LIST


echo "==> Done."

