#!/usr/bin/ksh
#
# SCRIPT: proc_wait.ksh
# AUTHOR: Randy Michael
# DATE: 02/14/2001
# REV: 1.1.A
# PLATFORM: Not Platform dependent
#
# PURPOSE: This script is used to wait for a process to start.
#     The process, specified by ARG 1 as passed to this script, should not
#     currently be running when this is started.  This script waits for the
#     process to start and exits.
#
# set -x # Uncomment to debug this script
# set -n # Uncomment to debug without any command execution
#
# Get the name of this shell script

SCRIPT_NAME=$(basename $0)

################ FUNCTIONS #############################
########################################################
function usage
{
     echo "\n\n"
     echo "USAGE: $SCRIPT_NAME {Process_to_monitor}"
     echo "\nEXAMPLE: $SCRIPT_NAME dtcalc\n"
     echo "Try again...EXITING...\n"
}
########################################################
function trap_exit
{
     echo "\nPROGRAM INTERRUPT...EXITING on trapped signal...\n"
}
########################################################


################ START OF MAIN##########################

if (( $# != 1 ))
then
     usage
     exit 1
else
     PROCESS="$1" # MUST Double Quote!! "  "
fi

# Set a trap...
trap 'trap_exit; exit 2' 1 2 3 15

# Check to execute or just exit...
ps -ef | grep -v "grep $PROCESS" | grep -v $SCRIPT_NAME | grep $PROCESS \
         > /dev/null

if (( $? == 0 ))
then
	echo "\n\nThe $PROCESS process is currently active...EXITING...\n"
	exit 1
fi

##### O.K. The process is NOT running, start monitoring for startup...

SLEEP_TIME="1"   # Seconds between monitoring Intervals
RC="1"		 # RC is the Return Code - Initialize to one, 1.
echo "\n\n"	 # Give a couple of blank lines to the display

# Give some user feedback...
echo "WAITING for $PROCESS to start...`date`\n"

until (( RC == 0 )) # Loop until the return code is zero
do
        # Check for the $PROCESS on each loop iteration

	ps -ef | grep $PROCESS | egrep -v "grep $PROCESS" \
               | grep -v $SCRIPT_NAME >/dev/null 2>&1

        # Check the Return Code!!!	
	if (( $? == 0 )) # Has it Started????
	then
		echo "$PROCESS has Started Execution...`date`\n\n"

                # Show the user what started!!

                ps -ef | grep $PROCESS | egrep -v "grep $PROCESS" \
                       | grep -v $SCRIPT_NAME

                echo "\n\n" # A Couple of Blank Lines Before Exit

		exit 0 # Exit time...
	fi
	sleep $SLEEP_TIME # Needed to reduce CPU load!! At least one second...
done 

# End of Script
