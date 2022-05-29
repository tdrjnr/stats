#!/usr/bin/perl

#-----------------------------------------------------------------------------
#
# tcpdumpPlot.pl
#
# This script scans a tcp dump file and plots data using GNU plot
# The data sent by ale is plotted as +ve and that received from mug:1357
# is plotted as -ve.
#
#-----------------------------------------------------------------------------

($dumpFile, $plotFile, $sendHost, $rcvHost) = @ARGV;

# if not specified on command line assign default values
$dumpFile = "10k-slowether" unless $dumpFile;
$plotFile = "tcpdump.dat" unless $plotFile;
$sendHost = "ale.isi.edu" unless $sendHost;
$rcvHost  = "mug.isi.edu" unless $rcvHost;

$psFile = $plotFile.".ps";
$miffFile = $plotFile.".mif";

# open tcpdump file and read line by line
open (TCPDUMP, "<$dumpFile") || die "Can't open $dumpFile for reading\n";
open (PLOTDAT, ">$plotFile") || die "Can't open $plotFile for writing\n";

# flag to indicate first line
$firstFlag = 1;
$sendFlag = 0;		# flags to determine sign of data
$rcvFlag  = 0;

while (<TCPDUMP>) {
    # separate lines for send and rcv hosts
    if (/$sendHost\.\d+ >/) {
	$sendFlag = 1;
    }
    if (/$rcvHost\.\d+ >/) {
	$rcvFlag  = 1;
    }

    ($fullTime, $sender, $gt, $rcvr, $flag, $seqNoData, $remainder) = split (/ /, $_, 7);
    ($time1, $time2, $time_sec) = split (/:/, $fullTime);

    # if first line assign value to t_0
    if ($firstFlag) {
	$t_0 = $time_sec;
	$firstFlag = 0;
    }
    
    $plotTime = $time_sec - $t_0; # get relative time for plotting
    
    if ($seqNoData =~ /^\d+:\d+\((\d+)\)/) {	# checks for digits
	if ($flag eq "S" || $flag eq "F") {
	    if ($sendFlag) {
		$data = 0;
		$ackData = 0;
		$synData = $1 + 40;
		$sendFlag = 0;
	    }
	    if ($rcvFlag) {
		$data = 0;
		$ackData = 0;
		$u_synData = $1 + 40;
		$synData = "-".$u_synData;
		$rcvFlag = 0;
	    }
	} else {
	    if ($sendFlag) {
		$data = $1 + 40;	# add ip header size to help 0 byte pkts to show up
		$ackData = 0;
		$synData = 0;
		$sendFlag = 0;
	    }
	    if ($rcvFlag) {
		$u_data = $1 + 40;
		$data = "-".$u_data;
		$ackData = 0;
		$synData = 0;
		$rcvFlag = 0;
	    }
	}
    }

    if ($seqNoData =~ /ack/) {
	if ($sendFlag) {
	    $data = 0;
	    $synData = 0;
	    $ackData = 40;
	    $sendFlag = 0;
	}
	if ($rcvFlag) {
	    $data = 0;
	    $synData = 0;
	    $ackData = "-40";
	    $rcvFlag = 0;
	}
    }

    printf PLOTDAT ("%.6f\t%d\t%d\t%d\n", $plotTime, $data, $ackData, $synData);
}
	
close (TCPDUMP);
close (PLOTDAT);

# plot using gnuplotfront
# use following to print to a mif file



