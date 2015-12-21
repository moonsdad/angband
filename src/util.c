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


/* Find a default user name from the system. */
void user_name(char *buf, int id)
{
    struct passwd      *pwd;

    pwd = getpwuid(id);
    (void)strcpy(buf, pwd->pw_name);
    if (*buf >= 'a' && *buf <= 'z')
	*buf = (*buf - 'a') + 'A';
}

/* expands a tilde at the beginning of a file name to a users home directory */
int tilde(cptr file, char *exp)
{
    *exp = '\0';
    if (file) {
	if (*file == '~') {
	    char                user[128];
	    struct passwd      *pw = NULL;
	    int                 i = 0;

	    user[0] = '\0';
	    file++;
	    while (*file != '/' && i < sizeof(user))
		user[i++] = *file++;
	    user[i] = '\0';
	    if (i == 0) {
		char               *login = (char *)getlogin();

		if (login != NULL)
		    (void)strcpy(user, login);
		else if ((pw = getpwuid(getuid())) == NULL)
		    return 0;
	    }
	    if (pw == NULL && (pw = getpwnam(user)) == NULL)
		return 0;
	    (void)strcpy(exp, pw->pw_dir);
	}
	(void)strcat(exp, file);
	return 1;
    }
    return 0;
}

/*
 * open a file just as does fopen, but allow a leading ~ to specify a home
 * directory 
 */
FILE *my_tfopen(cptr ffile, cptr fmode)
{
    char                buf[1024];
    extern int          errno;

    if (tilde(file, buf))
	return (fopen(buf, mode));
    errno = ENOENT;
    return NULL;
}


/*
 * open a file just as does open, but expand a leading ~ into a home
 * directory name 
 */
int my_topen(cptr file, int flags, int mode)
{
    char                buf[1024];
    extern int          errno;

    if (tilde(file, buf))
	return (open(buf, flags, mode));
    errno = ENOENT;
    return -1;
}

#endif