#define MAXCASES 50
#include <stdio.h>

typedef struct {
    char *option;               /* commands that cause fn's to be called */
    int (*(function))();        /* functions to call */
    struct MENU *submenu;       /* pointers to submenus */
    char *hlp;                  /* pointer to help string */
} OPT;

typedef struct MENU {
    char *name;                 /* name of this menu */
    OPT opt[MAXCASES];
} MENU;

#define NULLMENU       ((MENU *) 0)
#define ENDMENU        {0, 0, 0, 0}
#define AUTORETURN     1
#define NOAUTORETURN   0

extern int commands();
extern int noop();
extern char *arg(), *argline();
extern int setvariable();
extern int moreargs;
extern int eoffound;

/* Here is an example of a menu specification.
 * Options 1 and 2 call functions 1 and 2,
 * option 3 calls up another menu.
 * When a command calls up another menu, the associated function
 * must be "commands".
 *
 *  int function1(),function2();
 *
 *  MENU submenu3 = { ... };
 *
 *  MENU m1 = {
 *     "m1",
 *     {"option1", function1, NULLMENU, "this is option 1"},
 *     {"option2", function2, NULLMENU, "this is option 2"},
 *     {"option3", commands, &submenu3, "get the menu for option 3"},
       ENDMENU
 *  };
 *
 *  Notice that the submenu must be listed first.
 */
