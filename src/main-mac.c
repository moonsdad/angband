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







#endif /* MACINTOSH */

