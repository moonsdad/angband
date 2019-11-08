/* File: defines.h */

/* Purpose: global constants used by Angband */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */


/*
 * Note to the Wizard:
 *
 *       Tweaking these constants can *GREATLY* change the game.
 *       Two years of constant tuning have generated these      
 *       values.  Minor adjustments are encouraged, but you must
 *       be very careful not to unbalance the game.  Moria was  
 *       meant to be challenging, not a give away.  Many        
 *       adjustments can cause the game to act strangely, or even
 *       cause errors.
 */

/*
 * Addendum:
 * 
 * I have greatly expanded the number of defined constants.  However, if
 * you change anything below, without understanding EXACTLY how the game
 * uses the number, the program may stop working correctly.  Modify the
 * constants at your own risk.  Also note that not all constants in the
 * code are written using the proper symbolic constant.  Several hard
 * coded values are lurking around.
 */


/*
 * Current version number of Angband: 2.6.?
 *
 * Note that 5.2 must never be used, as it was used in Angband 2.4-2.5.
 * Actually, by that time, nobody will want to use 2.5 savefiles!
 */

#define CUR_VERSION_MAJ 2
#define CUR_VERSION_MIN 6
#define CUR_PATCH_LEVEL 1

/* Basics */

#ifndef FUZZY
#define FUZZY 2
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/* 
 * Some constants
 */



/*
 * These values are used to keep things "small"
 */
#define MAX_UCHAR       255
#define MAX_SHORT       32767
#define MAX_LONG        0xFFFFFFFFL


/*
 * Used for "Check Load"
 */
#ifndef MAXHOSTNAMELEN
# define MAXHOSTNAMELEN  64
#endif


/*
 * message line location
 * This value cannot be changed (yet)
 */
#define MSG_LINE		0

/*
 * number of messages to save in a buffer
 */
#define MAX_SAVE_MSG   22               /* How many messages to save -CJS- */

/*
 * This is the maximum number of high scores to save
 */
#define MAX_SAVE_HISCORES	100


/* Dungeon size parameters */
#define MAX_HEIGHT		66	/* Multiple of 11; >= 22 */
#define MAX_WIDTH		198	/* Multiple of 33; >= 66 */
#define SCREEN_HEIGHT		22	/* Hard Coded */
#define SCREEN_WIDTH		66	/* Hard Coded */


/*
 * Dungeon generation values
 *
 * Note: The entire design of dungeon can be changed by only
 * slight adjustments here.
 */

#define DUN_TUN_RND       9     /* 1/Chance of Random direction          */
#define DUN_TUN_CHG      70     /* Chance of changing direction (99 max) */
#define DUN_TUN_CON      15     /* Chance of extra tunneling             */
#define DUN_ROO_MEA      45     /* Mean of # of rooms, standard dev2     */
#define DUN_TUN_PEN      25     /* Chance of room doors                  */
#define DUN_TUN_JCT       8     /* Chance of doors at tunnel junctions   */
#define DUN_STR_DEN       5     /* Density of streamers                  */
#define DUN_STR_RNG       2     /* Width of streamers                    */
#define DUN_STR_MAG       3     /* Number of magma streamers             */
#define DUN_STR_MC       90     /* 1/chance of treasure per magma        */
#define DUN_STR_QUA       2     /* Number of quartz streamers            */
#define DUN_STR_QC       40     /* 1/chance of treasure per quartz       */
#define DUN_UNUSUAL      200    /* Level/chance of unusual room          */
#define DUN_DEST         15     /* 1/chance of having a destroyed level */


/* Store constants */
#define MAX_OWNERS       24     /* Number of owners to choose from       */
#define MAX_STORES        8     /* Number of different stores            */
#define STORE_INVEN_MAX  24     /* Max number of discrete objs in inven  */
#define STORE_CHOICES    30     /* NUMBER of items to choose stock from  */
#define STORE_TURNOVER    9     /* Normal shop turnover, per day */
#define STORE_MIN_KEEP    6     /* Min slots to "always" keep full */
#define STORE_MAX_KEEP   18     /* Max slots to "always" keep full */
#define COST_ADJ         100    /* Adjust prices for buying and selling  */
#define STORE_SHUFFLE	100	/* 1/Chance (per day) of an owner changing */
#define STORE_TURNS    1000	/* Number of turns between turnovers	*/


/*
 * Hack -- this is the chance (1/GREAT_OBJ) that when an item is
 * created by "get_obj_num()", that the "level" will be "enhanced"
 * by quite a large amount.  This will allow, for example, a ring
 * of speed to be found at 50 feet.
 */
#define GREAT_OBJ        11     /* 1/Chance of item being a Great Item  */

/* Number of dungeon objects */
#define MAX_DUNGEON_OBJ  423

/*
 * Note that the following constants are all related, if you change one,
 * you must also change all succeeding ones.  Also, player_init[] and
 * store_choice[] may also have to be changed.  And most importantly,
 * these constants are fixed by their index in the "k_list" file.
 */

#define OBJ_NOTHING             0

#define OBJ_FOOD_RATION		21

#define OBJ_BROAD_SWORD		34
#define OBJ_DAGGER		43
#define OBJ_SMALL_SWORD		46
#define OBJ_MACE		53
#define OBJ_LEAD_FILLED_MACE	56

#define OBJ_LONG_BOW		74
#define OBJ_ARROW		78
#define OBJ_BOLT		80
#define OBJ_SHOT		83

#define OBJ_IRON_CROWN		98

#define OBJ_SOFT_LEATHER	103
#define OBJ_CHAIN_MAIL		109

#define OBJ_ANTI_EVIL		209
#define OBJ_SCROLL_RECALL	220

#define OBJ_POTION_HEALING	242
#define OBJ_POTION_BERSERK	258

#define OBJ_SPELL_BOOK		330
#define OBJ_PRAYER_BOOK		334

#define OBJ_RUINED_CHEST        344

#define OBJ_TORCH		346

#define OBJ_PLAYER		440

#define OBJ_FLOOR		441
#define OBJ_GRANITE_WALL	442
#define OBJ_QUARTZ_VEIN		443
#define OBJ_MAGMA_VEIN		444

#define OBJ_RUBBLE              445
#define OBJ_OPEN_DOOR           446
#define OBJ_CLOSED_DOOR         447
#define OBJ_SECRET_DOOR         448
#define OBJ_UP_STAIR            449
#define OBJ_DOWN_STAIR          450

