/* treasure.c: dungeon object definitions

   Copyright (c) 1989 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include "constant.h"
#include "config.h"
#include "types.h"

/* Following are treasure arrays	and variables			*/

/* Object description:	Objects are defined here.  Each object has
  the following attributes:
	Descriptor : Name of item and formats.
		& is replaced with 'a', 'an', or a number.
		~ is replaced with null or 's'.
	Flags	   : Abilities of object.  Each ability is a
		     bit.  Bits 1-31 are used. (Signed integer)
	Type value : Value representing the type of object.
	Character  : Character that represents the item.
	P1	   : Catch all for magical abilities such as
		     plusses to strength, minuses to searching.
	Cost	   : Relative cost of item.
	Sub value  : separate value for each item of a type.
		0 - 63: object can not stack
		64 - 127: dungeon object, can stack with other D object
		128 - 191: unused, previously for store items
		192: stack with other iff have same pval value, always
			treated as individual objects
		193 - 255: object can stack with others iff they have
			the same pval value, usually considered one group
		Objects which have two type values, e.g. potions and
		scrolls, need to have distinct svals for
		each item regardless of its tval
	Number	   : number of items appearing in group.
	Weight	   : relative weight of an item.
	To hit	   : magical plusses to hit.
	To damage  : magical plusses to damage.
	AC	   : objects relative armor class.
		1 is worse than 5 is worse than 10 etc.
	To AC	   : Magical bonuses to AC.
	Damage	   : amount of damage item can cause.
	Level	   : Minimum level on which item can be found.
	Rarity     : Normally 0, but is 1/rare chance of it being found
	Flags2     : Even more abilities of objects...

	Special Abilities can be added to item by magic_init(),
	found in misc.c.

	Scrolls, Potions, and Food:
	Flags is used to define a function which reading/quaffing
	will cause.  Most scrolls and potions have only one bit
	set.  Potions will generally have some food value, found
	in pval.

	Wands and Staffs:
	Flags defines a function, pval contains number of charges
	for item.  pval is set in flavor_init() in misc.c.

	Chests:
        pval contains level chest is found on (randomly determined for
        store-bought chests).
	Traps are added randomly by flavor_init() in desc.c.	*/

