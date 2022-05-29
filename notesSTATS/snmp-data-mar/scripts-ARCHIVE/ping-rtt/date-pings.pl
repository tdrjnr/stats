#!/bin/bash
RTT='rtt'
PSIZES='psizes'
Ping_Data=$(cat Harro* | grep round | sed "s/\// /g" | awk '{ print $6,$7,$8 }' > $RTT)
abut $PSIZES $RTT > data.tmp
sort -n data.tmp > data
./chart-graph-data-rtt



#!/bin/bash
#echo 10.189.94.10 > ping-1; sed 's/=/ /g' Gateshead-p1450 | sed 's/:/ /g' | sed 's/ms/ /g' | awk '{ print $7 }' >> ping-1; #echo 10.156.24.3 > ping-2; sed 's/=/ /g' pings2 | sed 's/:/ /g' | sed 's/ms/ /g' | awk '{ print $7 }' >> ping-2; abut ping-1 #ping-2 > ping-data; ./chart-graph-data-pings


#DATA=$(sed 's/,/ /g' < $CSV_FILE | awk '{ print $1,$3,$4,$5,$6 }' | egrep -e '1[8-9]/10/2004' -e '2[0-2]/10/2004' -e '2[5-#9]/10/2004' -e '0[3-5]/11/2004' -e '0[8-9]/11/2004' -e '1[0-2]/11/2004' -e '15/11/2004' | awk '/20:15:00/,/21:.*:.*/' > $FILE)



