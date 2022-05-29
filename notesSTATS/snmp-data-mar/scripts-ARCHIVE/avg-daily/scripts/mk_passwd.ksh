#!/usr/bin/ksh
#
# AUTHOR: Randy Micahel
# SCRIPT: mk_passwd.ksh
# DATE: 11/12/2001
# REV: 1.2.P
#
# PLATFORM: Not Platform Dependent
#
# PURPOSE: This script is used to create pseudo-random passwords.
#          An extrernal keyboard data file is utilized, which is 
#          defined by the KEYBOARD_FILE variable. This keyboard
#          file is expected to have one character on each line.
#          These characters are loaded into an array, and using
#          pseudo-random numbers generated, the characters are
#          "randomly" put together to form a string of characters.
#          By default, this script produces 8 character passwords,
#          but this length can be changed on the command line by
#          adding an integer value after the script name. There are
#          two command lineoptions, -n, which creates the default
#          KEYBOARD_FILE, and -m, which prints the manager's 
#          password report. This password report is intended 
#          to be lock in a safe for safe keeping.
#
# EXIT CODES:
#               0 - Normal script execution
#               1 - Usage error
#               2 - Trap exit
#               3 - Missing Keyboard data file
#
# REV LIST:
#          6/26/2002: Added two command line options, -n, which 
#          creates a new $KEYBOARD_FILE, and -m, which prints 
#          the managers password report.
#
# set -x # Uncomment to debug
# set -n # Uncomment to check syntax without any command execution
#
####################################################
########### DEFINE SOME VARIABLES HERE #############
####################################################

LENGTH=8 # Default Password Length

# Notification List for Printing the Manager's
# Password Report for Locking Away Passwords
# Just in Case You are Unavaliable.

NOTIFICATION_LIST="Donald Duck, Yogi Bear, and Mr. Ranger"

# Define the Default Printer for Printing the Manager's 
# Password Report. The user has a chance to change this 
# printer at execution time.

DEFAULT_PRINTER="hp4@yogi"

SCRIPT=$(basename $0)

OUTFILE="/tmp/tmppdw.file"

KEYBOARD_FILE=/scripts/keyboard.keys

PRINT_PASSWORD_MANAGER_REPORT="TO_BE_SET"

RANDOM=$$  # Initialize the random number seed to the
           # process IN (PID) of this shell script.

####################################################
########## DEFINE FUNCTIONS HERE ###################
####################################################

function in_range_random_number
{
# Create a pseudo-random number less than or equal
# to the $UPPER_LIMIT value, which is defined in the
# main body of the shell script.

RANDOM_NUMBER=$(($RANDOM % $UPPER_LIMIT + 1))

echo "$RANDOM_NUMBER"
}
#
####################################################
#
function load_default_keyboard
{
# If a keyboard data file does not exist then the user
# prompted to load the standard keyboard data into the 
# $KEYBOARD_FILE, which is defined in the main body of
# the shell script.

clear  # Clear the screen

echo "\nLoad the default keyboard data file? (Y/N): \c"
read REPLY

case $REPLY in
y|Y) :
     ;;
  *) echo "\nSkipping the load of the default keyboard file...\n"
     return
     ;;
esac

cat /dev/null > $KEYBOARD_FILE

echo "\nLoading the Default Keyboard File...\c"

# Loop through each character in the following list and
# append each character to the $KEYBOARD_FILE file. This
# produces a file with one character on each line.

