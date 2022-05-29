






#cat brentwood.txt | grep -e 'Jan' -e 'IN' -e 'Maximum' | sed 's/-/ /g' | grep 'IN' | awk '{ print $3}'