#define OBJ_GLYPH               459
#define OBJ_OPEN_PIT		460

#define OBJ_STORE_LIST          451
#define OBJ_TRAP_LIST           460
#define OBJ_GOLD_LIST           480


/* The "Morgoth" artifacts */

#define OBJ_GROND		498
#define OBJ_MORGOTH		499


/* Start of the "Special Objects" */

#define OBJ_SPECIAL         	500

/* The actual special objects */
#define OBJ_NARYA		(OBJ_SPECIAL + 8)
#define OBJ_NENYA		(OBJ_SPECIAL + 9)
#define OBJ_VILYA		(OBJ_SPECIAL + 10)
#define OBJ_POWER		(OBJ_SPECIAL + 11)


/* Total Number of object kinds */
#define MAX_K_IDX		512


/* was 7*64, see flavor_p() in desc.c, could be MAX_K_IDX o_o() rewritten
 * now 8*64 beacuse of Rods
 */

#define OBJECT_IDENT_SIZE 1024

#define MAX_GOLD       18       /* Number of different types of gold     */


#define TREAS_ROOM_ALLOC  9     /* Amount of objects for rooms         */
#define TREAS_ANY_ALLOC   3     /* Amount of objects for corridors     */
#define TREAS_GOLD_ALLOC  3     /* Amount of gold (and gems)           */


/*
 * Magic Treasure Generation constants                  
 * Note: Number of special objects, and degree of enchantments
 *       can be adjusted here.
 */

#define OBJ_STD_ADJ     125     /* Adjust STD per level * 100        */
#define OBJ_STD_MIN       7     /* Minimum STD                       */
#define OBJ_TOWN_LEVEL    5     /* Town object generation level      */
#define OBJ_BASE_MAGIC   15     /* Base magic_chance                 */
#define OBJ_BASE_MAX     70     /* Maximum magic_chance              */
#define OBJ_DIV_SPECIAL   4     /* magic_chance/# special magic   */
#define OBJ_DIV_CURSED   13     /* 10*magic_chance/# cursed items    */

/* Constants describing limits of certain objects */
#define FUEL_LAMP	15000   /* Maximum amount that lamp can be filled  */
#define OBJ_BOLT_RANGE     18   /* Maximum range of bolts and balls        */
#define OBJ_BREAK_GLYPH   550   /* Rune of protection resistance           */

/* Monster race array limit */
#define MAX_R_IDX       549   /* Number of creatures defined for univ  */

/* Monster attack type limit */
#define MAX_A_IDX       285   /* Number of monster attack types.       */

#define WIN_MON_TOT         2   /* Total number of "win" creatures          */
#define WIN_MON_APPEAR    100   /* Level where winning creatures begin      */

#define QUEST_MAX         4     /* only 1 defined anyway --CFT */
#define DEFINED_QUESTS    1 
#define SAURON_QUEST      0 
#define Q_PLANE          -1



/*
 * with MAX_M_IDX 101, it is possible to get compacting monsters messages
 * while breeding/cloning monsters
 *
 * with MAX_I_IDX 150, it is possible to get compacting objects during
 * level generation, although it is extremely rare
 */

#define MIN_I_IDX           1   /* Minimum i_list index used                */
#define MAX_I_IDX         400   /* Max objects per level                    */
#define MAX_K_LEV         125   /* Maximum level of items in dungeon        */

#define MIN_M_IDX           2   /* Minimum index in m_list (1=player, 0=no mon) */
#define MAX_M_IDX         600   /* Max monsters that can be allocated per level */
#define MAX_R_LEV          99   /* Maximum level of creatures               */

#define MAX_SIGHT          20   /* Maximum dis a creature can be seen       */
#define MAX_SPELL_DIS      18   /* Maximum dis creat. spell can be cast     */
#define MON_MULT_ADJ        8   /* High value slows multiplication          */
#define MON_NASTY          50   /* 1/chance of high level creat             */
#define MAX_M_ALLOC_CHANCE 160  /* 1/chance of new monster each round       */
#define MIN_M_ALLOC_LEVEL  14   /* Minimum number of monsters/level         */
#define MIN_M_ALLOC_TD      4   /* Number of people on town level (day)     */
#define MIN_M_ALLOC_TN      8   /* Number of people on town level (night)   */
#define MON_SUMMON_ADJ      2   /* Adjust level of summoned creatures       */
#define MON_DRAIN_LIFE      2   /* Percent of player exp drained per hit    */

/* Mega Hack -- Trap constants */
#define MAX_TRAP           18   /* Number of defined traps      */

/* Descriptive constants                                        */
#define MAX_COLORS     57       /* Used with potions      */
#define MAX_SHROOM     21       /* Used with mushrooms    */
#define MAX_WOODS      32       /* Used with staffs       */
#define MAX_METALS     32       /* Used with wands & rods */
#define MAX_ROCKS      42       /* Used with rings        */
#define MAX_AMULETS    16       /* Used with amulets      */
#define MAX_TITLES     45       /* Used with scrolls      */
#define MAX_SYLLABLES 158       /* Used with scrolls      */

/* Player constants                                             */
#define MAX_EXP      99999999L  /* Maximum amount of experience -CJS-      */
#define MAX_PLAYER_LEVEL    50  /* Maximum possible character level        */
#define MAX_RACES           10  /* Number of defined races                 */
#define MAX_CLASS            6  /* Number of defined classes               */
#define USE_DEVICE           3  /* x> Harder devices x< Easier devices     */
#define MAX_BACKGROUND     128  /* Number of types of histories for univ   */
#define PLAYER_FOOD_FULL 10000  /* Getting full                            */
#define PLAYER_FOOD_MAX  15000  /* Maximum food value, beyond is wasted    */
#define PLAYER_FOOD_FAINT  300  /* Character begins fainting               */
#define PLAYER_FOOD_WEAK  1000  /* Warn player that he is getting very low */
#define PLAYER_FOOD_ALERT 2000  /* Warn player that he is getting low      */
#define PLAYER_REGEN_FAINT  33  /* Regen factor*2^16 when fainting         */
#define PLAYER_REGEN_WEAK   98  /* Regen factor*2^16 when weak             */
#define PLAYER_REGEN_NORMAL 197 /* Regen factor*2^16 when full             */
#define PLAYER_REGEN_HPBASE 1442 /* Min amount hp regen*2^16               */
#define PLAYER_REGEN_MNBASE 524 /* Min amount mana regen*2^16              */
#define PLAYER_WEIGHT_CAP   130 /* "#"*(1/10 pounds) per strength point	   */
#define PLAYER_EXIT_PAUSE    1  /* Pause time before player can re-roll    */

