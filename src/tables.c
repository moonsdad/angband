/* File: tables.c */

/* Purpose: store/attack/RNG/etc tables and variables */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


#ifdef CHECK_HOURS

/*
 * Operating hours for ANGBAND	-RAK-
 *	 X = Open; . = Closed
 */
char days[7][29] = {
	"SUN:XXXXXXXXXXXXXXXXXXXXXXXX",
	"MON:XXXXXXXX.........XXXXXXX",
	"TUE:XXXXXXXX.........XXXXXXX",
	"WED:XXXXXXXX.........XXXXXXX",
	"THU:XXXXXXXX.........XXXXXXX",
	"FRI:XXXXXXXX.........XXXXXXX",
	"SAT:XXXXXXXXXXXXXXXXXXXXXXXX"
};

#endif


#define MDO MAX_DUNGEON_OBJ

/* k_list[] index of objects that may appear in the store */
u16b store_choice[MAX_STORES][STORE_CHOICES] = {
	/* General Store */
{MDO,MDO,MDO,MDO,MDO,MDO,MDO,MDO,MDO+21,MDO+21,MDO+21,MDO+21,MDO+22,MDO+22,
 MDO+22,MDO+1,MDO+2,MDO+3,MDO+4,
 MDO+22,MDO+20,MDO+21,MDO+5,MDO+6,84,84,123,MDO+22,MDO+22,MDO+21},
	/* Armoury	 */
{103,104,105,106,107,108,109,91,92,125,126,128,129,130,91,92,94,95,96,
 103,104,105,125,128,94,95,111,112,113,121},
	/* Weaponsmith	 */
{29,29,29,31,34,35,42,46,49,58,60,61,63,64,68,73,74,75,77,78,80,82,83,83,
 78,80,82,35,65,66},
	/* Temple	 */
{334,335,336,337,334,335,336,337,257,237,261,262,233,233,240,241,260,
 260,MDO+14,MDO+15,MDO+15,MDO+15,53,54,55,52,335,180,237,240},
	/* Alchemy shop	 */
{227,227,230,230,236,206,252,252,253,253,MDO+7,MDO+7,MDO+7,MDO+8,MDO+8,MDO+8,
 MDO+9,MDO+10,MDO+11,MDO+12,MDO+13,MDO+15,MDO+15,173,174,175,185,185,185,206},
	/* Magic-User store*/
{330,331,332,333,330,331,332,333,326,293,293,299,303,301,302,318,326,
 282,277,279,292,164,167,168,153,137,142,326,328,299}
};

#ifndef MACINTOSH
/* MPW doesn't seem to handle this very well, so replace store_buy array
   with a function call on mac */
/* functions defined in sets.c */
extern int general_store(), armory(), weaponsmith(), temple(),
  alchemist(), magic_shop();

int blackmarket();
int home();

/* Each store will buy only certain items, based on TVAL */
int (*store_buy[MAX_STORES])() = {
       general_store, armory, weaponsmith, temple, alchemist, magic_shop,
       blackmarket, home};
#endif

/*
 * Each type of character starts out with a few provisions.
 * Note that the entries refer to elements of the k_list[] array
 * 356 = Food Ration, 365 = Wooden Torch, 123 = Cloak, 30 = Stiletto,
   103 = Soft Leather Armor, 318 = Beginners-Magic, 322 = Beginners Handbook
 */

u16b player_init[MAX_CLASS][5] = {

	/* Warrior */
		{ MDO, MDO+21,  34, 109, 258},

	/* Mage */
		{ MDO, MDO+21,  29, 330, 220},

	/* Priest */
		{ MDO, MDO+21,  53, 334, 242},

	/* Rogue */
		{ MDO, MDO+21,  46, 103, 330},

	/* Ranger */
		{ MDO, MDO+21,  34, 330,  74},

	/* Paladin */
		{ MDO, MDO+21,  34, 334, 209}
        /* Last array object added for one extra useful object per class */
};




/*
 * used to calculate the number of blows the player gets in combat
 */
byte blows_table[11][12] = {
/* STR/W:	   9  18  67  107  117  118  128  138  148  158  168 more : DEX */
/* <2 */	{  1,  1,  1,   1,   1,   1,   2,   2,   2,   2,   2,   3},
/* <3 */	{  1,  1,  1,   1,   2,   2,   3,   3,   3,   3,   3,   4},
/* <4 */    {  1,  1,  1,   2,   2,   3,   4,   4,   4,   4,   4,   5},
/* <6 */    {  1,  1,  2,   2,   3,   3,   4,   4,   4,   5,   5,   5},
/* <8 */    {  1,  2,  2,   3,   3,   4,   4,   4,   5,   5,   5,   5},
/* <10 */   {  1,  2,  2,   3,   4,   4,   4,   5,   5,   5,   5,   5},
/* <13 */   {  2,  2,  3,   3,   4,   4,   5,   5,   5,   5,   5,   6},
/* <15 */   {  2,  3,  3,   3,   4,   4,   5,   5,   5,   5,   5,   6},
/* <18 */   {  3,  3,  3,   4,   4,   4,   5,   5,   5,   5,   6,   6},
/* <20 */   {  3,  3,  3,   4,   4,   4,   5,   5,   5,   5,   6,   6},
/* else */  {  3,  3,  4,   4,   4,   4,   5,   5,   5,   6,   6,   6}

};


/*
 * This table is used to generate a psuedo-normal distribution.
 * See the function randnor() in misc1.c, this is much faster than calling
 * transcendental function to calculate a true normal distribution
 */
u16b normal_table[NORMAL_TABLE_SIZE] = {
     206,     613,    1022,    1430,	1838,	 2245,	  2652,	   3058,
    3463,    3867,    4271,    4673,	5075,	 5475,	  5874,	   6271,
    6667,    7061,    7454,    7845,	8234,	 8621,	  9006,	   9389,
    9770,   10148,   10524,   10898,   11269,	11638,	 12004,	  12367,
   12727,   13085,   13440,   13792,   14140,	14486,	 14828,	  15168,
   15504,   15836,   16166,   16492,   16814,	17133,	 17449,	  17761,
   18069,   18374,   18675,   18972,   19266,	19556,	 19842,	  20124,
   20403,   20678,   20949,   21216,   21479,	21738,	 21994,	  22245,
   22493,   22737,   22977,   23213,   23446,	23674,	 23899,	  24120,
   24336,   24550,   24759,   24965,   25166,	25365,	 25559,	  25750,
   25937,   26120,   26300,   26476,   26649,	26818,	 26983,	  27146,
   27304,   27460,   27612,   27760,   27906,	28048,	 28187,	  28323,
   28455,   28585,   28711,   28835,   28955,	29073,	 29188,	  29299,
   29409,   29515,   29619,   29720,   29818,	29914,	 30007,	  30098,
   30186,   30272,   30356,   30437,   30516,	30593,	 30668,	  30740,
   30810,   30879,   30945,   31010,   31072,	31133,	 31192,	  31249,
   31304,   31358,   31410,   31460,   31509,	31556,	 31601,	  31646,
   31688,   31730,   31770,   31808,   31846,	31882,	 31917,	  31950,
   31983,   32014,   32044,   32074,   32102,	32129,	 32155,	  32180,
   32205,   32228,   32251,   32273,   32294,	32314,	 32333,	  32352,
   32370,   32387,   32404,   32420,   32435,	32450,	 32464,	  32477,
   32490,   32503,   32515,   32526,   32537,	32548,	 32558,	  32568,
   32577,   32586,   32595,   32603,   32611,	32618,	 32625,	  32632,
   32639,   32645,   32651,   32657,   32662,	32667,	 32672,	  32677,
   32682,   32686,   32690,   32694,   32698,	32702,	 32705,	  32708,
   32711,   32714,   32717,   32720,   32722,	32725,	 32727,	  32729,
   32731,   32733,   32735,   32737,   32739,	32740,	 32742,	  32743,
   32745,   32746,   32747,   32748,   32749,	32750,	 32751,	  32752,
   32753,   32754,   32755,   32756,   32757,	32757,	 32758,	  32758,
   32759,   32760,   32760,   32761,   32761,	32761,	 32762,	  32762,
   32763,   32763,   32763,   32764,   32764,	32764,	 32764,	  32765,
   32765,   32765,   32765,   32766,   32766,	32766,	 32766,	  32766,
};



