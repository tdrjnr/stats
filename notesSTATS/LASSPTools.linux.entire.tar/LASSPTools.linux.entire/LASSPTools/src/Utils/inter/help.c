#include <stdio.h>
#include <string.h>

#define HELPCOMMAND "more "
#define HELPFILE    "inter.help"
#define HELPREDIR   " > /dev/tty"

longhelp() {
   char line[1024];
   strcpy(line, HELPCOMMAND);
   strcat(line, HELPDIR);
   strcat(line, "/");
   strcat(line, HELPFILE);
   strcat(line, HELPREDIR);
   system(line);
}