/* class level adjustment constants */
#define CLA_BTH         0
#define CLA_BTHB        1
#define CLA_DEVICE      2
#define CLA_DISARM      3
#define CLA_SAVE        4

/*
 * this depends on the fact that CLA_SAVE values are all the same, if not,
 * then should add a separate column for this
 */
#define CLA_MISC_HIT    4
#define MAX_LEV_ADJ     5

/* Base to hit constants                                        */
#define BTH_PLUS_ADJ    3       /* Adjust BTH per plus-to-hit   */

/*
 * Maximum number of "normal pack slots"
 * This value is also used as the index of the "overflow" slot.
 *
 */
#define INVEN_PACK	22

/*
 * Inventory slot values (do not change these)
 */
#define INVEN_WIELD	23		/* must be first item in equipment list */
#define INVEN_AUX       24
#define INVEN_LEFT      25
#define INVEN_RIGHT     26
#define INVEN_NECK      27
#define INVEN_LITE      28
#define INVEN_BODY      29
#define INVEN_OUTER     30
#define INVEN_ARM       31
#define INVEN_HEAD      32
#define INVEN_HANDS     33
#define INVEN_FEET      34

/*
 * Total number of inventory slots (23 pack, 12 equip)
 */
#define INVEN_TOTAL	35



/* Hardcoded Attribute indexes -CJS- */
#define A_STR	0
#define A_INT	1
#define A_WIS	2
#define A_DEX	3
#define A_CON	4
#define A_CHR	5


/*
 * Fval definitions: various types of dungeon floors and walls
 * Note that if numbers above "15" are ever used, then the test against
 * MIN_CAVE_WALL will have to be changed, also the save routines.
 * The "savefiles" in particular, will be totally trashed...
 */

#define NULL_WALL	0	/* Temp value for "generate.c" */

#define DARK_FLOOR      1
#define LIGHT_FLOOR     2
#define NT_DARK_FLOOR   3
#define NT_LIGHT_FLOOR  4
#define MAX_CAVE_ROOM   4
#define CORR_FLOOR	5	/* Floor, in a corridor */
#define BLOCKED_FLOOR   6       /* a corridor space with cl/st/se door or rubble */
#define MAX_CAVE_FLOOR  6

#define MAX_OPEN_SPACE  5
#define MIN_CLOSED_SPACE 6

#define TMP1_WALL	8
#define TMP2_WALL	9

#define MIN_CAVE_WALL   12
#define GRANITE_WALL	12	/* Granite */
#define QUARTZ_WALL	13	/* Quartz */
#define MAGMA_WALL	14	/* Magma */
#define BOUNDARY_WALL	15	/* Permanent */

/* Column for stats    */
#define COL_STAT     0

/* Class spell types */
#define NONE            0
#define MAGE            1
#define PRIEST          2

/* offsets to spell names in spell_names[] array */
#define SPELL_OFFSET    0
#define PRAYER_OFFSET   63

/* definitions for the psuedo-normal distribution generation */
#define NORMAL_TABLE_SIZE       256
#define NORMAL_TABLE_SD          64  /* the standard deviation for the table */


/* Masks for the player's status field */
#define PY_STATS        0x3F000000L

/* definitions for the player's status field */
#define PY_HUNGRY       0x00000001L
#define PY_WEAK         0x00000002L
#define PY_BLIND        0x00000004L
#define PY_CONFUSED     0x00000008L
#define PY_FEAR         0x00000010L
#define PY_POISONED     0x00000020L
#define PY_FAST         0x00000040L
#define PY_SLOW         0x00000080L
#define PY_SEARCH       0x00000100L
#define PY_REST         0x00000200L
#define PY_STUDY        0x00000400L

#define PY_INVULN       0x00001000L
#define PY_HERO         0x00002000L
#define PY_SHERO        0x00004000L
#define PY_BLESSED      0x00008000L
#define PY_DET_INV      0x00010000L
#define PY_TIM_INFRA    0x00020000L
#define PY_SPEED        0x00040000L
#define PY_STR_WGT      0x00080000L
#define PY_PARALYSED    0x00100000L
#define PY_REPEAT       0x00200000L
#define PY_ARMOR        0x00400000L

#define PY_STR          0x01000000L /* these 6 stat flags must be adjacent */
#define PY_INT          0x02000000L
#define PY_WIS          0x04000000L
#define PY_DEX          0x08000000L
#define PY_CON          0x10000000L
#define PY_CHR          0x20000000L

#define PY_HP           0x40000000L
#define PY_MANA         0x80000000L





/* inventory stacking svals
 * these never stack:
 */

#define ITEM_NEVER_STACK_MIN    0
#define ITEM_NEVER_STACK_MAX    63

/* these items always stack with others of same sval, always treated as
 * single objects, must be power of 2
 */

#define ITEM_SINGLE_STACK_MIN   64
#define ITEM_SINGLE_STACK_MAX   192     /* see NOTE below */


/* these items stack with others only if have same sval and same pval,
 * they are treated as a group for wielding, etc.
 */

#define ITEM_GROUP_MIN          192
#define ITEM_GROUP_MAX          255

/* NOTE: items with sval 192 are treated as single objects, but only stack
 * with others of same sval if have the same pval value, only used for
 * torches
 */


/* id's used for item description, stored in i_ptr->ident */

#define ID_MAGIK        0x1
#define ID_DAMD         0x2
#define ID_EMPTY        0x4	/* Item is now "empty" */
#define ID_KNOWN       0x8	/* Item is fully "known" */
#define ID_STOREBOUGHT  0x10
#define TR3_SHOW_MODS  0x20
#define ID_NOSHOW_P1    0x40    /* don't show (+x) even if pval != 0 -CWS   */
#define TR3_HIDE_TYPE  0x80    /* don't show (+x of yyy), just (+x) -CWS */

/*
 * Ego-Item indexes
 *
 * All the "Bad" Ego-Items are at the end.
 * The holes were left by artifacts and old ego-items.
 */

