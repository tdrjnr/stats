#!/bin/ksh
#
# SCRIPT: broot
#
# AUTHOR: Randy Michael
# DATE: 05/08/2002
# REV: 1.0.P
# PLATFOEM: Any UNIX
#
# PURPOSE: This shell script is used to monitor all root access by
#          capturing all of the terminal data in a log file using
#          the script command. This shell script is executed from the
#          command line using sudo (Super User Do). The log file
#          is kept locally and e-mailed to a log file administrative
#          user either locally or on a remote machine. Sudo must be 
#          configured for this shell script. Refer to your sudo notes.
#
# USAGE:  sudo  broot
#
# REV LIST:
#
#
# set -n # Uncomment to check syntax without any execution 
# set -x # Uncomment to debug this shell script
#
############# DEFINE AUDIT LOG MANAGER ###################

# This user receives all of the audit logs by e-mail. This
# Log MAnager can have a local or remote e-mail address. You
# can add more than one e-mail address if you want by separating
# each address with a space.

LOG_MANAGER="logman"     # List to e-mail audit log

##########################################################
################ DEFINE FUNCTIONS HERE ###################
##########################################################

cleanup_exit ()
{
# This function is executed on any type of exit except of course
# a kill -9, which cannot be trapped. The script log file is
# e-mailed either locally or remotely and the log file is 
# compressed. The last "exit" is needed so the user does not
# have the ability to get to the command line without logging.

if [[ -s ${LOGDIR}/${LOGFILE} ]]
then
    case `uname` in
    Linux)  # Linux does not have "mailx"
    mail -s "$TS - $LOGNAME Audit Report" $LOG_MANAGER < ${LOGDIR}/${LOGFILE}
    ;;
    *)
    mailx -s "$TS - $LOGNAME Audit Report" $LOG_MANAGER < ${LOGDIR}/${LOGFILE}
    ;;
    esac

    nohup compress ${LOGDIR}/${LOGFILE} 2>/dev/null & 
fi

exit
}

# Set a trap

trap 'cleanup_exit' 1 2 3 5 15

##########################################################
################ DEFINE VARIABLES HERE ###################
##########################################################

TS=$(date +%m%d%y%H%M%S)           # File time stamp
THISHOST=$(hostname)               # Host name of this machine
LOGDIR=/usr/local/logs/script      # Directory to hold the logs
LOGFILE="${THISHOST}.${LOGNAME}.$TS" # Creates the name of the log file
touch "$LOGDIR/$LOGFILE"           # Creates the actual file
TMOUT=300                          # Set the root shell timeout!!!
export TMOUT                       # Export the TMOUT variable 
set -o vi                          # To recall previous commands
stty erase                       # Set the backspace key
set -o vi

# Run root's .profile if one exists

if [[ -f $HOME/.profile ]]
then
     . $HOME/.profile
fi

# set path to include /usr/local/bin
echo $PATH|grep -q ':/usr/local/bin' || PATH=$PATH:/usr/local/bin

# Set the command prompt to override the /.profile default prompt

PS1="$THISHOST:broot> "
export PS1

#################### RUN IT HERE ##########################


chmod 600 "${LOGDIR}/${LOGFILE}"   # Change permission to RW for the owner

script -a "${LOGDIR}/${LOGFILE}"   # Start the script monitoring session

chmod 400 "${LOGDIR}/${LOGFILE}"   # Set permission to read-only for the owner

cleanup_exit                       # Execute the cleanup and exit function