/* Object list (All objects must be defined here)		 */
inven_kind k_list[MAX_OBJECTS] = {
/* Dungeon items from 0 to MAX_DUNGEON_OBJ */
{"Poison"			,0x00000001L,	TV_FOOD, ',',	/*  0*/
  500,	   0,	64,   1,   1,	0,   0,	 0,   0, {0,0}	,  7, 0, 0x0L},
{"Restoring"			,0x001F8040L,	TV_FOOD, ',',	/* 13*/
  500,	1000,	79,   1,   1,	0,   0,	 0,   0, {0,0}	, 30, 0, 0x0L},
{"Restoring"			,0x001F8040L,	TV_FOOD, ',',	/* 14*/
  500,	1000,	79,   1,   1,	0,   0,	 0,   0, {0,0}	, 40, 0, 0x0L},
{"& Two-Handed Sword"           ,0x00000000L,	TV_SWORD, '|',	/* 38*/
    0,	 775,	29,   1, 200,	0,   0,	 0,   0, {3,6}	, 40, 0, 0x0L},
{"& Longsword"			,0x00000000L,	TV_SWORD, '|',	/* 43*/
    0,	 300,	28,   1, 130,	0,   0,	 0,   0, {2,5} , 20, 0, 0x0L},
{"& Arrow~"			,0x00000000L,	TV_ARROW, '{',	/*144*/
    0,	   1, 193,   1,   2,	0,   0,	 0,   0, {1,4}	, 15, 0, 0x0L},
{"& Bolt~"			,0x00000000L,	TV_BOLT, '{',	/*170*/
    0,	   2, 193,   1,   3,	0,   0,	 0,   0, {1,5}	, 25, 0, 0x0L},
#if defined(SATISFY_HUNGER) /* new create food code -CWS */
{"Satisfy Hunger"		,0x10000000L,	TV_SCROLL1, '?',/*210*/
#else
{"Create Food"			,0x10000000L,	TV_SCROLL1, '?',/*210*/
#endif
    0,	  10,	92,   1,   5,	0,   0,	 0,   0, {0,0}	,  5, 0, 0x0L},
{"Cure Light Wounds"		,0x10001000L,	TV_POTION1, '!',/*238*/
   50,	  15,	79,   1,   4,	0,   0,	 0,   0, {1,1}	,  1, 0, 0x0L},
{"Cure Light Wounds"		,0x10001000L,	TV_POTION1, '!',/*239*/
   50,	  15,	79,   1,   4,	0,   0,	 0,   0, {1,1}	,  2, 0, 0x0L},
{"some filthy rags"		,0x00000000L,	TV_SOFT_ARMOR, '~',/*346*/
    0,	   0,	63,   1,  20,	0,   0,	 1,   0, {0,0}	,  0, 0, 0x0L},
{"*Remove Curse*",               0x40000000L,	TV_SCROLL1, '?',/*378*/
    0,	 8000,	76,   1,  30,   0,   0,	 0,   0, {1,1}	, 50, 2, 0x0L},
{"& Dagger"	         	,0x00000000L,	TV_SWORD, '|',	/*395*/
    0,	  10,	25,   1,  12,	0,   0,	 0,   0, {1,4}	,  5, 0, 0x0L},
{"& Dagger"	         	,0x00000000L,	TV_SWORD, '|',	/*396*/
    0,	  10,	26,   1,  12,	0,   0,	 0,   0, {1,4}	,  10, 0, 0x0L},
{"& Dagger"	         	,0x00000000L,	TV_SWORD, '|',	/*397*/
    0,	  10,	27,   1,  12,	0,   0,	 0,   0, {1,4}	,  20, 0, 0x0L},
{"& Short sword"		,0x00000000L,	TV_SWORD, '|',	/*398*/
    0,	  90,	28,   1,  80,	0,   0,	 0,   0, {1,7}	,  6, 0, 0x0L},

/* ADD NEW ITEMS HERE!!!!!!!!! */

{"& Cloak"		        ,0x00000000L,  TV_CLOAK,   '(', /*411*/
    0,     3,   1,   1,  10,    0,   0,  1,   0, {0,0},  20,  0, 0x0L},
{"Acquirement"			,0x00000400L, TV_SCROLL2, '?', /*412*/
    0, 100000,104,   1,   5,    0,   0,  0,   0, {0,0},  20,  10, 0x0L},
{"*Acquirement*"		,0x00000800L, TV_SCROLL2, '?', /*413*/
    0, 200000,105,   1,   5,    0,   0,  0,   0, {0,0},  60,  15, 0x0L},
{"Enlightenment"		,0x00200000L,	TV_POTION2, '!',/*420*/
    0,	800,  113,   1,   4,	0,   0,	 0,   0, {1,1}	, 25, 0, 0x0L},

/* end of Dungeon items */
/* Store items, which are not also dungeon items, some of these can be
   found above, except that the number is >1 below */

{"& Ration~ of Food"		,0x00000000L,	TV_FOOD, ',',	/*+0*/
 5000,	   3,	90,   5,  10,	0,   0,	 0,   0, {0,0}	,  0, 0, 0x0L},
{"& Hard Biscuit~"		,0x00000000L,	TV_FOOD, ',',	/*+1*/
  500,	   1,	93,   5,   2,	0,   0,	 0,   0, {0,0}	,  0, 0, 0x0L},
{"& Strip~ of Beef Jerky"	,0x00000000L,	TV_FOOD, ',',	/*+2*/
 1750,	   2,	94,   5,   2,	0,   0,	 0,   0, {0,0}	,  0, 0, 0x0L},
{"& Pint~ of Fine Ale"		,0x00000000L,	TV_FOOD, ',',	/* +3 */
  500,	   1,	95,   3,  10,	0,   0,	 0,   0, {0,0}	,  0, 0, 0x0L},
{"& Pint~ of Fine Wine"		,0x00000000L,	TV_FOOD, ',',
  400,	   2,	96,   1,  10,	0,   0,	 0,   0, {0,0}	,  0, 0, 0x0L},
{"& Pick"			,0x20000000L,	TV_DIGGING, '\\',
    1,	  50,	1,   1, 150,	0,   0,	 0,   0, {1,3}	,  0, 0, 0x0L},
{"& Shovel"			,0x20000000L,	TV_DIGGING, '\\',
    0,	  15,	4,   1,  60,	0,   0,	 0,   0, {1,2}	,  0, 0, 0x0L},
{"Identify"			,0x00000008L,	TV_SCROLL1, '?',
    0,	  50,  67,   2,   5,	0,   0,	 0,   0, {0,0}	,  0, 0, 0x0L},
{"Light"			,0x00000020L,	TV_SCROLL1, '?',
    0,	  15,  69,   3,   5,	0,   0,	 0,   0, {0,0}	,  0, 0, 0x0L},
{"Phase Door"			,0x00000080L,	TV_SCROLL1, '?',
    0,	  15,  71,   2,   5,	0,   0,	 0,   0, {0,0}	,  0, 0, 0x0L},
{"Mapping"               	,0x00000800L,	TV_SCROLL1, '?',/* +10 */
    0,	  40,  75,   2,   5,	0,   0,	 0,   0, {0,0}	,  0, 0, 0x0L},
{"Treasure Detection"		,0x00004000L,	TV_SCROLL1, '?',
    0,	  15,  78,   2,   5,	0,   0,	 0,   0, {0,0}	,  0, 0, 0x0L},
{"Object Detection"		,0x00008000L,	TV_SCROLL1, '?',
    0,	  15,  79,   2,   5,	0,   0,	 0,   0, {0,0}	,  0, 0, 0x0L},
{"Detect Invisible"		,0x00080000L,	TV_SCROLL1, '?',
    0,	  15,  83,   2,   5,	0,   0,	 0,   0, {0,0}	,  0, 0, 0x0L},
{"Blessing"			,0x00000020L,	TV_SCROLL2, '?',
    0,	  15,  99,   2,   5,	0,   0,	 0,   0, {0,0}	,  0, 0, 0x0L},
{"Word of Recall"		,0x00000100L,	TV_SCROLL2, '?',
    0,	 150, 102,   3,   5,	0,   0,	 0,   0, {0,0}	,  0, 0, 0x0L},
{"Cure Light Wounds"		,0x10001000L,	TV_POTION1, '!',
   50,	  15,  79,   2,   4,	0,   0,	 0,   0, {1,1}	,  0, 0, 0x0L},
{"Heroism"			,0x00000010L,	TV_POTION2, '!',
    0,	  35,  98,   2,   4,	0,   0,	 0,   0, {1,1}	,  0, 0, 0x0L},
{"Boldness"			,0x00000040L,	TV_POTION2, '!',
    0,	  10, 100,   2,   4,	0,   0,	 0,   0, {1,1}	,  0, 0, 0x0L},
{"Slow Poison"			,0x00000800L,	TV_POTION2, '!',
    0,	  25, 105,   2,   4,	0,   0,	 0,   0, {1,1}	,  0, 0, 0x0L},
{"& Brass Lantern~"		,0x00000000L,	TV_LITE, '~',	/* +20 */
 7500,	  35,	0,   1,  50,	0,   0,	 0,   0, {1,1}	,  1, 0, 0x0L},
{"& Wooden Torch~"		,0x00000000L,	TV_LITE, '~',
 4000,	   2, 192,   5,  30,	0,   0,	 0,   0, {1,1}	,  1, 0, 0x0L},
{"& Flask~ of oil"		,0x00040000L,	TV_FLASK, '!',
 7500,	   3,	64,   5,  10,	0,   0,	 0,   0, {2,6}	,  1, 0, 0x0L},
/* end store items */

/* start doors */
/* Secret door must have same sval as closed door in	*/
/* TRAP_LISTB.	See CHANGE_TRAP. Must use & because of stone_to_mud. */
/* end doors */


/* Stores are just special traps		*/
/* Traps are just Nasty treasures.				*/
/* Traps: Level represents the relative difficulty of disarming;	*/
/* and P1 represents the experienced gained when disarmed*/
/* rubble */
{"some rubble"			,0x00000000L, TV_RUBBLE, ':',	/* +54 */
    0,	   0,	1,   1,   0,	0,   0,	 0,   0, {0,0}	,  0, 0, 0x0L},
/* mush */
{"& pint~ of fine grade mush"	,0x00000000L, TV_FOOD, ',',	/* +55 */
 3500,	   0,  97,   1,   1,   0,   0,   0,   0, {1,1}  ,  1, 0, 0x0L},

/* Gold list (All types of gold and gems are defined here)	*/
/* nothing, used as inventory place holder */
/* must be stackable, so that can be picked up by inven_carry */
{"nothing"			,0x00000000L,  TV_NOTHING, ' ',	/* +75 */
   0,       0,  64,   0,   0,   0,   0,   0,   0, {0,0}    ,  0, 0, 0x0L},
/* next needed only for the name */
{""				,0x00000000L,  TV_NOTHING, ' ',	/* +77*/
   0,       0,   0,   0,   0,   0,   0,   0,   0, {0,0}  ,  0, 0, 0x0L},
{"Power named Narya"		,0x01C008BFL,	TV_RING, '=',	/* +78 */
    1,	100000,	31,   1,   2,	0,   0,	 0,   0, {0,0}	, 80, 0, 0x02000848L},

/* For some reason THIS is where SPECIAL_OBJ should point */    /* +79 */

{"Power named Nenya"		,0x05C008BFL,	TV_RING, '=',	/*467*/
    2,	200000,	32,   1,   2,	0,   0,	 0,   0, {0,0}	, 90, 0, 0x02000888L},
{"Power named Vilya"		,0x01C008BFL,	TV_RING, '=',	/*468*/
    3,	300000,	33,   1,   2,	0,   0,	 0,   0, {0,0}	, 100, 0, 0x02000A08L},
{"Power"			,0x83B80A3FL,	TV_RING, '=',	/*469*/
    5,5000000,	34,   1,   2,	0,   0,	 0,   0, {0,0}	, 110, 0, 0x02000BC0L},
{"The Phial of Galadriel"        ,0x00000000L,	TV_LITE, '~',	   /*470*/
    0,	10000,	4,    1,  10,	0,   0,	 0,   0, {1,1}	,  1, 0, 0x02000C00L},
{"Ingwe"                         ,0x43380024L,  TV_AMULET, '"',    /*471*/
    3,  90000,  10,   1,   3,   0,   0,  0,   0, {0,0}  ,  65, 0,0x02000800L},
{"Carlammas"                     ,0x00080010L,  TV_AMULET, '"',    /*472*/
    2,  60000,  11,   1,   3,   0,   0,  0,   0, {0,0}  ,  50, 0,0x02000800L},
{"The Star of Elendil"		 ,0x00000000L,  TV_LITE,   '~',   /*473*/
    0,  25000,  5,    1,   5,   0,   0,  0,   0, {1,1}  ,  25, 0,0x02000C00L},
{"The Arkenstone of Thrain"	 ,0x0100000CL,  TV_LITE,   '~',   /*474*/
    2,  60000,  6,    1,   5,   0,   0,  0,   0, {1,1}  ,  60, 0,0x02000C08L},
{"Tulkas"			 ,0x00000019L,  TV_RING, '=',     /*475*/
    4,  150000, 35,   1,   2,   0,   0,  0,   0, {0,0}  ,  90, 0,0x02000800L},
{"the Dwarves"			 ,0x41800811L,  TV_AMULET,  '"',  /*476*/
    3,  75000,  12,   1,   3,   0,   0,  0,   0, {0,0},    70, 0,0x02000000L},
{"Barahir"			 ,0x1000003FL,  TV_RING,   '=',   /*477*/
    1,  65000,  36,   1,   2,   0,   0,  0,   0, {0,0},    50, 0,0x02000000L}
};

cptr special_names[SN_ARRAY_SIZE] = { //===> ego_names, v_list
	NULL,			"of Resistance",	"of Resist Acid",
	"of Resist Fire",	"of Resist Cold",	"of Resist Lightning",
	"(Holy Avenger)",	"(Defender)",		"of Animal Slaying",
	"of Dragon Slaying",	"of Slay Evil",		"of Slay Undead",
	"of Flame",	        "of Frost",	        "of Free Action",
/* 15 */
	"of Slaying",		"of Clumsiness",	"of Weakness",
	"of Slow Descent",	"of Speed",		"of Stealth",
	"of Slowness",		"of Noise",		"of Great Mass",
	"of Intelligence",	"of Wisdom",		"of Infra-Vision",
	"of Might",		"of Lordliness",	"of the Magi",
/* 30 */
	"of Beauty",		"of Seeing",		"of Regeneration",
	"of Stupidity",		"of Dullness",		"of Blindness",
	"of Timidness",		"of Teleportation",	"of Ugliness",
	"of Protection",	"of Irritation",	"of Vulnerability",
	"of Enveloping",	"of Fire",		"of Slay Evil",
/* 45 */
	"of Dragon Slaying",	"(Empty)",		"(Locked)",
	"(Poison Needle)",	"(Gas Trap)",		"(Explosion Device)",
	"(Summoning Runes)",	"(Multiple Traps)",	"(Disarmed)",
	"(Unlocked)",		"of Slay Animal",       "'Grond'",
	"'Ringil'",             "'Aeglos'",             "'Arunruth'",
/* 60 */
        "'Mormegil'"    ,       "of Morgul",            "'Angrist'",
	"'Gurthang'",           "'Calris'",             "of Accuracy",
	"'Anduril'",            "of Orc Slaying",       "of Power",
	"of Durin",             "of Aule",              "of Westernesse",
        "(Blessed)",      "of Demon Slaying",     "of Troll Slaying",
/* 75 */
	"'Bloodspike'",         "'Thunderfist'",        "of Wounding",
	"'Orcrist'",            "'Glamdring'",          "'Sting'",
	"of Light",             "of Agility",           "of Backbiting",
	"'Doomcaller'",         "of Giant Slaying", 	"of Telepathy",
	"of Dragonkind",        "'Nenya'",		"'Narya'",
/* 90 */
	"'Vilya'",		"of Aman",		"'Belegennon'",
	"of Feanor",		"of Anarion",		"of Isildur",
	"of Fingolfin",		"of Elvenkind",		"'Soulkeeper'",
	"of Dor-Lomin",         "of Morgoth",           "'Belthronding'",
	"'Dal-i-thalion'",      "'Paurhach'",           "'Paurnimmen'",
/* 105 */
	"'Pauraegen'",          "'Cammithrim'",         "'Cambeleg'",
        "'Holhenneth'",     "'Paurnen'",        "'Aeglin'",
	"'Camlost'",        "'Nimloth'",        "'Nar-i-vagil'",
	"of Beruthiel",         "of Gorlim",            "'Narthanc'",
	"'Nimthanc'",       "'Dethanc'",        "'Gilettar'",
/* 120 */
	"of Rilia",             "'Belangil'",       "of Balli Stonehand",
	"'Lotharang'",      "'Firestar'",       "'Eriril'",
	"'Cubragol'",       "of Bard",              "'Colluin'",
	"'Holcolleth'",     "'Totila'",         "of Pain",
	"'Elvagil'",        "'Aglarang'",	"of the Rohirrim",
/* 135 */
	"of Eorlingas",     "'Barukkheled'",    "of Wrath",
	"'Haradekket'",     "'Mundwine'",       "'Gondricam'",
        "'Zarcuthra'",      "'Careth Asdriag'",	"'Forasgil'",
	"'Crisdurian'",     "'Colannon'",       "'Hithlomir'",
	"'Thalkettoth'",    "of Arvedui",     	"of Thranduil",
/* 150 */
   	"of Thengel",           "of Hammerhand",        "of Celegorm",
  	"of Thror",             "of Maedhros",          "of Olorin",
 	"'Anguirel'",       "of Thorin",		"of Celeborn",
	"of Orome",		"of Eonwe",		"of Gondor",
        "of Theoden",		"of Thingol",		"of Thorongil",
/* 165 */
        "of Luthien",           "of Tuor",              "of Ulmo",
	"'Osondir'",	        "'Turmil'",		"of Caspanion",
	"'Til-i-arc'",	        "'Deathwreaker'",	"'Avavir'",
	"'Taratol'",            "'Razorback'",          "'Bladeturner'",
	"(Shattered)",		"(Blasted)",            "of Extra Attacks"
/* 180 */
};

s16b sorted_objects[MAX_DUNGEON_OBJ];


s16b t_level[MAX_K_LEV+1];
inven_type i_list[MAX_I_IDX];

