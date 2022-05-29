#!/bin/bash
echo '########## CREATING DATA ##########';

CSV_FILE_LINK1='RGHIBGP1-Gateshead-to-MGIBGP2-14Mbps.csv'
CSV_FILE_LINK2='RHGIBGP1-14Mbps-PVC-Harrogate-to-FRCIBGP2.csv'
FILE_LINK1='link-1.txt'
FILE_LINK2='link-2.txt'
ABUT_CMD='/usr/local/bin/abut'
DM_CMD='/usr/local/bin/dm INPUT "x4 + x6" "x5 + x7"'
COMBINE='combine.txt'
FILE='data.txt'
COMBINE_DATA='combine-data.txt'
#echo '########## CREATING DATA for LINK 1 ##########'
DATA_LINK1=$(sed 's/,/ /g' < $CSV_FILE_LINK1 | awk '{ print $1,$3,$4,$5,$6 }' | egrep -e '1[8-9]/10/2004' -e '2[0-2]/10/2004' -e '2[5-9]/10/2004' -e '0[3-5]/11/2004' -e '0[8-9]/11/2004' -e '1[0-2]/11/2004' -e '15/11/2004' | awk '/18:.*:00/,/19:.*:.*/'  > $FILE_LINK1)

#echo '########## CREATING DATA for LINK 2 ##########'; 
DATA_LINK2=$(sed 's/,/ /g' < $CSV_FILE_LINK2 | awk '{ print $1,$3,$4,$5,$6 }' | egrep -e '1[8-9]/10/2004' -e '2[0-2]/10/2004' -e '2[5-9]/10/2004' -e '0[3-5]/11/2004' -e '0[8-9]/11/2004' -e '1[0-2]/11/2004' -e '15/11/2004' | awk '/18:.*:00/,/19:.*:.*/'  > $FILE_LINK2)

echo '########## MANIPULATING DATA for LINK 1&2 ##########'; 
COMBINE_LINKS=$($ABUT_CMD link-1.txt link-2.txt > $COMBINE)
COMBINE_DATA_LINKS= $(awk '{ print $1,$2,$7,$3,$4,$8,$9,$10 }' $COMBINE > $FILE)
MANIPULATE_LINKS=$(dm INPUT "x4 + x6" "x5 + x7" < $FILE > $COMBINE_DATA) 
ADD_DATA_LINKS=$(awk '{ print $1,$2,$3,$9,$10,$8 }' $COMBINE_DATA | awk '{ print $1,$2,$4,$5,$6 }' $ADD_DATA_LINKS > $FILE)

echo '########## CREATING STATISTICS and GRAPH ##########'; 

STATS_FILE='stats.txt'
SUMMARY_FILE='Harrogate-Gateshead-FAILOVER-18pm-19pm-14Mbps-stats.txt'
AVG_IN_1='gcc -lm avg-per-in.c'
AVG_OUT_1='gcc -lm avg-per-out.c'
AVG_IN_2='gcc -lm avg-per-in-2.c'
AVG_OUT_2='gcc -lm avg-per-out-2.c'
VALUES_IN='gcc -lm values-in.c'
VALUES_OUT='gcc -lm values-out.c'
HISTROGRAM_IN='gcc -lm histrogram-in.c'
HISTROGRAM_OUT='gcc -lm histrogram-out.c'
DISTRIBUTION_IN='gcc -lm distribution-in.c'
DISTRIBUTION_OUT='gcc -lm distribution-out.c'

$AVG_IN_1; awk '{ print $3, $4 }' $FILE > $STATS_FILE; awk '{ print $1 }' $STATS_FILE | ./a.out > $SUMMARY_FILE; $AVG_OUT_1; awk '{ print $2 }' $STATS_FILE | ./a.out >> $SUMMARY_FILE; $AVG_IN_2; awk '{ print $3, $4 }' $FILE > $STATS_FILE; awk '{ print $1 }' $STATS_FILE | ./a.out >> $SUMMARY_FILE; $AVG_OUT_2; awk '{ print $2 }' $STATS_FILE | ./a.out >> $SUMMARY_FILE; $VALUES_IN; awk '{ print $1 }' $STATS_FILE | ./a.out | grep MAX >> $SUMMARY_FILE; $VALUES_OUT; awk '{ print $2 }' $STATS_FILE | ./a.out | grep MAX >> $SUMMARY_FILE; $HISTROGRAM_IN; awk '{ print $1 }' $STATS_FILE | ./a.out >> $SUMMARY_FILE; $HISTROGRAM_OUT; awk '{ print $2 }' $STATS_FILE | ./a.out >> $SUMMARY_FILE; $DISTRIBUTION_IN; awk '{ print $1 }' $STATS_FILE | ./a.out >> $SUMMARY_FILE; $DISTRIBUTION_OUT; awk '{ print $2 }' $STATS_FILE | ./a.out >> $SUMMARY_FILE; 
./chart-graph-data-rbs;

echo '########## FINISHED ##########'

<< END_SCRIPT

END_SCRIPT
exit 0