/*
		Monster Attack types:
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
*/



/* These should never be created by accident (and probably won't ;-). -CWS */

/* ERROR: attack #35 is no longer used */
monster_attack a_list[MAX_A_IDX] = {
/* 0 */	{0, 0, 0, 0},	{1, 1, 1, 2},	{1, 1, 1, 3},	{1, 1, 1, 4},
	{1, 1, 1, 5},	{1, 1, 1, 6},	{1, 1, 1, 7},	{1, 1, 1, 8},
	{1, 1, 1, 9},	{1, 1, 1, 10},	{1, 1, 1, 12},	{1, 1, 2, 2},
	{1, 1, 2, 3},	{1, 1, 2, 4},	{1, 1, 2, 5},	{1, 1, 2, 6},
	{1, 1, 2, 8},	{1, 1, 3, 4},	{1, 1, 3, 5},	{1, 1, 3, 6},
/* 20 */{1, 1, 3, 8},	{1, 1, 4, 3},	{1, 1, 4, 6},	{1, 1, 5, 5},
	{1, 2, 1, 1},	{1, 2, 1, 2},	{1, 2, 1, 3},	{1, 2, 1, 4},
	{1, 2, 1, 5},	{1, 2, 1, 6},	{1, 2, 1, 7},	{1, 2, 1, 8},
	{1, 2, 1, 10},	{1, 2, 2, 3},	{1, 2, 2, 4},	{1, 2, 2, 5},
	{1, 2, 2, 6},	{1, 2, 2, 8},	{1, 2, 2, 10},	{1, 2, 2, 12},
/* 40 */{1, 2, 2, 14},	{1, 2, 3, 4},	{1, 2, 3, 12},	{1, 2, 4, 4},
	{1, 2, 4, 5},	{1, 2, 4, 6},	{1, 2, 4, 8},	{1, 2, 5, 4},
	{1, 2, 5, 8},	{1, 3, 1, 1},	{1, 3, 1, 2},	{1, 3, 1, 3},
	{1, 3, 1, 4},	{1, 3, 1, 5},	{1, 3, 1, 8},	{1, 3, 1, 9},
	{1, 3, 1, 10},	{1, 3, 1, 12},	{1, 3, 3, 3},	{1, 4, 1, 2},
/* 60 */{1, 4, 1, 3},	{1, 4, 1, 4},	{1, 4, 2, 4},	{1, 5, 1, 2},
	{1, 5, 1, 3},	{1, 5, 1, 4},	{1, 5, 1, 5},	{1, 10, 5, 6},
	{1, 12, 1, 1},	{1, 12, 1, 2},	{1, 13, 1, 1},	{1, 13, 1, 3},
	{1, 14, 0, 0},	{1, 16, 1, 4},	{1, 16, 1, 6},	{1, 16, 1, 8},
	{1, 16, 1, 10},	{1, 16, 2, 8},	{1, 17, 8, 12},	{1, 18, 0, 0},
/* 80 */{2, 1, 3, 4},	{2, 1, 4, 6},	{2, 2, 1, 4},	{2, 2, 2, 4},
	{2, 2, 4, 4},	{2, 4, 1, 4},	{2, 4, 1, 7},	{2, 5, 1, 5},
	{2, 7, 1, 6},	{3, 1, 1, 4},	{3, 5, 1, 8},	{3, 13, 1, 4},
	{3, 7, 0, 0},	{4, 1, 1, 1},	{4, 1, 1, 4},	{4, 2, 1, 2},
	{4, 2, 1, 6},	{4, 5, 0, 0},	{4, 7, 0, 0},	{4, 10, 0, 0},
/*100 */{4, 13, 1, 6},	{5, 1, 2, 6},	{5, 1, 3, 7},	{5, 1, 4, 6},
	{5, 1, 8, 12},	{5, 2, 1, 3},	{5, 2, 3, 6},	{5, 2, 3, 12},
	{5, 5, 4, 4},	{5, 9, 3, 7},	{5, 9, 4, 5},	{5, 12, 1, 6},
	{6, 2, 1, 3},	{6, 2, 2, 8},	{6, 2, 4, 4},	{6, 5, 1, 10},
	{6, 5, 2, 3},	{6, 8, 1, 5},	{6, 9, 2, 6},	{6, 9, 3, 6},
/*120 */{7, 1, 3, 6},	{7, 2, 1, 3},	{7, 2, 1, 6},	{7, 2, 3, 6},
	{7, 2, 3, 10},	{7, 5, 1, 6},	{7, 5, 2, 3},	{7, 5, 2, 6},
	{7, 5, 4, 4},	{7, 12, 1, 4},	{8, 1, 3, 8},	{8, 2, 1, 3},
	{8, 2, 2, 6},	{8, 2, 3, 8},	{8, 2, 5, 5},	{8, 5, 5, 4},
	{9, 5, 1, 2},	{9, 5, 2, 5},	{9, 5, 2, 6},	{9, 8, 2, 4},
/*140 */{9, 12, 1, 3},	{10, 2, 1, 6},	{10, 4, 1, 1},	{10, 7, 2, 6},
	{10, 9, 1, 2},	{11, 1, 1, 2},	{11, 7, 0, 0},	{11, 13, 2, 4},
	{12, 5, 0, 0},	{13, 5, 0, 0},	{13, 19, 0, 0},	{14, 1, 1, 3},
	{14, 1, 3, 4},	{14, 2, 1, 3},	{14, 2, 1, 4},	{14, 2, 1, 5},
	{14, 2, 1, 6},	{14, 2, 1, 10},	{14, 2, 2, 4},	{14, 2, 2, 5},
/*160 */{14, 2, 2, 6},	{14, 2, 3, 4},	{14, 2, 3, 9},	{14, 2, 4, 4},
	{14, 4, 1, 2},	{14, 4, 1, 4},	{14, 4, 1, 8},	{14, 4, 2, 5},
	{14, 5, 1, 2},	{14, 5, 1, 3},	{14, 5, 2, 4},	{14, 5, 2, 6},
	{14, 5, 3, 5},	{14, 12, 1, 2},	{14, 12, 1, 4},	{14, 13, 2, 4},
	{15, 2, 1, 6},	{15, 2, 3, 6},	{15, 5, 1, 8},	{15, 5, 2, 8},
/*180 */{15, 5, 2, 10},	{15, 5, 2, 12},	{15, 12, 1, 3},	{16, 13, 1, 2},
	{17, 3, 1, 10},	{18, 5, 0, 0},	{19, 5, 5, 8},	{19, 5, 12, 8},
	{19, 5, 14, 8},	{19, 5, 15, 8},	{19, 5, 18, 8},	{19, 5, 20, 8},
	{19, 5, 22, 8},	{19, 5, 26, 8},	{19, 5, 30, 8},	{19, 5, 32, 8},
	{19, 5, 34, 8},	{19, 5, 36, 8},	{19, 5, 38, 8},	{19, 5, 42, 8},
/*200 */{19, 5, 44, 8},	{19, 5, 46, 8},	{19, 5, 52, 8},	{20, 10, 0, 0},
	{21, 1, 0, 0},	{21, 5, 0, 0},	{21, 5, 1, 6},	{21, 7, 0, 0},
	{21, 12, 1, 4},	{22, 5, 2, 3},	{22, 12, 0, 0},	{22, 15, 1, 1},
/*212 */{1, 1, 10, 10},	{23, 5, 1, 3},	{24,  5, 0, 0}, {8,   1, 3, 8},
	{3,  1, 6, 6},	{4,  7, 4, 4},	{1,   1, 8, 6},	{1,   5, 2, 5},
	{5,  1, 9,12},

/*221 */{ 4, 7, 2, 4},	{10, 7, 2, 4},	{11, 7, 2, 4},  {19, 7, 20, 8},
	{17, 7, 2, 6},  {24, 7, 2, 6},  /* Beholder */

/*227 */{ 1,20, 4, 6},  { 1,20, 2, 6},  /* Butts */

/*229 */{ 9, 9, 3, 8},  /* Spit */

/*230 */{21, 1, 6, 8},

/*231 */{12, 1, 4, 4},  {13, 1, 4, 5}, /* Master Rogue */

/*233 */{ 1,21, 4, 4},
/*234 */{ 3, 2, 2, 2},
/*235 */{ 1, 1, 6, 6},
/*236 */{19, 2,52, 8}, /* Bite for xp drain */
/*237 */{18, 3, 5, 5}, /* Claw to drain Wisdom */
/*238 */{14, 3, 3, 3}, /* Algroth poisonous claw attack */
/*239 */{5, 22, 3, 3}, /* Fire */ /* vortices */
/*240 */{6, 22, 3, 3}, /* Acid */
/*241 */{7, 22, 3, 3}, /* Cold */
/*242 */{8, 22, 5, 5}, /* Lightning */
/*243 */{5, 22, 8, 8}, /* Plasma/fire */ /* vortices */
/*244 */{1, 22, 5, 5}, /* Hit */ /* vortices */
/*245 */{25, 1,10,12}, /* Morgoths dex drain attack */
/*246 */{19, 7,30,20}, /* Eye druj drain */
/*247 */{11, 2, 4, 4}, /* Skull */
/*248 */{17, 2, 4, 4}, /* druj */
/*249 */{18, 2, 4, 4}, /* attacks */
/*250 */{14, 2, 8, 6}, /* Lernean */
/*251 */{5,  2, 12, 6}, /* Hydra */
/*252 */{18, 3, 1, 10}, /* Another drain wisdom attack */
/*253 */{11, 4, 2, 6}, /* Carrion crawler */
/*254 */{16, 4, 8, 8}, /* Nightcrawler */
/*255 */{9,  2,10,10}, /* Nightcrawler */
/*256 */{21, 1,10,10}, /* Nightwalker */
/*257 */{21, 1, 7, 7}, /* Nightwalker */
/*258 */{12, 5, 5, 5}, /* Harowen  */
/*259 */{13, 5, 5, 5}, /*   the Black Hand */
/*260 */{10, 1,10, 5}, /* Harowen  */
/*261 */{14, 1, 8, 5}, /* Harowen  */
/*262 */{ 1, 1,20,10}, /* Morgoth attacks */
/*263 */{ 1, 6,20, 2}, /* Mystic kick */
/*264 */{14, 1,20, 1}, /* Mystic poison */
/*265 */{11, 1,15, 1}, /* Mystic paralysis */
/*266 */{ 1, 6,10, 2}, /* Mystic kick */
/*267 */{11,11, 6, 6}, /* Medusa paralyse */
/*268 */{ 1,19, 0, 0}, /* Nermal insults */
/*269 */{ 3, 1,12, 12}, /* Greater titan */
/*270 */{21, 1,10, 12}, /* Sauron punch */
/*271 */{ 1, 3, 3, 12}, 
/*272 */{ 1, 3, 4, 12},
/*273 */{ 1, 3, 5, 12},
/*274 */{ 1, 3, 6, 12},
/*275 */{ 1, 3, 8, 12},
/*276 */{ 1, 2, 3, 14}, /* New claws and bites for those wimpy dragons! */
/*277 */{ 1, 2, 4, 14},
/*278 */{ 1, 2, 5, 14},
/*279 */{ 1, 2, 6, 14},
/*280 */{ 1, 2, 8, 14},
/*281 */{ 1, 2, 10,14},
/*282 */{ 1,20, 12,13},
/*283 */{ 1,23,  0, 0},
/*284 */{ 8, 1, 12,12},
};



