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
 * Deletes a monster entry from the level. -RAK-
 */
void delete_monster(int j)
{
    register cave_type *c_ptr;
    register monster_type *m_ptr;
    register int fx, fy;

    /* Paranoia trouble? abort! -CFT */
    if (j < 2) return;

    /* Get the monster */
    m_ptr = &m_list[j];

    /* Get the monster location */
    fx = m_ptr->fx;
    fy = m_ptr->fy;

    if (r_list[m_ptr->mptr].cflags2 & MF2_UNIQUE) check_unique(m_ptr);

    /* One less of this monster on this level */
    l_list[m_ptr->r_idx].cur_num--;

    /* Forget that the monster is here */
    cave[fy][fx].cptr = 0;

    if (m_ptr->ml) lite_spot((int)m_ptr->fy, (int)m_ptr->fx);


#ifdef TARGET
    /* This targetting code stolen from Morgul -CFT */
    /* Targetted monster dead or compacted.      CDW */
    if (j == target_mon) target_mode = FALSE;
#endif

    /* One less monster */
    m_max--;

    /* Do structure dumping */
    if (j != m_max) {

#ifdef TARGET
	/* Targetted monster moved to replace dead or compacted monster CDW */
	if (target_mon == m_max-1) target_mon = j;
#endif

	/* Slide the last monster into the dead monster's slot */
	m_ptr = &m_list[m_max];
	c_ptr = &cave[m_ptr->fy][m_ptr->fx];
	c_ptr->cptr = j;
	m_list[j] = m_list[m_max];
    }

    /* Wipe the monster record */
    m_list[m_max] = blank_monster;
}




/*
 * The following two procedures implement the same function as delete
 * monster. However, they are used within process_monsters(), because deleting a
 * monster while scanning the m_list causes two problems, monsters might get
 * two turns, and m_ptr/monptr might be invalid after the delete_monster.
 * Hence the delete is done in two steps. 
 */
/*
 * fix1_delete_monster does everything delete_monster does except delete the
 * monster record and reduce m_max, this is called in breathe, and a couple
 * of places in creatures.c 
 */
void fix1_delete_monster(int j)
{
    register monster_type *m_ptr;

    /* Paranoia trouble? abort! -CFT */
    if (j < 2) return;

#ifdef TARGET
    /* Targetted monster dead or compacted.      CDW */
    if (j == target_mon) target_mode = FALSE;

    /* Targetted monster moved to replace dead or compacted monster   CDW */
    if (target_mon == m_max-1) target_mon = j;
#endif

    /* Get the monster */
    m_ptr = &m_list[j];

    if (r_list[m_ptr->mptr].cflags2 & MF2_UNIQUE) check_unique(m_ptr);

/* force the hp negative to ensure that the monster is dead, for example, if
 * the monster was just eaten by another, it will still have positive hit
 * points  */
    m_ptr->hp = (-1);

    cave[m_ptr->fy][m_ptr->fx].cptr = 0;
    if (m_ptr->ml) lite_spot((int)m_ptr->fy, (int)m_ptr->fx);
}


