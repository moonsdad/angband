/* File: signals.c */

/* Purpose: signal handlers */

/*
 * Copyright (c) 1989 James E. Wilson 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

/*
 * This signal package was brought to you by		-JEW- 
 * Completely rewritten by				-CJS-
 * Rewritten again by					-BEN-
 * One more time by Jeff Collins (Jan. 1995).
 */

/* must include before externs.h, because that uses SIGTSTP */
#include <signal.h>

#include "angband.h"

/*
 * We need a simple method to correctly refer to the signal handler
 */
#ifdef __MINT__
# define signal_handler ((__Sigfunc)(signal_handler_proc))
# define suspend_handler ((__Sigfunc)(suspend_handler_proc))
#else
# define signal_handler (signal_handler_proc)
# define suspend_handler (suspend_handler_proc)
#endif


static int          error_sig = (-1);
static int          signal_count = 0;

/*
 * signal_handler_proc - Handle signals
 *
 * The most important aspect of this is to catch SIGINT and SIGQUIT and
 * allow the user to rethink.  We don't want to kill a character because
 * of a stupid typo.
 */
#ifdef USG
static void signal_handler(int sig)
#else
static void signal_handler(int sig, int code, struct sigcontext  *scp)
#endif
{
    int                 smask;

    smask = sigsetmask(0) | (1 << sig);

    /* Ignore all second signals */
    if (error_sig >= 0) {
	if (++signal_count > 10)   /* Be safe. We will die if persistent enough. */
	    (void)signal(sig, SIG_DFL);
	return;
    }
    error_sig = sig;

/* Allow player to think twice. Wizard may force a core dump. */
    if (sig == SIGINT
#ifndef MSDOS
	|| sig == SIGQUIT
#endif
	) {
	if (death)
	    (void)signal(sig, SIG_IGN);	/* Can't quit after death. */
	else if (!character_saved && character_generated) {
	    if ((!total_winner) ? (!get_check("Really commit *Suicide*?"))
		: (!get_check("Do you want to retire?"))) {
		if (turn > 0)
		    disturb(1, 0);
		erase_line(0, 0);
		put_qio();
		error_sig = (-1);

		/* Restore handler for later. */
#ifdef USG
		(void)signal(sig, signal_handler);
#else
		(void)sigsetmask(smask);
#endif

	    /* in case control-c typed during msg_print */
		if (wait_for_more)
		    put_str(" -more-", MSG_LINE, 0);
		put_qio();

		/* OK. We don't quit. */
		return;
	    }

	    /* Death */
	    (void)strcpy(died_from, "Interrupting");
	}
	else {
	    (void)strcpy(died_from, "Abortion");
	}

	/* Interrupted */
	prt("Interrupt!", 0, 0);

	/* Suicide */
	death = TRUE;

	/* Save and exit */
	exit_game();
    }

    /* Die. */
    prt("OH NO!!!!!!  A gruesome software bug LEAPS out at you. There is NO defense!", 23, 0);

    /* Try to save anyway */
    if (!death && !character_saved && character_generated) {

	/* Try a panic save */
	panic_save = 1;
	prt("Your guardian angel is trying to save you.", 0, 0);

	/* Attempt to save */
	(void)sprintf(died_from, "(panic save %d)", sig);
	if (!save_char()) {
	/* Oops */
	(void)strcpy(died_from, "software bug");
	death = TRUE;
	turn = (-1);
	}
    }
    else {
	death = TRUE;
	/* Quietly save the memory anyway. */
	(void)_save_char(savefile);
    }

    restore_term();

#ifdef SET_UID
    /* generate a core dump if necessary */
    (void)signal(sig, SIG_DFL);
    (void)kill(getpid(), sig);
    (void)sleep(5);
#endif

    /* Quit anyway */
    exit(1);
}



void nosignals()
{
#if !defined(ATARIST_MWC)
#ifdef SIGTSTP
#ifdef linux
  (void) signal(SIGTSTP, (void (*)()) suspend);
#else
    (void)signal(SIGTSTP, SIG_IGN);
#endif
#ifndef USG
    mask = sigsetmask(0);
#endif
#endif
    if (error_sig < 0)
	error_sig = 0;
#endif
}

void signals()
{
#if !defined(ATARIST_MWC)
#ifdef SIGTSTP
#ifdef __MINT__
      (void)signal(SIGTSTP, (__Sigfunc)suspend);
#else
    (void)signal(SIGTSTP, suspend);
#endif
#ifndef USG
    (void)sigsetmask(mask);
#endif
#endif
    if (error_sig == 0)
	error_sig = (-1);
#endif
}

/*
 * Prepare to handle the relevant signals
 */
void signals_init()
{

#ifdef SIGHUP
    /* Ignore HANGUP, and let the EOF code take care of this case. */
    (void)signal(SIGHUP, SIG_IGN);
#endif

#ifdef SIGINT
    /* Catch the basic "Ctrl-C" interupt */
    (void)signal(SIGINT, signal_handler);
#endif

#ifdef SIGQUIT
    (void)signal(SIGQUIT, signal_handler);
#endif

#ifdef SIGFPE
    (void)signal(SIGFPE, signal_handler);
#endif

#ifdef SIGILL
    (void)signal(SIGILL, signal_handler);
#endif

#ifdef SIGTRAP
    (void)signal(SIGTRAP, signal_handler);
#endif

#ifdef SIGIOT
    (void)signal(SIGIOT, signal_handler);
#endif

#ifdef SIGKILL
    (void)signal(SIGKILL, signal_handler);
#endif

#ifdef SIGBUS
    (void)signal(SIGBUS, signal_handler);
#endif

#ifdef SIGSEGV
    (void)signal(SIGSEGV, signal_handler);
#endif

#ifdef SIGTERM
    (void)signal(SIGTERM, signal_handler);
#endif

#ifdef SIGPIPE
    (void)signal(SIGPIPE, signal_handler);
#endif

#ifdef SIGEMT			   /* in BSD systems */
    (void)signal(SIGEMT, signal_handler);
#endif

#ifdef SIGDANGER		   /* in SYSV systems */
    (void)signal(SIGDANGER, signal_handler);
#endif

#ifdef SIGSYS
    (void)signal(SIGSYS, signal_handler);
#endif

#ifdef SIGXCPU   /* BSD */
    (void)signal(SIGXCPU, signal_handler);
#endif

#ifdef SIGPWR   /* SYSV */
    (void)signal(SIGPWR, signal_handler);
#endif
}
 


void ignore_signals()
{
#if !defined(ATARIST_MWC)
    (void)signal(SIGINT, SIG_IGN);
#ifdef SIGQUIT
    (void)signal(SIGQUIT, SIG_IGN);
#endif
#endif
}

void default_signals()
{
#if !defined(ATARIST_MWC)
    (void)signal(SIGINT, SIG_DFL);
#ifdef SIGQUIT
    (void)signal(SIGQUIT, SIG_DFL);
#endif
#endif
}

void restore_signals()
{
#if !defined(ATARIST_MWC)
#ifndef linux
    (void)signal(SIGINT, signal_handler);
#endif /* linux */

#ifdef SIGQUIT
#ifdef linux
	(void) signal(SIGQUIT, (void (*)()) signal_handler);
#else
    (void)signal(SIGQUIT, signal_handler);
#endif
#endif
#endif
}

