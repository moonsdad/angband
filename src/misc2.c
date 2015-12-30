/* File: misc2.c */

/* Purpose: misc code for monsters */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"


/*
 * Ludwig's Brainstorm
 */
static int test_place(int y, int x)
{
    /* Require legal grid */
    if (!in_bounds(y, x)) return (FALSE);
    
    /* Require "empty" floor grid */
    if (cave[y][x].fval >= MIN_CLOSED_SPACE) return (FALSE);

    /* And do not use special walls */
    if (cave[y][x].fval == NULL_WALL) return (FALSE);

    /* Or the player himself */
    if(cave[y][x].cptr != 0) || ((y == char_row) && (x == char_col))) return (FALSE);

    /* Use it */
    return TRUE;
}



/*
 * Deletes a monster entry from the level		-RAK-
 */
void delete_monster(int j)
{
    register monster_type *m_ptr;

    /* trouble? abort! -CFT */
    if (j < 2) return;

    /* Get the monster */
    m_ptr = &m_list[j];

    if (r_list[m_ptr->mptr].cdefense & UNIQUE) check_unique(m_ptr);

    cave[m_ptr->fy][m_ptr->fx].cptr = 0;

    if (m_ptr->ml) lite_spot((int)m_ptr->fy, (int)m_ptr->fx);


#ifdef TARGET
    /* This targetting code stolen from Morgul -CFT */
    /* Targetted monster dead or compacted.      CDW */
    if (j == target_mon) target_mode = FALSE;
#endif

    /* One less monster */
    m_max--;

    /* Do structure dumping */
    if (j != mfptr) {

#ifdef TARGET
	/* Targetted monster moved to replace dead or compacted monster CDW */
	if (target_mon == mfptr-1) target_mon = j;
#endif

	m_ptr = &m_list[mfptr];
	cave[m_ptr->fy][m_ptr->fx].cptr = j;
	m_list[j] = m_list[mfptr];
    }

    /* Wipe the monster record */
    m_list[mfptr] = blank_monster;

    if (mon_tot_mult > 0) mon_tot_mult--;
}




/*
 * The following two procedures implement the same function as delete
 * monster. However, they are used within creatures(), because deleting a
 * monster while scanning the m_list causes two problems, monsters might get
 * two turns, and m_ptr/monptr might be invalid after the delete_monster.
 * Hence the delete is done in two steps. 
 */
/*
 * fix1_delete_monster does everything delete_monster does except delete the
 * monster record and reduce mfptr, this is called in breathe, and a couple
 * of places in creatures.c 
 */
void fix1_delete_monster(int j)
{
    register monster_type *m_ptr;

    /* trouble? abort! -CFT */
    if (j < 2) return;

#ifdef TARGET
    /* Targetted monster dead or compacted.      CDW */
    if (j == target_mon) target_mode = FALSE;

    /* Targetted monster moved to replace dead or compacted monster   CDW */
    if (target_mon == mfptr-1) target_mon = j;
#endif

    /* Get the monster */
    m_ptr = &m_list[j];

    if (r_list[m_ptr->mptr].cdefense & UNIQUE) check_unique(m_ptr);

/* force the hp negative to ensure that the monster is dead, for example, if
 * the monster was just eaten by another, it will still have positive hit
 * points  */
    m_ptr->hp = (-1);

    cave[m_ptr->fy][m_ptr->fx].cptr = 0;
    if (m_ptr->ml) lite_spot((int)m_ptr->fy, (int)m_ptr->fx);
    if (mon_tot_mult > 0) mon_tot_mult--;
}


/*
 * fix2_delete_monster does everything in delete_monster that wasn't done by
 * fix1_monster_delete above, this is only called in creatures() 
 */
void fix2_delete_monster(int j)
{
    register monster_type *m_ptr;

	/* trouble? abort! -CFT */
    if (j < 2) return;
    
#ifdef TARGET
    /* Targetted monster dead or compacted. CDW */
    if (j==target_mon) target_mode = FALSE;

    /* Targetted monster moved to replace dead or compacted monster   CDW */
    if (target_mon==mfptr-1) target_mon = j; 
#endif

    m_ptr = &m_list[j];		   /* Fixed from a r_list ptr to a m_list ptr. -CFT */
    if (r_list[m_ptr->mptr].cdefense & UNIQUE) check_unique(m_ptr);
    if (j != mfptr - 1) {
	m_ptr = &m_list[mfptr - 1];
	cave[m_ptr->fy][m_ptr->fx].cptr = j;
	m_list[j] = m_list[mfptr - 1];
    }
    m_list[mfptr - 1] = blank_monster;
    mfptr--;
}




/*
 * Compact monsters	-RAK-
 *
 * Return TRUE if any monsters were deleted, FALSE if could not delete any
 * monsters. 
 */
int compact_monsters(void)
{
    register int           i;
    int                    cur_dis, delete_any;
    monster_type	*mon_ptr;

    msg_print("Compacting monsters...");


    /* Start 66 units away */
    cur_dis = 66;

    delete_any = FALSE;

    do {
	for (i = mfptr - 1; i >= MIN_M_IDX; i--) {
	    mon_ptr = &m_list[i];
	    if ((cur_dis < mon_ptr->cdis) && (randint(3) == 1)) {
	    /* Don't compact Melkor! */
		if (r_list[mon_ptr->mptr].cmove & CM_WIN)
		/* do nothing */
		    ;

	    /* in case this is called from within creatures(), this is a
	     * horrible hack, the m_list/creatures() code needs to be
	     * rewritten 
	     */
		else if (hack_m_idx < i) {
		    delete_monster(i);
		    delete_any = TRUE;
		} else

		/* fix1_delete_monster() does not decrement mfptr, so don't
		 * set delete_any if this was called 
		 */
		    fix1_delete_monster(i);
	    }
	}
	if (!delete_any) {
	    cur_dis -= 6;
	/* can't do anything else but abort, if can't delete any monsters */
	    if (cur_dis < 0)
		return FALSE;
	}
    }
    while (!delete_any);
    return TRUE;
}



/*
 * Returns a pointer to next free space			-RAK-
 */
int m_pop(void)
{
    /* Out of space?  Compact. */
    if (mfptr == MAX_M_IDX) if (!compact_monsters())

    /* XXX XXX XXX XXX Out of memory! */    
    return (-1);

    /* Return (and increase) free slot */
    return (mfptr++);
}




/*
 * Places a monster at given location
 */
