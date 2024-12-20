#!/bin/bash

#RBS Output
#gcc -lm avg-per-in.c; awk '{ print $7, $8 }' dates | sed 's/,/ /g' > data; awk '{ print $1 }' data | ./a.out > brighton-b-stats; gcc -lm avg-per-out.c; awk '{ print $2 }' data  | ./a.out >> brighton-b-stats

#gcc -lm avg-per-in.c; awk '{ print $3, $4 }' data* > stats; awk '{ print $1 }' stats | ./a.out > Telford-01-mon-fri-9am-17pm-stats; gcc -lm avg-per-out.c; awk '{ print $2 }' stats | ./a.out >> Telford-01-mon-fri-9am-17pm-stats

#RBS OUTPUT
gcc -lm avg-per-in.c; awk '{ print $1, $2 }' data > stats; awk '{ print $1 }' stats | ./a.out > hour-stats-data-15-wsi-60MB-stats; gcc -lm avg-per-out.c; awk '{ print $2 }' stats | ./a.out >> hour-stats-data-15-wsi-60MB-stats; gcc -lm values-in.c; awk '{ print $1 }' stats | ./a.out | grep MAX >> hour-stats-data-15-wsi-60MB-stats; gcc -lm values-out.c; awk '{ print $2 }' stats | ./a.out | grep MAX >> hour-stats-data-15-wsi-60MB-stats; gcc -lm histrogram-in.c; awk '{ print $1 }' stats | ./a.out >> hour-stats-data-15-wsi-60MB-stats; gcc -lm histrogram-out.c; awk '{ print $2 }' stats | ./a.out >> hour-stats-data-15-wsi-60MB-stats; gcc -lm distribution-in.c; awk '{ print $1 }' stats | ./a.out >> hour-stats-data-15-wsi-60MB-stats; gcc -lm distribution-out.c; awk '{ print $2 }' stats | ./a.out >> hour-stats-data-15-wsi-60MB-stats

############################BT OUTPUT#########################

#gcc -lm latency.c; awk '{ print $3, $4, $5 }' graph-data > stats;  awk '{ print $1 }' stats | ./a.out > Edinburgh-A-mon-fri-9am-17pm-stats; gcc -lm avg-per-in.c; awk '{ print $2 }' stats | ./a.out >> Edinburgh-A-mon-fri-9am-17pm-stats; gcc -lm avg-per-out.c; awk '{ print $3 }' stats | ./a.out >> Edinburgh-A-mon-fri-9am-17pm-stats 

#gcc -lm avg-per-in.c; awk '{ print $3, $4 }' graph-data > stats; awk '{ print $1 }' stats | ./a.out > St-Albans-vpn-mon-fri-9am-17pm-stats; gcc -lm avg-per-out.c; awk '{ print $2 }' stats | ./a.out >> St-Albans-vpn-mon-fri-9am-17pm-stats;

#BT OUTPUT
#gcc -lm avg-per-in.c; awk '{ print $3, $4 }' graph-data > stats; awk '{ print $1 }' stats | ./a.out > Redhill-VPN-mon-fri-08am-17pm-256KB-stats; gcc -lm avg-per-out.c; awk '{ print $2 }' stats | ./a.out >> Redhill-VPN-mon-fri-08am-17pm-256KB-stats; gcc -lm values-in.c; awk '{ print $1 }' stats | ./a.out | grep MAX >> Redhill-VPN-mon-fri-08am-17pm-256KB-stats; gcc -lm values-out.c; awk '{ print $2 }' stats | ./a.out | grep MAX >> Redhill-VPN-mon-fri-08am-17pm-256KB-stats; gcc -lm histrogram-in.c; awk '{ print $1 }' stats | ./a.out >> Redhill-VPN-mon-fri-08am-17pm-256KB-stats; gcc -lm histrogram-out.c; awk '{ print $2 }' stats | ./a.out >> Redhill-VPN-mon-fri-08am-17pm-256KB-stats; gcc -lm distribution-in.c; awk '{ print $1 }' stats | ./a.out >> Redhill-VPN-mon-fri-08am-17pm-256KB-stats; gcc -lm distribution-out.c; awk '{ print $2 }' stats | ./a.out >> Redhill-VPN-mon-fri-08am-17pm-256KB-stats

###################
# sed = data | sed 'N; s/^/     /; s/ *\(.\{6,\}\)\n/\1    /' | egrep -e '24[3-4]    ' | more