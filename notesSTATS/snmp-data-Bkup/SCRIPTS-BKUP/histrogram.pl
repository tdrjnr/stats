#!/bin/bash

gcc -lm histrogram-in.c; awk '{ print $1,$2 }' data > stats; awk '{ print $1 }' stats | ./a.out > combined-hist-stats; gcc -lm histrogram-out.c; awk '{ print $2 }' stats | ./a.out >> combined-hist-stats