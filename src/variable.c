/* File: variable.c */

/* Purpose: Global variables */

#include "angband.h"


/* Link a copyright message into the executable */
cptr copyright[5] = {
    "Copyright (c) 1989 James E. Wilson, Robert A. Keoneke",
    "",
    "This software may be copied and distributed for educational, research,",
    "and not for profit purposes provided that this copyright and statement",
    "are included in all such copies."
};


/* Link the "version" into the executable */
int cur_version_maj = CUR_VERSION_MAJ;
int cur_version_min = CUR_VERSION_MIN;
int cur_patch_level = CUR_PATCH_LEVEL;

/* a horrible hack: needed because compact_monster() can be called from
   process_monsters() via summon_monster() and place_monster() */
int hack_m_idx = (-1);		/* XXX Current monster in "process_monsters()" */


int cur_lite = 0;		/* Current light radius (zero for none) */
int old_lite = 1;		/* Previous light radius (assume lite) */

s16b total_winner = FALSE;	/* Semi-Hack -- Game has been won */

int character_generated = 0;	/* A character has been generated */
int character_saved = 0;	/* The character has been saved */


int LOAD = 0;			/* Hack -- used for CHECK_LOAD */


u32b randes_seed;		/* Hack -- consistent object colors */
u32b town_seed;		/* Hack -- consistent town layout */

int command_cmd = 0;		/* Current "Angband Command" */
int command_old = 0;		/* Memory of previous command. */
int command_rep = 0;		/* Gives repetition of commands. -CJS- */
int command_dir = -1;		/* Use last direction for repeated command */

int create_up_stair = FALSE;
int create_down_stair = FALSE;

s16b log_index = (-1);		/* Index to log file. (<= 0 means no log) */

int death = FALSE;		/* True if player has died */
int free_turn_flag;		/* Command is "free", do not move creatures */
int find_flag;			/* Number of turns spent running */

int msg_flag;			/* Set with first msg Used in msg_print() for "buffering" */
vtype old_msg[MAX_SAVE_MSG];	/* Last message */
s16b last_msg = 0;		/* Where last is held */

s16b cur_height;		/* Cur dungeon height */
s16b cur_width;			/* Cur dungeon width */
s16b dun_level = 0;		/* Cur dungeon level */
int object_level = 0;		/* level for objects to be created -CWS  */

s32b turn = 0;			/* Cur turn of game    */
s32b old_turn = 0;		/* Last feeling message */

int wizard = FALSE;		/* Is the player currently in Wizard mode? */
int to_be_wizard = FALSE;	/* used during startup, when -w option used */

s16b panic_save = FALSE;	/* this is true if playing from a panic save */
s16b noscore = FALSE;		/* Don't log the game. -CJS- */

int in_store_flag = FALSE;	/* Don't redisplay light in stores -DGK */

int coin_type = 0;		/* Hack -- force coin type */
int opening_chest = 0;          /* Hack -- prevent chest generation */


/* Inventory info */
s16b inven_ctr = 0;		/* Total different obj's	*/
s16b inven_weight = 0;		/* Cur carried weight	*/
s16b equip_ctr = 0;		/* Cur equipment ctr	*/
s16b missile_ctr = 0;		/* Counter for missiles */
int weapon_heavy = FALSE;
int pack_heavy = FALSE;


s16b i_max;			/* Treasure heap size */
s16b m_max;			/* Monster heap size */


/* OPTION: options set via the '=' command */
/* note that the values set here will be the default settings */
/* the default keyset can thus be chosen via "rogue_like_commands" */

/* Option set 1 */

int rogue_like_commands = FALSE;	/* Pick initial keyset */
int quick_messages = FALSE;		/* Quick messages -CWS */
int prompt_carry_flag = FALSE;		/* Require "g" key to pick up */
int carry_query_flag = FALSE;		/* Prompt for pickup */
int always_repeat = FALSE;		/* Always repeat commands */

