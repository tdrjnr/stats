#!/usr/bin/ksh
#
# SCRIPT: equate_base_2_to_10.ksh
#
#
#
#
#
#
#
#
# EXIT CODES:
#               0 - Normal script execution
#               1 - Usage error
#

# set -x # Uncomment to debug

SCRIPT_NAME=`basename $0`
typeset -i BASE_2_NUM
typeset -i BASE_10_NUM

function usage
{
        echo "\nUSAGE: $SCRIPT_NAME base_2_number"
        echo "\nEXAMPLE: $SCRIPT_NAME 1110"
        echo "\nWill return the decimal base 10 number 14 ...EXITING...\n"
}

if [ $# -ne 1 ]
then
        echo "\nERROR: A base 2 number must be supplied..."
        usage
        exit 1
fi

BASE_2_NUM="$1"

BASE_10_NUM=$((2#${BASE_2_NUM}))

echo $BASE_10_NUM
