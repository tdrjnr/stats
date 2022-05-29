#!/bin/bash

gcc -lm values-in.c; awk '{ print $1,$2 }' data > stats; awk '{ print $1 }' stats | ./a.out | grep MAX > combined-values-stats; gcc -lm values-out.c; awk '{ print $2 }' stats | ./a.out | grep MAX >> combined-values-stats