int new_screen_layout = TRUE;	/* Use the new screen layout */
int equippy_chars = TRUE;	/* do equipment characters -CWS */
int depth_in_feet = TRUE;	/* Display the depth in "feet" */
int notice_seams = TRUE;	/* Highlight mineral seams */


/* Option Set 2 -- Disturbance */

int find_cut = TRUE;		/* Cut corners */
int find_examine = TRUE;	/* Examine corners */
int find_prself = TRUE;		/* Print self */
int find_bound = TRUE;		/* Stop on borders */
int find_ignore_doors = TRUE;	/* Run through doors */

int wait_for_more = FALSE;	/* used when ^C hit during -more- prompt */
int eof_flag = FALSE;		/* Used to signal EOF/HANGUP condition */


/* Option set 3 -- Gameplay */

int player_light;		/* Player carrying light */
int light_flag = FALSE;		/* Track if temporary light about player.  */

int no_haggle_flag = FALSE;	/* Cancel haggling */

int show_inven_weight = TRUE;	/* Show weights in inven */
int show_equip_weight = TRUE;	/* Show weights in equip */
int plain_descriptions = TRUE;	/* Plain descriptions */





int hitpoint_warn = 1;		/* Hitpoint warning (0 to 9) */
int delay_spd = 1;		/* Delay factor (0 to 9) */

term *term_screen = NULL;	/* The main screen */
term *term_recall = NULL;	/* The recall window */
term *term_choice = NULL;	/* The choice window */


int peek = FALSE;		/* Let user "see" internal stuff */

int feeling = 0;		/* Most recent feeling */
int rating = 0;			/* Level's current rating */
int good_item_flag = FALSE;	/* True if "Artifact" on this level */

int monster_is_afraid = 0;      /* redo monster fear messages -CWS */
char doing_inven = 0;		/* Hack -- track inventory commands */
int screen_change = FALSE;	/* Hack -- disturb inventory commands */

int new_level_flag;		/* Start a new level */
int teleport_flag;		/* Hack -- handle teleport traps */

int closing_flag = FALSE;	/* Dungeon is closing */

/*  Following are calculated from max dungeon sizes		*/
s16b max_panel_rows, max_panel_cols;
int panel_row, panel_col;
int panel_row_min, panel_row_max;
int panel_col_min, panel_col_max;
int panel_col_prt, panel_row_prt;

/* Player location in dungeon */
s16b char_row;
s16b char_col;

#ifdef TARGET
/* Targetting information, this code stolen from Morgul -CFT */
int target_mode = FALSE;
u16b target_col;
u16b target_row;
u16b target_mon;
#endif




/* Current player's user id */
int player_uid = 0;


/* What killed the player */
vtype died_from;

/* The savefile to use. */
vtype savefile;
int NO_SAVE=FALSE;

/* Was: cave_type cave[MAX_HEIGHT][MAX_WIDTH]; */
cave_type *cave[MAX_HEIGHT];


/* The player's inventory */
inven_type inventory[INVEN_TOTAL];



/* The array of dungeon monsters [MAX_M_IDX] */
monster_type *m_list;


/* The array of monster "memory" [MAX_R_IDX] */
monster_lore *l_list;

/* Unique check list... -LVB- */
struct unique_mon u_list[MAX_R_IDX];



static player_type p_body;	/* Static player info record */
player_type *p_ptr = &p_body;	/* Pointer to the player info */

u32b spell_learned = 0;       /* bit mask of spells learned */
u32b spell_learned2 = 0;      /* bit mask of spells learned */
u32b spell_worked = 0;        /* bit mask of spells tried and worked */
u32b spell_worked2 = 0;       /* bit mask of spells tried and worked */
u32b spell_forgotten = 0;     /* bit mask of spells learned but forgotten */
u32b spell_forgotten2 = 0;    /* bit mask of spells learned but forgotten */
byte spell_order[64];          /* order spells learned/remembered/forgotten */

/*
 * Calculated base hp values for player at each level,
 * store them so that drain life + restore life does not
 * affect hit points.  Also prevents shameless use of backup
 * savefiles for hitpoint acquirement.
 */
u16b player_hp[MAX_PLAYER_LEVEL];

