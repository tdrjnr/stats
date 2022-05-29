#!/bin/bash

##############################RBS Output SCRIPT###########################

#MON - FRI
sed 's/,/ /g' hour-stats-data-15-wsi.txt | awk '{ print $3,$4,$5 }' > data; ./chart-graph-data-rbs; ./stats.pl

#sed 's/,/ /g' redhill*.csv | awk '{ print $1, $3, $4, $5 }' | egrep -e '05/12/2003' -e '0[8-9]/12/2003' -e '1[0-2]/12/2003' -e '1[5-9]/12/2003' | awk '/09:.*:/,/17:.*:/' > data; ./chart-graph; ./stats.pl

#MON - FRI and WEEKENDS
#sed 's/,/ /g' melv*.csv | awk '{ print $1,$3,$4,$5,$6 }' > data; ./chart-graph-data-rbs; ./stats.pl

#HOURS amn MINUTES
#sed 's/,/ /g' *Roth*.csv | awk '{ print $1, $7, $4, $5 }' | egrep -e '07/01/2004' > data; ./chart-graph-time; #./stats.pl

#sed 's/,/ /g' oxford*wan*.csv | awk '{ print $1,$3,$4,$5,$8 }' | egrep -e '30/04/2004' -e '0[3-7]/05/2004' -e '1[0-2]/05/2004' | awk '/08:.*:/,/18:.*:/' > data; ./chart-graph-data-rbs; ./stats.pl

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