int place_monster(int y, int x, int z, int slp)
{
    register int           cur_pos, j, ny, nx, count;
    register monster_type *mon_ptr;
    char                   buf[100];

    /* another paranoia check -CFT */
    if ((z < 0) || (z >= MAX_R_IDX)) return FALSE;

    /* YA paranoia check -CFT */
    if (!test_place(y, x)) return FALSE;

    if (r_list[z].cdefense & UNIQUE) {
	if (u_list[z].exist) {

	/* Note for wizard */
	if (wizard) {
	    (void)sprintf(buf, "Tried to create %s but exists.",
			  r_list[z].name);
	    msg_print(buf);
	}

	/* Cannot create */
	return FALSE;
    }

    if (u_list[z].dead) {

	/* Note for wizard */
	if (wizard) {
	    (void)sprintf(buf, "Tried to create %s but dead.",
	    		  r_list[z].name);
	    msg_print(buf);
	}

	/* Cannot create */
	return FALSE;
    }

	u_list[z].exist = 1;
    }

    /* from um55, paranoia error check... */
    cur_pos = m_pop();

    /* Mega-Paranoia */
    if (cur_pos == -1) return FALSE;

    /* Note the monster */
    if ((wizard || peek) && (r_list[z].cdefense & UNIQUE)) {
	msg_print(r_list[z].name);
    }
    
    /* Powerful monster */
    if (r_list[z].level > (unsigned)dun_level) {
	int                 c;

	rating += ((c = r_list[z].level - dun_level) > 30) ? 15 : c / 2;
	if (r_list[z].cdefense & UNIQUE) {
	/* Normal monsters are worth "half" as much */
	    rating += (r_list[z].level - dun_level) / 2;
	}
    }

    /* Get a new monster record */
    mon_ptr = &m_list[cur_pos];

    /* Place the monster at the location */
    mon_ptr->fy = y;
    mon_ptr->fx = x;

    mon_ptr->mptr = z;

    /* Assign maximal hitpoints */
    if ((r_list[z].cdefense & MAX_HP) || be_nasty) {
	mon_ptr->hp = max_hp(r_list[z].hd);
    }
    else {
	mon_ptr->hp = pdamroll(r_list[z].hd);
    }

    /* And start out fully healthy */
    mon_ptr->maxhp = mon_ptr->hp;

    /* Extract the monster base speed */
    mon_ptr->mspeed = r_list[z].speed - 10;
    
    /* No "damage" yet */
    mon_ptr->stunned = 0;
    mon_ptr->confused = 0;
    mon_ptr->monfear = 0;

    mon_ptr->cdis = distance(char_row, char_col, y, x);

    /* Default to invisible */
    mon_ptr->ml = FALSE;

    cave[y][x].cptr = cur_pos;

    /* Update the monster sleep info */
    if (slp) {
	if (r_list[z].sleep == 0) {
	    mon_ptr->csleep = 0;
	}
	else {
	    mon_ptr->csleep = ((int)r_list[z].sleep * 2) +
			     randint((int)r_list[z].sleep * 10);
	}
    } else

    /* to give the player a sporting chance, any monster that appears in */
    /* line-of-sight and can cast spells or breathe, should be asleep.   */
    /* This is an extension of Um55's sleeping dragon code...            */

    /* if asleep only to prevent summon-breathe-breathe-breathe-die, then don't sleep long -CFT */
    if (((r_list[z].spells & (CAUSE_LIGHT|CAUSE_SERIOUS|HOLD_PERSON|
                                  BLINDNESS|CONFUSION|FEAR|SLOW|BREATH_L|
                                  BREATH_G|BREATH_A|BREATH_FR|BREATH_FI|
                                  FIRE_BOLT|FROST_BOLT|ACID_BOLT|MAG_MISS|
                                  CAUSE_CRIT|FIRE_BALL|FROST_BALL|MANA_BOLT))
          || (r_list[z].spells2 & (BREATH_CH|BREATH_SH|BREATH_SD|BREATH_CO|
                                  BREATH_DI|BREATH_LD|LIGHT_BOLT|LIGHT_BALL|
                                  ACID_BALL|TRAP_CREATE|RAZOR|MIND_BLAST|
                                  MISSILE|PLASMA_BOLT|NETHER_BOLT|ICE_BOLT|
                                  FORGET|BRAIN_SMASH|ST_CLOUD|TELE_LEV|
                                  WATER_BOLT|WATER_BALL|NETHER_BALL|BREATH_NE))
          || (r_list[z].spells3 & (BREATH_WA|BREATH_SL|BREATH_LT|BREATH_TI|
                                  BREATH_GR|BREATH_DA|BREATH_PL|ARROW|
                                  DARK_STORM|MANA_STORM)))
       && los(y, x, char_row, char_col)) {
	mon_ptr->csleep = randint(4);
    }

    /* light up the monster if we can see it... -CFT */
    else {
	mon_ptr->csleep = 0;
    }

    update_mon(cur_pos);

    /* Unique kobolds, Liches, orcs, Ogres, Trolls, yeeks, and demons */
    /* get a "following" of escorts.  -DGK-    But not skeletons, */
    /* because that would include druj, making Cantoras amazingly tough -CFT */

    if (r_list[z].cdefense & UNIQUE) {

	j = r_list[z].cchar;

	/* Monsters with escorts */
	if ((j=='k')||(j=='L')||(j=='o')||(j=='O')||(j=='T')||(j=='y')|| (j=='I')||(j=='&')) {

	    /* Try for the highest level monster we can get */
	    for (cur_pos=MAX_R_IDX-1;cur_pos>=0;cur_pos--) {

		/* Find a similar, lower level, non-unique, monster */
		if ((r_list[cur_pos].cchar==j) &&
		    (r_list[cur_pos].level<=r_list[z].level) &&
		    !(r_list[cur_pos].cdefense & UNIQUE)) {

		    /* Try up to 50 nearby places */
		    count = 0;
		    do {
			ny = y+randint(7)-4;
			nx = x+randint(7)-4;
			count++;
		    } while (!test_place(ny,nx) && (count<51));

		    /* Certain monsters come in groups */
		    if ((j=='k') || (j=='y') || (j=='&') ||
			(r_list[cur_pos].cdefense&GROUP)) {
			place_group(ny,nx,cur_pos,slp);
		    }

		    /* Otherwise, just use a single escort */
		    else {
			place_monster(ny,nx,cur_pos,slp);
		    }
		}
	    }
	}
    }

    /* Success */
    return TRUE;
}


