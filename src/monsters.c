/* monsters.c: monster definitions

   Copyright (c) 1989 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include "constant.h"
#include "config.h"
#include "types.h"
#include "defines.h"

/*
		Attack types:
		1	Normal attack
		2	Poison Strength
		3	Confusion attack
		4	Fear attack
		5	Fire attack
		6	Acid attack
		7	Cold attack
		8	Lightning attack
		9	Corrosion attack
		10	Blindness attack
		11	Paralysis attack
		12	Steal Money
		13	Steal Object
		14	Poison
		15	Lose dexterity
		16	Lose constitution
		17	Lose intelligence
		18	Lose wisdom
		19	Lose experience
		20	Aggravation
		21	Disenchant
		22	Eats food
		23	Eat light
		24	Energy drain from pack
		25      Drain all stats
		99	Blank

		Attack descriptions:
		1	hits you.
		2	bites you.
		3	claws you.
		4	stings you.
		5	touches you.
		6	kicks you.
		7	gazes at you.
		8	breathes on you.
		9	spits on you.
		10	makes a horrible wail.
		11	embraces you.
		12	crawls on you.
		13	releases a cloud of spores.
		14	begs you for money.
		15	You've been slimed.
		16	crushes you.
		17	tramples you.
		18	drools on you.
		19	insults you.

		20	butts you.
		21	charges you.
		22	engulfs you.
                23      talks to you about mushrooms and dogs

		99	is repelled.

	Example:  For a creature which bites for 1d6, then stings for
		  2d4 and loss of dex you would use:
			{1,2,1,6},{15,4,2,4}

	Sleep (sleep)	:	A measure in turns of how fast creature
				will notice player (on the average).
	Area of affect (aaf) :	Max range that creature is able to "notice"
				the player.
									*/

creature_type r_list[MAX_R_IDX] = {
{NULL                       ,(NONE8)
                            ,(NONE8)
			    ,(MF2_EVIL|MF2_CHARM_SLEEP|MF2_UNDEAD|MF2_UNIQUE|MF2_GOOD
			     |MF2_IM_POIS|MF2_IM_COLD|MF2_NO_INFRA),(NONE8)
                            ,(NONE8)
			    ,0,0,100,0,11
		            ,'@',{5,5},{0,0,0,0},32000,255,'n'}
};

s16b r_level[MAX_R_LEV+1];

/* Blank monster values	*/
monster_type blank_monster = {0,0,0,0,0,0,0,FALSE,0,FALSE};
