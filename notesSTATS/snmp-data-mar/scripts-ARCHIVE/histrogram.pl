#!/bin/bash

gcc -lm histrogram-in.c; awk '{ print $1 }' stats | ./a.out >> ccb-oxford-mon-fri-traffic-8am-18pm-stats; gcc -lm histrogram-out.c; awk '{ print $2 }' stats | ./a.out >> ccb-oxford-mon-fri-traffic-8am-18pm-stats