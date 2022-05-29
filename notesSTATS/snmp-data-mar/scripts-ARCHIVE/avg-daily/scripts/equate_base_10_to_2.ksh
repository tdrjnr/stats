#!/usr/bin/ksh
#
# SCRIPT: equate_base_10_to_2.ksh
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
typeset -i2 BASE_2_NUM
typeset -i BASE_10_NUM

function usage
{
        echo "\nUSAGE: $SCRIPT_NAME base_10_number"
        echo "\nEXAMPLE: $SCRIPT_NAME 14"
        echo "\nWill return the decimal base 2 number 1110 ...EXITING...\n"
}

if [ $# -ne 1 ]
then
        echo "\nERROR: A base 10 number must be supplied..."
        usage
        exit 1
fi

BASE_10_NUM="$1"

BASE_2_NUM=$((10#${BASE_10_NUM}))

echo $BASE_2_NUM | grep "#" >/dev/null 2>&1
if [ $? -eq 0 ]
then
	echo $BASE_2_NUM | cut -f2 -d "#"
else
	echo $BASE_2_NUM
fi
