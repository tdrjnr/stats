#!/bin/ksh
# 
# SCRIPT: broadcast.ksh
# AUTHOR: Randy Michael
#         Systems Administrator
# DATE: 1/12/2000
# REV: 1.2.P
# PLATFORM: Not platform dependent but requires Samba
#
# PURPOSE: This script is used to broadcast a pop-up message to
# Windows desktops. The Windows machines must be defined in 
# the $WINNODEFILE file, which is where the master list of
# nodes is defined. The $WINNODELIST filename is defined in the
# variable definitions section of this shell script.
# 
# You also have the ability of setting up individual GROUPS of
# users/nodes by defining the group name to the GROUPLIST variable.
# Then define the filename of the group. For example, to define a
# Unix and DBA group the following entries need to be made in this
# shell script:
#
#    GROUPLIST="UNIX DBA"
#    UNIX="/scripts/UNIXlist"
#    DBA="/scripts/DBAlist"
#
# Assuming that the filenames presented above are acceptable to you.
#
# There are four options for sending a message:
# 
#    1) Execute this script without any argument prompts for the 
#       message to send and then send the message to all nodes
#       defined in the $WINNODEFILE.
#    2) Specify the "-M" switch if you want to send a message to a 
#       specific node or a list of nodes. The user is prompted for
#       the message to send.
#    3) Specify the -N or -H switches to specify the specific nodes
#       to receive the message. Add the node list after the -N or
#       -H switch.
#    4) Specify the -G switch, followed by the group name, that the
#       message is intended be sent.
#
#    EXAMPLES: 
#          To send a message to all nodes defined in the $WINNODEFILE:
#
#          # broadcast.ksh
#
#          To send a message to only the "booboo" and "yogi" machines:
#
#          # broadcast.ksh -H "booboo yogi"
#          OR
#          # broadcast.ksh -N "booboo yogi"
#
#          To send a message to specific machines without specifying
#          each one on the command line:
#
#          # broadcast.ksh -M 
#
#          To send a message to all users in the Unix and DBA 
#          groups only:
#
#          # broadcast.ksh -G "UNIX DBA"
#
#          Each switch is valid in uppercase or lowercase.
#
#   NOTE: This script uses SAMBA!!!  SAMBA must be installed 
#         and configured on this system for this shell script
#         to function!
#
# EXIT CODES:   0 ==> Normal Execution
#               1 ==> Usage Error
#               2 ==> Missing Node List File
#               3 ==> The "smbclient" program is not in the $PATH
#               4 ==> The "smbclient" program is not executable
#
# REV LIST:
#
#
# set -x # Uncomment to debug this script
# set -n # Uncomment to check syntax without any execution
#
############################################################
####### DEFINE BROADCAST GROUPS AND GROUP FILES HERE #######
############################################################

# Define the file directory for this shell script.

GRP_DIR="/usr/local/bin"

# Define all valid groups to send messages

GROUPLIST="UNIX SAP ORACLE DBA APPA APPB"

# Define all of the Group files

UNIX="${GRP_DIR}/Unixlist"
SAP="${GRP_DIR}/SAPlist"
ORACLE="${GRP_DIR}/ORACLElist"
DBA="${GRP_DIR}/DBAlist"
APPA="${GRP_DIR}/APPAlist"
APPB="${GRP_DIR}/APPBlist"

# File that contains the master list of nodes

WINNODEFILE="${GRP_DIR}/WINlist" 

############################################################
################# DEFINE FUNCTIONS HERE ####################
############################################################

function display_listfile_error
{
# The function is used to inform the users that the
# $WINNODEFILE file does not exist. The $WINNODEFILE
# filename is defined in the main body of the shell script.

echo "\n\tERROR: ...MISSING NODE LIST FILE..."
echo "\nCannot find the $WINNODEFILE node list file!"
echo "\nThe $WINNODEFILE file is a file that contains a list of"
echo "nodes to broadcast a message. Create this file with"
echo "one node name per line and save the file.\n\n"

exit 2
}

############################################################

function usage
{
echo "\nUSAGE: $THISSCRIPT [-M] [-H Host List] [-N Node List] \
[-G Group List]\n\n"
echo "EXAMPLES:"
echo "\nTo send a message to all nodes defined in the master list"
echo "$WINNODEFILE file enter the scriptname without any options:"
echo "\n$THISSCRIPT"
echo "\nTo send a message to one or more nodes only,"
echo "enter the following command:"
echo "\n$THISSCRIPT -M"
echo "\nTo specify the nodes to send the message to on"
echo "the command-line enter the following command:"
echo "\n$THISSCRIPT -H \"yogi booboo dino\" "
echo "\nTo send a message to one or more groups use the"
echo "following command syntax:"
echo "\n$THISSCRIPT -G \"UNIX DBA\" \n\n"
}

############################################################ 

function check_for_null_message
{
if [[ -z "$MESSAGE" && "$MESSAGE" = "" ]]
then
     return 1
else
     return 0
fi
}

############################################################

function check_for_null_winlist
{
if [[ -z "$WINLIST" && "$WINLIST" = "" ]]
then
     return 1
else
     return 0
fi
}

############################################################

