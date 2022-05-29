#!/bin/bash

echo '########## CREATING DATA ##########';

CSV_FILE='rotherham-redhill-combined-08am-18pm-10Mb.csv'
FILE='data.txt'

#DATA=$(sed 's/,/ /g' < $CSV_FILE | awk '{ print $1,$3,$9,$10,$11 }' | egrep -e '1[7-9]/11/2004' -e '2[2-6]/09/2004' | awk '/08:.*:.*/,/18:.*:.*/' > $FILE)

#DATA=$(sed 's/,/ /g' < $CSV_FILE | egrep -e '0[1-5]/11/2004' -e '0[8-9]/11/2004' -e '1[0-2]/11/2004' -e '1[5-9]/11/2004' -e '2[2-6]/11/2004' -e '29/11/2004' -e '30/11/2004' | awk '/08:.*:.*/,/18:.*:.*/' > $FILE)

DATA=$(sed 's/,/ /g' < $CSV_FILE | awk '{ print $1,$3,$9,$10,$11 }' | awk '/14:.*:00/,/15:.*:.*/' > $FILE) 

echo '########## CREATING STATISTICS and GRAPH ##########'; 

STATS_FILE='stats.txt'
SUMMARY_FILE='rotheram-redhill-combined-14pm-15pm-10Mb.txt'
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