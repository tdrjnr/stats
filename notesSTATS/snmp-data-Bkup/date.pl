#!/bin/bash

##############################RBS Output SCRIPT###########################

#MON - FRI
echo '########## CREATING DATA ##########'; sed 's/,/ /g' RHIBGP1-to-MGIBGP1-60Mbps.csv | awk '{ print $1,$2,$3,$4,$7 }' | egrep -e '2[7-9]/09/2004' -e '30/09/2004' -e '01/10/2004' -e '0[4-8]/10/2004' | awk '/08:.*:00/,/18:00:00/' > data; echo '########## CREATING STATISTICS and GRAPH ##########'; ./chart-graph-data-rbs; ./stats.pl; echo '########## FINISHED ##########';

#echo '########## CREATING DATA ##########'; sed 's/,/ /g' RHIBGP1-to-MGIBGP1-60Mbps.csv > data; echo '########## CREATING STATISTICS and GRAPH ##########'; ./chart-graph-data-rbs; ./stats.pl; echo '########## FINISHED ##########';

#MON - FRI and WEEKENDS
#sed 's/,/ /g' melv*.csv | awk '{ print $1,$3,$4,$5,$6 }' > data; ./chart-graph-data-rbs; ./stats.pl

#HOURS amn MINUTES
#sed 's/,/ /g' *Roth*.csv | awk '{ print $1, $7, $4, $5 }' | egrep -e '07/01/2004' > data; ./chart-graph-time; #./stats.pl

#sed 's/,/ /g' oxford*wan*.csv | awk '{ print $1,$3,$4,$5,$8 }' | egrep -e '30/04/2004' -e '0[3-7]/05/2004' -e '1[0-2]/05/2004' | awk '/08:.*:/,/18:.*:/' > data; ./chart-graph-data-rbs; ./stats.pl

                 #'/08:.*:00/,/18:00:00/'

#WEEKENDS
#sed 's/,/ /g' rhibgp1*bgp2*.csv | awk '{ print $1,$3,$4,$5,$6 }' | egrep -e '05/06/2004' -e '12/06/2004' | awk '/10:.*:/,/15:.*:/' > data; ./chart-graph-data-rbs; ./stats.pl

########################################################
#BT Output

#MON - FRI and PEAK TIMES
#sed 's/,/ /g' Cambr*.csv | awk '{ print $1, $2, $7, $8 }' | egrep -e '1[6-9]/12/2003' -e '2[2-6]/12/2003' -e '29/12/2003' -e '3[0-1]/12/2003' -e '[1-2]/01/2004' -e '[5-9]/01/2004' -e '12/01/2004' | awk '/18:.*:/,/19:.*:/' > data; ./chart-graph-data-bt; ./stats.pl

#######################BT SCRIPT############################
#sed 's/,/ /g' *Edin*A*.csv | awk '{ print $1, $3, $7, $8, $9, $10 }' | egrep -e '23/01/2004' -e '2[6-9]/01/2004' -e '30/01/2004' -e '0[2-6]/02/2004' -e '09/02/2004' -e '1[0-3]/02/2004' -e '1[6-9]/02/2004' | awk '/08:.*:/,/17:.*:/' > data; awk '{ print $3, $4, $5 }' data | dm INPUT "8*x1/1000" "8*x2/1000" "8*x3/1000" | awk '{ print $4, $5, $6 }' > data-bits; awk '{ print $1, $2, $6 }' data > time; abut time data-bits | awk '{printf "%s %s   %s   %s   %s   %s\n",$1,$2,$3,$4,$5,$6}' > graph-data; ./chart-graph-data-bt; ./stats.pl

#WITHOUT LATENCY
#sed 's/,/ /g' Camb*.csv | awk '{ print $1, $2, $7, $8, $9 }' | egrep -e '29/06/2004' -e '30/06/2004' -e '30/01/2004' -e '0[1-2]/07/2004' -e '0[5-9]/07/2004' -e '1[2-6]/07/2004' -e '19/07/2004' -e '2[0-3]/07/2004' -e '26/07/2004' | awk '/08:.*/,/17:.*/' > data; awk '{ print $3, $4, $5 }' data | dm INPUT "8*x1/1000" "8*x2/1000" "8*x3/1000" | awk '{ print $4, $5, $6 }' > data-bits; awk '{ print $1, $2, $6 }' data > time; abut time data-bits | awk '{printf "%s %s   %s   %s   %s\n",$1,$2,$3,$4,$5}' > graph-data; #./chart-graph-data-bt; ./stats.pl



######################BT SCRIPT######################

#!/bin/bash

CSV_FILE='RHIBGP1-to-MGIBGP1-60Mbps.csv'
FILE='data.txt'
STATS_FILE='stats.txt'
SUMMARY_FILE='stats.txt'
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

#MON - FRI
echo '########## CREATING DATA ##########'; sed 's/,/ /g' $CSV_FILE | awk '{ print $1,$2,$3,$4,$7 }' | egrep -e '2[7-9]/09/2004' -e '30/09/2004' -e '01/10/2004' -e '0[4-8]/10/2004' | awk '/08:.*:00/,/18:00:00/' > $FILE; echo '########## CREATING STATISTICS and GRAPH ##########';

$AVG_IN_1; awk '{ print $3, $4 }' $FILE > $STATS_FILE; awk '{ print $1 }' $STATS_FILE | ./a.out > $SUMMARY_FILE; $AVG_OUT_1; awk '{ print $2 }' $STATS_FILE | ./a.out >> $SUMMARY_FILE; $AVG_IN_2; awk '{ print $3, $4 }' data > $STATS_FILE; awk '{ print $1 }' $STATS_FILE | ./a.out >> $SUMMARY_FILE; $AVG_OUT_2; awk '{ print $2 }' $STATS_FILE | ./a.out >> $SUMMARY_FILE; $VALUES_IN; awk '{ print $1 }' $STATS_FILE | ./a.out | grep MAX >> $SUMMARY_FILE; $VALUES_OUT; awk '{ print $2 }' $STATS_FILE | ./a.out | grep MAX >> $SUMMARY_FILE; $HISTROGRAM_IN; awk '{ print $1 }' $STATS_FILE | ./a.out >> $SUMMARY_FILE; $HISTROGRAM_OUT; awk '{ print $2 }' $STATS_FILE | ./a.out >> $SUMMARY_FILE; $DISTRIBUTION_IN; awk '{ print $1 }' $STATS_FILE | ./a.out >> $SUMMARY_FILE; $DISTRIBUTION_OUT; awk '{ print $2 }' $STATS_FILE | ./a.out >> $SUMMARY_FILE; echo '########## FINISHED ##########' << END_SCRIPT

END_SCRIPT
exit 0