for CHAR in \` 1 2 3 4 5 6 7 8 9 0 - = \\ q w e r t y u i o \
             p \[ \] a s d f g h j k l \; \' z x c v b n m \, \
             \. \/ \\ \~ \! \@ \# \$ \% \^ \& \* \( \) _ \+ \| \
             Q W E R T Y U I O P \{ \} A S D F G H J K L \: \" \
             Z X C V B N M \< \> \? \| \. 0 1 2 3 4 5 6 7 8 9 \/ \
             \* \- \+
do
     echo "$CHAR" >> $KEYBOARD_FILE
done
echo "\n\n\t...Done...\n"

sleep 1
}
#
####################################################
#
function check_for_and_create_keyboard_file
{
# If the $KEYBOARD_FILE does not exist then
# ask the user to load the "standard" keyboard
# layout, which is done with the load_default_keyboard
# function.

if [ ! -s $KEYBOARD_FILE ]
then
     echo "\n\nERROR: Missing Keyboard File"
     echo "\n\nWould You Like to Load the"
     echo "Default Keyboard Layout?"
     echo "\n\t(Y/N): \c"
     typeset -u REPLY=FALSE
     read REPLY
     if [ $REPLY != Y ]
     then
          echo "\n\nERROR: This shell script cannot operate"
          echo "without a keyboard data file located in"
          echo "\n==>  $KEYBOARD_FILE\n"
          echo "\nThis file expects one character per line."
          echo "\n\t...EXITING...\n"
          exit 3
     else
          load_default_keyboard
          echo "\nPress ENTER when you are you ready to continue: \c"
          read REPLY
          clear
     fi
fi
}
#
####################################################
#
function build_manager_password_report
{
# Build a file to print for the secure envelope
(
echo "\n                RESTRICTED USE!!!"
echo "\n\n\tImmediately send an e-mail to:\n"

echo "    $NOTIFICATION_LIST"

echo "\n\tif this password is revealed!"
echo "\n\tAIX root password:  $PW\n"

echo "\n\n"

echo "\n                RESTRICTED USE!!!"
echo "\n\n\tImmediately send an e-mail to:\n"

echo "    $NOTIFICATION_LIST"

echo "\n\tif this password is revealed!"
echo "\n\tAIX root password:  $PW\n"

echo "\n\n"

echo "\n                RESTRICTED USE!!!"
echo "\n\n\tImmediately send an e-mail to:\n"

echo "    $NOTIFICATION_LIST"

echo "\n\tif this password is revealed!"
echo "\n\tAIX root password:  $PW\n"

    ) > $OUTFILE

}
#
####################################################
#
function usage
{
echo "\nUSAGE: $SCRIPT [-m] [-n]  [password_length]\n"
echo "   Where:

     -m  Creates a password printout for Security

     -n  Loads the default keyboard data keys file

     password_length - Interger value that overrides
                       the default 8 character
                       password length.\n"
}
#
####################################################
#
function trap_exit
{
rm -f $OUTFILE >/dev/null 2>&1
}

####################################################
########## END OF FUNCTION DEFINITIONS #############
####################################################

####################################################
####### VALIDATE EACH COMMAND LINE ARGUMENT ########
####################################################
 
# Check command line arguments - $# < 3

if (($# > 3))
then
     usage
     exit 1
fi

####################################################
#
# Test for valid command line arguments - 
# Valid auguments are "-n, -N, -m, -M, and any integer

if (($# != 0))
then
   for CMD_ARG in $@
   do
         case $CMD_ARG in
         +([0-9]))
               # The '+([0-9]))' test notation is looking for
               # an integer. Any integer is assigned to the
               # length of password variable, LENGTH

               LENGTH=$CMD_ARG
               ;;
         -n) :
               ;;
         -N)  : 
               ;;
         -m)  :
               ;;
         -M)  :
               ;;
         *)
               usage
               exit 1
               ;;
         esac
   done
fi

####################################################
#
# Ensure that the $LENGTH variable is an integer

case $LENGTH in
+([0-9])) : # The '+([0-9]))' test notation is looking for
            # an integer. If an integer then the
            # No-Op, specified by a colon, (Do Nothing)
            # command is executed, otherwise this script
            # exits with a return code of 1, one.
          ;;
*) usage
   exit 1
   ;;
esac

####################################################
#
# Use the getopts function to parse the command
# line arguments. 

while getopts ":n N m M" ARGUMENT 2>/dev/null
do
     case $ARGUMENT in
     n|N)
         # Create a new Keyboard Data file
         load_default_keyboard
         echo "\nPress ENTER when you are you ready to continue: \c"
         read REPLY
         clear
         ;;
     m|M)
         # Print the Manager Password Report
         PRINT_PASSWORD_MANAGER_REPORT=TRUE
         ;;
     \?) # Show the usage message
         usage
         exit 1
     esac 
done

####################################################
################ START OF MAIN #####################
####################################################

# Set a trap

trap 'trap_exit;exit 2' 1 2 3 15

####################################################
#
# Check for a keyboard data file

check_for_and_create_keyboard_file

####################################################
############### LOAD THE ARRAY #####################
####################################################

X=0 # Initialize the array counter to zero

# Load the array called "KEYS" with keyboard elements
# located in the $KEYBOARD_FILE.

while read ARRAY_ELEMENT
do
     ((X = X + 1)) # Increment the counter by 1

     # Load an array element in the the array

     KEYS[$X]=$ARRAY_ELEMENT

done < $KEYBOARD_FILE


UPPER_LIMIT=$X  # Random Number Upper Limit

####################################################
#
# Produce the "for" loop list of elements that represent
# the length of the password: '1 2 3 4 5 6 7 8' is 
# the default "for" loop list.

FOR_COUNT=$(
X=0
while ((X < LENGTH))
do
    # Build the list here

    ((X = X + 1))
    echo "$X "
done
)

####################################################
#
# Create the pseudo-random password in this section

clear   # Clear the screen

PW=     # Initialize the password to NULL

# Build the password using random numbers to grab array
# elements from the KEYS array.

for i in $FOR_COUNT
do
       PW=${PW}${KEYS[$(in_range_random_number $UPPER_LIMIT)]}
done

# Done building the password

####################################################
#
# Display the new pseudo-random password to the screen

echo "\n\n     The new $LENGTH character password is:\n"
echo "\n          ${PW}\n"

####################################################
#
# Print the Manager's password report, if specified
# on the command with the -m command switch.

if [[ $PRINT_PASSWORD_MANAGER_REPORT = TRUE ]]
then

  typeset -u REPLY=N

  echo "\nPrint Password Sheet for the Secure Envelope? (Y/N)? \c"
  read REPLY

  if [[ $REPLY = 'Y' ]]
  then
     build_manager_password_report

     REPLY=    # Set REPLY to NULL

     echo "\nPrint to the Default Printer ${DEFAULT_PRINTER} (Y/N)? \c"
     read REPLY
     if [[ $REPLY = 'Y' ]]
     then
          echo "\nPrinting to $DEFAULT_PRINTER\n"
          lp -c -d $DEFAULT_PRINTER $OUTFILE
          
     else
          echo "\nNEW PRINT QUEUE: \c"
          read DEFAULT_PRINTER 
          echo "\nPrinting to $DEFAULT_PRINTER\n"
          lp -c -d $DEFAULT_PRINTER $OUTFILE
     fi
  else
     echo "\n\n\tO.K. - Printing Skipped..."
  fi
fi

####################################################
#
# Remove the $OUTFILE, if it exists and has a size
# greater than zero bytes.

[ -s $OUTFILE ] && rm $OUTFILE

####################################################
#
# Clear the screen and exit

echo "\n\nPress ENTER to Clear the Screen and EXIT: \c"
read X
clear

# End of mk_passwd.ksh shell script
