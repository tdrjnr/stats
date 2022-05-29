#!/bin/sh

plot=""
plot2=""

rm -f avg.tmp

outname=`echo $1 | sed -e "s/-/ /" | awk '{print $1}'`

# cycle through all filenames
until [ -z "$1" ]
do
  echo "processing $1"

  # grep only for the lines with ping data,
  # plot the field's we're interested in and
  # write to temp file
  MIN=`grep avg $1 | sed -e "s/\// /g" | awk '{print $7}'`
  AVG=`grep avg $1 | sed -e "s/\// /g" | awk '{print $8}'`
  PSIZE=`grep icmp_seq $1 | head -1 | awk '{print int($1)}'`
  echo "$PSIZE $AVG $MIN" >> avg.tmp
  grep icmp_seq $1 | sed -e "s/=/ /g" | awk '{print $6 " " $8 " " $1}' > $1.tmp

  #compose plot command for gnuplot
  plot="$plot \"$1.tmp\" u 1:2 not w l"
  plot2="$plot2 \"$1.tmp\" u 3:2 not w p"
  shift
  if [ -n "$1" ] ; then # no , after last file
      plot="$plot,"
      plot2="$plot2,"
  fi

done

#sort -n avg.tmp > avg-sort.tmp

# delete temp files
#rm -f *.tmp
