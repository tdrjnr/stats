#!/usr/local/bin/ksh
#
# SCRIPT: ./stats-tcpdump.ksh
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
 set -x   # Uncomment to debug this shell script (Korn shell only)
#          
##########################################################
########### DEFINE FILES AND VARIABLES HERE ##############
##########################################################
echo '########## CREATING DATA ##########';

trace_file='climber-ftp.cap'
stats='stats'
directory="$trace_file-DATA"

client_1="50.144.16.71"
server_1="10.221.22.52"
src="*.src-host.cap"

sample_period='Mar-2005'
sample_time='21h49m51s-21h54m06s'
sample_days='Mon-Fri'

#time='/21:.*:.*/,/21:.*:.*/'

tcptrace='/usr/local/bin/tcptrace'
xpl2gpl='/usr/local/bin/xpl2gpl'
ngrep='/usr/local/bin/ngrep'

protocol="tcp"
port="21"



##########################################################
############### DEFINE FUNCTIONS HERE ####################
##########################################################

function tcptrace_stats
{
mkdir $directory;
echo "######CREATE packet trace for Src Dst######";
sleep 1.5
$tcptrace -S $trace_file > $trace_file-conn; mv -f *.xpl *-conn $directory; cd $directory; \
mkdir $trace_file.SrcDst; mv -f *-conn *.xpl $trace_file.SrcDst
}


function client_trace
{
for clients in $client_1; do
echo "######CREATE Src ONLY packet traces######";
sleep 1.5
$ngrep -I $trace_file src $clients and $protocol port $port -q -l -O $clients.$sample_period.src-host.cap \
 > /dev/null
done

function src_tcptraces
{

for srces $src; do

tcptrace $srces > /dev/null

done
}
src_dircetory="src-host-TRACES";



#then
#mkdir $src_directory; mv *src-host.cap $src_directory; \
#mv $src_directory $directory

 

done
}



function plot_graph
{
echo "######Plotting graph $trace_file between $sample_time######"
sleep 1.5
./graph-tcpdump

}

######################################################
################ BEGINNING OF MAIN #######################
##########################################################

#tcptrace_stats
#client_trace
src_tcptraces



# End of script

