/* File: main-mac.c */

/* Purpose: Support for MACINTOSH Angband */

/*
 * Adapted from "MacAngband 2.6.1" by Keith Randall
 *
 */

#include "angband.h"


#ifdef MACINTOSH

#include <OSUtils.h>
#include <GestaltEqu.h>
#include <Palettes.h>
#include <Files.h>
#include <Fonts.h>
#include <Menus.h>
#include <Dialogs.h>
#include <StandardFile.h>
#include <ToolUtils.h>
#include <Desk.h>
#include <Resources.h>
#include <QuickDraw.h>
#include <Controls.h>





/*
 * Delay for "x" milliseconds
 */
void delay(int x)
{
    long t; t=TickCount(); while (TickCount() < t + (x*60)/1000);
}

/*** Some Hooks for various routines ***/


/*
 * See "z-virt.c"
 */

static vptr hook_ralloc(huge size)
{
    return (NewPtr(size));
}

static errr hook_rnfree(vptr v, huge size)
{
    DisposePtr(v);
    return (0);
}


/*** Main program ***/


/*
 * Macintosh Main loop
 */
void main(void)
{
    /* Mac-Mega-Hack -- about 200K of extra stack */
    SetApplLimit(GetApplLimit()-200000L);
    MaxApplZone();

    /* Set up the Macintosh */
    InitGraf(&qd.thePort);

    /* Mark ourself as the file creator */
    _fcreator = 'A271';

    /* Default to saving a "text" file */
    _ftype = 'TEXT';


    /* Hook in some "z-virt.c" hooks */
    ralloc_aux = hook_ralloc;
    rnfree_aux = hook_rnfree;

    /* Hooks in some "z-util.c" hooks */
    plog_aux = hook_plog;
    quit_aux = hook_quit;
    core_aux = hook_core;


}






#endif /* MACINTOSH */