/*
 * Places a monster at given location	    -RAK-
 */
int place_win_monster()
{
    register int y, x, cur_pos;
    register monster_type *mon_ptr;

    if (!total_winner) {
	cur_pos = m_pop();
    /* paranoia error check, from um55 -CFT */
	if (cur_pos == -1) return FALSE;

    /* Attempt to place */
    if (wizard || peek) {
	msg_print("Placing win monster");
    }

	mon_ptr = &m_list[cur_pos];

    /* Find a legal, distant, unoccupied, space */
	do {

	/* Pick a location */
	    y = randint(cur_height - 2);
	    x = randint(cur_width - 2);
	}
	while ((cave[y][x].fval >= MIN_CLOSED_SPACE) || (cave[y][x].cptr != 0)
	       || (cave[y][x].tptr != 0) ||
	       (distance(y, x, char_row, char_col) <= MAX_SIGHT));

	mon_ptr->fy = y;
	mon_ptr->fx = x;
	mon_ptr->mptr = MAX_R_IDX - 2;
	if (r_list[mon_ptr->mptr].cdefense & MAX_HP)
	    mon_ptr->hp = max_hp(r_list[mon_ptr->mptr].hd);
	else
	    mon_ptr->hp = pdamroll(r_list[mon_ptr->mptr].hd);
	mon_ptr->mspeed = r_list[mon_ptr->mptr].speed - 10;
	mon_ptr->stunned = 0;
	mon_ptr->cdis = distance(char_row, char_col, y, x);
	cave[y][x].cptr = cur_pos;
	mon_ptr->csleep = 0;
    }
    return TRUE;
}


static char *cap(char *str)
{
    if ((*str >= 'a') && (*str <= 'z'))
	*str = *str - 'a' + 'A';
    return str;
}

/*
 * Prepare the "ghost" monster_race info
 *
 */