function group_error
{
(($# != 1)) && (echo "ERROR: function group_error expects \
an argument"; exit 1)
GRP=$1
echo "\nERROR: Undefined Group - $GRP"
usage
exit 1
}

############################################################

function group_file_error
{
(($# != 1)) && (echo "ERROR: function group_file_error expects \
an argument"; exit 1)

GPF=$1
echo "\nERROR: Missing group file - $GPF\n"
usage
exit 1
}

############################################################

function check_for_smbclient_command
{
# Check to ensure that the "smbclient" command is in the $PATH

SMBCLIENT=$(which smbclient)

# If the $SMBCLIENT variable begins with "which:" or "no" for
# Solaris and HP-UX then the command is not in the $PATH on
# this system. A correct result would be something like:
# "/usr/local/bin/smbclient" or "/usr/bin/smbclient".

if [[ $(echo $SMBCLIENT | awk '{print $1}') = 'which:' ]] || \
   [[ $(echo $SMBCLIENT | awk '{print $1}') = 'no' ]]
then
     echo "\n\nERROR: This script requires Samba to be installed
and configure. Specifically, this script requires that the
\"sbmclient\" program is in the \$PATH. Please correct this problem
and send your message again.\n"

     echo "\n\t...EXITING...\n"

     exit 3
elif [  ! -x $SMBCLIENT ]
then
     echo "\nERROR: $SMBCLIENT command is not executable\n"
     echo "Please correct this problem and try again\n"
     exit 4
fi
}

############################################################
################ DEFINE VARIABLES HERE #####################
############################################################

THISSCRIPT=$(basename $0) # The name of this shell script

MESSAGE=   # Initialize the MESSAGE variable to NULL

WINLIST=   # Initialize the list of node to NULL

############################################################
################ TEST USER INPUT HERE ######################
############################################################

# Check for the "smbclient" command's existence

check_for_smbclient_command

# If no command-line arguments are present then test for
# the master $WINNODEFILE, which is defined at the top
# of this shell script.

if (($# == 0)) # No command-line arguments - Use the master list
then
     [ -s $WINNODEFILE ] || display_listfile_error

     # Load the file data into the WINLIST variable ignoring
     # any line in the file that begins with a # sign.

     WINLIST=$(cat $WINNODEFILE | grep -v ^# \
              | awk '{print $1}' | uniq)
else
   # Parse the command-line arguments for any switches. A command
   # line switch must begin with a hyphen (-).

   # A colon (:) AFTER a variable (below) means that the switch
   # must have a switch-argument on the command line

   while getopts ":mMh:H:n:N:g:G:" ARGUMENT
   do
     case $ARGUMENT in
       
         m|M)  echo "\nEnter One or More Nodes to Send This Message:"
               echo "\nPress ENTER when finished \n\n"
               echo "Node List ==> \c"
               read WINLIST
               ;;
         h|H|n|N)  WINLIST=$OPTARG
               ;;
         g|G)  GROUP=$OPTARG # $OPTARG is the value of
                             # the switch-argument!

               # Make sure that the group has been defined
               for G in $GROUP
               do
                    echo "$GROUPLIST" | grep -q $G || group_error $G
               done
               # All of the groups are valid if you get here! 
               WINLIST=          # NULL out the WINLIST variable
               # Loop through each group in the $GROUP
               # and build a list of nodes to send the message to.

               for GRP in $GROUP
               do
                    # Use "eval" to show what a variable is pointing to!
                    # Make sure that each group has a non-empty list
                    # file

                    if [ -s $(eval echo \$"$GRP") ]
                    then
                         WINLIST="$WINLIST $(eval cat \$"$GRP" \
                                  | grep -v ^# | awk '{print $1}' \
                                  | uniq)"
                    else
                         group_file_error $(eval echo \$"$GRP")
                    fi
               done
               ;;
          \?)  echo "\nERROR: Invalid Augument(s)!"
               usage
               exit 1
               ;;
      esac
   done

   ############################################################
   ################## BEGINNING OF MAIN #######################
   ############################################################

   # Ensure that at least one node is defined to send the message.
   # If not stay in this loop until one or more nodes are entered
   # on the command line

   until check_for_null_winlist
   do
     echo "\n\nEnter One or More Nodes to Send This Message:"
     echo "\n Press ENTER when finished \n\n"
     echo "Node List ==> \c"
     read  WINLIST
   done

   ############################################################


fi  # End of "if (($# == 0))" test.

# Prompt the user for a message to send. Loop until the 
# user has entered at least one character for the message 
# to send.

until check_for_null_message
do
    echo "\nEnter the message to send:"
    echo "\nPress ENTER when finished\n\n"
    echo "Message ==> \c"
    read MESSAGE
done

############################################################

# Inform the user of the host list this message is sent to...

echo "\nSending message to the following hosts:\n"
echo "\nWIN_HOSTS:\n$WINLIST\n\n"

############################################################

echo "\nSending the Message...\n"

# Loop through each host in the $WINLIST and send the pop-up message

for NODE in $WINLIST
do
      echo "Sending to ==> $NODE"
      echo $MESSAGE | $SMBCLIENT -M $NODE # 1>/dev/null
      if (($? == 0))
      then
           echo "Sent OK    ==> $NODE"
      else
           echo "FAILED to  ==> $NODE Failed"
      fi
done

echo "\n"

############################################################
#
# This code segment is commented out by default
#
# Send the message to the Unix machines too using "wall"
# and "rwall" if you desire to do so. This code is commented
# out by default.
#
# echo "\nSending Message to the Unix machines...\n"
#
# echo $MESSAGE | rwall -h $UnixHOSTLIST
# echo $MESSAGE | wall 
# echo "\n\nMessage sent...\n\n"
#
############################################################

# Remove the message file from the system

rm -f $ 