#define EGO_R			1
#define EGO_RESIST_A		2
#define EGO_RESIST_F		3
#define EGO_RESIST_C		4
#define EGO_RESIST_E		5
#define EGO_HA			6
#define EGO_DF			7
#define EGO_SLAY_A		8
#define EGO_SLAY_D		9
#define EGO_SLAY_E		10
#define EGO_SLAY_U		11
#define EGO_FT			12
#define EGO_FB			13
#define EGO_FREE_ACTION		14
#define EGO_SLAYING		15

#define EGO_SLOW_DESCENT	18
#define EGO_SPEED		19
#define EGO_STEALTH		20

#define EGO_INTELLIGENCE	24
#define EGO_WISDOM		25
#define EGO_INFRAVISION		26
#define EGO_MIGHT		27
#define EGO_LORDLINESS		28
#define EGO_MAGI		29
#define EGO_BEAUTY		30
#define EGO_SEEING		31
#define EGO_REGENERATION	32

#define EGO_PROTECTION		39

#define EGO_FIRE		43
#define EGO_SLAY_EVIL		44
#define EGO_DRAGON_SLAYING	45

#define EGO_SLAY_ANIMAL		55

#define EGO_ACCURACY		65

#define EGO_SLAY_O		67
#define EGO_POWER		68

#define EGO_WEST		71
#define EGO_BLESS_BLADE		72
#define EGO_SLAY_DEMON		73
#define EGO_SLAY_T		74

#define EGO_WOUNDING		77

#define EGO_LITE		81
#define EGO_AGILITY		82

#define EGO_SLAY_G		85
#define EGO_TELEPATHY		86

#define EGO_ELVENKIND		87	/* Was 97 */

#define EGO_ATTACKS		90	/* Was 79 */

#define EGO_AMAN		91

#define EGO_MIN_WORTHLESS	96	/* First "worthless" Ego-Item */

#define EGO_WEAKNESS		104
#define EGO_STUPIDITY		105
#define EGO_DULLNESS		106
#define EGO_CLUMSINESS		108
#define EGO_UGLINESS		109
#define EGO_TELEPORTATION	110

#define EGO_IRRITATION		112
#define EGO_VULNERABILITY	113
#define EGO_ENVELOPING		114

#define EGO_SLOWNESS		116
#define EGO_NOISE		117
#define EGO_GREAT_MASS		118

#define EGO_BACKBITING		120

#define EGO_MORGUL		124

#define EGO_SHATTERED		126
#define EGO_BLASTED		127

#define EGO_MAX			128



/*
 * Artifact indexes
 */

	/* Rings */
#define ART_NARYA		10
#define ART_NENYA		11
#define ART_VILYA		12
#define ART_POWER		13

	/* Dragon Scale */
#define ART_RAZORBACK		16
#define ART_BLADETURNER		17

	/* Hard Armour */
#define ART_SOULKEEPER		19
#define ART_ISILDUR		20
#define ART_ROHIRRIM		21
#define ART_BELEGENNON		22
#define ART_CELEBORN		23
#define ART_ARVEDUI		24
#define ART_CASPANION		25

	/* Soft Armour */
#define ART_HITHLOMIR		27
#define ART_THALKETTOTH		28

	/* Shields */
#define ART_THORIN		30
#define ART_CELEGORM		31
#define ART_ANARION		32

	/* Helms */
#define ART_MORGOTH		34
#define ART_BERUTHIEL		35
#define ART_THRANDUIL		36
#define ART_THENGEL		37
#define ART_HAMMERHAND		38
#define ART_DOR			39
#define ART_HOLHENNETH		40
#define ART_GORLIM		41
#define ART_GONDOR		42

	/* Cloaks */
#define ART_COLLUIN		44
#define ART_HOLCOLLETH		45
#define ART_THINGOL		46
#define ART_THORONGIL		47
#define ART_COLANNON		48
#define ART_LUTHIEN		49
#define ART_TUOR		50

	/* Gloves */
#define ART_CAMBELEG		52
#define ART_CAMMITHRIM		53
#define ART_PAURHACH		54
#define ART_PAURNIMMEN		55
#define ART_PAURAEGEN		56
#define ART_PAURNEN		57
#define ART_CAMLOST		58
#define ART_FINGOLFIN		59

	/* Boots */
#define ART_FEANOR		60
#define ART_DAL			61
#define ART_THROR		62


	/* Swords */
#define ART_MAEDHROS		64
#define ART_ANGRIST		65
#define ART_NARTHANC		66
#define ART_NIMTHANC		67
#define ART_DETHANC		68
#define ART_RILIA		69
#define ART_BELANGIL		70
#define ART_CALRIS		71
#define ART_ARUNRUTH		72
#define ART_GLAMDRING		73
#define ART_AEGLIN		74
#define ART_ORCRIST		75
#define ART_GURTHANG		76
#define ART_ZARCUTHRA		77
#define ART_MORMEGIL		78
#define ART_GONDRICAM		79
#define ART_CRISDURIAN		80
#define ART_AGLARANG		81
#define ART_RINGIL		82
#define ART_ANDURIL		83
#define ART_ANGUIREL		84
#define ART_ELVAGIL		85
#define ART_FORASGIL		86
#define ART_CARETH		87
#define ART_STING		88
#define ART_HARADEKKET		89
#define ART_GILETTAR		90
#define ART_DOOMCALLER		91

	/* Polearms */
#define ART_THEODEN		93
#define ART_PAIN		94
#define ART_OSONDIR		95
#define ART_TIL			96
#define ART_AEGLOS		97
#define ART_OROME		98
#define ART_NIMLOTH		99
#define ART_EORLINGAS		100
#define ART_DURIN		101
#define ART_EONWE		102
#define ART_BALLI		103
#define ART_LOTHARANG		104
#define ART_MUNDWINE		105
#define ART_BARUKKHELED		106
#define ART_WRATH		107
#define ART_ULMO		108
#define ART_AVAVIR		109

	/* Hafted */
#define ART_GROND		111
#define ART_TOTILA		112
#define ART_THUNDERFIST		113
#define ART_BLOODSPIKE		114
#define ART_FIRESTAR		115
#define ART_TARATOL		116
#define ART_AULE		117
#define ART_NAR			118
#define ART_ERIRIL		119
#define ART_OLORIN		120
#define ART_DEATHWREAKER	121
#define ART_TURMIL		122

	/* Bows */
#define ART_BARD		125
#define ART_CUBRAGOL		126

	/* Number of entries */
#define SN_ARRAY_SIZE          180 /* must be at end of this list */