/*
 * Class titles for different levels			
 */
cptr player_title[MAX_CLASS][MAX_PLAYER_LEVEL] = {

	/* Warrior	 */
{"Rookie","Private","Soldier","Mercenary","Veteran(1st)","Veteran(2nd)",
"Veteran(3rd)","Warrior(1st)","Warrior(2nd)","Warrior(3rd)","Warrior(4th)",
"Swordsman-1","Swordsman-2","Swordsman-3","Hero","Swashbuckler","Myrmidon",
"Champion-1","Champion-2","Champion-3","Superhero","Knight","Superior Knt",
"Gallant Knt","Knt Errant","Guardian Knt","Baron","Duke","Lord (1st)",
"Lord (2nd)","Lord (3rd)","Lord (4th)","Lord (5th)","Lord (6th)","Lord (7th)",
"Lord (8th)","Lord (9th)","Lord (10th)","Lord (11th)","Lord (12th)",
"Lord (13th)","Lord (14th)","Lord (15th)","Lord (16th)","Lord (17th)",
"Lord (18th)","Lord (19th)","Lord Gallant","Lord Keeper","Lord Noble"},

	/* Mage		 */
{"Novice","Apprentice","Trickster-1","Trickster-2","Trickster-3","Cabalist-1",
"Cabalist-2","Cabalist-3","Visionist","Phantasmist","Shadowist","Spellbinder",
"Illusionist","Evoker (1st)","Evoker (2nd)","Evoker (3rd)","Evoker (4th)",
"Conjurer","Theurgist","Thaumaturge","Magician","Enchanter","Warlock",
"Sorcerer","Necromancer","Mage (1st)","Mage (2nd)","Mage (3rd)","Mage (4th)",
"Mage (5th)","Wizard (1st)","Wizard (2nd)","Wizard (3rd)","Wizard (4th)",
"Wizard (5th)","Wizard (6th)","Wizard (7th)","Wizard (8th)","Wizard (9th)",
"Wizard (10th)","Wizard (11th)","Wizard (12th)","Wizard (13th)",
"Wizard (14th)","Wizard (15th)","Wizard (16th)","Wizard (17th)",
"Wizard (18th)","Wizard (19th)","Wizard Lord"},

	/* Priests	 */
{"Believer","Acolyte(1st)","Acolyte(2nd)","Acolyte(3rd)","Adept (1st)",
"Adept (2nd)","Adept (3rd)","Priest (1st)","Priest (2nd)","Priest (3rd)",
"Priest (4th)","Priest (5th)","Priest (6th)","Priest (7th)","Priest (8th)",
"Priest (9th)","Curate (1st)","Curate (2nd)","Curate (3rd)","Curate (4th)",
"Curate (5th)","Curate (6th)","Curate (7th)","Curate (8th)","Curate (9th)",
"Canon (1st)","Canon (2nd)","Canon (3rd)","Canon (4th)","Canon (5th)",
"Canon (6th)","Canon (7th)","Canon (8th)","Canon (9th)",
"Low Lama","Lama-1","Lama-2","Lama-3","Lama-4","Lama-5","Lama-6","Lama-7",
"Lama-8","Lama-9","High Lama","Great Lama","Patriarch",
"High Priest","Great Priest","Noble Priest"},

	/* Rogues	 */
{"Vagabond","Footpad","Cutpurse","Robber","Burglar","Filcher","Sharper",
"Magsman","Common Rogue","Rogue (1st)","Rogue (2nd)","Rogue (3rd)",
"Rogue (4th)","Rogue (5th)","Rogue (6th)","Rogue (7th)","Rogue (8th)",
"Rogue (9th)","Master Rogue","Expert Rogue","Senior Rogue","Chief Rogue",
"Prime Rogue","Low Thief","Thief (1st)","Thief (2nd)","Thief (3rd)",
"Thief (4th)","Thief (5th)","Thief (6th)","Thief (7th)","Thief (8th)",
"Thief (9th)","Thief (10th)","Thief (11th)","Thief (12th)","Thief (13th)",
"Thief (14th)","Thief (15th)","Thief (16th)","Thief (17th)","Thief (18th)",
"Thief (19th)","High Thief","Master Thief","Executioner","Low Assassin",
"Assassin","High Assassin","Guildsmaster"},

	/* Rangers	 */
{"Runner (1st)","Runner (2nd)","Runner (3rd)","Strider (1st)","Strider (2nd)",
"Strider (3rd)","Scout (1st)","Scout (2nd)","Scout (3rd)","Scout (4th)",
"Scout (5th)","Courser (1st)","Courser (2nd)","Courser (3rd)","Courser (4th)",
"Courser (5th)","Tracker (1st)","Tracker (2nd)","Tracker (3rd)",
"Tracker (4th)","Tracker (5th)","Tracker (6th)","Tracker (7th)",
"Tracker (8th)","Tracker (9th)","Guide (1st)","Guide (2nd)","Guide (3rd)",
"Guide (4th)","Guide (5th)","Guide (6th)","Guide (7th)","Guide (8th)",
"Guide (9th)","Guide (10th)","Guide (11th)","Guide (12th)","Guide (13th)",
"Guide (14th)","Guide (15th)",
"Pathfinder-1","Pathfinder-2","Pathfinder-3","Pathfinder-4","Pathfinder-5",
"Pathfinder-6","Pathfinder-7","Ranger","High Ranger","Ranger Lord"},

	/* Paladins	 */
{"Gallant","Keeper (1st)","Keeper (2nd)","Keeper (3rd)","Keeper (4th)",
"Keeper (5th)","Keeper (6th)","Keeper (7th)","Keeper (8th)","Keeper (9th)",
"Protector-1","Protector-2","Protector-3","Protector-4","Protector-5",
"Protector-6","Protector-7","Protector-8","Defender-1","Defender-2",
"Defender-3","Defender-4","Defender-5","Defender-6","Defender-7","Defender-8",
"Warder (1st)","Warder (2nd)","Warder (3rd)","Warder (4th)","Warder (5th)",
"Warder (6th)","Warder (7th)","Warder (8th)","Warder (9th)","Warder (10th)",
"Warder (11th)","Warder (12th)","Warder (13th)","Warder (14th)",
"Warder (15th)","Warder (16th)","Warder (17th)","Warder (18th)",
"Warder (19th)","Guardian","Chevalier","Justiciar","Paladin","High Lord"}

};


