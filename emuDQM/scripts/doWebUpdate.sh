#!/bin/bash

source $HOME/.bashrc

echo "***** Starting Monitoring job as $(whoami) at `date` *****"

is_running1=$HOME/logs/quick_web_update.is_running
is_running2=$HOME/logs/web_update.is_running
gateway=cmsusr2
user=$(whoami)

if [ -e $is_running1 ];then
        echo "WARN: Quick web updater job is locked or already running. Skipping..."
else
        touch $is_running1
        ssh -2 ${user}@${gateway} ~/data/cron_job_tf_quick.pl
        unlink $is_running1
fi

if [ -e $is_running2 ];then
        echo "WARN: Web updater job is locked or already running. Skipping..."
else
        touch $is_running2
        ssh -2 ${user}@${gateway} ~/data/cron_job_tf.pl
        unlink $is_running2
fi

echo "***** Finished Web Updater job at `date` in ${SECONDS} seconds *****"
