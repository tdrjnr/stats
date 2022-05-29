/* Print opening messages. */
 
#include <stdio.h>

extern char version[];
#define EMAIL "langer@sfu.ca"

messages() {
fprintf(stderr, " *    INTER %s\n", version);
fprintf(stderr, " *    Send comments and questions to %s\n", EMAIL);
fprintf(stderr, " *    Type '?' to display options. Type 'help' for help.\n");
fprintf(stderr, " *\n");
}