/* Base experience levels, may be adjusted up for race and/or class*/
u32b player_exp[MAX_PLAYER_LEVEL] = {
	10,		25,		45,		70,
	100,		140,		200,		280,
	380,		500,		650,		850,
	1100,		1400,		1800,		2300,
	2900,		3600,		4400,		5400,
	6800,		8400,		10200,		12500,
	17500,		25000,		35000L,		50000L,
	75000L,		100000L,	150000L,	200000L,
	275000L,	350000L,	450000L,	550000L,
	700000L,	850000L,	1000000L,	1250000L,
	1500000L,	1800000L,	2100000L,	2400000L,
	2700000L,	3000000L,	3500000L,	4000000L,
	4500000L,	5000000L
};


/*
 * Player Race Information:
 *	STR,INT,WIS,DEX,CON,CHR,
 *	Ages, heights, and weights (male then female)
 *	Racial Bases for: dis,srh,stl,fos,bth,bthb,bsav,hitdie,
 *	infra, exp base, choice-classes
 */

player_race race[MAX_RACES] = {
   {"Human",	 0,  0,	 0,  0,	 0,  0,
      14,  6, 72,  6,180, 25, 66,  4,150, 20,
      0,  0,  0,  0,  0,  0,  0, 10,  0, 100, 0x3F,
    },
   {"Half-Elf", -1,  1,	 0,  1, -1,  1,
      24, 16, 66,  6,130, 15, 62,  6,100, 10,
      2,  6,  1, -1, -1,  5,  3,  9,  2, 110, 0x3F,
    },
   {"Elf",	-1,  2,	 1,  1, -2,  1,
      75, 75, 60,  4,100,  6, 54,  4, 80,  6,
      5,  8,  1, -2, -5, 15,  6,  8,  3, 120, 0x1F,
    },
   {"Hobbit", -2,  2,	 1,  3,	 2,  1,
      21, 12, 36,  3, 60,  3, 33,  3, 50,  3,
      15, 12,  4, -5,-10, 20, 18,  7,  4, 110, 0x0B,
    },
   {"Gnome",	-1,  2,	 0,  2,	 1, -2,
      50, 40, 42,  3, 90,  6, 39,  3, 75,  3,
      10,  6,  3, -3, -8, 12, 12,  8,  4, 125, 0x0F,
    },
   {"Dwarf",	 2, -3,	 2, -2,	 2, -3,
      35, 15, 48,  3,150, 10, 46,  3,120, 10,
      2,  7,  -1,  0, 15,  0,  9,  11,  5, 120, 0x05,
    },
   {"Half-Orc",	 2, -1,	 0,  0,	 1, -4,
      11,  4, 66,  1,150,  5, 62,  1,120,  5,
      -3,  0, -1,  3, 12, -5, -3, 10,  3, 110, 0x0D,
    },
   {"Half-Troll",4, -4, -2, -4,	 3, -6,
      20, 10, 96, 10,255, 50, 84,  8,225, 40,
      -5, -1, -2,  5, 20,-10, -8, 12,  3, 120, 0x05,
    },
   {"Dunadan",  1,  2,  2,  2,  3,  2,
      50, 20, 82, 5, 190, 20, 78,  6, 180, 15,
      4,   3,  2, -3, 15, 10,  5, 10,  0, 180, 0x3F,
    },
   {"High-Elf",  1,  3, -1,  3,  1,  5,
     100, 30, 90,10, 190, 20, 82, 10, 180, 15,
      4,   3,  3, -4, 15, 25, 20, 10,  4, 200, 0x1F,
    }
 };


