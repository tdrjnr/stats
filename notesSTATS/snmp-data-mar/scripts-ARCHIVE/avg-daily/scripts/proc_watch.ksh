#!/bin/ksh
#
# SCRIPT: proc_watch.ksh
# AUTHOR: Randy Michael
# DATE: 09-12-2001
# REV: 1.0.P
# PLATFORM: Not Platform Dependent
#
# PURPOSE" This script is used to monitoring and log 
#          the status of a process as it starts and stops.
#
# REV LIST:
#
# set -x # Uncomment to debug this script
# set -n # Uncomment to check syntax without ANY execution
#
####################################################
########## DEFINE FILES AND VARIABLES HERE #########
####################################################

LOGFILE="/tmp/proc_status.log"
[[ ! -s $LOGFILE ]] && touch $LOGFILE

PROCESS="$1"
SCRIPT_NAME=$(basename $0)
TTY=$(tty)

####################################################
############# DEFINE FUNCTIONS HERE ################
####################################################

usage ()
{
     echo "\nUSAGE:  $SCRIPT_NAME  process_to_monitor\n"
}

####################################################

trap_exit ()
{
    # Log an ending time for process monitoring
    TIMESTAMP=$(date +%D@%T) # Get a new time stamp...
    echo "MON_STOP: Monitoring for $PROCESS ended ==> $TIMESTAMP" \
         | tee -a $LOGFILE

    # Kill all functions
    kill -9 $(jobs -p) 2>/dev/null
}

####################################################

mon_proc_end ()
{
     END_RC="0"
     until (( END_RC != 0 ))
     do
          ps -ef | grep -v "grep $PROCESS" | grep -v $SCRIPT_NAME \
                 | grep $PROCESS >/dev/null 2>&1

          END_RC=$? # Check the Return Code!!
          sleep 1   # Needed to reduce CPU load!
     done

     print 'N' # Turn the RUN flag off

     # Grab a TimeStamp
     TIMESTAMP=$(date +%D@%T)

     echo "END PROCESS: $PROCESS ended ==> $TIMESTAMP" >> $LOGFILE &
     echo "END PROCESS: $PROCESS ended ==> $TIMESTAMP" > $TTY
}

####################################################

mon_proc_start ()
{
     START_RC="-1"
     until (( START_RC == 0 ))
     do
          ps -ef | grep -v "grep $PROCESS" | grep -v $SCRIPT_NAME \
                 | grep $PROCESS >/dev/null 2>&1

          START_RC=$? # Check the Return Code!!!
          sleep 1     # Needed to reduce CPU load!
     done

     print 'Y' # Turn the RUN flag on

     # Grab the Timestamp
     TIMESTAMP=$(date +%D@%T)

     echo "START PROCESS: $PROCESS began ==> $TIMESTAMP" >> $LOGFILE &
     echo "START PROCESS: $PROCESS began ==> $TIMESTAMP" > $TTY
}

####################################################
############## START OF MAIN #######################
####################################################

### SET A TRAP ####

trap 'trap_exit; exit 0' 1 2 3 15

# Check for the Correct Command Line Argument - Only 1

if (( $# != 1 ))
then
     usage
     exit 1
fi

# Get an Initial Process State and Set the RUN Flag

ps -ef | grep -v "grep $PROCESS" | grep -v $SCRIPT_NAME \
       | grep $PROCESS >/dev/null

PROC_RC=$?  # Check the Return Code!!

# Give some initial feedback before starting the loop

if (( PROC_RC == 0 ))
then
     echo "The $PROCESS process is currently running...Monitoring..."
     RUN="Y"      # Set the RUN Flag to YES
else
     echo "The $PROCESS process is not currently running...Monitoring..."
     RUN="N"      # Set the RUN Flag to NO
fi

TIMESTAMP=$(date +%D@%T) # Greb a timestamp for the log

# Use a "tee -a $#LOGFILE" to send output to both standard output
# and to the file referenced by $LOGFILE

echo "MON_START: Monitoring for $PROCESS began ==> $TIMESTAMP" \
      | tee -a $LOGFILE 

# Loop Forever!!

while :
do
     case $RUN in
     'Y') # Loop Until the Process Ends
          RUN=$(mon_proc_end)
          ;;
     'N') # Loop Until the Process Starts
          RUN=$(mon_proc_start)
          ;;
     esac
done

# End of Script