void set_ghost(monster_race *g, char *name, int r, int c, int l)
{
    char ghost_race[20];
    char ghost_class[20];
    int  i;

    /* Allocate storage for name -TL -- braindamaged ghost name spoo -CWS */
    if (r_list[MAX_R_IDX - 1].name == NULL) {
	r_list[MAX_R_IDX - 1].name = (char*)malloc(101);
	bzero((char *) r_list[MAX_R_IDX - 1].name, 101);
	*((char *) r_list[MAX_R_IDX - 1].name) = 'A';
    }

    switch (r) {
      case 0:
      case 8:
	strcpy(ghost_race, "human");
	break;
      case 1:
      case 2:
      case 9:
	strcpy(ghost_race, "elf");
	break;
      case 3:
	strcpy(ghost_race, "hobbit");
	break;
      case 4:
	strcpy(ghost_race, "gnome");
	break;
      case 5:
	strcpy(ghost_race, "dwarf");
	break;
      case 6:
	strcpy(ghost_race, "orc");
	break;
      case 7:
	strcpy(ghost_race, "troll");
	break;
    }
    switch (c) {
      case 0:
	strcpy(ghost_class, "warrior");
	break;
      case 1:
	strcpy(ghost_class, "mage");
	break;
      case 2:
	strcpy(ghost_class, "priest");
	break;
      case 3:
	strcpy(ghost_class, "rogue");
	break;
      case 4:
	strcpy(ghost_class, "ranger");
	break;
      case 5:
	strcpy(ghost_class, "paladin");
	break;
    }

    /* Save the level, extract the experience */
    g->level = l;
    g->mexp = l * 5 + 5;

    /* Never asleep (?) */
    g->sleep = 0;

    /* Very attentive (?) */
    g->aaf = 100;


    g->spells2 = NONE8;


    /* Town ghost */
    if (!dun_level) {

	/* A wanderer in the town */
	sprintf((char *)g->name, "%s, the %s %s", cap(name),
		cap(ghost_race), cap(ghost_class));

	g->cmove |= (THRO_DR | MV_ATT_NORM | CARRY_OBJ | HAS_90 | HAS_60 | GOOD);

	if (l > 10) g->cmove |= (HAS_1D2);
	if (l > 18) g->cmove |= (HAS_2D2);
	if (l > 23) g->cmove |= (HAS_4D2);
	if (l > 40) {
	    g->cmove |= (SPECIAL);
	    g->cmove &= (~HAS_4D2);
	}

	/* Add some random resists -DGK */
	for (i = 0; i <= (l / 5); i++) {
	    switch ((int) randint(13)) {
	      case 1:
	      case 2:
	      case 3:
		g->cdefense |= (IM_FIRE);
	      case 4:
	      case 5:
	      case 6:
		g->cdefense |= (IM_ACID);
	      case 7:
	      case 8:
	      case 9:
		g->cdefense |= (IM_FROST);
	      case 10:
	      case 11:
	      case 12:
		g->cdefense |= (IM_LIGHTNING);
	      case 13:
		g->cdefense |= (IM_POISON);
	    }
	}

	switch (c) {
	  case 0:		   /* Warrior */
	    g->spells = NONE8;
	    break;
	  case 1:		   /* Mage */
	    g->spells |= (0x3L | BLINK | MAG_MISS |
			   SLOW | CONFUSION);
	    if (l > 5) g->spells2 |= ST_CLOUD;
	    if (l > 7) g->spells2 |= LIGHT_BOLT;
	    if (l > 10) g->spells |= FROST_BOLT;
	    if (l > 12) g->spells |= TELE;
	    if (l > 15) g->spells |= ACID_BOLT;
	    if (l > 20) g->spells |= FIRE_BOLT;
	    if (l > 25) g->spells |= FROST_BALL;
	    if (l > 25) g->spells2 |= HASTE;
	    if (l > 30) g->spells |= FIRE_BALL;
	    if (l > 40) g->spells |= MANA_BOLT;
	    break;
	  case 3:		   /* Rogue */
	    g->spells |= (0x5L | BLINK);
	    if (l > 10) g->spells |= CONFUSION;
	    if (l > 18) g->spells |= SLOW;
	    if (l > 25) g->spells |= TELE;
	    if (l > 30) g->spells |= HOLD_PERSON;
	    if (l > 35) g->spells |= TELE_TO;
	    break;
	  case 4:		   /* Ranger */
	    g->spells |= (0x8L | MAG_MISS);
	    if (l > 5) g->spells2 |= ST_CLOUD;
	    if (l > 7) g->spells2 |= LIGHT_BOLT;
	    if (l > 10) g->spells |= FROST_BOLT;
	    if (l > 18) g->spells |= ACID_BOLT;
	    if (l > 25) g->spells |= FIRE_BOLT;
	    if (l > 30) g->spells |= FROST_BALL;
	    if (l > 35) g->spells |= FIRE_BALL;
	    break;
	  case 2:		   /* Priest */
	  case 5:		   /* Paladin */
	    g->spells |= (0x4L | CAUSE_LIGHT | FEAR);
	    if (l > 5) g->spells2 |= HEAL;
	    if (l > 10) g->spells |= (CAUSE_SERIOUS | BLINDNESS);
	    if (l > 18) g->spells |= HOLD_PERSON;
	    if (l > 25) g->spells |= CONFUSION;
	    if (l > 30) g->spells |= CAUSE_CRIT;
	    if (l > 35) g->spells |= MANA_DRAIN;
	    break;
	}

	g->cdefense |= (CHARM_SLEEP | EVIL);
	if (r == 6) g->cdefense |= ORC;
	if (r == 7) g->cdefense |= TROLL;

	g->ac = 15 + randint(15);
	if (c == 0 || c >= 3) g->ac += randint(60);

	/* High level mages and rogues are fast... */
	if ((c == 1 || c == 3) && l > 25) g->speed = 12;
	else g->speed = 11;

	/* Use the letter 'p' */
	g->cchar = 'p';

	/* XXX */
	g->hd[1] = 1;

	g->damage[0] = 5 + ((l > 18) ? 18 : l);
	g->damage[1] = g->damage[0];

	switch (c) {
	  case 0:
	    g->damage[2] = ((l < 30) ? (5 + ((l > 18) ? 18 : l)) : 235);
	    g->damage[3] = g->damage[2];
	    break;
	  case 1:
	  case 2:
	    g->damage[2] = 0;
	    g->damage[3] = 0;
	    break;
	  case 3:
	    g->damage[2] = g->damage[3] = ((l < 30) ? 149 : 232);
	    break;
	  case 4:
	  case 5:
	    g->damage[2] = g->damage[3] = g->damage[1];
	    break;
	}

	return;
    }
    /* Make a ghost with power based on the ghost level */
    switch ((int) (g->level / 4) + randint(3)) {

      case 1:
      case 2:
      case 3:
	sprintf((char *)g->name, "%s, the Skeleton %s", name, ghost_race);
	g->cmove |= (THRO_DR | MV_ATT_NORM | CARRY_OBJ | HAS_90 | GOOD);
	g->spells |= (NONE8);
	g->cdefense |= (IM_POISON | CHARM_SLEEP | UNDEAD | EVIL | IM_FROST | NO_INFRA);
	if (r == 6) g->cdefense |= ORC;
	if (r == 7) g->cdefense |= TROLL;
	g->ac = 26;
	g->speed = 11;
	g->cchar = 's';
	g->hd[1] = 1;
	g->damage[0] = 5;
	g->damage[1] = 5;
	g->damage[2] = 0;
	g->damage[3] = 0;
	break;

      case 4:
      case 5:
	sprintf((char *)g->name, "%s, the %s zombie", name, cap(ghost_race));
	g->cmove |= (THRO_DR | MV_ATT_NORM | CARRY_OBJ | HAS_60 | HAS_90 | GOOD);
	g->spells |= (NONE8);
	g->cdefense |= (IM_POISON | CHARM_SLEEP | UNDEAD | EVIL | NO_INFRA);
	if (r == 6) g->cdefense |= ORC;
	if (r == 7) g->cdefense |= TROLL;
	g->ac = 30;
	g->speed = 11;
	g->cchar = 'z';
	g->hd[1] *= 2;
	g->damage[0] = 8;
	g->damage[1] = 0;
	g->damage[2] = 0;
	g->damage[3] = 0;
	break;

      case 6:
	sprintf((char *) g->name, "%s, the Poltergeist", name);
	g->cmove |= (MV_INVIS | MV_ATT_NORM | CARRY_OBJ | GOOD | HAS_1D2 | MV_75 | THRO_WALL);
	g->spells |= (NONE8);
	g->cdefense |= (IM_POISON | CHARM_SLEEP | UNDEAD | EVIL | IM_FROST | NO_INFRA);
	g->ac = 20;
	g->speed = 13;
	g->cchar = 'G';
	g->damage[0] = 5;
	g->damage[1] = 5;
	g->damage[2] = 93;
	g->damage[3] = 93;
	g->mexp = (g->mexp * 3) / 2;
	break;

      case 7:
      case 8:
	sprintf((char *)g->name, "%s, the Mummified %s", name, cap(ghost_race));
	g->cmove |= (MV_ATT_NORM | CARRY_OBJ | HAS_1D2 | GOOD);
	g->spells |= (NONE8);
	g->cdefense |= (CHARM_SLEEP | UNDEAD | EVIL | IM_POISON | NO_INFRA);
	if (r == 6) g->cdefense |= ORC;
	if (r == 7) g->cdefense |= TROLL;
	g->ac = 35;
	g->speed = 11;
	g->cchar = 'M';
	g->hd[1] *= 2;
	g->damage[0] = 16;
	g->damage[1] = 16;
	g->damage[2] = 16;
	g->damage[3] = 0;
	g->mexp = (g->mexp * 3) / 2;
	break;

      case 9:
      case 10:
	sprintf((char *)g->name, "%s%s spirit", name,
		(name[strlen(name) - 1] == 's') ? "'" : "'s");
	g->cmove |= (MV_INVIS | THRO_WALL | MV_ATT_NORM | CARRY_OBJ | HAS_1D2 | GOOD);
	g->spells |= (NONE8);
	g->cdefense |= (CHARM_SLEEP | UNDEAD | EVIL | IM_POISON | IM_FROST | NO_INFRA);
	g->ac = 20;
	g->speed = 11;
	g->cchar = 'G';
	g->hd[1] *= 2;
	g->damage[0] = 19;
	g->damage[1] = 185;
	g->damage[2] = 99;
	g->damage[3] = 178;
	g->mexp = g->mexp * 3;
	break;

      case 11:
	sprintf((char *)g->name, "%s%s ghost", name, (name[strlen(name) - 1] == 's') ?
		"'" : "'s");
	g->cmove |= (MV_INVIS | THRO_WALL | MV_ATT_NORM | CARRY_OBJ | HAS_1D2 | GOOD);
	g->spells |= (0xFL | HOLD_PERSON | MANA_DRAIN | BLINDNESS);
	g->cdefense |= (CHARM_SLEEP | UNDEAD | EVIL | IM_POISON | IM_FROST | NO_INFRA);
	g->ac = 40;
	g->speed = 12;
	g->cchar = 'G';
	g->hd[1] *= 2;
	g->damage[0] = 99;
	g->damage[1] = 99;
	g->damage[2] = 192;
	g->damage[3] = 184;
	g->mexp = (g->mexp * 7) / 2;
	break;

      case 12:
	sprintf((char *) g->name, "%s, the Vampire", name);
	g->cmove |= (THRO_DR | MV_ATT_NORM | CARRY_OBJ | HAS_2D2 | GOOD);
	g->spells |= (0x8L | HOLD_PERSON | FEAR | TELE_TO | CAUSE_SERIOUS);
	g->cdefense |= (CHARM_SLEEP | UNDEAD | EVIL | IM_POISON | NO_INFRA | HURT_LIGHT);
	g->ac = 40;
	g->speed = 11;
	g->cchar = 'V';
	g->hd[1] *= 3;
	g->damage[0] = 20;
	g->damage[1] = 20;
	g->damage[2] = 190;
	g->damage[3] = 0;
	g->mexp = g->mexp * 3;
	break;

      case 13:
	sprintf((char *)g->name, "%s%s Wraith", name,
		(name[strlen(name) - 1] == 's') ? "'" : "'s");
	g->cmove |= (THRO_DR | MV_ATT_NORM | CARRY_OBJ | HAS_4D2 | HAS_2D2 | GOOD);
	g->spells |= (0x7L | HOLD_PERSON | FEAR | BLINDNESS | CAUSE_CRIT);
	g->spells2 |= (NETHER_BOLT);
	g->cdefense |= (CHARM_SLEEP | UNDEAD | EVIL | IM_POISON | IM_FROST | NO_INFRA |
			HURT_LIGHT);
	g->ac = 60;
	g->speed = 12;
	g->cchar = 'W';
	g->hd[1] *= 3;
	g->damage[0] = 20;
	g->damage[1] = 20;
	g->damage[2] = 190;
	g->damage[3] = 0;
	g->mexp = g->mexp * 5;
	break;

      case 14:
	sprintf((char *) g->name, "%s, the Vampire Lord", name);
	g->cmove |= (THRO_DR | MV_ATT_NORM | CARRY_OBJ | HAS_1D2 | SPECIAL);
	g->spells |= (0x8L | HOLD_PERSON | FEAR | TELE_TO | CAUSE_CRIT);
	g->spells2 |= (NETHER_BOLT);
	g->cdefense |= (CHARM_SLEEP | UNDEAD | EVIL | IM_POISON | NO_INFRA | HURT_LIGHT);
	g->ac = 80;
	g->speed = 11;
	g->cchar = 'V';
	g->hd[1] *= 2;
	g->hd[0] = (g->hd[0] * 5) / 2;
	g->damage[0] = 20;
	g->damage[1] = 20;
	g->damage[2] = 20;
	g->damage[3] = 198;
	g->mexp = g->mexp * 20;
	break;

      case 15:
	sprintf((char *)g->name, "%s%s ghost", name,
		 (name[strlen(name) - 1] == 's') ? "'" : "'s");
	g->cmove |= (MV_INVIS | THRO_WALL | MV_ATT_NORM | CARRY_OBJ | HAS_2D2 | SPECIAL);
	g->spells |= (0x5L | HOLD_PERSON | MANA_DRAIN | BLINDNESS | CONFUSION);
	g->cdefense |= (CHARM_SLEEP | UNDEAD | EVIL | IM_FROST | IM_POISON | NO_INFRA);
	g->ac = 90;
	g->speed = 13;
	g->cchar = 'G';
	g->hd[1] *= 3;
	g->damage[0] = 99;
	g->damage[1] = 99;
	g->damage[2] = 192;
	g->damage[3] = 184;
	g->mexp = g->mexp * 20;
	break;

      case 17:
	sprintf((char *)g->name, "%s, the Lich", name);
	g->cmove |= (THRO_DR | MV_ATT_NORM | CARRY_OBJ | HAS_2D2 | HAS_1D2 | SPECIAL);
	g->spells |= (0x3L | FEAR | CAUSE_CRIT | TELE_TO | BLINK |
		       S_UNDEAD | FIRE_BALL | FROST_BALL | HOLD_PERSON |
		       MANA_DRAIN | BLINDNESS | CONFUSION | TELE);
	g->spells2 |= (BRAIN_SMASH | RAZOR);
	g->cdefense |= (CHARM_SLEEP | UNDEAD | EVIL | IM_FROST | NO_INFRA | IM_POISON
			| INTELLIGENT);
	g->ac = 120;
	g->speed = 12;
	g->cchar = 'L';
	g->hd[1] *= 3;
	g->hd[0] *= 2;
	g->damage[0] = 181;
	g->damage[1] = 201;
	g->damage[2] = 214;
	g->damage[3] = 181;
	g->mexp = g->mexp * 50;
	break;

      default:
	sprintf((char *)g->name, "%s%s ghost", name,
		(name[strlen(name) - 1] == 's') ? "'" : "'s");
	g->cmove |= (MV_INVIS | THRO_WALL | MV_ATT_NORM | CARRY_OBJ |
		     HAS_1D2 | HAS_2D2 | SPECIAL);
	g->spells |= (0x2L | HOLD_PERSON | MANA_DRAIN | 
		       BLINDNESS | CONFUSION | TELE_TO);
	g->spells2 |= (NETHER_BOLT | NETHER_BALL | BRAIN_SMASH | TELE_LEV);
	g->cdefense |= (CHARM_SLEEP | UNDEAD | EVIL | IM_POISON | IM_FROST | NO_INFRA |
			INTELLIGENT);
	g->ac = 130;
	g->speed = 13;
	g->cchar = 'G';
	g->hd[1] *= 2;
	g->hd[0] = (g->hd[0] * 5) / 2;
	g->damage[0] = 99;
	g->damage[1] = 99;
	g->damage[2] = 192;
	g->damage[3] = 184;
	g->mexp = g->mexp * 30;
	break;
    }
}



