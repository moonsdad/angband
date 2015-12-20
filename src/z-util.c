/* File: z-util.c */

#include "z-util.h"


#ifndef HAS_STRICMP

/*
 * For those systems that don't have stricmp. -hmj
 */
int stricmp(cptr c1,cptr c2)
{
 char c3;
 char c4;

  /* Scan the strings */
  for (;;)
  {
    c3 = (islower(*c1)?toupper(*c1):*c1);
    c4 = (islower(*c2)?toupper(*c2):*c2);
    if (c3 < c4) return(-1);
    if (c3 > c4) return(1);
    if (c3 == '\0') return(0);
    c1++;
    c2++;
  }
}

#endif