/*
 * Background information				
 */
player_background background[MAX_BACKGROUND] = {

{"You are the illegitimate and unacknowledged child ",		 10, 1, 2, 25},
{"You are the illegitimate but acknowledged child ",		 20, 1, 2, 35},
{"You are one of several children ",				 95, 1, 2, 45},
{"You are the first child ",					100, 1, 2, 50},

{"of a Serf.  ",						 40, 2, 3, 65},
{"of a Yeoman.  ",						 65, 2, 3, 80},
{"of a Townsman.  ",						 80, 2, 3, 90},
{"of a Guildsman.  ",						 90, 2, 3,105},
{"of a Landed Knight.  ",					 96, 2, 3,120},
{"of a Titled Noble.  ",					 99, 2, 3,130},
{"of a Royal Blood Line.  ",					100, 2, 3,140},

{"You are the black sheep of the family.  ",			 20, 3,50, 20},
{"You are a credit to the family.  ",				 80, 3,50, 55},
{"You are a well liked child.  ",				100, 3,50, 60},

{"Your mother was of the Teleri.  ",				 40, 4, 1, 50},
{"Your father was of the Teleri.  ",				 75, 4, 1, 55},
{"Your mother was of the Noldor.  ",			 	 90, 4, 1, 55},
{"Your father was of the Noldor.  ",			 	 95, 4, 1, 60},
{"Your mother was of the Vanyar.  ",				 98, 4, 1, 65},
{"Your father was of the Vanyar.  ",				100, 4, 1, 70},

{"You are one of several children ",				 60, 7, 8, 50},
{"You are the only child ",					100, 7, 8, 55},

{"of a Teleri ",						 75, 8, 9, 50},
{"of a Noldor ",						 95, 8, 9, 55},
{"of a Vanyar ",						100, 8, 9, 60},

{"Ranger.  ",							 40, 9,54, 80},
{"Archer.  ",							 70, 9,54, 90},
{"Warrior.  ",							 87, 9,54,110},
{"Mage.  ",							 95, 9,54,125},
{"Prince.  ",							 99, 9,54,140},
{"King.  ",							100, 9,54,145},

{"You are one of several children of a Hobbit ",		 85,10,11, 45},
{"You are the only child of a Hobbit ",			        100,10,11, 55},

{"Bum.  ",							 20,11, 3, 55},
{"Tavern Owner.  ",						 30,11, 3, 80},
{"Miller.  ",							 40,11, 3, 90},
{"Home Owner.  ",						 50,11, 3,100},
{"Burglar.  ",							 80,11, 3,110},
{"Warrior.  ",							 95,11, 3,115},
{"Mage.  ",							 99,11, 3,125},
{"Clan Elder.  ",						100,11, 3,140},

{"You are one of several children of a Gnome ",			 85,13,14, 45},
{"You are the only child of a Gnome ",				100,13,14, 55},

{"Beggar.  ",							 20,14, 3, 55},
{"Braggart.  ",							 50,14, 3, 70},
{"Prankster.  ",						 75,14, 3, 85},
{"Warrior.  ",							 95,14, 3,100},
{"Mage.  ",							100,14, 3,125},

{"You are one of two children of a Dwarven ",			 25,16,17, 40},
{"You are the only child of a Dwarven ",			100,16,17, 50},

{"Thief.  ",							 10,17,18, 60},
{"Prison Guard.  ",						 25,17,18, 75},
{"Miner.  ",							 75,17,18, 90},
{"Warrior.  ",							 90,17,18,110},
{"Priest.  ",							 99,17,18,130},
{"King.  ",							100,17,18,150},

{"You are the black sheep of the family.  ",			 15,18,57, 10},
{"You are a credit to the family.  ",				 85,18,57, 50},
{"You are a well liked child.  ",				100,18,57, 55},

{"Your mother was an Orc, but it is unacknowledged.  ",		 25,19,20, 25},
{"Your father was an Orc, but it is unacknowledged.  ",		100,19,20, 25},

{"You are the adopted child ",					100,20, 2, 50},

{"Your mother was a Cave-Troll ",				 30,22,23, 20},
{"Your father was a Cave-Troll ",				 60,22,23, 25},
{"Your mother was a Hill-Troll ",				 75,22,23, 30},
{"Your father was a Hill-Troll ",				 90,22,23, 35},
{"Your mother was a Water-Troll ",				 95,22,23, 40},
{"Your father was a Water-Troll ",				100,22,23, 45},

{"Cook.  ",							  5,23,62, 60},
{"Warrior.  ",							 95,23,62, 55},
{"Shaman.  ",							 99,23,62, 65},
{"Clan Chief.  ",						100,23,62, 80},

{"You have dark brown eyes, ",					 20,50,51, 50},
{"You have brown eyes, ",					 60,50,51, 50},
{"You have hazel eyes, ",					 70,50,51, 50},
{"You have green eyes, ",					 80,50,51, 50},
{"You have blue eyes, ",					 90,50,51, 50},
{"You have blue-gray eyes, ",					100,50,51, 50},

{"straight ",							 70,51,52, 50},
{"wavy ",							 90,51,52, 50},
{"curly ",							100,51,52, 50},

{"black hair, ",						 30,52,53, 50},
{"brown hair, ",						 70,52,53, 50},
{"auburn hair, ",						 80,52,53, 50},
{"red hair, ",							 90,52,53, 50},
{"blond hair, ",						100,52,53, 50},

{"and a very dark complexion.",					 10,53, 0, 50},
{"and a dark complexion.",					 30,53, 0, 50},
{"and an average complexion.",					 80,53, 0, 50},
{"and a fair complexion.",					 90,53, 0, 50},
{"and a very fair complexion.",					100,53, 0, 50},

{"You have light grey eyes, ",					 85,54,55, 50},
{"You have light blue eyes, ",					 95,54,55, 50},
{"You have light green eyes, ",					100,54,55, 50},

{"straight ",							 75,55,56, 50},
{"wavy ",							100,55,56, 50},

{"black hair, and a fair complexion.",				 75,56, 0, 50},
{"brown hair, and a fair complexion.",				 85,56, 0, 50},
{"blond hair, and a fair complexion.",				 95,56, 0, 50},
{"silver hair, and a fair complexion.",				100,56, 0, 50},

{"You have dark brown eyes, ",					 99,57,58, 50},
{"You have glowing red eyes, ",					100,57,58, 60},

{"straight ",							 90,58,59, 50},
{"wavy ",							100,58,59, 50},

{"black hair, ",						 75,59,60, 50},
{"brown hair, ",						100,59,60, 50},

{"a one foot beard, ",						 25,60,61, 50},
{"a two foot beard, ",						 60,60,61, 51},
{"a three foot beard, ",					 90,60,61, 53},
{"a four foot beard, ",						100,60,61, 55},

{"and a dark complexion.",					100,61, 0, 50},

{"You have slime green eyes, ",					 60,62,63, 50},
{"You have puke yellow eyes, ",					 85,62,63, 50},
{"You have blue-bloodshot eyes, ",				 99,62,63, 50},
{"You have glowing red eyes, ",					100,62,63, 55},

{"dirty ",							 33,63,64, 50},
{"mangy ",							 66,63,64, 50},
{"oily ",							100,63,64, 50},

{"sea-weed green hair, ",					 33,64,65, 50},
{"bright red hair, ",						 66,64,65, 50},
{"dark purple hair, ",						100,64,65, 50},

{"and green ",							 25,65,66, 50},
{"and blue ",							 50,65,66, 50},
{"and white ",							 75,65,66, 50},
{"and black ",							100,65,66, 50},

{"ulcerous skin.",						 33,66, 0, 50},
{"scabby skin.",						 66,66, 0, 50},
{"leprous skin.",						100,66, 0, 50}

};