/*
 * fix2_delete_monster does everything in delete_monster that wasn't done by
 * fix1_monster_delete above, this is only called in process_monsters() 
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
    if (target_mon==m_max-1) target_mon = j; 
#endif

    m_ptr = &m_list[j];		   /* Fixed from a r_list ptr to a m_list ptr. -CFT */
    if (r_list[m_ptr->mptr].cflags2 & MF2_UNIQUE) check_unique(m_ptr);
    if (j != m_max - 1) {
	m_ptr = &m_list[m_max - 1];
	cave[m_ptr->fy][m_ptr->fx].cptr = j;
	m_list[j] = m_list[m_max - 1];
    }
    m_list[m_max - 1] = blank_monster;
    m_max--;
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
    monster_type	*m_ptr;

    msg_print("Compacting monsters...");


    /* Start 66 units away */
    cur_dis = 66;

    delete_any = FALSE;

    do {
	for (i = m_max - 1; i >= MIN_M_IDX; i--) {

	    m_ptr = &m_list[i];
	    if ((cur_dis < m_ptr->cdis) && (randint(3) == 1)) {
	    /* Don't compact Melkor! */
		if (r_list[m_ptr->mptr].cflags1 & MF1_WINNER)
		/* do nothing */
		    ;

	    /* in case this is called from within process_monsters(), this is a
	     * horrible hack, the m_list/process_monsters() code needs to be
	     * rewritten 
	     */
		else if (hack_m_idx < i) {
		    delete_monster(i);
		    delete_any = TRUE;
		} else

		/* fix1_delete_monster() does not decrement m_max, so don't
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
    if (m_max == MAX_M_IDX) if (!compact_monsters())

    /* XXX XXX XXX XXX Out of memory! */    
    return (-1);

    /* Return (and increase) free slot */
    return (m_max++);
}




/*
 * Places a monster at given location
 */
int place_monster(int y, int x, int r_idx, int slp)
{
    register int           cur_pos, j, ny, nx, count;
    register monster_type *m_ptr;
    char                   buf[100];

    /* Verify monster race another paranoia check -CFT */
    if ((r_idx < 0) || (r_idx >= MAX_R_IDX)) return FALSE;

    /* Verify location YA paranoia check -CFT */
    if (!test_place(y, x)) return FALSE;

    /* Get the race */
    r_ptr = &r_list[r_idx];

    /* See if we can make any more of them */
    if (l_list[r_idx].cur_num >= l_list[r_idx].max_num) {

	/* Note for wizard */
	if (wizard) {
	    (void)sprintf(buf, "Ignoring %s monster (%s).",
			  (l_list[r_idx].max_num ? "excessive" : "dead"),
			  r_ptr->name);
	    msg_print(buf);
	}

	/* Cannot create */
	return FALSE;
    }

    if (r_list[r_idx].cflags2 & MF2_UNIQUE) {
	if (u_list[r_idx].exist) {

	/* Note for wizard */
	if (wizard) {
	    (void)sprintf(buf, "Tried to create %s but exists.",
	    		  r_list[r_idx].name);
	    msg_print(buf);
	}

	/* Cannot create */
	return FALSE;
    }

    /* Count the monsters on the level */
    l_list[r_idx].cur_num++;

    /* from um55, paranoia error check... Get the next monster record */
    cur_pos = m_pop();

    /* Mega-Paranoia */
    if (cur_pos == -1) return FALSE;

    /* Note the monster */
    if ((wizard || peek) && (r_list[r_idx].cflags2 & MF2_UNIQUE)) {
	msg_print(r_list[r_idx].name);
    }
    
    /* Powerful monster */
    if (r_list[r_idx].level > (unsigned)dun_level) {
	int                 c;

	rating += ((c = r_list[r_idx].level - dun_level) > 30) ? 15 : c / 2;
	if (r_list[r_idx].cflags2 & MF2_UNIQUE) {
	/* Normal monsters are worth "half" as much */
	    rating += (r_list[r_idx].level - dun_level) / 2;
	}
    }

    /* Get a new monster record */
    m_ptr = &m_list[cur_pos];

    /* Place the monster at the location */
    m_ptr->fy = y;
    m_ptr->fx = x;

    /* Save the race */
    m_ptr->mptr = r_idx;

    /* Assign maximal hitpoints */
    if ((r_list[r_idx].cflags2 & MF2_MAX_HP) || be_nasty) {
	m_ptr->hp = max_hp(r_list[r_idx].hd);
    }
    else {
	m_ptr->hp = pdamroll(r_list[r_idx].hd);
    }

    /* And start out fully healthy */
    m_ptr->maxhp = m_ptr->hp;

    /* Extract the monster base speed */
    m_ptr->mspeed = r_list[r_idx].speed - 10;
    
    /* No "damage" yet */
    m_ptr->stunned = 0;
    m_ptr->confused = 0;
    m_ptr->monfear = 0;

    m_ptr->cdis = distance(char_row, char_col, y, x);

    /* Default to invisible */
    m_ptr->ml = FALSE;

    /* Update the monster. */
    cave[y][x].cptr = cur_pos;

    /* Update the monster sleep info */
    if (slp) {
	if (r_list[r_idx].sleep == 0) {
	    m_ptr->csleep = 0;
	}
	else {
	    m_ptr->csleep = ((int)r_list[r_idx].sleep * 2) +
			     randint((int)r_list[r_idx].sleep * 10);
	}
    }

    /* to give the player a sporting chance, any monster that appears in */
    /* line-of-sight and can cast spells or breathe, should be asleep.   */
    /* This is an extension of Um55's sleeping dragon code...            */

    /* if asleep only to prevent summon-breathe-breathe-breathe-die, then don't sleep long -CFT */
    else if (((r_list[r_idx].spells1 & (MS1_CAUSE_1|MS1_CAUSE_2|MS1_HOLD|
                                  MS1_BLIND|MS1_CONF|MS1_FEAR|MS1_SLOW|MS1_BR_ELEC|
                                  MS1_BR_POIS|MS1_BR_ACID|MS1_BR_COLD|MS1_BR_FIRE|
                                  MS1_BO_FIRE|MS1_BO_COLD|MS1_BO_ACID|MS1_ARROW_1|
                                  MS1_CAUSE_3|MS1_BA_FIRE|MS1_BA_COLD|MS1_BO_MANA))
          || (r_list[r_idx].spells2 & (MS2_BR_CHAO|MS2_BR_SHAR|MS2_BR_SOUN|MS2_BR_CONF|
                                  MS2_BR_DISE|MS2_BR_LIFE|MS2_BO_ELEC|MS2_BA_ELEC|
                                  MS2_BA_ACID|MS2_TRAP_CREATE|MS2_RAZOR|MS2_MIND_BLAST|
                                  MS2_ARROW_2|MS2_BO_PLAS|MS2_BO_NETH|MS2_BO_ICEE|
                                  MS2_FORGET|MS2_BRAIN_SMASH|MS2_BA_POIS|MS2_TELE_LEVEL|
                                  MS2_BO_WATE|MS2_BA_WATE|MS2_BA_NETH|MS2_BR_NETH))
          || (r_list[r_idx].spells3 & (MS3_BR_WALL|MS3_BR_SLOW|MS3_BR_LITE|MS3_BR_TIME|
                                  MS3_BR_GRAV|MS3_BR_DARK|MS3_BR_PLAS|MS3_ARROW_3|
                                  MS3_DARK_STORM|MS3_MANA_STORM)))
       && los(y, x, char_row, char_col)) {
	m_ptr->csleep = randint(4);
    }

    /* light up the monster if we can see it... -CFT */
    else {
	m_ptr->csleep = 0;
    }

    update_mon(cur_pos);

    /* Unique kobolds, Liches, orcs, Ogres, Trolls, yeeks, and demons */
    /* get a "following" of escorts.  -DGK-    But not skeletons, */
    /* because that would include druj, making Cantoras amazingly tough -CFT */

    if (r_list[r_idx].cflags2 & MF2_UNIQUE) {

	j = r_list[r_idx].cchar;

	/* Monsters with escorts */
	if (strchr("kLoOTyI&", j)) {

	    /* Try for the highest level monster we can get */
	    for (z = MAX_R_IDX-1; z>=0; z--) {

		/* Find a similar, lower level, non-unique, monster */
		if ((r_list[z].cchar==j) &&
		    (r_list[z].level<=r_list[z].level) &&
		    !(r_list[z].cflags2 & MF2_UNIQUE)) {

		    /* Try up to 50 nearby places */
		    count = 0;
		    do {
			ny = y+randint(7)-4;
			nx = x+randint(7)-4;
			count++;
		    } while (!test_place(ny,nx) && (count<51));

		    /* Certain monsters come in groups */
		    if ((j=='k') || (j=='y') || (j=='&') ||
			(r_list[z].cflags2 & MF2_GROUP)) {
			place_group(ny,nx,z,slp);
		    }

		    /* Otherwise, just use a single escort */
		    else {
			place_monster(ny,nx,z,slp);
		    }
		}
	    }
	}
    }

    /* Success */
    return TRUE;
}


/*
 * Places a "winning" monster at given location	    -RAK-
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
	if (r_list[mon_ptr->mptr].cflags2 & MF2_MAX_HP)
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
void set_ghost(monster_race *g, char *name, int gr, int gc, int lev)
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

    switch (gr) {
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
    switch (gc) {
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
    g->level = lev;
    g->mexp = lev * 5 + 5;

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

	g->cflags1 |= (MF1_THRO_DR | MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF1_HAS_90 | MF1_HAS_60 | MF2_GOOD);

	if (lev > 10) g->cflags1 |= (MF1_HAS_1D2);
	if (lev > 18) g->cflags1 |= (MF1_HAS_2D2);
	if (lev > 23) g->cflags1 |= (MF1_HAS_4D2);
	if (lev > 40) g->cflags1 |= (MF2_SPECIAL);
	if (lev > 40) g->cflags1 &= (~MF1_HAS_4D2);

	/* Add some random resists -DGK */
	for (i = 0; i <= (lev / 5); i++) {
	    switch ((int) randint(13)) {
	      case 1:
	      case 2:
	      case 3:
		g->cflags2 |= (MF2_IM_FIRE);
	      case 4:
	      case 5:
	      case 6:
		g->cflags2 |= (MF2_IM_ACID);
	      case 7:
	      case 8:
	      case 9:
		g->cflags2 |= (MF2_IM_COLD);
	      case 10:
	      case 11:
	      case 12:
		g->cflags2 |= (MF2_IM_ELEC);
	      case 13:
		g->cflags2 |= (MF2_IM_POIS);
	    }
	}

	switch (gc) {
	  case 0:		   /* Warrior */
	    g->spells1 = NONE8;
	    break;
	  case 1:		   /* Mage */
	    g->spells1 |= (0x3L | MS1_BLINK | MS1_ARROW_1 |
			   MS1_SLOW | MS1_CONF);
	    if (lev > 5) g->spells2 |= MS2_BA_POIS;
	    if (lev > 7) g->spells2 |= MS2_BO_ELEC;
	    if (lev > 10) g->spells1 |= MS1_BO_COLD;
	    if (lev > 12) g->spells1 |= TELE;
	    if (lev > 15) g->spells1 |= MS1_BO_ACID;
	    if (lev > 20) g->spells1 |= MS1_BO_FIRE;
	    if (lev > 25) g->spells1 |= MS1_BA_COLD;
	    if (lev > 25) g->spells2 |= MS2_HASTE;
	    if (lev > 30) g->spells1 |= MS1_BA_FIRE;
	    if (lev > 40) g->spells1 |= MS1_BO_MANA;
	    break;
	  case 3:		   /* Rogue */
	    g->spells1 |= (0x5L | MS1_BLINK);
	    if (lev > 10) g->spells1 |= MS1_CONF;
	    if (lev > 18) g->spells1 |= MS1_SLOW;
	    if (lev > 25) g->spells1 |= TELE;
	    if (lev > 30) g->spells1 |= MS1_HOLD;
	    if (lev > 35) g->spells1 |= MS1_TELE_TO;
	    break;
	  case 4:		   /* Ranger */
	    g->spells1 |= (0x8L | MS1_ARROW_1);
	    if (lev > 5) g->spells2 |= MS2_BA_POIS;
	    if (lev > 7) g->spells2 |= MS2_BO_ELEC;
	    if (lev > 10) g->spells1 |= MS1_BO_COLD;
	    if (lev > 18) g->spells1 |= MS1_BO_ACID;
	    if (lev > 25) g->spells1 |= MS1_BO_FIRE;
	    if (lev > 30) g->spells1 |= MS1_BA_COLD;
	    if (lev > 35) g->spells1 |= MS1_BA_FIRE;
	    break;
	  case 2:		   /* Priest */
	  case 5:		   /* Paladin */
	    g->spells1 |= (0x4L | MS1_CAUSE_1 | MS1_FEAR);
	    if (lev > 5) g->spells2 |= MS2_HEAL;
	    if (lev > 10) g->spells1 |= (MS1_CAUSE_2 | MS1_BLIND);
	    if (lev > 18) g->spells1 |= MS1_HOLD;
	    if (lev > 25) g->spells1 |= MS1_CONF;
	    if (lev > 30) g->spells1 |= MS1_CAUSE_3;
	    if (lev > 35) g->spells1 |= MS1_MANA_DRAIN;
	    break;
	}

	g->cflags2 |= (MF2_CHARM_SLEEP | MF2_EVIL);
	if (gr == 6) g->cflags2 |= MF2_ORC;
	if (gr == 7) g->cflags2 |= MF2_TROLL;

	g->ac = 15 + randint(15);
	if (gc == 0 || gc >= 3) g->ac += randint(60);

	/* High level mages and rogues are fast... */
	if ((gc == 1 || gc == 3) && lev > 25) g->speed = 12;
	else g->speed = 11;

	/* Use the letter 'p' */
	g->cchar = 'p';

	/* XXX */
	g->hd[1] = 1;

	g->dd = 5 + ((lev > 18) ? 18 : lev);
	g->ds = g->dd;

	switch (gc) {
	  case 0:
	    g->ddds = ((lev < 30) ? (5 + ((lev > 18) ? 18 : lev)) : 235);
	    g->damage[3] = g->ddds;
	    break;
	  case 1:
	  case 2:
	    g->ddds = 0;
	    g->damage[3] = 0;
	    break;
	  case 3:
	    g->ddds = g->damage[3] = ((lev < 30) ? 149 : 232);
	    break;
	  case 4:
	  case 5:
	    g->ddds = g->damage[3] = g->ds;
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
	g->cflags1 |= (MF1_THRO_DR | MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF1_HAS_90 | MF2_GOOD);
	g->spells1 |= (NONE8);
	g->cflags2 |= (MF2_IM_POIS | MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_IM_COLD | MF2_NO_INFRA);
	if (gr == 6) g->cflags2 |= MF2_ORC;
	if (gr == 7) g->cflags2 |= MF2_TROLL;
	g->ac = 26;
	g->speed = 11;
	g->cchar = 's';
	g->hd[1] = 1;
	g->dd = 5;
	g->ds = 5;
	g->ddds = 0;
	g->damage[3] = 0;
	break;

      case 4:
      case 5:
	sprintf((char *)g->name, "%s, the %s zombie", name, cap(ghost_race));
	g->cflags1 |= (MF1_THRO_DR | MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF1_HAS_60 | HAS_90 | MF2_GOOD);
	g->spells1 |= (NONE8);
	g->cflags2 |= (MF2_IM_POIS | MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_NO_INFRA);
	if (gr == 6) g->cflags2 |= MF2_ORC;
	if (gr == 7) g->cflags2 |= MF2_TROLL;
	g->ac = 30;
	g->speed = 11;
	g->cchar = 'z';
	g->hd[1] *= 2;
	g->dd = 8;
	g->ds = 0;
	g->ddds = 0;
	g->damage[3] = 0;
	break;

      case 6:
	sprintf((char *) g->name, "%s, the Poltergeist", name);
	g->cflags1 |= (MF1_MV_INVIS | MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF2_GOOD | MF1_HAS_1D2 | MF1_MV_75 | MF1_THRO_WALL);
	g->spells1 |= (NONE8);
	g->cflags2 |= (MF2_IM_POIS | MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_IM_COLD | MF2_NO_INFRA);
	g->ac = 20;
	g->speed = 13;
	g->cchar = 'G';
	g->dd = 5;
	g->ds = 5;
	g->ddds = 93;
	g->damage[3] = 93;
	g->mexp = (g->mexp * 3) / 2;
	break;

      case 7:
      case 8:
	sprintf((char *)g->name, "%s, the Mummified %s", name, cap(ghost_race));
	g->cflags1 |= (MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF1_HAS_1D2 | MF2_GOOD);
	g->spells1 |= (NONE8);
	g->cflags2 |= (MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_IM_POIS | MF2_NO_INFRA);
	if (gr == 6) g->cflags2 |= MF2_ORC;
	if (gr == 7) g->cflags2 |= MF2_TROLL;
	g->ac = 35;
	g->speed = 11;
	g->cchar = 'M';
	g->hd[1] *= 2;
	g->dd = 16;
	g->ds = 16;
	g->ddds = 16;
	g->damage[3] = 0;
	g->mexp = (g->mexp * 3) / 2;
	break;

      case 9:
      case 10:
	sprintf((char *)g->name, "%s%s spirit", name,
		(name[strlen(name) - 1] == 's') ? "'" : "'s");
	g->cflags1 |= (MF1_MV_INVIS | MF1_THRO_WALL | MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF1_HAS_1D2 | MF2_GOOD);
	g->spells1 |= (NONE8);
	g->cflags2 |= (MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_IM_POIS | MF2_IM_COLD | MF2_NO_INFRA);
	g->ac = 20;
	g->speed = 11;
	g->cchar = 'G';
	g->hd[1] *= 2;
	g->dd = 19;
	g->ds = 185;
	g->ddds = 99;
	g->damage[3] = 178;
	g->mexp = g->mexp * 3;
	break;

      case 11:
	sprintf((char *)g->name, "%s%s ghost", name, (name[strlen(name) - 1] == 's') ?
		"'" : "'s");
	g->cflags1 |= (MF1_MV_INVIS | MF1_THRO_WALL | MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF1_HAS_1D2 | MF2_GOOD);
	g->spells1 |= (0xFL | MS1_HOLD | MS1_MANA_DRAIN | MS1_BLIND);
	g->cflags2 |= (MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_IM_POIS | MF2_IM_COLD | MF2_NO_INFRA);
	g->ac = 40;
	g->speed = 12;
	g->cchar = 'G';
	g->hd[1] *= 2;
	g->dd = 99;
	g->ds = 99;
	g->ddds = 192;
	g->damage[3] = 184;
	g->mexp = (g->mexp * 7) / 2;
	break;

      case 12:
	sprintf((char *) g->name, "%s, the Vampire", name);
	g->cflags1 |= (MF1_THRO_DR | MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF1_HAS_2D2 | MF2_GOOD);
	g->spells1 |= (0x8L | MS1_HOLD | MS1_FEAR | MS1_TELE_TO | MS1_CAUSE_2);
	g->cflags2 |= (MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_IM_POIS | MF2_NO_INFRA | MF2_HURT_LITE);
	g->ac = 40;
	g->speed = 11;
	g->cchar = 'V';
	g->hd[1] *= 3;
	g->dd = 20;
	g->ds = 20;
	g->ddds = 190;
	g->damage[3] = 0;
	g->mexp = g->mexp * 3;
	break;

      case 13:
	sprintf((char *)g->name, "%s%s Wraith", name,
		(name[strlen(name) - 1] == 's') ? "'" : "'s");
	g->cflags1 |= (MF1_THRO_DR | MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF1_HAS_4D2 | HAS_2D2 | MF2_GOOD);
	g->spells1 |= (0x7L | MS1_HOLD | MS1_FEAR | MS1_BLIND | MS1_CAUSE_3);
	g->spells2 |= (MS2_BO_NETH);
	g->cflags2 |= (MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_IM_POIS | MF2_IM_COLD | MF2_NO_INFRA |
			MF2_HURT_LITE);
	g->ac = 60;
	g->speed = 12;
	g->cchar = 'W';
	g->hd[1] *= 3;
	g->dd = 20;
	g->ds = 20;
	g->ddds = 190;
	g->damage[3] = 0;
	g->mexp = g->mexp * 5;
	break;

      case 14:
	sprintf((char *) g->name, "%s, the Vampire Lord", name);
	g->cflags1 |= (MF1_THRO_DR | MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF1_HAS_1D2 | MF2_SPECIAL);
	g->spells1 |= (0x8L | MS1_HOLD | MS1_FEAR | MS1_TELE_TO | MS1_CAUSE_3);
	g->spells2 |= (MS2_BO_NETH);
	g->cflags2 |= (MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_IM_POIS | MF2_NO_INFRA | MF2_HURT_LITE);
	g->ac = 80;
	g->speed = 11;
	g->cchar = 'V';
	g->hd[1] *= 2;
	g->hd[0] = (g->hd[0] * 5) / 2;
	g->dd = 20;
	g->ds = 20;
	g->ddds = 20;
	g->damage[3] = 198;
	g->mexp = g->mexp * 20;
	break;

      case 15:
	sprintf((char *)g->name, "%s%s ghost", name,
		 (name[strlen(name) - 1] == 's') ? "'" : "'s");
	g->cflags1 |= (MF1_MV_INVIS | MF1_THRO_WALL | MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF1_HAS_2D2 | MF2_SPECIAL);
	g->spells1 |= (0x5L | MS1_HOLD | MS1_MANA_DRAIN | MS1_BLIND | MS1_CONF);
	g->cflags2 |= (MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_IM_COLD | MF2_IM_POIS | MF2_NO_INFRA);
	g->ac = 90;
	g->speed = 13;
	g->cchar = 'G';
	g->hd[1] *= 3;
	g->dd = 99;
	g->ds = 99;
	g->ddds = 192;
	g->damage[3] = 184;
	g->mexp = g->mexp * 20;
	break;

      case 17:
	sprintf((char *)g->name, "%s, the Lich", name);
	g->cflags1 |= (MF1_THRO_DR | MF1_MV_ATT_NORM | MF1_CARRY_OBJ | MF1_HAS_2D2 | HAS_1D2 | MF2_SPECIAL);
	g->spells1 |= (0x3L | MS1_FEAR | MS1_CAUSE_3 | MS1_TELE_TO | MS1_BLINK |
		       MS1_S_UNDEAD | MS1_BA_FIRE | MS1_BA_COLD | MS1_HOLD |
		       MS1_MANA_DRAIN | MS1_BLIND | MS1_CONF | TELE);
	g->spells2 |= (MS2_BRAIN_SMASH | MS2_RAZOR);
	g->cflags2 |= (MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_IM_COLD | MF2_NO_INFRA | MF2_IM_POIS
			| MF2_INTELLIGENT);
	g->ac = 120;
	g->speed = 12;
	g->cchar = 'L';
	g->hd[1] *= 3;
	g->hd[0] *= 2;
	g->dd = 181;
	g->ds = 201;
	g->ddds = 214;
	g->damage[3] = 181;
	g->mexp = g->mexp * 50;
	break;

      default:
	sprintf((char *)g->name, "%s%s ghost", name,
		(name[strlen(name) - 1] == 's') ? "'" : "'s");
	g->cflags1 |= (MF1_MV_INVIS | MF1_THRO_WALL | MF1_MV_ATT_NORM | MF1_CARRY_OBJ |
		     MF1_HAS_1D2 | HAS_2D2 | MF2_SPECIAL);
	g->spells1 |= (0x2L | MS1_HOLD | MS1_MANA_DRAIN | 
		       MS1_BLIND | MS1_CONF | MS1_TELE_TO);
	g->spells2 |= (MS2_BO_NETH | MS2_BA_NETH | MS2_BRAIN_SMASH | MS2_TELE_LEVEL);
	g->cflags2 |= (MF2_CHARM_SLEEP | MF2_UNDEAD | MF2_EVIL | MF2_IM_POIS | MF2_IM_COLD | MF2_NO_INFRA |
			MF2_INTELLIGENT);
	g->ac = 130;
	g->speed = 13;
	g->cchar = 'G';
	g->hd[1] *= 2;
	g->hd[0] = (g->hd[0] * 5) / 2;
	g->dd = 99;
	g->ds = 99;
	g->ddds = 192;
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
    register monster_type  *m_ptr;
    monster_race           *r_ptr;
    char                   name[100];
    int                    i, j, level;
    int                    gr;
    int                    gc;
    FILE		   *fp;

    char                   tmp[100];


    /* The race is convenient */
    r_ptr = &r_list[MAX_R_IDX - 1];


    /* In the town, ghosts have the same level as the player */
    if (!dun_level) {

	/* You have to be level 5, and even then its only 10% */
	if (p_ptr->misc.lev < 5 || randint(10) > 1) return 0;

	/* Look for a proper bones file */
	sprintf(tmp, "%s%s%d", ANGBAND_DIR_BONES, PATH_SEP, p_ptr->misc.lev);
	fp = my_tfopen(tmp, "r");
	if (!fp) return (0);

	/* Read the bones info */
	if (fscanf(fp, "%[^\n]\n%d\n%d\n%d", name, &i, &gr, &gc) < 4) {
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
	    r_ptr->hd[0] = i;	   /* set_ghost may adj for race/class/lv */
	    r_ptr->hd[1] = j;
	    level = p_ptr->misc.lev;
    }

    /* In the dungeon, ghosts have the same level as the level */    
    else {

	/* And even then, it only happens sometimes */
	if (14 > randint((dun_level / 2) + 11)) return 0;

	/* Or rather, 1/3 of that often :-) */
	if (randint(3) != 1) return (0);

	/* Open the bones file */
	sprintf(tmp, "%s%s%d", ANGBAND_DIR_BONES, PATH_SEP, dun_level);
	fp = my_tfopen(tmp, "r");
	if (!fp) return (0);

	if (fscanf(fp, "%[^\n]\n%d\n%d\n%d", name, &i, &gr, &gc) < 4) {
	    fclose(fp);
	    if (wizard) msg_print("Ghost:Failed to scan in info properly!");
	    return 0;
	}
	fclose(fp);


    /* Break up the hitpoints */
    j = 1;
    if (i > 255) { /* avoid wrap-around of byte hitdice, by factoring */
	j = i / 32;
	i = 32;
    }

    /* set_ghost may adj for race/class/lv */
    r_ptr->hd[0] = i;
    r_ptr->hd[1] = j;

		level = dun_level;
    }

    /* Set up the ghost */
    set_ghost(r_ptr, name, gr, gc, level);

    /* Note for wizard (special ghost name) */
    if (wizard || peek) msg_print(r_ptr->name);

    cur_pos = m_pop();
    m_ptr = &m_list[cur_pos];

    /* Hack -- pick a nice (far away) location */
    do {

	/* Pick a location */
	y = randint(cur_height - 2);
	x = randint(cur_width - 2);
    } while ((cave[y][x].fval >= MIN_CLOSED_SPACE) || (cave[y][x].cptr != 0)
	     || (cave[y][x].tptr != 0) ||
	/* Accept far away grids */
	     (distance(y, x, char_row, char_col) <= MAX_SIGHT));

    m_ptr->fy = y;
    m_ptr->fx = x;

    m_ptr->mptr = (MAX_R_IDX - 1);

    /* Assign hitpoints */
    m_ptr->hp = (s16b) r_ptr->hd[0] * (s16b) r_ptr->hd[1];

    /* the r_list speed value is 10 greater, so that it can be a byte */
    m_ptr->mspeed = r_list[m_ptr->mptr].speed - 10;

    m_ptr->stunned = 0;

    m_ptr->cdis = distance(char_row, char_col, y, x);
    cave[y][x].cptr = cur_pos;
    m_ptr->csleep = 0;

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

	    if (level > MAX_R_LEV) level = MAX_R_LEV;

	    /* Make a Nasty Monster */
	    if (randint(MON_NASTY) == 1) {
		/* be a little more civilized about monster depths for the first levels -CWS */
		i = level / 4 + 1;
		if (i > 4) i = 4;
		level = level + MY_ABS(i) + 1;
		if (level > MAX_R_LEV) level = MAX_R_LEV;
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
	    (r_list[i].cflags2 & MF2_UNIQUE)) {
	    goto again;
	}

	/* Quest Monsters never appear out of depth */
	if ((r_list[i].level > (unsigned) dun_level) &&
	    (r_list[i].cflags2 & MF2_QUESTOR)) {
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

	    if (level > MAX_R_LEV) level = MAX_R_LEV;

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

	if ((r_list[i].level > old) && (r_list[i].cflags2 & MF2_UNIQUE)) {
	    goto again;
	}

	if (( r_list[i].level > (unsigned) dun_level) &&
	    (r_list[i].cflags2 & MF2_QUESTOR)) {
	    goto again;
	}

    /* Accept the monster */
    return i;
}


void place_group(int y, int x, int r_idx, int slp)
{
    /* prevent level rating from skyrocketing if they are out of depth... */
    int old = rating;
    int extra = 0;

    /* reduce size of group if out-of-depth */
    if (r_list[r_idx].level > (unsigned) dun_level) {
	extra = 0 - randint(r_list[r_idx].level - dun_level);
    }

    /* if monster is deeper than normal, then travel in bigger packs -CFT */
    else if (r_list[r_idx].level < (unsigned) dun_level) {
	extra = randint(dun_level - r_list[r_idx].level);
    }

    /* put an upper bounds on it... -CFT */
    if (extra > 12) extra = 12;

    switch (randint(13) + extra) {
      case 25:
	place_monster(y, x - 3, r_idx, 0);
      case 24:
	place_monster(y, x + 3, r_idx, 0);
      case 23:
	place_monster(y - 3, x, r_idx, 0);
      case 22:
	place_monster(y + 3, x, r_idx, 0);
      case 21:
	place_monster(y - 2, x + 1, r_idx, 0);
      case 20:
	place_monster(y + 2, x - 1, r_idx, 0);
      case 19:
	place_monster(y + 2, x + 1, r_idx, 0);
      case 18:
	place_monster(y - 2, x - 1, r_idx, 0);
      case 17:
	place_monster(y + 1, x + 2, r_idx, 0);
      case 16:
	place_monster(y - 1, x - 2, r_idx, 0);
      case 15:
	place_monster(y + 1, x - 2, r_idx, 0);
      case 14:
	place_monster(y - 1, x + 2, r_idx, 0);
      case 13:
	place_monster(y, x - 2, r_idx, 0);
      case 12:
	place_monster(y, x + 2, r_idx, 0);
      case 11:
	place_monster(y + 2, x, r_idx, 0);
      case 10:
	place_monster(y - 2, x, r_idx, 0);
      case 9:
	place_monster(y + 1, x + 1, r_idx, 0);
      case 8:
	place_monster(y + 1, x - 1, r_idx, 0);
      case 7:
	place_monster(y - 1, x - 1, r_idx, 0);
      case 6:
	place_monster(y - 1, x + 1, r_idx, 0);
      case 5:
	place_monster(y, x + 1, r_idx, 0);
      case 4:
	place_monster(y, x - 1, r_idx, 0);
      case 3:
	place_monster(y + 1, x, r_idx, 0);
      case 2:
	place_monster(y - 1, x, r_idx, 0);
	rating = old;
      case 1:
      default:			   /* just in case I screwed up -CFT */
	place_monster(y, x, r_idx, 0);
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
    int          r_idx;

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
	    r_idx = get_mons_num(dun_level);
	} while (randint(r_list[r_idx].rarity) > 1);

    /*
     * to give the player a sporting chance, any monster that appears in
     * line-of-sight and can cast spells or breathe, should be asleep. This
     * is an extension of Um55's sleeping dragon code... 
     */
	if (((r_list[r_idx].spells1 & (MS1_CAUSE_1 | MS1_CAUSE_2 | MS1_HOLD |
			    MS1_BLIND | MS1_CONF | MS1_FEAR | MS1_SLOW | MS1_BR_ELEC |
			       MS1_BR_POIS | MS1_BR_ACID | MS1_BR_COLD | MS1_BR_FIRE |
			     MS1_BO_FIRE | MS1_BO_COLD | MS1_BO_ACID | MS1_ARROW_1 |
			   MS1_CAUSE_3 | MS1_BA_FIRE | MS1_BA_COLD | MS1_BO_MANA))
	     || (r_list[r_idx].spells2 & (MS2_BR_CHAO | MS2_BR_SHAR | MS2_BR_SOUN | MS2_BR_CONF |
			   MS2_BR_DISE | MS2_BR_LIFE | MS2_BO_ELEC | MS2_BA_ELEC |
			      MS2_BA_ACID | MS2_TRAP_CREATE | MS2_RAZOR | MS2_MIND_BLAST |
			    MS2_ARROW_2 | MS2_BO_PLAS | MS2_BO_NETH | MS2_BO_ICEE |
				MS2_FORGET | MS2_BRAIN_SMASH | MS2_BA_POIS | MS2_TELE_LEVEL |
			 MS2_BO_WATE | MS2_BA_WATE | MS2_BA_NETH | MS2_BR_NETH))
	     || (r_list[r_idx].spells3 & (MS3_BR_WALL | MS3_BR_SLOW | MS3_BR_LITE | MS3_BR_TIME |
				 MS3_BR_GRAV | MS3_BR_DARK | MS3_BR_PLAS | MS3_ARROW_3 |
					MS3_DARK_STORM | MS3_MANA_STORM)))
	    && (los(y, x, char_row, char_col))) {
	    slp = TRUE;
	}

	if (!(r_list[r_idx].cflags2 & MF2_GROUP)) {
	    place_monster(y, x, r_idx, slp);
	}
	else {
	    place_group(y, x, r_idx, slp);
	}
    }
}


/*
 * Places a random creature at or adjacent to the given location
 * We modify the given location to hold the location used, if any.
 * We return TRUE if a monster (or group of monsters) was summoned.
 */
int summon_monster(int *yp, int *xp, int slp)
{
    register int        i, y, x, r_idx;
    int                 summon;
    register cave_type *cave_ptr;

    i = 0;
    summon = FALSE;
    r_idx = get_mons_num(dun_level + MON_SUMMON_ADJ);
    do {

	/* Pick a nearby location */
	y = *yp - 2 + randint(3);
	x = *xp - 2 + randint(3);

	/* Require legal grid */
	if (in_bounds(y, x)) {
	    cave_ptr = &cave[y][x];
	
	/* Require "empty" floor grids */
	if (cave_ptr->fval <= MAX_OPEN_SPACE && (cave_ptr->cptr == 0)) {
	
	/* Place the monster */
	if (r_list[r_idx].cflags2 & MF2_GROUP) {
	    place_group(y, x, r_idx, slp);
	}
	else {
	    place_monster(y, x, r_idx, slp);
	}

		summon = TRUE;
		i = 9;

	/* Save the location */
	*yp = y;
	*xp = x;
		
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
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if ((r_list[m].cflags2 & MF2_UNDEAD) && !(r_list[m].cflags2 & MF2_UNIQUE) &&
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
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].cflags2 & MF2_DEMON && !(r_list[m].cflags2 & MF2_UNIQUE) &&
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
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].cflags2 & MF2_DRAGON && !(r_list[m].cflags2 & MF2_UNIQUE)) {
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
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].cchar == 'W' && (r_list[m].cflags2 & MF2_UNIQUE)) {
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
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].cchar == 'R' && !(r_list[m].cflags2 & MF2_UNIQUE)) {
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
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].cchar == 'S' && !(r_list[m].cflags2 & MF2_UNIQUE)) {
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
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].cchar == 'A' && !(r_list[m].cflags2 & MF2_UNIQUE)) {
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
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].cchar == 'a' && !(r_list[m].cflags2 & MF2_UNIQUE)) {
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
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (!(r_list[m].cchar == 'P') && (r_list[m].cflags2 & MF2_UNIQUE)) {
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
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if (r_list[m].cchar == 'J' && !(r_list[m].cflags2 & MF2_UNIQUE)) {
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
    l = m_level[MAX_R_LEV];
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
    l = m_level[MAX_R_LEV];
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
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	ctr = 0;
	do {
	    if ((r_list[m].cchar == 'C' || r_list[m].cchar == 'Z')
		&& !(r_list[m].cflags2 & MF2_UNIQUE)) {
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
    l = m_level[MAX_R_LEV];
    do {
	m = randint(l) - 1;
	if (r_list[m].cchar == 'J') {
	    summon = TRUE;
	    place_monster(*y, *x, m, TRUE);
	}
    } while (!summon);
    return (summon);
}

void monster_name(char *m_name, monster_type *m_ptr, monster_race *r_ptr)
{
    if (!m_ptr->ml)
	(void)strcpy(m_name, "It");
    else {
	if (r_ptr->cflags2 & MF2_UNIQUE)
	    (void)sprintf(m_name, "%s", r_ptr->name);
	else
	    (void)sprintf(m_name, "The %s", r_ptr->name);
    }
}



void lower_monster_name(char *m_name, monster_type *m_ptr, monster_race *r_ptr)
{
    if (!m_ptr->ml)
	(void)strcpy(m_name, "it");
    else {
	if (r_ptr->cflags2 & MF2_UNIQUE)
	    (void)sprintf(m_name, "%s", r_ptr->name);
	else
	    (void)sprintf(m_name, "the %s", r_ptr->name);
    }
}



