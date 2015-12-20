/* File: util.c */

/* Purpose: miscellanous utilities */

/*
 * Copyright (c) 1989 James E. Wilson 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"



#ifndef HAS_USLEEP

/*
 * for those systems that don't have usleep
 * grabbed from the inl netrek server -cba 
 * I think we include too many files above!
 */
int usleep(huge microSeconds)
{
    struct timeval      Timer;

    int                 nfds, readfds, writefds, exceptfds;
    unsigned int        Seconds, uSec;

    nfds = readfds = writefds = exceptfds = 0;


    /* Paranoia -- No excessive sleeping */
    if (microSeconds > (unsigned long)4000000) {
	errno = ERANGE;		   /* value out of range */
	perror("usleep time out of range ( 0 -> 4000000 ) ");
	return -1;
    }

    /* Wait for it */
    Seconds = microSeconds / (unsigned long)1000000;
    uSec = microSeconds % (unsigned long)1000000;
    Timer.tv_sec = Seconds;
    Timer.tv_usec = uSec;

    /* Wait for it */
    if (select(nfds, &readfds, &writefds, &exceptfds, &Timer) < 0) {

	/* Hack -- ignore interrupts */
	if (errno != EINTR) {
	    perror("usleep (select) failed");
	    return -1;
	}
    }
    
    /* Success */
    return 0;
}

#endif


#ifdef MACINTOSH

/* See "main-mac.c" */

#else

#ifdef AMIGA

void delay(int t)
{
    if (t >= 20) Delay(t / 20);
}

#else

#ifdef __EMX__

void delay(int x)
{
    _sleep2(x);
}

#else

#ifndef MSDOS

/*
 * Unix port for "delay"
 */
void delay(int x)
{
    /* Do it in micro-seconds */
    usleep(1000 * x);
}

#endif	/* MSDOS */

#endif	/* __EMX__ */

#endif	/* AMIGA */

#endif	/* MACINTOSH */


#ifdef AMIGA

/*
 * Is this actually used?
 */
int getuid()
{
  return 0;
}

/*
 * Is this actually used?
 */
void umask(int x)
{
}

#endif /* AMIGA */

