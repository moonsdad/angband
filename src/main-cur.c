/* File: main-cur.c */

/* Purpose: Actual Unix Curses support for Angband */

/*
 * Copyright (c) 1989 James E. Wilson, Christopher J. Stuart , Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

/*
 * Some annoying machines define "bool" in various packages
 * Note that this "redefinition" should work on any machine.
 */

#define bool bool_hack
#include "angband.h"
#undef bool


# ifdef linux
#  include <bsd/sgtty.h>
# endif

# ifdef GEMDOS
#  define ATARIST_MWC
#  include "curses.h"
#  include <osbind.h>
   char               *getenv();
# endif


# ifdef MSDOS

/*** OPEN MSDOS ***/
#  include <process.h>
#  if defined(ANSI)
#   include "ms_ansi.h"
#  endif
/*** SHUT MSDOS ***/

# else /* MSDOS */
#  ifndef ATARIST_MWC

/*** OPEN NORMAL ***/
#   include <signal.h>
#   undef TRUE
#   undef FALSE
#   include <curses.h>
#   ifndef VMS
#    include <sys/ioctl.h>
#   endif
#   ifdef USG
#    ifndef __MINT__
#     include <termio.h>
#    endif
#   endif

#ifdef M_XENIX
# include <sys/select.h>
#endif

#ifdef USG
# ifndef __MINT__
#  include <termio.h>
# endif
#else
# if defined(atarist) && defined(__GNUC__) && !defined(__MINT__)
   /* doesn't have <sys/wait.h> */
# else
#  include <sys/wait.h>
# endif
# include <sys/resource.h>
# include <sys/param.h>
#  include <sys/param.h>
#  include <sys/file.h>
#  include <sys/types.h>
#  ifndef VMS
#   include <sys/wait.h>
#  endif /* !VMS */

#endif

/* Hack --  Brute force never hurt... [cjh] */
# if defined(__MINT__) && !defined(_WAIT_H)
#  include <wait.h>
# endif


/*** SHUT NORMAL ***/

#  endif
# endif /* MSDOS */

#ifdef ATARIST_MWC
extern WINDOW *newwin();
#endif


#if !defined(MSDOS) && !defined(ATARIST_MWC) && !defined(__MINT__)
#ifdef USG
static struct termio save_termio;
#else
#ifndef VMS
static struct ltchars save_special_chars;
static struct sgttyb save_ttyb;
static struct tchars save_tchars;
static int          save_local_chars;
#endif
#endif
#endif




/*
 * Provides for a timeout on input. Does a non-blocking read, consuming the
 * data if any, and then returns 1 if data was read, zero otherwise. 
 *
 * Porting: 
 *
 * In systems without the select call, but with a sleep for fractional numbers
 * of seconds, one could sleep for the time and then check for input. 
 *
 * In systems which can only sleep for whole number of seconds, you might sleep
 * by writing a lot of nulls to the terminal, and waiting for them to drain,
 * or you might hack a static accumulation of times to wait. When the
 * accumulation reaches a certain point, sleep for a second. There would need
 * to be a way of resetting the count, with a call made for commands like run
 * or rest.
 */
int check_input(int microsec)
{
#if defined(USG) && !defined(M_XENIX)
    int                 arg, result;

#else
    struct timeval      tbuf;
    int                 ch;

#if defined(BSD4_3) || defined(M_XENIX) || defined(linux)
    fd_set              smask;

#else
    int                 smask;

#endif
#endif

/* Return true if a read on descriptor 1 will not block. */
#if !defined(USG) || defined(M_XENIX)
    tbuf.tv_sec = 0;
    tbuf.tv_usec = microsec;
#if defined(BSD4_3) || defined(M_XENIX) || defined(linux)
    FD_ZERO(&smask);
    FD_SET(fileno(stdin), &smask);
    if (select(1, &smask, (fd_set *) 0, (fd_set *) 0, &tbuf) == 1)
#else
    smask = 1;			   /* i.e. (1 << 0) */
    if (select(1, &smask, (int *)0, (int *)0, &tbuf) == 1)
#endif
    {
	ch = getch();
    /* check for EOF errors here, select sometimes works even when EOF */
	if (ch == -1) {
	    eof_flag++;
	    return 0;
	}
	return 1;
    } else
	return 0;
#else				   /* SYS V code follows */
    if (microsec != 0 && (turn & 0x7F) == 0)
	(void)sleep(1);		   /* mod 128, sleep one sec every 128 turns */
/* Can't check for input, but can do non-blocking read, so... */
/* Ugh! */
    arg = 0;
    arg = fcntl(0, F_GETFL, arg);
    arg |= O_NDELAY;
    (void)fcntl(0, F_SETFL, arg);

    result = getch();

    arg = 0;
    arg = fcntl(0, F_GETFL, arg);
    arg &= ~O_NDELAY;
    (void)fcntl(0, F_SETFL, arg);
    if (result == -1)
	return 0;
    else
	return 1;
#endif
}

#if 0

/*
 * This is not used.  It will only work on some systems.
 * this should be compared against shell_out in io.c 
 */

/*
 * A command for the operating system. Standard library function 'system' is
 * unsafe, as it leaves various file descriptors open. This also is very
 * careful with signals and interrupts, and does rudimentary job control, and
 * puts the terminal back in a standard mode. 
 */
int system_cmd(char *p)
{
    int                 pgrp, pid, i, mask;
    union wait          w;
    extern char        *getenv();

    mask = sigsetmask(~0);	   /* No interrupts. */
    restore_term();		   /* Terminal in original state. */

    /* Are we in the control terminal group? */
    if (ioctl(0, TIOCGPGRP, (char *)&pgrp) < 0 || pgrp != getpgrp(0)) {
	pgrp = (-1);
    }

    pid = fork();
    if (pid < 0) {
	(void)sigsetmask(mask);
	moriaterm();
	return (-1);
    }
    if (pid == 0) {
	(void)sigsetmask(0);	   /* Interrupts on. */
    /* Transfer control terminal. */
	if (pgrp >= 0) {
	    i = getpid();
	    (void)ioctl(0, TIOCSPGRP, (char *)&i);
	    (void)setpgrp(i, i);
	}
	for (i = 2; i < 30; i++)
	    (void)close(i);	   /* Close all but standard in and out. */
	(void)dup2(1, 2);	   /* Make standard error as standard out. */
	if (p == 0 || *p == 0) {
	    p = getenv("SHELL");
	    if (p)
		execl(p, p, 0);
	    execl("/bin/sh", "sh", 0);
	}
	else {
	    execl("/bin/sh", "sh", "-c", p, 0);
	}

	/* Hack (?) */
	_exit(1);
    }

/* Wait for child termination. */
    for (;;) {
	i = wait3(&w, WUNTRACED, (struct rusage *) 0);
	if (i == pid) {
	    if (WIFSTOPPED(w)) {
	    /* Stop outselves, if child stops. */
		(void)kill(getpid(), SIGSTOP);
	    /* Restore the control terminal, and restart subprocess. */
		if (pgrp >= 0)
		    (void)ioctl(0, TIOCSPGRP, (char *)&pid);
		(void)killpg(pid, SIGCONT);
	    } else
		break;
	}
    }

    /* Get the control terminal back. */
    if (pgrp >= 0) {
	(void)ioctl(0, TIOCSPGRP, (char *)&pgrp);
    }

    (void)sigsetmask(mask);	   /* Interrupts on. */
    moriaterm();		   /* Terminal in moria mode. */
    return 0;
}

#endif


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