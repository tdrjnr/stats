#!/usr/bin/ksh
#
# SCRIPT: equate_base_16_to_10.ksh
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

function usage
{
        echo "\nUSAGE: $SCRIPT_NAME base_16_number"
        echo "\nEXAMPLE: $SCRIPT_NAME FC23"
        echo "\nWill return the decimal base 10 number 64547 ...EXITING...\n"
}

if [ $# -ne 1 ]
then
        echo "\nERROR: A base 16 number must be supplied..."
        usage
        exit 1
fi

BASE_16_NUM="$1"

BASE_10_NUM=$((16#${BASE_16_NUM}))
echo $BASE_10_NUM
