#!/usr/bin/perl -w
use strict;
use IO::File;
use Getopt::Long;

# Parse command line arguments and assign corresponding variables
GetOptions
(
    'i|input=s'     => \( my $INPUT  = undef ),
    'o|output=s'    => \( my $OUTPUT = "./data-in,./data-out" ),
    'g|graph=s'     => \( my $GRAPH  = "./graph" ),
);

unless ( defined $INPUT )
{
    print <<USAGE
SNMP data parser

Usage: $0 [option]
Options:
  -i|--input [filename]     SNMP router data file
  -o|--output [in,out]      Optional. Default is "datain,dataout".
  -g|--graph [command]      Optional. Default is "./graph"

USAGE
;
    exit(1);
}

# get the names of the datain and dataout files
# note that you can override these names from
# the command-line
my ($datain,$dataout) = split /,\s*/, $OUTPUT;

my $f = new IO::File $INPUT, "r"
    or die "Could not open input file $INPUT";

my $di = new IO::File $datain, "w"
    or die "Could not create output file $datain";

my $do = new IO::File $dataout, "w"
    or die "Could not create output file $dataout";

while (<$f>) {          # for each line in the input file
    next if /^\s*$/;    # ignore empty lines

    my @col = split /,\s+/;   # split on , followed by spaces
    my ($date) = $col[0] =~ /^"(.*)\s.*"$/; # want date only

    print $di "$date, $col[2]\n";
    print $do "$date, $col[3]\n";
}

undef $di;   # force close of files
undef $do;
undef $f;

# do the graph
system($GRAPH) or die "Could not execute $GRAPH";

exit(0);


