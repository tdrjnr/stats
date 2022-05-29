#!/bin/bash

#RBS Output
#gcc -lm avg-per-in.c; awk '{ print $7, $8 }' dates | sed 's/,/ /g' > data; awk '{ print $1 }' data | ./a.out > brighton-b-stats; gcc -lm avg-per-out.c; awk '{ print $2 }' data  | ./a.out >> brighton-b-stats

#gcc -lm avg-per-in.c; awk '{ print $3, $4 }' data* > stats; awk '{ print $1 }' stats | ./a.out > Telford-01-mon-fri-9am-17pm-stats; gcc -lm avg-per-out.c; awk '{ print $2 }' stats | ./a.out >> Telford-01-mon-fri-9am-17pm-stats

#RBS OUTPUT
gcc -lm avg-per-in.c; awk '{ print $3, $4 }' data > stats; awk '{ print $1 }' stats | ./a.out > melville-lord-8am-18pm-stats; gcc -lm avg-per-out.c; awk '{ print $2 }' stats | ./a.out >> melville-lord-8am-18pm-stats

############################BT OUTPUT#########################

#gcc -lm latency.c; awk '{ print $3, $4, $5 }' graph-data > stats;  awk '{ print $1 }' stats | ./a.out > Edinburgh-A-mon-fri-9am-17pm-stats; gcc -lm avg-per-in.c; awk '{ print $2 }' stats | ./a.out >> Edinburgh-A-mon-fri-9am-17pm-stats; gcc -lm avg-per-out.c; awk '{ print $3 }' stats | ./a.out >> Edinburgh-A-mon-fri-9am-17pm-stats 

#gcc -lm avg-per-in.c; awk '{ print $3, $4 }' graph-data > stats; awk '{ print $1 }' stats | ./a.out > St-Albans-vpn-mon-fri-9am-17pm-stats; gcc -lm avg-per-out.c; awk '{ print $2 }' stats | ./a.out >> St-Albans-vpn-mon-fri-9am-17pm-stats;