/*
 * Player Classes.
 */
player_class class[MAX_CLASS] = {

/*	  HP Dis Src Stl Fos bth btb sve S  I  W  D Co Ch  Spell Exp  spl */

{"Warrior",9, 25, 14, 1, 38, 70, 55, 18, 5,-2,-2, 2, 2,-1, NONE,    0, 0},
{"Mage",   0, 30, 16, 2, 20, 34, 20, 36,-5, 3, 0, 1,-2, 1, MAGE,   30, 1},
{"Priest", 2, 25, 16, 2, 32, 48, 35, 30,-1,-3, 3,-1, 0, 2, PRIEST, 20, 1},
{"Rogue",  6, 45, 32, 5, 16, 60, 66, 30, 2, 1,-2, 3, 1,-1, MAGE,   25, 5},
{"Ranger", 4, 30, 24, 3, 24, 56, 72, 30, 2, 2, 0, 1, 1, 1, MAGE,   30, 3},
{"Paladin",6, 20, 12, 1, 38, 68, 40, 24, 3,-3, 1, 0, 2, 2, PRIEST, 35, 1}
};



/*
 * making it 16 bits wastes a little space, but saves much
 * signed/unsigned headaches in its use
 * CLA_MISC_HIT is identical to CLA_SAVE, which takes advantage of
 * the fact that the save values are independent of the class
 */
s16b class_level_adj[MAX_CLASS][MAX_LEV_ADJ] = {

/*	       bth    bthb   device  disarm   save/misc hit  */
/* Warrior */ {	4,	4,	2,	2,	3 },
/* Mage    */ { 2,	2,	4,	2,	3 },
/* Priest  */ { 2,	2,	4,	3,	3 },
/* Rogue   */ { 3,	4,	3,	4,	3 },
/* Ranger  */ { 3,	4,	3,	3,	3 },
/* Paladin */ { 3,	2,	3,	2,	3 }

};


/*
 * Warriors don't have spells, so there is no entry for them.  Note that
 * this means you must always subtract one from the p_ptr->pclass before
 * indexing into magic_spell[].
 */
