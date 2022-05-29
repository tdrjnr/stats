#!/bin/bash
SUMMARY_FILE='feltham-A-08am-18pm-1Mb-stats.txt'
FILE='data.txt'

echo 'Burst Times' >> $SUMMARY_FILE; sed = $FILE | sed 'N; s/^/     /; s/ *\(.\{6,\}\)\n/\1   /' | egrep -e '12[4-7][0-9]   ' -e '13[8-9][0-9]   '  -e '15[0-3][0-9]   ' -e '19[7-9][0-9]   ' >> $SUMMARY_FILE

#echo 'Burst Times' >> $SUMMARY_FILE; sed = $FILE | sed 'N; s/^/     /; s/ *\(.\{6,\}\)\n/\1   /' | awk '/12[4-7].*   /'  >> $SUMMARY_FILE

<< END_SCRIPT

END_SCRIPT
exit 0