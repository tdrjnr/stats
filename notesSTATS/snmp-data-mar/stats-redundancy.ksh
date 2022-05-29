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
#        DATE: 03/03/2005
#        BY:	  Seun Ewulomi   
#        MODIFICATION: change variables to show date,time,days of statistics
#
#
# set -n   # Uncomment to check your syntax, without execution.
#          # NOTE: Do not forget to put the comment back in or
#          #       the shell script will not execute!
# set -x   # Uncomment to debug this shell script (Korn shell only)
#          
##########################################################
########### DEFINE FILES AND VARIABLES HERE ##############
##########################################################
echo '########## CREATING DATA ##########';

directory="chandlers-ford-combine-DATA"
csv_file='chandlers-ford-combine.csv'
file='rawdata'
graph='gnuplot-data.txt'
awk="/bin/awk"

summary_file="chandlers-ford-combine.txt"
avg_in_1='gcc -lm avg-per-in.c'
avg_out_1='gcc -lm avg-per-out.c'
avg_in_2='gcc -lm avg-per-in-2.c'
avg_out_2='gcc -lm avg-per-out-2.c'
values_in='gcc -lm values-in.c'
values_out='gcc -lm values-out.c'
histrogram_in='gcc -lm histrogram-in.c'
histrogram_out='gcc -lm histrogram-out.c'
distribution_in='gcc -lm distribution-in.c'
distribution_out='gcc -lm distribution-out.c'

sample_period='04FEB-15FEB-2005'
sample_time='08am-18pm'
sample_days='Mon-Fri'


f1=1
f2=2
f3=3
f4=4
f5=5
f6=6


##########################################################
############### DEFINE FUNCTIONS HERE ####################
##########################################################
function get_all_weeks_data
{
echo "######Getting raw data for $csv_file period $sample_period between $sample_time for $sample_days######";
sleep 1.5
sed 's/"/ /g' $csv_file | sed 's/,/ /g' | sed "s/\//-/g" | \
$awk '{ print $'$f1',$'$f2',$'$f3',$'$f4' }' \
> $csv_file.$sample_period.$sample_time.$sample_days.$file
}

function get_all_weeks_data_in_out
{
sleep 1.5
echo "######Getting IN and OUT data for $csv_file period $sample_period at $sample_time for $sample_days######"
sleep 1.5
awk '{ print $1, $2 }' $csv_file.$sample_period.$sample_time.$sample_days.$file > $summary_file.$sample_period.$sample_time.$sample_days.stats; 
}


function get_week_data
{
for weeks in $week1 $week2 $week3 $week4; do
echo "getting day $weeks raw data"
#sleep 0.5
sed 's/"/ /g' $csv_file | sed 's/,/ /g' | sed "s/\//-/g" | \
$awk '{ print $'$f1',$'$f2',$'$f4',$'$f5' }' | \
egrep -e $weeks | awk $time > $weeks.$csv_file
done
}

function getdata_stats_1
{
echo "######Getting descr_stats-1 for $csv_file $sample_period between $sample_time for $sample_days######"
sleep 1.5
$avg_in_1;
echo "########$csv_file $sample_period between $sample_time for $sample_days########" > $summary_file.$sample_period.$sample_time.$sample_days.distribution;
awk '{ print $1 }' $summary_file.$sample_period.$sample_time.$sample_days.stats | ./a.out >> $summary_file.$sample_period.$sample_time.$sample_days.distribution;
$avg_out_1;
awk '{ print $2 }' $summary_file.$sample_period.$sample_time.$sample_days.stats | ./a.out >> $summary_file.$sample_period.$sample_time.$sample_days.distribution;
}

function getdata_stats_2
{
echo "######Getting descr_stats-2 for $csv_file $sample_period between $sample_time for $sample_days######"
sleep 1.5
$avg_in_2;
echo "########$csv_file $sample_period between $sample_time for $sample_days########" >> $summary_file.$sample_period.$sample_time.$sample_days.distribution;
awk '{ print $1 }' $summary_file.$sample_period.$sample_time.$sample_days.stats | ./a.out >> $summary_file.$sample_period.$sample_time.$sample_days.distribution;
$avg_out_2;
awk '{ print $2 }' $summary_file.$sample_period.$sample_time.$sample_days.stats | ./a.out >> $summary_file.$sample_period.$sample_time.$sample_days.distribution;
}

function getdata_stats_3
{
echo "######Getting descr_stats-3 for $csv_file $sample_period between $sample_time for $sample_days######"
sleep 1.5
$values_in;
echo "########$csv_file $sample_period between $sample_time for $sample_days########" >> $summary_file.$sample_period.$sample_time.$sample_days.distribution;
awk '{ print $1 }' $summary_file.$sample_period.$sample_time.$sample_days.stats | ./a.out | grep MAX >> $summary_file.$sample_period.$sample_time.$sample_days.distribution;
$values_out;
awk '{ print $2 }' $summary_file.$sample_period.$sample_time.$sample_days.stats | ./a.out | grep MAX >> $summary_file.$sample_period.$sample_time.$sample_days.distribution;
$histrogram_in;
echo "########$csv_file $sample_period between $sample_time for $sample_days########" >> $summary_file.$sample_period.$sample_time.$sample_days.distribution;
awk '{ print $1 }' $summary_file.$sample_period.$sample_time.$sample_days.stats | ./a.out >> $summary_file.$sample_period.$sample_time.$sample_days.distribution;
$histrogram_out;
awk '{ print $2 }' $summary_file.$sample_period.$sample_time.$sample_days.stats | ./a.out >> $summary_file.$sample_period.$sample_time.$sample_days.distribution;
$distribution_in;
echo "########$csv_file $sample_period between $sample_time for $sample_days########" >> $summary_file.$sample_period.$sample_time.$sample_days.distribution;
awk '{ print $1 }' $summary_file.$sample_period.$sample_time.$sample_days.stats | ./a.out >> $summary_file.$sample_period.$sample_time.$sample_days.distribution;
$distribution_out;
awk '{ print $2 }' $summary_file.$sample_period.$sample_time.$sample_days.stats | ./a.out >> $summary_file.$sample_period.$sample_time.$sample_days.distribution;
}

function get_graph_data
{
echo "######Getting data to plot graph $csv_file $sample_period between $sample_time for $sample_days######"
sleep 1.5
cp $csv_file.$sample_period.$sample_time.$sample_days.$file $graph
}

function plot_graph
{
echo "######Plotting graph $csv_file $sample_period between $sample_time for $sample_days######"
sleep 1.5
./chart-graph-data

}

function move_data
{
echo "######Moving data######"
sleep 1.5
mkdir $directory.$sample_time.$sample_days; \
mv $csv_file $csv_file.$sample_period.$sample_time.$sample_days.$file $summary_file.$sample_period.$sample_time.$sample_days.distribution $summary_file.$sample_period.$sample_time.$sample_days.stats $graph *.png $directory.$sample_time.$sample_days;
}

######################################################
################ BEGINNING OF MAIN #######################
##########################################################

get_all_weeks_data
get_all_weeks_data_in_out
############################get_week_data
getdata_stats_1
getdata_stats_2
getdata_stats_3
get_graph_data
plot_graph
move_data


# End of script

