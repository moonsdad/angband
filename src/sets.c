/*
 * sets.c: code to emulate the original Pascal sets 
 *
 * Copyright (c) 1989 James E. Wilson 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "constant.h"
#include "config.h"
#include "types.h"
#include "externs.h"

int 
set_room(element)
register int element;
{
    if ((element == DARK_FLOOR) || (element == LIGHT_FLOOR) ||
	(element == NT_DARK_FLOOR) || (element == NT_LIGHT_FLOOR))
	return (TRUE);
    return (FALSE);
}

int 
set_corr(element)
register int element;
{
    if (element == CORR_FLOOR || element == BLOCKED_FLOOR)
	return (TRUE);
    return (FALSE);
}

int 
set_floor(element)
int element;
{
    if (element <= MAX_CAVE_FLOOR)
	return (TRUE);
    else
	return (FALSE);
}







int 
set_meteor_destroy(e)		   /* added -DGK */
inven_type *e;
{
    byte fi, fo;

    fi = set_fire_destroy(e);
    fo = set_frost_destroy(e);
    return (fi | fo);
}


int 
set_mana_destroy(e)		   /* added -DGK */
inven_type *e;
{				   /* destroy everything but artifacts */
    int element = e->tval;

    if ((element >= TV_MIN_ENCHANT) && (element <= TV_MAX_WEAR) &&
	(e->flags2 & TR_ARTIFACT))
	return (FALSE);
    return (TRUE);
}


int 
set_holy_destroy(e)		   /* added -DGK */
inven_type *e;
{
    int element = e->tval;

    if ((element >= TV_MIN_ENCHANT) && (element <= TV_MAX_WEAR) &&
	(e->flags3 & TR3_CURSED) && (!(e->flags2 & TR_ARTIFACT)))
	return (TRUE);
    return (FALSE);
}


int 
set_plasma_destroy(e)		   /* added -DGK */
inven_type *e;
{
    byte fi, li;

    fi = set_fire_destroy(e);
    li = set_lightning_destroy(e);
    return (fi | li);
}






int 
set_null(e)
inven_type *e;
{
    return (FALSE);
}







int 
general_store(element)
int element;
{
    switch (element) {
      case TV_DIGGING:
      case TV_BOOTS:
      case TV_CLOAK:
      case TV_FOOD:
      case TV_FLASK:
      case TV_LITE:
      case TV_SPIKE:
	return (TRUE);
    }
    return (FALSE);
}


int 
armory(element)
int element;
{
    switch (element) {
      case TV_BOOTS:
      case TV_GLOVES:
      case TV_HELM:
      case TV_SHIELD:
      case TV_HARD_ARMOR:
      case TV_SOFT_ARMOR:
	return (TRUE);
    }
    return (FALSE);
}


int 
weaponsmith(element)
int element;
{
    switch (element) {
      case TV_SHOT:
      case TV_BOLT:
      case TV_ARROW:
      case TV_BOW:
      case TV_HAFTED:
      case TV_POLEARM:
      case TV_SWORD:
	return (TRUE);
    }
    return (FALSE);
}


int 
temple(element)
int element;
{
    switch (element) {
      case TV_HAFTED:
      case TV_SCROLL1:
      case TV_SCROLL2:
      case TV_POTION1:
      case TV_POTION2:
      case TV_PRAYER_BOOK:
	return (TRUE);
    }
    return (FALSE);
}


int 
alchemist(element)
int element;
{
    switch (element) {
      case TV_SCROLL1:
      case TV_SCROLL2:
      case TV_POTION1:
      case TV_POTION2:
	return (TRUE);
    }
    return (FALSE);
}


int 
magic_shop(element)
int element;
{
    switch (element) {
      case TV_AMULET:
      case TV_RING:
      case TV_STAFF:
      case TV_WAND:
      case TV_SCROLL1:
      case TV_SCROLL2:
      case TV_POTION1:
      case TV_POTION2:
      case TV_MAGIC_BOOK:
      case TV_ROD:
	return (TRUE);
    }
    return (FALSE);
}

int 
blackmarket(element)
int element;
{
    return (TRUE);
}

int 
home(element)
int element;
{
    return (TRUE);
}
