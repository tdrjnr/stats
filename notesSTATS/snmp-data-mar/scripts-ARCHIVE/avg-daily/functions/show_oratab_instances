function show_oratab_instances
{
if [ ! -f "$ORATAB" ]
then
     echo "\nOracle instance file $ORATAB does not exist\n"
     return 3
else
     cat $ORATAB | sed /^#/b | awk -F: '{print $1}'
fi
}

