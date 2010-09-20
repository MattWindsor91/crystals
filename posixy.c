/* posixy.c - an implementation of any POSIX stuff we need/want */

#include <stdlib.h>
#include "posixy.h"

/* Duplicate a string, leaving the original intact */
char*
strdup (const char *string)
{
  char* out = (char*) malloc (strlen (string) * sizeof (char) + 1);
  strcpy (out, string);
  return out;
}

/* Concatenate two strings, storing the result in the first */
char*
strcat (char *dest, const char *string)
{
  char *newstr = (char*) malloc ((strlen (dest)  + strlen (string)) * sizeof (char) + 1);

  strcpy (newstr, dest);
  strcpy ((newstr + strlen (dest)), string);
  dest = newstr;

  return dest;
}
