function load_EXCEPTIONS_file
{
# Ignore any line that begins with a pound sign, #
# and also remove all blank lines

cat $EXCEPTIONS |  grep -v "^#" | sed /^$/d > $DATA_EXCEPTIONS
}

