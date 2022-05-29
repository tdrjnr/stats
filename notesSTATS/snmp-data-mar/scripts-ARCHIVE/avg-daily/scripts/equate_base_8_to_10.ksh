#!/usr/bin/ksh
#
# SCRIPT: equate_base_8_to_10.ksh
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
typeset -i BASE_8_NUM
typeset -i BASE_10_NUM

function usage
{
        echo "\nUSAGE: $SCRIPT_NAME base_8_number"
        echo "\nEXAMPLE: $SCRIPT_NAME 2774"
        echo "\nWill return the decimal base 10 number 1532 ...EXITING...\n"
}

if [ $# -ne 1 ]
then
        echo "\nERROR: A base 8 number must be supplied..."
        usage
        exit 1
fi

BASE_8_NUM="$1"

BASE_10_NUM=$((8#${BASE_8_NUM}))

echo $BASE_10_NUM