/*
 * Places a ghost somewhere.
 * Probably not the best possible algorithm.
 */
int place_ghost()
{
    register int           y, x, cur_pos;
    register monster_type *mon_ptr;
    monster_race         *ghost = &r_list[MAX_R_IDX - 1];
    char                   tmp[100];
    char                   name[100];
    int                    i, j, level;
    int                    ghost_race;
    int                    cl;
    FILE		   *fp;

    /* In the town, ghosts have the same level as the player */
    if (!dun_level) {

	/* You have to be level 5, and even then its only 10% */
	if (p_ptr->misc.lev < 5 || randint(10) > 1) return 0;

	/* Look for a proper bones file */
	sprintf(tmp, "%s/%d", ANGBAND_BONES, p_ptr->misc.lev);
	if ((fp = my_tfopen(tmp, "r")) != NULL) {

	/* Read the bones info */
	if (fscanf(fp, "%[^\n]\n%d\n%d\n%d", name, &i, &ghost_race, &cl) < 4) {
	    fclose(fp);
	    if (wizard) msg_print("Town:Failed to scan in info properly!");
	    return 0;
	}

	fclose(fp);

    /* Break up the hitpoints */
	    j = 1;
	    if (i > 255) {	   /* avoid wrap-around of byte hitdice, by factoring */
		j = i / 32;
		i = 32;
	    }
	    ghost->hd[0] = i;	   /* set_ghost may adj for race/class/lv */
	    ghost->hd[1] = j;
	    level = p_ptr->misc.lev;
	} else {
	    return 0;
	}
    }

    /* In the dungeon, ghosts have the same level as the level */    
    else {

	/* And even then, it only happens sometimes */
	if (14 > randint((dun_level / 2) + 11)) return 0;

	/* Or rather, 1/3 of that often :-) */
	if (randint(3) == 1) {

	/* Open the bones file */
	sprintf(tmp, "%s/%d", ANGBAND_BONES, dun_level);
	if ((fp = my_tfopen(tmp, "r")) != NULL) {

	if (fscanf(fp, "%[^\n]\n%d\n%d\n%d", name, &i, &ghost_race, &cl) < 4) {
	    fclose(fp);
	    if (wizard) msg_print("Ghost:Failed to scan in info properly!");
	    return 0;
	}
	fclose(fp);

    /* Break up the hitpoints */
		j = 1;
		if (i > 255) {	   /* avoid wrap-around of byte hitdice, by factoring */
		j = i / 32;
		i = 32;
		}

    /* set_ghost may adj for race/class/lv */
		ghost->hd[0] = i;
		ghost->hd[1] = j;

		level = dun_level;
	    } else return 0;
	} else return 0;
    }

    /* Set up the ghost */
    set_ghost(ghost, name, ghost_race, cl, level);

    /* Note for wizard (special ghost name) */
    if (wizard || peek) msg_print(ghost->name);

    cur_pos = m_pop();
    mon_ptr = &m_list[cur_pos];

    /* Hack -- pick a nice (far away) location */
    do {

	/* Pick a location */
	y = randint(cur_height - 2);
	x = randint(cur_width - 2);
    } while ((cave[y][x].fval >= MIN_CLOSED_SPACE) || (cave[y][x].cptr != 0)
	     || (cave[y][x].tptr != 0) ||
	     (distance(y, x, char_row, char_col) <= MAX_SIGHT));

    mon_ptr->fy = y;
    mon_ptr->fx = x;

    mon_ptr->mptr = (MAX_R_IDX - 1);

    mon_ptr->hp = (s16b) ghost->hd[0] * (s16b) ghost->hd[1];

    /* the r_list speed value is 10 greater, so that it can be a byte */
    mon_ptr->mspeed = r_list[mon_ptr->mptr].speed - 10;

    mon_ptr->stunned = 0;

    mon_ptr->cdis = distance(char_row, char_col, y, x);
    cave[y][x].cptr = cur_pos;
    mon_ptr->csleep = 0;

    return 1;
}




