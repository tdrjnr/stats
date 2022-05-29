#!/bin/bash
echo '########## CREATING DATA ##########';

CSV_FILE_LINK1='feltham-A-1Mb.csv'
CSV_FILE_LINK2='feltham-B-1Mb.csv'
FILE_LINK1='link-1.txt'
FILE_LINK2='link-2.txt'
ABUT_CMD='/usr/local/bin/abut'
COMBINE='combine.txt'
FILE='data.txt'
COMBINE_DATA='combine-data.txt'

echo '########## CREATING DATA for LINK 1 ##########'; 
#sed 's/,/ /g' feltham-A-1Mb.csv | egrep -e '0[1-5]/11/2004' -e '0[8-9]/11/2004' -e '1[0-2]/11/2004' -e '1[5-9]/11/2004' -e '2[2-6]/11/2004' -e '29/11/2004' -e '30/11/2004' > $FILE_LINK1; 

#echo '########## CREATING DATA for LINK 2 ##########'; 
#sed 's/,/ /g' feltham-B-1Mb.csv | egrep -e '0[2-6]/08/2004' -e '09/08/2004' -e '1[0-3]/08/2004' -e '1[6-9]/08/2004' -e '20/08/2004' -e '2[3-7]/08/2004' -e '3[0-1]/08/2004' -e '0[1-3]/09/2004' | awk '/10:.*:00/,/14:00:00/' > $FILE_LINK2;

echo '########## COMBINING DATA for LINK 1&2 ##########';
#abut $FILE_LINK1 $FILE_LINK2 > $COMBINE; awk '{ print $1,$2,$8,$3,$4,$9,$10,$11,$12 }' $COMBINE > $FILE; dm INPUT "x4 + x6" "x5 + x7" < $FILE > $COMBINE_DATA; awk '{ print $1,$2,$3,$10,$11,$8,$9 }' $COMBINE_DATA > $COMBINE; awk '{ print $1,$2,$3,$4,$5,$6,$7 }' $COMBINE > $FILE

#ADD_DATA_LINKS=$(awk '{ print $1,$2,$12,$13 }' $COMBINE_DATA | awk '{ print $1,$2,$4,$5,$6 }' $ADD_DATA_LINKS > $FILE)

echo '########## CREATING STATISTICS and GRAPH ##########'; 

STATS_FILE='stats.txt'
SUMMARY_FILE='feltham-FAILOVER-1Mb-stats.txt'
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

$AVG_IN_1; awk '{ print $4,$5 }' $FILE > $STATS_FILE; awk '{ print $1 }' $STATS_FILE | ./a.out > $SUMMARY_FILE; $AVG_OUT_1; awk '{ print $2 }' $STATS_FILE | ./a.out >> $SUMMARY_FILE; $AVG_IN_2; awk '{ print $3, $4 }' $FILE > $STATS_FILE; awk '{ print $1 }' $STATS_FILE | ./a.out >> $SUMMARY_FILE; $AVG_OUT_2; awk '{ print $2 }' $STATS_FILE | ./a.out >> $SUMMARY_FILE; $VALUES_IN; awk '{ print $1 }' $STATS_FILE | ./a.out | grep MAX >> $SUMMARY_FILE; $VALUES_OUT; awk '{ print $2 }' $STATS_FILE | ./a.out | grep MAX >> $SUMMARY_FILE; $HISTROGRAM_IN; awk '{ print $1 }' $STATS_FILE | ./a.out >> $SUMMARY_FILE; $HISTROGRAM_OUT; awk '{ print $2 }' $STATS_FILE | ./a.out >> $SUMMARY_FILE; $DISTRIBUTION_IN; awk '{ print $1 }' $STATS_FILE | ./a.out >> $SUMMARY_FILE; $DISTRIBUTION_OUT; awk '{ print $2 }' $STATS_FILE | ./a.out >> $SUMMARY_FILE; 
./chart-graph-data-rbs;

echo '########## FINISHED ##########'

<< END_SCRIPT

END_SCRIPT
exit 0