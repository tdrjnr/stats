#!/usr/local/bin/ksh
#
# SCRIPT: ./stats.ksh
# AUTHOR: Seun Ewulomi
# DATE:   09/02/2005
# REV:    1.0.D (Valid are A, B, D, T and P)
#               (For Alpha, Beta, Dev, Test and Production)
#
# PLATFORM: AIX, HP-UX, Linux, and Solaris
#                      
#
# PURPOSE: This shell script takes a given tabular snmp data for a given element(node)
#	   and collects the data per each day and gives the decriptive
#	   statistics and percentiles to enable trendline to be determined by using
#	   the maximum, percentile or average values.
#
# REV LIST:
#        DATE: 14/02/2005
#        BY:	  Seun Ewulomi   
#        MODIFICATION: filtering to get maximum and percentile values
#
#
# set -n   # Uncomment to check your syntax, without execution.
#          # NOTE: Do not forget to put the comment back in or
#          #       the shell script will not execute!
 set -x   # Uncomment to debug this shell script (Korn shell only)
#          
##########################################################
########### DEFINE FILES AND VARIABLES HERE ##############
##########################################################
echo '########## CREATING DATA ##########';

csv_file='brentwood'
file='data.txt'
awk="/bin/awk"

summary_file="brent.txt"
avg_in_1='gcc -lm avg-per-in.c'
avg_out_1='gcc -lm avg-per-out.c'
#avg_in_2='gcc -lm avg-per-in-2.c'
#avg_out_2='gcc -lm avg-per-out-2.c'

day1="10-01-2005"
day2="11-01-2005"
day3="12-01-2005"
day4="13-01-2005"
day5="14-01-2005"

f1=1
f2=2
f4=4
f5=5

#fields="'{ print $1,$2,$4,$5 }'"


##########################################################
############### DEFINE FUNCTIONS HERE ####################
##########################################################
function getdates_data
{
for dates in $day1 $day2 $day3 $day4 $day5; do
echo "getting day $dates raw data"
#sleep 0.5
sed 's/"/ /g' $csv_file | sed 's/,/ /g' | sed "s/\//-/g" | \
$awk '{ print $'$f1',$'$f2',$'$f4',$'$f5' }' | \
egrep -e $dates | awk '/08:.*:.*/,/18:.*:.*/' > $dates
done
}

function getdata_in_out
{
for dates in $day1 $day2 $day3 $day4 $day5; do
#sleep 1.5
echo "getting IN and OUT data for $dates"
#echo "########$dates########" > $summary_file_stats_1\n; 
awk '{ print $3, $4 }' $dates > $dates.stats; 
done
}

function getdata_stats_1
{
for dates in $day1.stats $day2.stats $day3.stats $day4.stats $day5.stats; do
echo "getting descr_stats for $dates data"
#sleep 0.5
$avg_in_1;
echo "########$dates########" >> $summary_file.distribution;
awk '{ print $1 }' $dates | ./a.out >> $summary_file.distribution;
$avg_out_1;
awk '{ print $2 }' $dates | ./a.out >> $summary_file.distribution;
done
}

function getdata_stats_2
{
for dates in $day1.stats $day2.stats $day3.stats $day4.stats $day5.stats; do
echo "getting descr_stats for $dates data"
#sleep 0.5
$avg_in_2;
echo "########$dates########" >> $summary_file.percentiles;
awk '{ print $1 }' $dates | ./a.out >> $summary_file.percentiles;
$avg_out_2;
awk '{ print $2 }' $dates | ./a.out >> $summary_file.percentiles;
done
}

function getdata_maximums
{
cat $summary_file.distribution | grep '#' | sed 's/#/ /g' | \
sed 's/.stats/ /g' | sed 's/ //g' > $summary_file.dates 	# filter and get dates only

cat $summary_file.distribution | grep 'IN' | grep 'Max' | awk '{ print $3 }' > $summary_file.inbound  # get inbound data only

cat $summary_file.distribution | grep Maximum | grep OUT | awk '{ print $3 }' > $summary_file.outbound   # get outbound data only
}

function joindata_maximums
{

}

##########################################################
################ BEGINNING OF MAIN #######################
##########################################################

#getdates_data
#getdata_in_out
#getdata_stats_1
#getdata_stats_2
getdata_maximums


# End of script