/*
 * Return a monster suitable to be placed at a given level.  This makes high
 * level monsters (up to the given level) slightly more common than low level
 * monsters at any given level.   -CJS- 
 */
int get_mons_num(int level)
{
    register int i, j, num;

    int          old = level;

again:
    if (level == 0) {
	    i = randint(m_level[0]) - 1;
	}

	else {

	    if (level > MAX_MONS_LEVEL) level = MAX_MONS_LEVEL;

	    /* Make a Nasty Monster */
	    if (randint(MON_NASTY) == 1) {
		/* be a little more civilized about monster depths for the first levels -CWS */
		i = level / 4 + 1;
		if (i > 4) i = 4;
		level = level + MY_ABS(i) + 1;
		if (level > MAX_MONS_LEVEL) level = MAX_MONS_LEVEL;
	    }
	    else {
	/* This code has been added to make it slightly more likely to get
	 * the higher level monsters. Originally a uniform distribution over
	 * all monsters of level less than or equal to the dungeon level.
	 * This distribution makes a level n monster occur approx 2/n% of the
	 * time on level n, and 1/n*n% are 1st level. */
		num = m_level[level] - m_level[0];
		i = randint(num) - 1;
		j = randint(num) - 1;
		if (j > i) i = j;
		level = r_list[i + m_level[0]].level;
	    }

	    /* Bizarre function */            
	    i = m_level[level] - m_level[level - 1];
	    if (i == 0) i++;
	    i = randint(i) - 1 + m_level[level - 1];
	}

	/* Uniques never appear out of "modified" depth */
	if ((r_list[i].level > old) &&
	    (r_list[i].cdefense & UNIQUE)) {
	    goto again;
	}

	/* Quest Monsters never appear out of depth */
	if ((r_list[i].level > (unsigned) dun_level) &&
	    (r_list[i].cdefense & QUESTOR)) {
	    goto again;
	}
    return i;
}


/*
 * Get a monster race index.  Method 2.
 */
