#!/usr/bin/perl -w
                # This script executes tcpdump every 15 min creating one file of 50000 packet lines per execution.

                $NUM = 20000;  # Number of packets during one measurement

                for ($countd = 21; $countd < 40; $countd++ )      #measurements have to be started at 6pm until 23pm (approximately)
                {
                                  $time = time;
                                  $file = "dumpmore$countd";
                                  $tcpdump = "tcpdump -c $NUM -w $file -e";
                                  open(DUMP,"$tcpdump |");
                                  close(DUMP); #dumpfile created
                                  open(ZIP,"gzip dumpmore$countd |");
                                  close(ZIP);                              
                                  sleep(900); #sleep for 15 min           
                }
