/* subroutine accepts a string as produced by sprintf possibly
 * using g format including e formats and returns a string
 * in the UGS extended character set.
 */

extern char *index();

g_to_ugs (c0, c1)
 char *c0, *c1;
{
   int i, n;
   char *p;

   *c1 = '\0';
   n = strlen (c0);
   p = index (c0, 'e');

/* easy case */

   if (p == (char*) 0) {
      strcpy (c1, c0);
      return;
   }

/* next easiest case  10^xxx */

   *p = '\0';
   if (!((p == &c0[1]) && (*c0 == '1'))) {
      strcpy (c1, c0);
      n = strlen (c1);
      c1[n] = 0x82;   /* UGS "times" sign */
      c1[n+1] = '\0';
   }

   p++;
   strcat (c1, "10");
   n = strlen (c1);
   c1[n] = 0x90;   /* UGS begin superscript */
   c1[n+1] = '\0';
   sscanf (p, "%d", &i);
   sprintf (p, "%d", i);
   strcat (c1, p);

   return;
}
