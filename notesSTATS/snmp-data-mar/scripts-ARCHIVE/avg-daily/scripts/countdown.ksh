#!/bin/ksh

# SCRIPT: countdown.ksh
#
#
# PURPOSE:  This script will do the same thing as a sleep command
#	while giving the user feedback as to the number of seconds
#	remaining.  It takes input between 1 and 999 seconds only.
#
# DATE: 02-29-2000
# 
# AUTHOR: Randy Michael - Systems Administrator
#
#
if [ $# -ne 1 ];
then
	echo "USAGE:  countdown.ksh  [seconds]"
	exit 0
fi

if [ $1 -gt 0 -a $1 -lt 1000 ];
then
   S=$1
   echo "Seconds Remaining:  $S\c"
   /usr/bin/sleep 1
   while [ $S -gt 0 ];
   do
	if [ $S -lt "10" ];
	then
		echo "\b\b\c"
		echo "   \c"
		echo "\b\b\c"
        elif [ $S -ge "10" -a $S -lt "100" ];
	then
                echo "\b\b\b\c"
                echo "    \c"
                echo "\b\b\b\c"
        elif [ $S -ge "100" -a $S -lt "1000" ];
        then
                echo "\b\b\b\b\c"
                echo "     \c"
                echo "\b\b\b\b\c"
        fi

	((S = $S - 1))
	echo "$S\c"
	/usr/bin/sleep 1
   done
   echo ""
else
        echo "Invalid input ==> $1"
	echo "Range 1 - 999 seconds"
        echo "Exiting..."
        exit 0

fi
