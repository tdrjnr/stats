#!/usr/bin/ksh
#
# SCRIPT: operations_menu.ksh
# AUTHOR: Randy Michael
# DATE: 09-06-2001
# REV 2.0.P
#
# PLATFORM: Any Unix OS, with modifications
# 
# PURPOSE: This script gives the operations staff an easy-
#          to-follow menu to handle daily tasks, such
#          as managing the backup tapes and changing
#          their password
#
# REV LIST:
#
#
# set -n # Uncomment to check script syntax without any execution
# set -x # Uncomment to debug this script
#
###############################################
####### DEFINE FILES AND VARIABLES HERE #######
###############################################

BINDIR="/usr/local/bin"
PASSWORD_SERVER="yogi"
THIS_HOST=$(hostname)

###############################################
########## INITIALIZE VARIABLES HERE ##########
###############################################

MSG="                        "
OPT=" " # Variable for menu selection

###############################################
############## SET A TRAP HERE ################
###############################################

trap 'echo "\nEXITING on a TRAPPED SIGNAL\n"; \
      exit 1' 1 2 3 15

###############################################
############ BEGINNING OF MAIN ################
###############################################

# Loop until option 99 is Selected

# We use 99 as a character instead of an integer
# in case a user enters a non-integer selection,
# which would cause the script to fail.

while [[ $OPT != 99 ]]
do

   # Display a reverse video image bar across the top
   # of the screen with the hostname of the machine.

   clear        # Clear the screen first
   tput smso    # Turn on reverse video
   echo "                                  ${THIS_HOST}\c"
   echo "                                       "
   tput rmso    # Turn off reverse video
   echo "\n"    # Add one blank line of output

   # Show the menu options available to the user with the
   # numbered options highlighted in reverse video
   #
   # $(tput smso) Turns ON reverse video
   # $(tput rmso) Turns OFF reverse video

   echo "$(tput smso)1$(tput rmso) - Tape Management"
   echo "$(tput smso)2$(tput rmso) - Label Tapes"
   echo "$(tput smso)3$(tput rmso) - Query Volumes in Library"
   echo "$(tput smso)4$(tput rmso) - Query Tape Volumes"
   echo "$(tput smso)5$(tput rmso) - Audit/Check-in Scratch Volumes"
   echo "$(tput smso)6$(tput rmso) - Print Tape Volume Audit Report"
 
   echo "\n\n" # Print two new lines

   echo "$(tput smso)7$(tput rmso) - Change Password"

   echo "$(tput smso)8$(tput rmso) - Enable all Print Queues"


   echo "\n\n\n\n"

   echo "$(tput smso)99$(tput rmso) - Logout\n"

   # Draw a reverse video message bar across bottom of screen,
   # with the error message displayed, if there is a message.

   tput smso  # Turn on reverse video

   echo "                              ${MSG}\c"
   echo "                          "

   tput rmso  # Turn off reverse video

   # Prompt for menu option.

   echo "Selection: \c"
   read OPT

   # Assume the selection was invalid. Because a message is always
   # displayed we need to blank it out when a valid option 
   # is selected.

   MSG="Invalid option selected."

   # Process the Menu Selection

   case $OPT in
   1)
        # Option 1 - Tape Management

        ${BINDIR}/manage_tapes.ksh
        MSG="                        "
        ;;
   2)
        # Option 2 - Tape Labeling

        ${BINDIR}/label_tapes.ksh
        MSG="                        "
        ;;
   3)
        # Option 3 - Query Tape Volumes in Library

        dsmadmc -ID=admin -Password=pass query libvol
        print "Press ENTER to continue"
        read
        MSG="                        "
        ;;
   4)
        # Option 4 - Query Tape Volumes

        clear # Clear the screen
        print "Enter Tape Volume to Query:"
        read ANSWER
        dsmadmc -ID=admin -PAssword=pass query vol $ANSWER \
                 format=detailed
        if (($? == "11")) # Check for "Not Found"
        then
            print "Tape Volume $ANSWER not found in database."
            print "Press ENTER to continue."
            read
        fi
        MSG="                        "
        ;;
   5)
        # Option 5 - Audit/Checkin Scratch Volumes

#        dsmadmc -ID=admin -PAssword=pass audit library mainmount
#        dsmadmc -ID=admin -PAssword=pass checkin libvol mainmount\
#               status=scratch search=yes

        # Not for Operations anymore!!!

        MSG="  Option is disabled.   "
        ;;
   6)
        # Option 6 - Print Tape Volume Audit Report

        ${BINDIR}/print_audit_report.ksh
        MSG="                        "
        ;;
   7)
        # Option 7 - Change Password

        echo "Remote Shell into $PASSWORD_SERVER for Password Change"
        echo "Press ENTER to continue: \c"
        read KEY
        rsh $PASSWORD_SERVER passwd
        # ssh $PASSWORD_SERVER passwd
        MSG="                        "
        ;;
   8)
        # Option 8 - Enable all print queues

        echo "Attempting to Enable all print queues...\c"
        ${BINDIR}/enable_all_queues.ksh
        echo "\nQueue Enable Attempt Complete\n"
        sleep 1
        MSG="                        "
        ;;
   esac

   # End of Loop until 99 is selected

done

# Erase menu from screen upon exiting with the "clear" command

clear

# End of Script