spell_type magic_spell[MAX_CLASS-1][63] = {

  {
      /*** Mage ***/

/* Beginners Magic */
     {	1,  1, 22,   1},
     {	1,  1, 23,   1},
     {	1,  2, 24,   1},
     {	1,  2, 26,   1},
     { 99, 99,  0,   0},
     {	3,  3, 25,   2},
     { 99, 99,  0,   0},
     {	3,  3, 25,   1},
     {	3,  3, 27,   2},

/* Conjuring and Tricks */
     {	3,  4, 30,   1},
     {	5,  4, 30,   6},
     {	5,  5, 30,   8},
     {	5,  5, 30,   5},
     {	5,  5, 35,   6},
     {	7,  6, 35,   9},
     {	7,  6, 30,  10},
     {	7,  6, 40,  12},
     {	9,  7, 44,  19},

/* Incantations and Illusions */
     {	9,  7, 45,  19},
     {	9,  7, 75,  22},
     {	9,  7, 45,  19},
     { 11,  7, 45,  25},
     { 11,  7, 75,  19},
     { 13,  7, 50,  22},
     { 15,  9, 50,  25},
     { 17,  9, 50,  31},

/* Sorcery and Evocoations */
     { 19, 12, 55,  38},
     { 21, 12, 90,  44},
     { 23, 12, 60,  50},
     { 25, 12, 65,  63},
     { 29, 18, 65,  88},
     { 33, 21, 80, 125},
     { 37, 25, 95, 200},

     { 7,   7, 20,  50},
     { 9,  12, 40, 100},
     { 15, 17, 60, 110},
     { 20, 18, 60, 120},
     { 25, 25, 75, 120},

     { 10, 6,  50,  30},
     { 12, 9,  60,  50},
     { 20, 15, 70, 100},
     { 27, 25, 75, 200},
     { 35, 35, 85, 300},
     { 42, 45, 95,2000},

     { 5,  5,  50,  10},
     { 10, 10, 70,  100},
     { 25, 30, 95, 1000},
     { 30, 50, 70,  300},
     { 40, 50, 80, 1000},

     { 4, 5,  50,   100},
     { 4, 5,  50,   100},
     { 4, 5,  50,   100},
     { 8, 10, 75,   200},
     { 15, 20,  85, 1000},

     { 5, 5,  50,   100},
     { 10, 12, 75,  300},
     { 15, 20, 80,  800},
     { 22, 30, 50, 2000},
     { 45, 70, 75, 5000},

     { 99, 99,  0,   0},
     { 99, 99,  0,   0},
     { 99, 99,  0,   0},
     { 99, 99,  0,   0}
   },

   {
	/*** Priest ***/

     {	1,  1, 10,   1},
     {	1,  2, 15,   1},
     {	1,  2, 20,   1},
     {	1,  2, 25,   1},
     {	3,  2, 25,   1},
     {	3,  3, 27,   2},
     {	3,  3, 27,   2},
     {	3,  3, 28,   3},

     {	5,  4, 29,   4},
     {	5,  4, 30,   5},
     {	5,  4, 32,   5},
     {	5,  5, 34,   5},
     {	7,  5, 36,   6},
     {	7,  5, 38,   7},
     {	7,  6, 38,   9},
     {	7,  7, 38,   9},

     {	9,  6, 38,  10},
     {	9,  7, 38,  10},
     {	9,  7, 40,  10},
     { 11,  8, 42,  10},
     { 11,  8, 42,  12},
     { 11,  9, 55,  15},
     { 13, 10, 45,  15},
     { 13, 11, 45,  16},
     { 15, 12, 50,  20},
     
     { 15, 14, 50,  22},
     { 17, 14, 55,  32},
     { 21, 16, 60,  38},
     { 25, 20, 70,  75},
     { 33, 55, 90, 125},
     { 39, 32, 95, 200},

     { 3,  3,  50,   2},
     { 10, 10, 80,   50},
     { 20, 20, 80,   100},
     { 25, 10, 80,  1000},
     { 35, 50, 80,  2000},

     { 15,  5, 50,   100},
     { 17,  7, 60,   200},
     { 30, 50, 80,  1000},
     { 35, 70, 90,  2000},
     { 35, 70, 90,  3000},

     { 15, 7,  70,  100},
     { 20, 10, 75,  300},
     { 25, 25, 80,  1500},
     { 35, 35, 80,  1000},
     { 45, 60, 75,  4000},

     { 5, 6,  50,   50},
     { 15, 20, 80,  100},
     { 25, 40, 80,  1000},
     { 35, 50, 80,  2000},
     { 37, 60, 85,  3000},
     { 45, 95, 85,  6000},

     { 3, 3,  50,   5},
     { 10, 10,  50,  20},
     { 20, 20,  80,  80},
     { 30, 40,  75, 1000},
     { 35, 50,  75, 100},
     { 40, 60,  75, 3000},

     { 99, 99,  0,   0},
     { 99, 99,  0,   0},
     { 99, 99,  0,   0},
     { 99, 99,  0,   0},
     { 99, 99,  0,   0}
   },

   {
	/*** Rogue ***/

     { 99, 99,	0,   0},
     {	5,  1, 50,   1},
     {	7,  2, 55,   1},
     {	9,  3, 60,   2},
     { 10,  3, 60,   2},
     { 11,  4, 65,   2},
     { 12,  4, 65,   3},
     { 13,  5, 70,   3},
     { 99, 99,	0,   0},

     { 15,  6, 75,   3},
     { 99, 99,	0,   0},
     { 17,  7, 80,   4},
     { 19,  8, 85,   5},
     { 21,  9, 90,   6},
     { 22,  9, 50,   7},
     { 23, 10, 95,   7},
     { 99, 99,	0,   0},
     { 24, 11, 70,  10},

     { 25, 12, 95,  11},
     { 27, 15, 99,  12},
     { 99, 99,	0,   0},
     { 99, 99,	0,   0},
     { 28, 18, 50,  19},
     { 99, 99,	0,   0},
     { 99, 99,	0,   0},
     { 99, 99,	0,   0},

     { 99, 99,	0,   0},
     { 99, 99,	0,   0},
     { 99, 99,	0,   0},
     { 32, 25, 70,  50},
     { 99, 99,	0,   0},
     { 99, 99,	0,   0},
     { 99, 99,	0,   0},

     { 7,   7, 20,  50},
     { 9,  12, 40, 100},
     { 15, 17, 60, 110},
     { 99, 99,	0,   0},
     { 30, 35, 75, 120},

     { 13, 16,  50,  30},
     { 18, 20,  60,  50},
     { 99, 99,	0,   0},
     { 99, 99,	0,   0},
     { 99, 99,	0,   0},
     { 99, 99,	0,   0},

     { 5,  5,  50,  10},
     { 10, 10, 70,  100},
     { 35, 40, 95, 1000},
     { 99, 99,	0,   0},
     { 99, 99,	0,   0},

     { 10, 12,  50,   100},
     { 10, 12,  50,   100},
     { 10, 12,  50,   100},
     { 15, 20,  75,   200},
     { 25, 30,  85,  1000},

     { 10, 11,  50,   100},
     { 15, 20, 75,  300},
     { 20, 25, 80,  800},
     { 26, 30, 50, 2000},
     { 99, 99,  0,   0},

     { 99, 99,  0,   0},
     { 99, 99,  0,   0},
     { 99, 99,  0,   0},
     { 99, 99,  0,   0}
   },

   {
	/*** Ranger ***/

     {	3,  1, 30,   1},
     {	3,  2, 35,   2},
     {	3,  2, 35,   2},
     {	5,  3, 35,   2},
     { 99, 99,  0,   0},
     {	5,  3, 40,   2},
     { 99, 99,  0,   0},
     {	5,  4, 45,   3},
     {	7,  5, 40,   6},
     {	7,  6, 40,   5},
     {	9,  7, 40,   7},
     {	9,  8, 45,   8},
     { 11,  8, 40,  10},
     { 11,  9, 45,  10},
     { 13, 10, 45,  12},
     { 13, 11, 55,  13},
     { 15, 12, 50,  15},
     { 15, 13, 50,  15},
     { 17, 17, 55,  15},
     { 17, 17, 90,  17},
     { 21, 17, 55,  17},
     { 21, 19, 60,  18},
     { 23, 25, 90,  20},
     { 23, 20, 60,  20},
     { 25, 20, 60,  20},
     { 25, 21, 65,  20},
     { 27, 21, 65,  22},
     { 29, 23, 95,  23},
     { 31, 25, 70,  25},
     { 33, 25, 75,  38},
     { 35, 25, 80,  50},
     { 37, 30, 95, 100},
     { 99, 99,	0,   0},

     { 8,  17, 20,  50},
     { 19,  22, 40, 100},
     { 25, 27, 60, 110},
     { 30, 28, 60, 120},
     { 35, 35, 75, 120},

     { 20, 16,  50,  30},
     { 22, 19,  60,  50},
     { 30, 25, 70, 100},
     { 37, 35, 75, 200},
     { 35, 45, 85, 300},
     { 99, 99, 0,    0},

     { 10,  15,  50,  10},
     { 15, 20, 70,  100},
     { 35, 60, 95, 1000},
     { 99, 99, 0,    0},
     { 99, 99, 0,    0},

     { 8, 15,  50,   100},
     { 8, 15,  50,   100},
     { 8, 15,  50,   100},
     { 16, 25, 75,   200},
     { 25, 40,  85, 1000},

     { 10, 15,  50,   100},
     { 15, 20, 75,  300},
     { 25, 30, 80,  800},
     { 32, 50, 50, 2000},
     { 99, 99,  0,   0},

     { 99, 99,  0,   0},
     { 99, 99,  0,   0},
     { 99, 99,  0,   0},
     { 99, 99,  0,   0}
   },

   {
	/*** Paladin ***/

     {	1,  1, 30,   1},
     {	2,  2, 35,   2},
     {	3,  3, 35,   3},
     {	5,  3, 35,   5},
     {	5,  4, 35,   5},
     {	7,  5, 40,   6},
     {	7,  5, 40,   6},
     {	9,  7, 40,   7},
     {	9,  7, 40,   8},
     {	9,  8, 40,   8},
     { 11,  9, 40,  10},
     { 11, 10, 45,  10},
     { 11, 10, 45,  10},
     { 13, 10, 45,  12},
     { 13, 11, 45,  13},
     { 15, 13, 45,  15},
     { 15, 15, 50,  15},
     { 17, 15, 50,  17},
     { 17, 15, 50,  18},
     { 19, 15, 50,  19},
     { 19, 15, 50,  19},
     { 21, 17, 50,  20},
     { 23, 17, 50,  20},
     { 25, 20, 50,  20},
     { 27, 21, 50,  22},
     { 29, 22, 50,  24},
     { 31, 24, 60,  25},
     { 33, 28, 60,  31},
     { 35, 32, 70,  38},
     { 37, 70, 90,  50},
     { 39, 38, 95, 100},

     { 5,  5,  50,   2},
     { 15, 15, 80,   50},
     { 25, 25, 80,   100},
     { 30, 15, 80,  1000},
     { 37, 55, 80,  2000},

     { 17,  15, 50,   100},
     { 23,  25, 60,   200},
     { 35, 60, 80,  1000},
     { 40, 80, 90,  2000},
     { 40, 80, 90,  3000},

     { 20, 13,  70,  100},
     { 30, 20, 75,  300},
     { 30, 35, 80,  1500},
     { 40, 40, 80,  1000},
     { 47, 70, 75,  4000},

     { 10, 16,  50,   50},
     { 25, 30, 80,  100},
     { 30, 50, 80,  1000},
     { 40, 70, 80,  2000},
     { 42, 80, 85,  3000},
     { 47, 95, 85,  6000},

     { 7, 7,  50,   5},
     { 20, 20,  50,  20},
     { 25, 25,  80,  80},
     { 35, 50,  75, 1000},
     { 40, 60,  75, 100},
     { 45, 70,  75, 3000},

     { 99, 99,  0,   0},
     { 99, 99,  0,   0},
     { 99, 99,  0,   0},
     { 99, 99,  0,   0},
     { 99, 99,  0,   0}
   }
 };