/*
 * The values for type values (tval) field of various objects.
 * This value is the primary means by which items are sorted in the
 * player inventory.  It also groups things for MIN_WEAR/MAX_WEAR.
 *
 * Note that all items with tval from 10 to 50 are "wearable_p()",
 * which means that the special "TR#_*" flags apply to them.  Note
 * that shots/arrows/bolts have no slot to be wielded into, and that
 * bows are wielded into the special "bow slot".  This leaves the real
 * "weapon" slot for shovels, swords, polearms, and hafted weapons.
 *
 * TV_MIN_ENCHANT items tested for enchantments,
 * i.e. the MAGIK inscription, see the enchanted() procedure
 *
 * objects between TV_MIN_VISIBLE and TV_MAX_VISIBLE are always visible,
 * i.e. the cave fm flag is set when they are present
 */

#define TV_NEVER        -1	/* used by find_range() for non-search */
#define TV_NOTHING       0	/* Nothing (used in OBJ_NOTHING) */
#define TV_MISC          1
#define TV_SPIKE         5	/* Spikes ('~') */
#define TV_CHEST         7	/* Chests ('~') */
#define TV_MIN_WEAR     10	/* Min tval for "wearable" items */
#define TV_MIN_ENCHANT  10
#define TV_LITE         10	/* Torches, Lanterns, Specials */
#define TV_SHOT		16	/* Ammo for slings */
#define TV_ARROW        17	/* Ammo for bows */
#define TV_BOLT         18	/* Ammo for x-bows */
#define TV_BOW          19	/* Slings/Bows/Xbows */
#define TV_DIGGING      20	/* Shovels/Picks */
#define TV_HAFTED       21	/* Priest Weapons */
#define TV_POLEARM      22	/* Axes and Pikes */
#define TV_SWORD        23	/* Edged Weapons */
#define TV_BOOTS        30	/* Boots */
#define TV_GLOVES       31	/* Gloves */
#define TV_HELM         33	/* Helms/Crowns */
#define TV_SHIELD       34	/* Shields */
#define TV_CLOAK        35	/* Cloaks */
#define TV_SOFT_ARMOR   36	/* Soft Armor */
#define TV_HARD_ARMOR   37	/* Hard Armor */
#define TV_MAX_ENCHANT  39 /* max tval that uses the TR_* flags */
#define TV_AMULET       40
#define TV_RING         45
#define TV_MAX_WEAR     50   /* max tval for wearable items */
#define TV_STAFF        55
#define TV_WAND         65
#define TV_ROD          66
#define TV_SCROLL1      70
#define TV_SCROLL2      71
#define TV_POTION1      75
#define TV_POTION2      76
#define TV_FLASK        77
#define TV_FOOD         80
#define TV_MAGIC_BOOK   90
#define TV_PRAYER_BOOK  91
#define TV_MAX_OBJECT   99	/* This is the max TV monsters pick up */
#define TV_GOLD         100	/* Gold can only be picked up by players */
#define TV_MAX_PICK_UP  100     /* This is the max TV players pick up */
#define TV_INVIS_TRAP   101	/* Invisible traps -- see visible traps */
#define TV_MIN_VISIBLE  102	/* This is the first "visible landmark" */
#define TV_VIS_TRAP     102     /* Visible traps */
/* Doors and Stairs are never deleted when trying to create another
 * one during level generation */
#define TV_OPEN_DOOR    104	/* Open doorway */
#define TV_UP_STAIR     107	/* Staircase up */
#define TV_DOWN_STAIR   108	/* Staircase down */
#define TV_STORE_DOOR   110	/* Entrance to store */
#define TV_SECRET_DOOR  117	/* Secret door -- treated as a "wall" */
#define TV_CLOSED_DOOR  118	/* Closed door -- treated as a "wall" */
#define TV_RUBBLE       119	/* Rubble pile -- treated as a "wall" */

#define TV_MIN_DOORS    104
#define TV_MAX_VISIBLE  110


/* The "sval" codes for TV_STAFF */
#define SV_STAFF_LITE		0
#define SV_STAFF_DOOR_STAIR_LOC	1
#define SV_STAFF_TRAP_LOC	2
#define SV_STAFF_TREASURE_LOC	3
#define SV_STAFF_OBJECT_LOC	4
#define SV_STAFF_TELEPORTATION	5
#define SV_STAFF_EARTHQUAKES	6
#define SV_STAFF_SUMMONING	7
#define SV_STAFF_DESTRUCTION	8
#define SV_STAFF_STARLITE	9
#define SV_STAFF_HASTE_MONSTERS	10
#define SV_STAFF_SLOW_MONSTERS	11
#define SV_STAFF_SLEEP_MONSTERS	12
#define SV_STAFF_CURE_LIGHT	13
#define SV_STAFF_DETECT_INVIS	14
#define SV_STAFF_SPEED		15
#define SV_STAFF_SLOWNESS	16
#define SV_STAFF_REMOVE_CURSE	18
#define SV_STAFF_DETECT_EVIL	19
#define SV_STAFF_CURING		20
#define SV_STAFF_DISPEL_EVIL	21
#define SV_STAFF_DARKNESS	22
#define SV_STAFF_GENOCIDE	23
#define SV_STAFF_POWER		24
#define SV_STAFF_THE_MAGI	25
#define SV_STAFF_IDENTIFY	26
#define SV_STAFF_HOLINESS	27
#define SV_STAFF_MAPPING	28
#define SV_STAFF_HEALING	29
#define SV_STAFF_PROBING	30

/* The "sval" codes for TV_WAND */
#define SV_WAND_LITE		0
#define SV_WAND_ELEC		1
#define SV_WAND_COLD		2
#define SV_WAND_FIRE		3
#define SV_WAND_STONE_TO_MUD	4
#define SV_WAND_POLYMORPH	5
#define SV_WAND_HEAL_MONSTER	6
#define SV_WAND_HASTE_MONSTER	7
#define SV_WAND_SLOW_MONSTER	8
#define SV_WAND_CONFUSE_MONSTER	9
#define SV_WAND_SLEEP_MONSTER	10
#define SV_WAND_DRAIN_LIFE	11
#define SV_WAND_TRAP_DOOR_DEST	12
#define SV_WAND_MAGIC_MISSILE	13
#define SV_WAND_FEAR_MONSTER	14
#define SV_WAND_CLONE_MONSTER	15
#define SV_WAND_TELEPORT_AWAY	16
#define SV_WAND_DISARMING	17
#define SV_WAND_ELEC_BALL	18
#define SV_WAND_COLD_BALL	19
#define SV_WAND_FIRE_BALL	20
#define SV_WAND_STINKING_CLOUD	21
#define SV_WAND_ACID_BALL	22
#define SV_WAND_WONDER		23
#define SV_WAND_DRAGON_FIRE	24
#define SV_WAND_DRAGON_COLD	25
#define SV_WAND_DRAGON_BREATH	26
#define SV_WAND_ANNIHILATION	27
#define SV_WAND_ACID		28