int get_nmons_num(int level)
{
    register int i, j, num;
    int          old;

    old = level;

again:

	if (level == 0) {
	    i = randint(m_level[0]) - 1;
	}

	else {

	    if (level > MAX_MONS_LEVEL) level = MAX_MONS_LEVEL;

	    num = m_level[level] - m_level[0];

	    i = rand_int(num);
	    i += 15;
	    if (i >= num) i = num - 1;

	    j = rand_int(num);
	    if (j > i) i = j;

	    j = rand_int(num);
	    if (j > i) i = j;

	    level = r_list[i + m_level[0]].level;
	    i = m_level[level] - m_level[level - 1];
	    if (i == 0) i = 1;

	    i = randint(i) - 1 + m_level[level - 1];
	}

	if ((r_list[i].level > old) && (r_list[i].cdefense & UNIQUE)) {
	    goto again;
	}

	if (( r_list[i].level > (unsigned) dun_level) &&
	    (r_list[i].cdefense & QUESTOR)) {
	    goto again;
	}

    /* Accept the monster */
    return i;
}


void place_group(int y, int x, int mon, int slp)
{
    /* prevent level rating from skyrocketing if they are out of depth... */
    int old = rating;
    int extra = 0;

    /* reduce size of group if out-of-depth */
    if (r_list[mon].level > (unsigned) dun_level) {
	extra = (-randint(r_list[mon].level - dun_level));
    }

    /* if monster is deeper than normal, then travel in bigger packs -CFT */
    else if (r_list[mon].level < (unsigned) dun_level) {
	extra = randint(dun_level - r_list[mon].level);
    }

    /* put an upper bounds on it... -CFT */
    if (extra > 12) extra = 12;

    switch (randint(13) + extra) {
      case 25:
	place_monster(y, x - 3, mon, 0);
      case 24:
	place_monster(y, x + 3, mon, 0);
      case 23:
	place_monster(y - 3, x, mon, 0);
      case 22:
	place_monster(y + 3, x, mon, 0);
      case 21:
	place_monster(y - 2, x + 1, mon, 0);
      case 20:
	place_monster(y + 2, x - 1, mon, 0);
      case 19:
	place_monster(y + 2, x + 1, mon, 0);
      case 18:
	place_monster(y - 2, x - 1, mon, 0);
      case 17:
	place_monster(y + 1, x + 2, mon, 0);
      case 16:
	place_monster(y - 1, x - 2, mon, 0);
      case 15:
	place_monster(y + 1, x - 2, mon, 0);
      case 14:
	place_monster(y - 1, x + 2, mon, 0);
      case 13:
	place_monster(y, x - 2, mon, 0);
      case 12:
	place_monster(y, x + 2, mon, 0);
      case 11:
	place_monster(y + 2, x, mon, 0);
      case 10:
	place_monster(y - 2, x, mon, 0);
      case 9:
	place_monster(y + 1, x + 1, mon, 0);
      case 8:
	place_monster(y + 1, x - 1, mon, 0);
      case 7:
	place_monster(y - 1, x - 1, mon, 0);
      case 6:
	place_monster(y - 1, x + 1, mon, 0);
      case 5:
	place_monster(y, x + 1, mon, 0);
      case 4:
	place_monster(y, x - 1, mon, 0);
      case 3:
	place_monster(y + 1, x, mon, 0);
      case 2:
	place_monster(y - 1, x, mon, 0);
	rating = old;
      case 1:
      default:			   /* just in case I screwed up -CFT */
	place_monster(y, x, mon, 0);
    }
}


/*
 * Allocates some random monsters   -RAK-	 
 * Place the monsters at least "dis" distance from the player.
 * Use "slp" to choose the initial "sleep" status
 */
void alloc_monster(int num, int dis, int slp)
{
    register int y, x, i;
    int          mon;

    for (i = 0; i < num; i++) {

	/* Find a legal, distant, unoccupied, space */
	do {
	    /* Pick a location */
	    y = randint(cur_height - 2);
	    x = randint(cur_width - 2);
	}
	while (cave[y][x].fval >= MIN_CLOSED_SPACE || (cave[y][x].cptr != 0) ||
	       (distance(y, x, char_row, char_col) <= dis));

	do {
	    mon = get_mons_num(dun_level);
	} while (randint(r_list[mon].rarity) > 1);

    /*
     * to give the player a sporting chance, any monster that appears in
     * line-of-sight and can cast spells or breathe, should be asleep. This
     * is an extension of Um55's sleeping dragon code... 
     */
	if (((r_list[mon].spells & (CAUSE_LIGHT | CAUSE_SERIOUS | HOLD_PERSON |
			    BLINDNESS | CONFUSION | FEAR | SLOW | BREATH_L |
			       BREATH_G | BREATH_A | BREATH_FR | BREATH_FI |
			     FIRE_BOLT | FROST_BOLT | ACID_BOLT | MAG_MISS |
			   CAUSE_CRIT | FIRE_BALL | FROST_BALL | MANA_BOLT))
	     || (r_list[mon].spells2 & (BREATH_CH | BREATH_SH | BREATH_SD | BREATH_CO |
			   BREATH_DI | BREATH_LD | LIGHT_BOLT | LIGHT_BALL |
			      ACID_BALL | TRAP_CREATE | RAZOR | MIND_BLAST |
			    MISSILE | PLASMA_BOLT | NETHER_BOLT | ICE_BOLT |
				FORGET | BRAIN_SMASH | ST_CLOUD | TELE_LEV |
			 WATER_BOLT | WATER_BALL | NETHER_BALL | BREATH_NE))
	     || (r_list[mon].spells3 & (BREATH_WA | BREATH_SL | BREATH_LT | BREATH_TI |
				 BREATH_GR | BREATH_DA | BREATH_PL | ARROW |
					DARK_STORM | MANA_STORM)))
	    && (los(y, x, char_row, char_col))) {
	    slp = TRUE;
	}

	if (!(r_list[mon].cdefense & GROUP)) {
	    place_monster(y, x, mon, slp);
	}
	else {
	    place_group(y, x, mon, slp);
	}
    }
}


/*
 * Places a random creature at or adjacent to the given location
 * We modify the given location to hold the location used, if any.
 * We return TRUE if a monster (or group of monsters) was summoned.
 */
int summon_monster(int *y, int *x, int slp)
{
    register int        i, j, k;
    int                 l, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = get_mons_num(dun_level + MON_SUMMON_ADJ);
    do {

	/* Pick a nearby location */
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);

	/* Require legal grid */
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	
	/* Require "empty" floor grids */
	if (cave_ptr->fval <= MAX_OPEN_SPACE && (cave_ptr->cptr == 0)) {
	
	/* Place the monster */
	if (r_list[l].cdefense & GROUP) {
	    place_group(j, k, l, slp);
	}
	else {
	    place_monster(j, k, l, slp);
	}

		summon = TRUE;
		i = 9;

	/* Save the location */
	*y = j;
	*x = k;
		
	}
	}
	i++;
    }
    while (i <= 9);

    return (summon);
}


