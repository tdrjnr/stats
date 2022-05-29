#!/usr/local/bin/ksh
#
# SCRIPT: mk_large_file.ksh
# AUTHOR: Randy Michael
# DATE: 03/15/2002
# REV: 1.2.P
#
# PURPOSE: This script is used to create a text file that is
#          the exact size specified on the command line.
#
# set -n # Uncomment to check syntax without any execution
# set -x # Uncomment to debug this shell script
#
################################################
# Define functions here
################################################

function usage {

echo "...USAGE ERROR..."
echo "USAGE: $SCRIPT_NAME  number_of_80_char_lines"

}

################################################
# Check for the correct number is parameters
################################################

if (( $# != 1 ))
then
     usage
     exit 1
fi

################################################
# Define files and variables here
################################################

OUT_FILE=/var/snmp-data/snmp-data/avg-daily/scripts/bigfile  # File to create
>$OUT_FILE                 # Create a zero size file
SCRIPT_NAME=$(basename $0) # Extract the name of the script
TOTAL_LINES=$1             # Total number of lines to create
LINE_LENGTH=80             # Length of each line
LINE_COUNT=0               # Character counter
CHAR=X                     # Character to write to the file

################################################
#   BEGINNING of MAIN
################################################

while ((LINE_COUNT < TOTAL_LINES))
do
    CHAR_COUNT=0
    while ((CHAR_COUNT < LINE_LENGTH))
    do
         echo "${CHAR}\c" >> $OUT_FILE
         ((CHAR_COUNT = CHAR_COUNT + 1))
    done
    ((LINE_COUNT = LINE_COUNT + 1))
    echo>>$OUT_FILE
done