cptr spell_names[127] = {

    /*** Mage Spells ***/

  "Magic Missile",  "Detect Monsters", "Phase Door",  "Light Area",
  "Treasure Detection", "Cure Light Wounds",  "Object Detection",
  "Find Hidden Traps/Doors",  "Stinking Cloud",

  "Confusion", "Lightning Bolt",  "Trap/Door Destruction", "Sleep I",
  "Cure Poison",  "Teleport Self",  "Spear of Light",  "Frost Bolt",
  "Turn Stone to Mud",
  
#ifdef SATISFY_HUNGER
  "Staisfy Hunger",
#else
  "Create Food",
#endif
  "Recharge Item I",  "Sleep II", "Polymorph Other",
  "Identify",  "Sleep III",  "Fire Bolt",  "Slow Monster",

  "Frost Ball", "Recharge Item II", "Teleport Other", "Haste Self",
  "Fire Ball", "Word of Destruction", "Genocide",

    /* Mordenkainen's Escapes */
  "Door Creation",
  "Stair Creation",
  "Teleport Level",
  "Earthquake",
  "Word of Recall",

    /* Raal's Tome of Destruction */
  "Acid Bolt",
  "Cloud Kill",
  "Acid Ball",
  "Ice Storm",
  "Meteor Swarm",
  "Hellfire",

    /*Kelek's Grimoire of Power*/
  "Detect Evil",
  "Detect Enchantment",
  "Recharge Item III",
  "Genocide",
  "Mass Genocide",

    /* Resistance of Scarabtarices */
  "Resist Fire",
  "Resist Cold",
  "Resist Acid",
  "Resist Poison",
  "Resistance",

    /* Tenser's transformations...*/
  "Heroism",
  "Shield",
  "Berserker",
  "Essence of Speed",
  "Globe of Invulnerability",

  "blank",
  "blank",
  "blank",
  "blank",


    /*** Priest Spells (starting at 63) ***/

  "Detect Evil",  "Cure Light Wounds", "Bless",  "Remove Fear",
  "Call Light", "Find Traps",  "Detect Doors/Stairs",  "Slow Poison",
  
  "Blind Creature", "Portal",  "Cure Medium Wounds",  "Chant",
  "Sanctuary",
#ifdef SATISFY_HUNGER
  "Satisfy Hunger",
#else
  "Create Food",
#endif
  "Remove Curse",  "Resist Heat and Cold",
  
  "Neutralize Poison", "Orb of Draining",  "Cure Serious Wounds",
  "Sense Invisible", "Protection from Evil",  "Earthquake",
  "Sense Surroundings", "Cure Critical Wounds",  "Turn Undead",
  
  "Prayer",  "Dispel Undead", "Heal",  "Dispel Evil",
  "Glyph of Warding", "Holy Word",

    /* Godly Insights... */
  "Detect Monsters",
  "Detection",
  "Perception",
  "Probing",
  "Clairvoyance",

    /* Purifications and Healing */
  "Cure Serious Wounds",
  "Cure Critical Wounds",
  "Healing",
  "Restoration",
  "Remembrance",

    /* Wrath of God */
  "Dispel Undead",
  "Dispel Evil",
  "Banishment",
  "Word of Destruction",
  "Annihilation",

    /* Holy Infusions */
  "Unbarring Ways",
  "Recharging",
  "Dispel Curse",
  "Enchant Weapon",
  "Enchant Armour",
  "Elemental Brand",

    /* Ethereal openings */
  "Blink",
  "Teleport",
  "Teleport Away",
  "Teleport Level",
  "Word of Recall",
  "Alter Reality",

  "blank",
  "blank",
  "blank",
  "blank",
  "blank"
};


/*
 * spellmasks[][] is used to control the "you seem to be missing a book"
 * messages, because they look stupid otherwise.
 */
u32b spellmasks[MAX_CLASS][2] = {
	{ 0x0L, 0x0L },			/* warrior */
	{ 0xffffffafL, 0x0fffffffL },	/* mage */
	{ 0xffffffffL, 0x03ffffffL },	/* priest */
	{ 0x284efafeL, 0x03fe70eeL },	/* rogue */
	{ 0xffffffafL, 0x03fe77feL },	/* ranger */
	{ 0xffffffffL, 0x03ffefffL }	/* paladin */
};



