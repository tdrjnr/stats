function check_for_and_create_keyboard_file
{
# If the $KEYBOARD_FILE does not exist then
# ask the user to load the "standard" keyboard
# layout, which is done with the load_default_keyboard
# function.

if [ ! -s $KEYBOARD_FILE ]
then
     echo "\n\nERROR: Missing Keyboard File"
     echo "\n\nWould You Like to Load the"
     echo "Default Keyboard Layout?"
     echo "\n\t(Y/N): \c"
     typeset -u REPLY=FALSE
     read REPLY
     if [ $REPLY != Y ]
     then
          echo "\n\nERROR: This shell script cannot operate"
          echo "without a keyboard data file located in"
          echo "\n==>  $KEYBOARD_FILE\n"
          echo "\nThis file expects one character per line."
          echo "\n\t...EXITING...\n"
          exit 3
     else
          load_default_keyboard
          echo "\nPress ENTER when you are you ready to continue: \c"
          read REPLY
          clear
     fi
fi
}

