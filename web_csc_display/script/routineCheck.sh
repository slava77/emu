# kill the previous process
kill -9 $(ps aux | grep "routineCheck.sh" | grep -v "grep" | awk '{print $2}')

# check wheter the instance of thread exsits
while [ 1 ] ; do
        echo 'begin checking...'
        # check_process "startCSCDisp.sh" # the thread name
        # PROCESS_NUM => get the process number regarding the given thread name
        PROCESS_NUM=$(ps -ef | grep "startCSCDisp.sh" | grep -v "grep" | wc -l)
        # for degbuging...
        echo $PROCESS_NUM
        if [ $PROCESS_NUM -eq 1 ];
        then
            echo "Process is up"
            # echo "The process is down, restart the process"
            # ./script/kill-run-and-tail > output.txt &
        else 
            echo "The process is down, restart the process"
            ./script/kill-run-and-tail > output.txt &
        fi
        
        
        # CHECK_RET = $?
        # if [ $CHECK_RET -eq 0 ]; # none exist
        # then
                # # do something...
                # ./script/kill-run-and-tail
        # fi
        
        sleep 60
done