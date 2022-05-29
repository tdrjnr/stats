#!/usr/bin/ksh
#
# SCRIPT: equate_base_16_to_2.ksh
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
        echo "\nWill return the binary base 2 number 64547 ...EXITING...\n"
}

if [ $# -ne 1 ]
then
        echo "\nERROR: A base 16 number must be supplied..."
        usage
        exit 1
fi

# Check for a hexidecimal number!

case $1 in
+([-0-9]|[a-f]|[A-F])) BASE_16_NUM=$1
                     ;;
                  *)   usage
                     ;;
esac

BASE_2_NUM=$((16#${BASE_16_NUM}))
typeset -i2 BASE_2_NUM
echo $BASE_2_NUM