/* The "sval" codes for TV_ROD */
#define SV_ROD_LIGHT		0
#define SV_ROD_ELEC		1
#define SV_ROD_COLD		2
#define SV_ROD_FIRE		3
#define RD_ST_MUD       5L
#define SV_ROD_POLYMORPH	4
#define SV_ROD_SLOW_MONSTER	5
#define RD_CONF_MN      8L
#define SV_ROD_SLEEP_MONSTER	6
#define SV_ROD_DRAIN_LIFE	7
#define RD_TR_DEST      11L
#define RD_MAG_MIS      12L
#define SV_ROD_TELEPORT_AWAY	8
#define SV_ROD_DISARMING	9
#define SV_ROD_ELEC_BALL	10
#define SV_ROD_COLD_BALL	11
#define SV_ROD_FIRE_BALL	12
#define RD_ST_CLD       18L
#define SV_ROD_ACID_BALL	13
#define SV_ROD_ACID		14
#define RD_ANHIL        21L
#define SV_ROD_MAPPING		15
#define SV_ROD_IDENTIFY		16
#define SV_ROD_CURING		17
#define SV_ROD_HEALING		18
#define SV_ROD_DETECTION	19
#define SV_ROD_RESTORATION	20
#define SV_ROD_SPEED		21
#define SV_ROD_ILLUMINATION	22
#define SV_ROD_PROBING		23
#define SV_ROD_RECALL		24
#define SV_ROD_TRAP_LOC		25
#define SV_ROD_MK_WALL      33L

#define SN_BELEG               101
#define SN_BLINDNESS            35
#define SN_DRAGONKIND           87



/*
 * The "TR_xxx" values apply ONLY to the items with tval's between
 * TV_MIN_WEAR and TV_MAX_WEAR, that is, items which can be wielded
 * or worn.  Use the macro "wearable_p()" to check this condition.
 * The macros "artifact_p()" and "cursed_p()" call "wearable_p()".
 *
 * Do NOT under any circumstances use these flags on any object which
 * could conceivably be a book, scroll, potion, chest, etc.
 *
 * Note that chests no longer use the "TR_xxx" flags, see "CH2_xxx" below.
 *
 * Note that "flags1" contains all flags dependant on "pval" (including
 * stat bonuses, but NOT stat sustainers), plus all "extra attack damage"
 * flags (SLAY_XXX and BRAND_XXX).
 *
 * Note that "flags2" contains all "resistances" (including "Stat Sustainers",
 * actual immunities, and resistances).  Note that "Hold Life" is really an
 * "immunity" to ExpLoss, and "Free Action" is "immunity to paralysis".
 *
 * Note that "flags3" contains everything else -- including the "CURSED"
 * flags, and the "BLESSED" flag, and flags which affect the player in a "general"
 * way (LITE, TELEPATHY, SEE_INVIS, SLOW_DIGEST, REGEN), including the "general"
 * curses (TELEPORT, AGGRAVATE, EXP_DRAIN).  It also contains four new flags
 * called "ITEM_IGNORE_XXX" which lets an item specify that it can not be
 * affected by various forms of destruction.  This is NOT as powerful as
 * actually granting resistance to the wearer.  Also "FEATHER" floating.
 */

#define TR_STATS        0x0000003FL /* the stats must be the low 6 bits */
#define TR1_STR			0x00000001L	/* Uses "pval" */
#define TR1_INT			0x00000002L	/* Uses "pval" */
#define TR1_WIS			0x00000004L	/* Uses "pval" */
#define TR1_DEX			0x00000008L	/* Uses "pval" */
#define TR1_CON			0x00000010L	/* Uses "pval" */
#define TR1_CHR			0x00000020L	/* Uses "pval" */
#define TR1_STEALTH		0x00000100L	/* Uses "pval" */
#define TR1_SEARCH		0x00000200L	/* Uses "pval" */
#define TR1_INFRA		0x00000400L	/* Uses "pval" */
#define TR1_TUNNEL		0x00000800L	/* Uses "pval" */
#define TR1_SPEED		0x00001000L	/* Uses "pval" */
#define TR1_ATTACK_SPD		0x00002000L	/* Uses "pval" extra attacks/round -DGK */
#define TR1_SLAY_ANIMAL		0x00010000L
#define TR1_SLAY_EVIL		0x00020000L
#define TR1_SLAY_UNDEAD		0x00040000L
#define TR1_SLAY_DEMON		0x00080000L
#define TR1_SLAY_ORC		0x00100000L
#define TR1_SLAY_TROLL		0x00200000L
#define TR1_SLAY_GIANT		0x00400000L
#define TR1_SLAY_DRAGON		0x00800000L
#define TR1_KILL_DRAGON		0x01000000L	/* Execute Dragon */
#define TR1_IMPACT		0x04000000L	/* Start Earthquakes */
#define TR1_BRAND_FIRE		0x40000000L
#define TR1_BRAND_COLD		0x80000000L
#define TR_ARTIFACT		0x02000000L	/* means "is an artifact" -CFT */

#define TR2_SUST_STR		0x00000001L
#define TR2_SUST_INT		0x00000002L
#define TR2_SUST_WIS		0x00000004L
#define TR2_SUST_DEX		0x00000008L
#define TR2_SUST_CON		0x00000010L
#define TR2_SUST_CHR		0x00000020L
#define TR2_IM_ACID		0x00000100L
#define TR2_IM_ELEC		0x00000200L
#define TR2_IM_FIRE		0x00000400L
#define TR2_IM_COLD		0x00000800L
#define TR2_IM_POIS		0x00001000L
#define TR2_FREE_ACT		0x00004000L	/* Free Action */
#define TR2_HOLD_LIFE	 	0x00008000L	/* Hold Life */

#define TR2_RES_ACID		0x00010000L
#define TR2_RES_ELEC		0x00020000L
#define TR2_RES_FIRE		0x00040000L
#define TR2_RES_COLD		0x00080000L
#define TR2_RES_POIS		0x00100000L
#define TR2_RES_LITE		0x00400000L	/* Oops */
#define TR2_RES_DARK		0x00800000L	/* Oops */

