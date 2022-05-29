#!/usr/bin/ksh
#
# SCRIPT: equate_base_10_to_8.ksh
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
typeset -i8 BASE_8_NUM
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

BASE_10_NUM="$1"

BASE_8_NUM=$((10#${BASE_10_NUM}))
echo $BASE_8_NUM | grep "#" >/dev/null 2>&1
if [ $? -eq 0 ]
then
        echo $BASE_8_NUM | cut -f2 -d "#"
else
        echo $BASE_8_NUM
fi
