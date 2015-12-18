/* File: variable.c */

/* Purpose: Global variables */

#include "constant.h"
#include "angband.h"


/* Link a copyright message into the executable */
cptr copyright[5] = {
    "Copyright (c) 1989 James E. Wilson, Robert A. Keoneke",
    "",
    "This software may be copied and distributed for educational, research,",
    "and not for profit purposes provided that this copyright and statement",
    "are included in all such copies."
};

/* a horrible hack: needed because compact_monster() can be called from
   creatures() via summon_monster() and place_monster() */
int hack_monptr = (-1);

int weapon_heavy = FALSE;
int pack_heavy = FALSE;
s16b log_index = (-1);		/* Index to log file. (<= 0 means no log) */
vtype died_from;

vtype savefile;			/* The savefile to use. */

int light_rad = 0;		/* Current light radius (zero for none) */
int old_rad;		/* Previous light radius (assume lite) */

s16b total_winner = FALSE;	/* Semi-Hack -- Game has been won */

int NO_SAVE=FALSE;

int character_generated = 0;	/* A character has been generated */
int character_saved = 0;	/* The character has been saved */

int highscore_fd;		/* File descriptor to high score file */

int LOAD = 0;			/* Hack -- used for CHECK_LOAD */


u32b randes_seed;		/* Hack -- consistent object colors */
u32b town_seed;		/* Hack -- consistent town layout */

char *old_state = NULL;         /* state array initialized by time -CWS */
char *dummy_state = NULL;       /* dummy state array -CWS */

s16b missile_ctr = 0;		/* Counter for missiles */
int msg_flag;			/* Set with first msg  */
vtype old_msg[MAX_SAVE_MSG];	/* Last message	      */
s16b last_msg = 0;		/* Where last is held */

int death = FALSE;		/* True if player has died */
int free_turn_flag;		/* Command is "free", do not move creatures */
int find_flag;			/* Number of turns spent running */

s16b cur_height,cur_width;	/* Cur dungeon size    */
s16b dun_level = 0;		/* Cur dungeon level   */
int object_level = 0;		/* level for objects to be created -CWS  */

int command_count;		/* Gives repetition of commands. -CJS- */
int default_dir = FALSE;	/* Use last direction for repeated command */

s32b turn = (-1);			/* Cur turn of game    */
s32b old_turn = (-1);		/* Last feeling message */

int wizard = FALSE;		/* Wizard flag	      */
int to_be_wizard = FALSE;	/* used during startup, when -w option used */

s16b panic_save = FALSE;	/* this is true if playing from a panic save */
s16b noscore = FALSE;		/* Don't log the game. -CJS- */

int is_home = FALSE;		/* Are we in our home? */
int in_store_flag = FALSE;	/* Don't redisplay light in stores -DGK */

int monster_is_afraid = 0;      /* redo monster fear messages -CWS */

int coin_type;			/* Hack -- force coin type */
int opening_chest = 0;          /* Hack -- prevent chest generation */


struct unique_mon u_list[MAX_CREATURES]; /* Unique check list... -LVB- */

int rogue_like_commands;	/* set in config.h/main.c */


/*  OPTION: options set via the '=' command */
/* note that the values set here will be the default settings */

int find_cut = TRUE;
int find_examine = TRUE;
int find_bound = FALSE;
int find_prself = FALSE;
int prompt_carry_flag = FALSE;
int show_weight_flag = FALSE;
int show_equip_weight_flag = FALSE;
int highlight_seams = FALSE;
int find_ignore_doors = FALSE;
int hitpoint_warn = 1;
int feeling = 0;
int carry_query_flag = TRUE;
int delay_spd = 5;
int plain_descriptions = FALSE;
int no_haggle_flag = FALSE;	/* does the player not want to haggle? -CWS */
int quick_messages = FALSE;     /* quick messages -CWS */
int equippy_chars = FALSE;      /* do equipment characters -CWS */
char doing_inven = FALSE;	/* Track inventory commands. -CJS- */
int screen_change = FALSE;	/* Track screen updates for inven_commands. */
char last_command = ' ';  	/* Memory of previous command. */

/* these used to be in dungeon.c */
int new_level_flag;		/* Next level when true	 */
int teleport_flag;		/* Handle teleport traps  */
int player_light;		/* Player carrying light */
int eof_flag = FALSE;		/* Used to signal EOF/HANGUP condition */
int light_flag = FALSE;		/* Track if temporary light about player.  */

int wait_for_more = FALSE;	/* used when ^C hit during -more- prompt */
int closing_flag = FALSE;	/* Used for closing   */

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

#ifdef MAC
cave_type (*cave)[MAX_WIDTH];
#else
cave_type cave[MAX_HEIGHT][MAX_WIDTH];
#endif

#ifdef MAC
monster_lore *c_recall;
#else
monster_lore c_recall[MAX_CREATURES];	/* Monster memories */
#endif

/* Player record for most player related info */
player_type py;	/* player info record */

/* 
 * Calculated base hp values for player at each level,
 * store them so that drain life + restore life does not
 * affect hit points.
 */
u16b player_hp[MAX_PLAYER_LEVEL];