#define TR2_RES_BLIND		0x01000000L	/* Oops */
#define TR2_RES_CONF		0x02000000L	/* Oops */
#define TR2_RES_SOUND		0x04000000L	/* Oops */
#define TR2_RES_SHARDS		0x08000000L	/* Oops */

#define TR2_RES_NETHER		0x10000000L	/* Oops */
#define TR2_RES_NEXUS		0x20000000L	/* Oops */
#define TR2_RES_CHAOS		0x40000000L	/* Oops */
#define TR2_RES_DISEN		0x80000000L	/* Oops */

#define TR2_RES_FEAR     0x10000000L
#define TR_EGO_WEAPON   0x0007E000L
#define TR_LIGHTNING    0x00001000L

#define TR3_FEATHER	 	0x00001000L	/* Feather Falling */
#define TR3_LITE		0x00002000L	/* Permanent Light */
#define TR3_SEE_INVIS		0x00004000L	/* See Invisible */
#define TR3_TELEPATHY		0x00008000L	/* Telepathy */
#define TR3_SLOW_DIGEST		0x00010000L	/* Item slows down digestion */
#define TR3_REGEN		0x00020000L	/* Item induces regeneration */
#define TR3_IGNORE_ACID		0x00100000L	/* Item ignores Acid Damage */
#define TR3_IGNORE_ELEC		0x00200000L	/* Item ignores Elec Damage */
#define TR3_IGNORE_FIRE		0x00400000L	/* Item ignores Fire Damage */
#define TR3_IGNORE_COLD		0x00800000L	/* Item ignores Cold Damage */
#define TR3_ACTIVATE		0x01000000L	/* Item can be activated */
#define TR3_TELEPORT		0x04000000L	/* Item teleports player */
#define TR3_AGGRAVATE		0x08000000L	/* Item aggravates monsters */
#define TR3_BLESSED		0x10000000L	/* priests use w/o penalty -DGK*/
#define TR3_CURSED		0x20000000L	/* Item is Cursed */


/* definitions for objects that can be worn */

/* Mask for chest traps (flags2) */
#define CH2_TRAP_MASK      0x000001F0L

/* Definitions for chests (flags2) */
#define CH2_LOCKED		0x00000001L	/* Chest is locked */

/* Chest Trap Flags (flags2) */
#define CH2_LOSE_STR		0x00000010L
#define CH2_POISON		0x00000020L
#define CH2_PARALYSED		0x00000040L
#define CH2_EXPLODE		0x00000080L
#define CH2_SUMMON		0x00000100L

/* Chests -- Hack from pre-2.7.0 */
#define CH1_CARRY_OBJ   	0x01000000L
#define CH1_CARRY_GOLD		0x02000000L
#define CH1_HAS_60		0x04000000L
#define CH1_HAS_90		0x08000000L
#define CH1_HAS_1D2		0x10000000L
#define CH1_HAS_2D2		0x20000000L
#define CH1_HAS_4D2		0x40000000L



/*
 * Spell types used by get_flags(), breathe(), fire_bolt() and fire_ball()
 */
#define GF_NONE		0
#define GF_ELEC         1
#define GF_POIS         2
#define GF_ACID         3
#define GF_COLD         4
#define GF_FIRE         5
#define GF_MISSILE      10
#define GF_ARROW        11
#define GF_PLASMA       12
#define GF_HOLY_ORB     13
#define GF_WATER        14
#define GF_LITE         15	/* Lite, plus Lite damage */
#define GF_DARK         16	/* Dark, plus Dark damage */
#define GF_SHARDS       20
#define GF_SOUND        21
#define GF_CONFUSION    22
#define GF_FORCE        23
#define GF_INERTIA      24
#define GF_MANA         26
#define GF_METEOR       27
#define GF_ICE          28
#define GF_CHAOS        30
#define GF_NETHER       31
#define GF_DISENCHANT   32
#define GF_NEXUS        33
#define GF_TIME         34
#define GF_GRAVITY      35



/* bit flags used in my revamped enchant() code -CFT */
#define ENCH_TOHIT   0x01
#define ENCH_TODAM   0x02
#define ENCH_TOAC    0x04



/*
 * Adaptation of the old monster.h file
 *
 * Definitions of various monster flags
 *
 */


/* definitions for creatures, cflags1 field */
#define MF1_MV_ONLY_ATT		0x00000001L /* The monster does not move */
#define MF1_MV_ATT_NORM 	0x00000002L /* The monster moves normally */
#define MF1_MV_20	        0x00000004L /* 20% random */
#define MF1_MV_40		0x00000008L /* 40% random */
#define MF1_MV_75		0x00000010L /* 75% random */
/* 3 bits missing */
#define MF1_MV_INVIS		0x00010000L
#define MF1_THRO_DR		0x00020000L
#define MF1_THRO_WALL		0x00040000L
#define MF1_THRO_CREAT		0x00080000L
#define MF1_PICK_UP		0x00100000L
#define MF1_MULTIPLY		0x00200000L
/* 2 bits missing */
#define MF1_CARRY_OBJ   	0x01000000L
#define MF1_CARRY_GOLD		0x02000000L
#define MF1_HAS_60		0x04000000L
#define MF1_HAS_90		0x08000000L
#define MF1_HAS_1D2		0x10000000L
#define MF1_HAS_2D2		0x20000000L
#define MF1_HAS_4D2		0x40000000L
#define MF1_WINNER		0x80000000L

#define MF2_ANIMAL		0x00000001L
#define MF2_EVIL		0x00000002L
#define MF2_ORC			0x00000004L
#define MF2_UNDEAD		0x00000008L
#define MF2_DRAGON		0x00000010L
#define MF2_DEMON		0x00000020L
#define MF2_HURT_LITE		0x00000040L
#define	MF2_HURT_ROCK		0x00000080L
#define MF2_CHARM_SLEEP		0x00000100L
#define MF2_NO_INFRA		0x00000200L
#define MF2_MAX_HP		0x00000400L
#define MF2_IM_COLD		0x00000800L
#define MF2_IM_FIRE		0x00001000L
#define MF2_IM_ELEC		0x00002000L
#define MF2_IM_POIS		0x00004000L
#define MF2_IM_ACID		0x00008000L
#define MF2_TROLL		0x00010000L
#define MF2_GIANT		0x00020000L
#define MF2_SPECIAL       	0x00040000L
#define MF2_GROUP         	0x00080000L
#define MF2_GOOD           	0x00100000L
#define MF2_BREAK_WALL		0x00200000L
#define MF2_DESTRUCT		0x00400000L
#define MF2_QUESTOR		0x00800000L /* killing it completes a quest */
#define MF2_INTELLIGENT		0x01000000L /* casts spells intelligently   */
#define MF2_MINDLESS		0x02000000L /* does not have a mind -CWS    */
/* 5 bits missing */
#define MF2_UNIQUE		0x80000000L /* unique monster */