/*
 * Places undead adjacent to given location		-RAK-	 */
int summon_undead(int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_MONS_LEVEL];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if ((r_list[m].cdefense & UNDEAD) && !(r_list[m].cdefense & UNIQUE) &&
		(r_list[m].level < dun_level + 5)) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	} while (ctr <= 19);
    } while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (cave_ptr->fval <= MAX_OPEN_SPACE && (cave_ptr->cptr == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    } while (i <= 9);
    return (summon);
}

/*
 * As for summon undead */
int summon_demon(int lev, int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_MONS_LEVEL];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].cdefense & DEMON && !(r_list[m].cdefense & UNIQUE) &&
		(r_list[m].level <= lev)) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	} while (ctr <= 19);
    } while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (cave_ptr->fval <= MAX_OPEN_SPACE && (cave_ptr->cptr == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    } while (i <= 9);
    return (summon);
}

/*
 * As for summon demon:-) ~Ludwig */
int summon_dragon(int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_MONS_LEVEL];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].cdefense & DRAGON && !(r_list[m].cdefense & UNIQUE)) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	}
	while (ctr <= 19);
    }
    while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (cave_ptr->fval <= MAX_OPEN_SPACE && (cave_ptr->cptr == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    }
    while (i <= 9);
    return (summon);
}

/*
 * Summon ringwraiths */
int summon_wraith(int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_MONS_LEVEL];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].cchar == 'W' && (r_list[m].cdefense & UNIQUE)) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	}
	while (ctr <= 19);
    }
    while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (cave_ptr->fval <= MAX_OPEN_SPACE && (cave_ptr->cptr == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    }
    while (i <= 9);
    return (summon);
}

/*
 * Summon reptiles */
int summon_reptile(int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_MONS_LEVEL];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].cchar == 'R' && !(r_list[m].cdefense & UNIQUE)) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	}
	while (ctr <= 19);
    }
    while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (cave_ptr->fval <= MAX_OPEN_SPACE && (cave_ptr->cptr == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    }
    while (i <= 9);
    return (summon);
}


/*
 * As for summon dragon, but keys on character ~Decado */
int summon_spider(int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_MONS_LEVEL];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].cchar == 'S' && !(r_list[m].cdefense & UNIQUE)) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	}
	while (ctr <= 19);
    }
    while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (cave_ptr->fval <= MAX_OPEN_SPACE && (cave_ptr->cptr == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    }
    while (i <= 9);
    return (summon);
}

/*
 * As for summon dragon, but keys on character ~Decado */
int summon_angel(int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_MONS_LEVEL];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].cchar == 'A' && !(r_list[m].cdefense & UNIQUE)) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	} while (ctr <= 19);
    }
    while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (cave_ptr->fval <= MAX_OPEN_SPACE && (cave_ptr->cptr == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    } while (i <= 9);
    return (summon);
}

/*
 * Summon ants */
int summon_ant(int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_MONS_LEVEL];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].cchar == 'a' && !(r_list[m].cdefense & UNIQUE)) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	}
	while (ctr <= 19);
    }
    while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (cave_ptr->fval <= MAX_OPEN_SPACE && (cave_ptr->cptr == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    }
    while (i <= 9);
    return (summon);
}

/*
 * Summon uniques */
int summon_unique(int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_MONS_LEVEL];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (!(r_list[m].cchar == 'P') && (r_list[m].cdefense & UNIQUE)) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	}
	while (ctr <= 19);
    }
    while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (cave_ptr->fval <= MAX_OPEN_SPACE && (cave_ptr->cptr == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    }
    while (i <= 9);
    return (summon);
}

/*
 * Summon jabberwocks, for extra effect to the summon_unique spell */
int summon_jabberwock(int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_MONS_LEVEL];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].cchar == 'J' && !(r_list[m].cdefense & UNIQUE)) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	}
	while (ctr <= 19);
    }
    while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (cave_ptr->fval <= MAX_OPEN_SPACE && (cave_ptr->cptr == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    }
    while (i <= 9);
    return (summon);
}

/*
 * Summon greater undead */
int summon_gundead(int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_MONS_LEVEL];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if ((r_list[m].cchar == 'L') || (r_list[m].cchar == 'V')
		|| (r_list[m].cchar == 'W')) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	}
	while (ctr <= 19);
    }
    while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (cave_ptr->fval <= MAX_OPEN_SPACE && (cave_ptr->cptr == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    }
    while (i <= 9);
    return (summon);
}

/*
 * Summon ancient dragons */
int summon_ancientd(int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_MONS_LEVEL];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].cchar == 'D') {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	}
	while (ctr <= 19);
    }
    while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (cave_ptr->fval <= MAX_OPEN_SPACE && (cave_ptr->cptr == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    }
    while (i <= 9);
    return (summon);
}

/*
 * As for summon hound, but keys on character ~Decado */
int summon_hound(int *y, int *x)
{
    register int        i, j, k;
    int                 l, m, ctr, summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    l = m_level[MAX_MONS_LEVEL];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if ((r_list[m].cchar == 'C' || r_list[m].cchar == 'Z')
		&& !(r_list[m].cdefense & UNIQUE)) {
		ctr = 20;
		l = 0;
	    } else {
		m++;
		if (m > l)
		    ctr = 20;
		else
		    ctr++;
	    }
	}
	while (ctr <= 19);
    }
    while (l != 0);
    do {
	j = *y - 2 + randint(3);
	k = *x - 2 + randint(3);
	if (in_bounds(j, k)) {
	    cave_ptr = &cave[j][k];
	    if (cave_ptr->fval <= MAX_OPEN_SPACE && (cave_ptr->cptr == 0)) {
		place_monster(j, k, m, FALSE);
		summon = TRUE;
		i = 9;
		*y = j;
		*x = k;
	    }
	}
	i++;
    }
    while (i <= 9);
    return (summon);
}

/*
 * Place a sleepy jelly at the required coordinates ~Ludwig */
int summon_jelly(int *y, int *x)
{
    int l, m, summon;

    summon = FALSE;
    l = m_level[MAX_MONS_LEVEL];
    do {
	m = randint(l) - 1;
	if (r_list[m].cchar == 'J') {
	    summon = TRUE;
	    place_monster(*y, *x, m, TRUE);
	}
    } while (!summon);
    return (summon);
}

