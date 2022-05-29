#!/bin/bash

# for loop over packet sizes
for size in 40 250 500 750 1000 1250 1450; do
    # account for 8 bytes ping data
    let "realsize = size - 8"
    echo "ping -n $1 -s $realsize -c 10 > $1-$size"

    # call ping (100 packets) and write to file
    ping -n  $1 -s $realsize -c 10 > $1-p$size
done