/* r_ptr->spells1 */
#define MS1_BLINK		0x00000010L
#define MS1_TELEPORT		0x00000020L
#define MS1_TELE_TO	 	0x00000040L
#define MS1_CAUSE_1		0x00000080L
#define MS1_CAUSE_2		0x00000100L
#define MS1_HOLD		0x00000200L
#define MS1_BLIND		0x00000400L
#define MS1_CONF		0x00000800L
#define MS1_FEAR		0x00001000L
#define MS1_S_MONSTER		0x00002000L
#define MS1_S_UNDEAD		0x00004000L
#define MS1_SLOW		0x00008000L
#define MS1_MANA_DRAIN		0x00010000L
#define MS1_S_DEMON		0x00020000L
#define MS1_S_DRAGON		0x00040000L
#define MS1_BR_ELEC		0x00080000L
#define MS1_BR_POIS		0x00100000L
#define MS1_BR_ACID		0x00200000L
#define MS1_BR_COLD		0x00400000L
#define MS1_BR_FIRE		0x00800000L
#define MS1_BO_FIRE   		0x01000000L
#define MS1_BO_COLD		0x02000000L
#define MS1_BO_ACID     	0x04000000L
#define MS1_ARROW_1      	0x08000000L
#define MS1_CAUSE_3    		0x10000000L
#define MS1_BA_FIRE     	0x20000000L
#define MS1_BA_COLD    		0x40000000L
#define MS1_BO_MANA     	0x80000000L

/* r_ptr->spells2 */
#define MS2_BR_CHAO   		0x00000001L /* Chaos */
#define MS2_BR_SHAR    		0x00000002L /* Shards */
#define MS2_BR_SOUN    		0x00000004L /* Sound */
#define MS2_BR_CONF   		0x00000008L /* Confusion */
#define MS2_BR_DISE 		0x00000010L /* Disenchantment */
#define MS2_BR_LIFE  		0x00000020L /* Life Draining */
#define MS2_BO_ELEC 		0x00000040L
#define MS2_BA_ELEC   		0x00000080L
#define MS2_BA_ACID    		0x00000100L
#define MS2_TRAP_CREATE  	0x00000200L
#define MS2_RAZOR        	0x00000400L
#define MS2_MIND_BLAST   	0x00000800L
#define MS2_TELE_AWAY    	0x00001000L
#define MS2_HEAL         	0x00002000L
#define MS2_HASTE        	0x00004000L
#define MS2_ARROW_2      	0x00008000L
#define MS2_BO_PLAS  		0x00010000L
#define MS2_S_SUMMON       	0x00020000L
#define MS2_BO_NETH  		0x00040000L
#define MS2_BO_ICEE     	0x00080000L
#define MS2_DARKNESS     	0x00100000L
#define MS2_FORGET       	0x00200000L
#define MS2_BRAIN_SMASH  	0x00400000L
#define MS2_BA_POIS     	0x00800000L
#define MS2_TELE_LEVEL     	0x01000000L
#define MS2_BO_WATE   		0x02000000L
#define MS2_BA_WATE   		0x04000000L
#define MS2_BA_NETH  		0x08000000L
#define MS2_S_ANGEL      	0x10000000L
#define MS2_S_SPIDER     	0x20000000L
#define MS2_S_HOUND      	0x40000000L
#define MS2_BR_NETH    		0x80000000L /* Nexus */

/* m_ptr->spells3 */
#define MS3_BR_WALL   		0x00000001L /* Wall Building */
#define MS3_BR_SLOW   	 	0x00000002L /* Slowness */
#define MS3_BR_LITE    		0x00000004L /* Light */
#define MS3_BR_TIME    		0x00000008L /* Time */
#define MS3_BR_GRAV    		0x00000010L /* Gravity */
#define MS3_BR_DARK    		0x00000020L /* Darkness */
#define MS3_BR_PLAS    		0x00000040L /* Plasma */
#define MS3_ARROW_3        	0x00000080L /* fires an arrow */
#define MS3_S_WRAITH     	0x00000100L /* Summon ringwraiths */
#define MS3_DARK_STORM   	0x00000200L /* Big darkness breath */
#define MS3_MANA_STORM   	0x00000400L /* Mana storm */
#define MS3_S_REPTILE    	0x00000800L /* Summon reptiles */
#define MS3_S_ANT	     	0x00001000L /* Summon ants/ant lions */
#define MS3_S_UNIQUE     	0x00002000L /* Summon uniques */
#define MS3_S_GUNDEAD    	0x00004000L /* Summon greater undead */
#define MS3_S_ANCIENTD   	0x00008000L /* Summon ancient dragon */

/*
 * Eight bits of zeros
 */
#define NONE8			0x00000000L


/*
 * The "recall" of monster memory is a MESS
 */

/* Hack -- scan for "movement" */
#define CM1_ALL_MV_FLAGS 0x0000001FL
#define CM1_RANDOM_MOVE  0x0000001CL

/* Hack -- scan for "special movement" */
#define CM1_SPECIAL      0x003F0000L

/* Hack -- used to "count" treasures */
#define CM1_TREASURE     0x7C000000L
#define CM1_TR_SHIFT     26              /* used for recall of treasure */

/* Hack -- used to "count" spell attacks */
#define CS1_FREQ        0x0000000FL

/* Hack -- separate out the "breath" spells */
#define CS1_BREATHE     0x00F80000L
#define CS2_BREATHE     0x8000003FL
#define CS3_BREATHE     0x0000007FL

/* Hack -- take note of "intelligent" spells */
#define CS1_INT         0x0006FC30L     /* was 0x80060020L -DGK */
#define CS2_INT         0x71027200L     /* was 0x51023400L -DGK */
#define CS3_INT         0x0000F900L     /* was 0x00000000L -DGK */


