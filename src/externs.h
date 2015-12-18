/* File: externs.h */

/* Purpose: macros, and extern's for functions and global variables */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

/*
 * This file is not realy in very good shape.  What we SHOULD have
 * is a correct list of available variables and functions.  What we
 * actually have seems to work, but does not reflect the current state
 * of the various files.
 *
 * We should NOT have both ANSI and non-ANSI versions in the same file.
 * What we SHOULD have is a quick utility that converts ANSI to non-ANSI,
 * and we should provide a version of the files on which this has been run.
 */


/*****************************************************************************/

#define get_Yn get_check

#include <stdio.h>

/* many systems don't define these anywhere */
#if defined(USG) || defined(DGUX) || defined(atarist)
#if !(defined(HPUX) || defined(linux))
extern int sprintf();
#endif
#else
#if !(defined(MSDOS) || defined(NeXT) || defined(ultrix) || defined(linux) \
|| defined(__386BSD__) || defined(SOLARIS))
extern char *sprintf();
#endif
#endif

#ifndef MSDOS
extern int errno;
#endif

/*****************************************************************************/


/*
 * Variable access to the version
 */
extern char *copyright[5];

extern int NO_SAVE;

/*
 * horrible hack:
 * Needed because compact_monster() can be called from within
 * creatures() via place_monster() and summon_monster() 
 */

extern int hack_monptr;			/* The "current" monster, if any */

extern int player_uid;			/* The player's uid, or zero */

extern vtype savefile;		/* The save file name */

extern vtype died_from;		/* Cause of death */

extern s16b log_index;		/* Index to log file. -CJS- */

extern char days[7][29];		/* The load check info */


/*
 * These are options, set with set_options command -CJS-
 */

/* Option set 1 */

extern int rogue_like_commands;	/* Pick initial keyset */
extern int quick_messages;		/* Quick messages -CWS */
extern int prompt_carry_flag;		/* Require "g" key to pick up */
extern int carry_query_flag;		/* Prompt for pickup */

extern int equippy_chars;	/* do equipment characters -CWS */
extern int highlight_seams;	/* Highlight magma and quartz */

/* Option Set 2  */

extern int find_cut;		/* Cut corners */
extern int find_examine;	/* Examine corners */
extern int find_prself;		/* Print self on a run (slower) */
extern int find_bound;		/* Stop on borders when the map shifts */
extern int find_ignore_doors;	/* Run through doors */


/* Option set 3 -- Gameplay */

extern int no_haggle_flag;	/* Cancel haggling */

extern int show_weight_flag;	/* Show weights in inven */
extern int show_equip_weight_flag;	/* Show weights in equip  */
extern int plain_descriptions;	/* Plain descriptions don't add color */


/*
 * More options
 */
extern int delay_spd;		/* 1-10 for delays */
extern int hitpoint_warn;	/* Low hitpoint warning */


/*
 * More flags
 */
extern int in_store_flag;		/* Currently in a store */
extern int peek;			/* should we display additional msgs */
extern int coin_type;			/* Hack -- creeping _xxx_ coin type */
extern int opening_chest;		/* Hack -- do not generate another chest */

extern int is_home;			/* are we in our home? */

extern int wait_for_more;

/* Unique artifact weapon flags */
extern s32b GROND, RINGIL, AEGLOS, ARUNRUTH, MORMEGIL, ANGRIST, GURTHANG,
  CALRIS, ANDURIL, STING, ORCRIST, GLAMDRING, DURIN, AULE, THUNDERFIST,
  BLOODSPIKE, DOOMCALLER, NARTHANC, NIMTHANC, DETHANC, GILETTAR, RILIA,
  BELANGIL, BALLI, LOTHARANG, FIRESTAR, ERIRIL, CUBRAGOL, BARD, COLLUIN,
  HOLCOLLETH, TOTILA, PAIN, ELVAGIL, AGLARANG, EORLINGAS, BARUKKHELED,
  WRATH, HARADEKKET, MUNDWINE, GONDRICAM, ZARCUTHRA, CARETH, FORASGIL,
  CRISDURIAN, COLANNON, HITHLOMIR, THALKETTOTH, ARVEDUI, THRANDUIL, THENGEL,
  HAMMERHAND, CELEGORM, THROR, MAEDHROS, OLORIN, ANGUIREL, OROME,
  EONWE, THEODEN, ULMO, OSONDIR, TURMIL, TIL, DEATHWREAKER, AVAVIR, TARATOL;

/* Unique artifact armour flags */
extern s32b DOR_LOMIN, NENYA, NARYA, VILYA, BELEGENNON, FEANOR, ISILDUR,
SOULKEEPER, FINGOLFIN, ANARION, POWER, PHIAL, BELEG, DAL, PAURHACH,
PAURNIMMEN, PAURAEGEN, PAURNEN, CAMMITHRIM, CAMBELEG, INGWE, CARLAMMAS,
HOLHENNETH, AEGLIN, CAMLOST, NIMLOTH, NAR, BERUTHIEL, GORLIM, ELENDIL,
THORIN, CELEBORN, THRAIN, GONDOR, THINGOL, THORONGIL, LUTHIEN, TUOR, ROHAN,
TULKAS, NECKLACE, BARAHIR, CASPANION, RAZORBACK, BLADETURNER;

/* Brand new extra effecient and kind way to add unique monsters... HOORAY!! */
extern struct unique_mon u_list[MAX_CREATURES];

extern int quests[MAX_QUESTS];

/*
 * global flags
 */

extern int LOAD;
extern int closing_flag;		/* Used for closing   */

extern int in_store_flag;		/* Notice when in stores */
extern int good_item_flag;		/* Notice artifact created... */
extern int feeling;			/* level feeling */
extern int unfelt;
extern int new_level_flag;		/* Next level when true  */
extern int teleport_flag;		/* Handle teleport traps  */
extern int eof_flag;			/* Used to handle eof/HANGUP */
extern int player_light;		/* Player carrying light */
extern int light_rad, old_rad;           /* Light radius */
extern int find_flag;			/* Are we running */
extern int free_turn_flag;		/* Is this turn free */

extern int weapon_heavy;		/* Flag if the weapon too heavy -CJS- */
extern int pack_heavy;			/* Flag if the pack too heavy -CJS- */

extern char doing_inven;		/* Track inventory commands */
extern int screen_change;		/* Notice disturbing of inventory */

extern int be_nasty;
extern int monster_is_afraid;	/* redo monster fear messages -CWS */

extern int character_generated;	/* Character generation complete */
extern int character_saved;		/* prevents save on kill after save_char() */

extern int highscore_fd;		/* High score file descriptor */
extern int command_count;		/* Repetition of commands. -CJS- */
extern int default_dir;			/* Use last direction in repeated commands */

extern s16b noscore;			/* Don't score this game. -CJS- */

extern u32b randes_seed;		/* Hack -- consistent object colors */
extern u32b town_seed;		/* Hack -- consistent town layout */

extern char *old_state;			/* state array initialized by time -CWS */
extern char *dummy_state;		/* dummy state array so that town/colors look  the same -CWS */

extern s16b dun_level;			/* Cur dungeon level   */
extern s16b object_level;		/* used to generate objects -CWS */
extern s16b missile_ctr;		/* Counter for missiles */
extern int msg_flag;			/* Set with first msg  */
extern vtype old_msg[MAX_SAVE_MSG];	/* Last messages -CJS- */
extern s16b last_msg;			/* Where in the array is the last */
extern int death;			/* True if died	      */
extern s32b turn;			/* Current game turn */
extern s32b old_turn;			/* Last turn feeling was felt */
extern int wizard;			/* Is the player currently a Wizard? */
extern int to_be_wizard;		/* Does the player want to be a wizard? */
extern s16b panic_save;		/* true if playing from a panic save */



/*
 * Dungeon info
 */
extern s16b cur_height, cur_width;
extern s16b max_panel_rows, max_panel_cols;
extern int panel_row, panel_col;
extern int panel_row_min, panel_row_max;
extern int panel_col_min, panel_col_max;
extern int panel_col_prt, panel_row_prt;

#ifdef TARGET
/* Targetting code, stolen from Morgul -CFT */
extern int target_mode;
extern u16b target_col;
extern u16b target_row;
extern u16b target_mon;
#endif

/*  Following are all floor definitions				*/
#ifdef MAC
extern cave_type (*cave)[MAX_WIDTH];
#else
extern cave_type cave[MAX_HEIGHT][MAX_WIDTH];
#endif

/* A pointer to the main player record */
extern player_type py;

#ifdef MACGAME
extern char *(*player_title)[MAX_PLAYER_LEVEL];
extern player_race *race;
extern player_background *background;
#else
extern char *player_title[MAX_CLASS][MAX_PLAYER_LEVEL];
extern player_race race[MAX_RACES];
extern player_background background[MAX_BACKGROUND];
#endif

extern u32b player_exp[MAX_PLAYER_LEVEL];
extern u16b player_hp[MAX_PLAYER_LEVEL];

extern s16b char_row;
extern s16b char_col;

#if 0 /* not used? */
extern char *dsp_race[MAX_RACES]; /* Short strings for races. -CJS- */
#endif

extern byte rgold_adj[MAX_RACES][MAX_RACES];

extern player_class class[MAX_CLASS];
extern s16b class_level_adj[MAX_CLASS][MAX_LEV_ADJ];

/* used to check if player knows all spells knowable to him -CFT */
extern u16b player_init[MAX_CLASS][5];

extern s16b total_winner;


/*** Spell Information ***/

/* Warriors don't have spells, so there is no entry for them. */
#ifdef MACGAME
extern spell_type (*magic_spell)[63];
#else
extern spell_type magic_spell[MAX_CLASS-1][63];
#endif
extern cptr spell_names[127];
extern u32b spell_learned;	/* Bit field for spells learnt -CJS- */
extern u32b spell_learned2;	/* Bit field for spells learnt -CJS- */
extern u32b spell_worked;	/* Bit field for spells tried -CJS- */
extern u32b spell_worked2;	/* Bit field for spells tried -CJS- */
extern u32b spell_forgotten;	/* Bit field for spells forgotten -JEW- */
extern u32b spell_forgotten2;	/* Bit field for spells forgotten -JEW- */
extern byte spell_order[64];	/* remember order that spells are learned in */
extern u32b spellmasks[MAX_CLASS][2];	/* what spells can classes learn */


/*** Store information ***/

/* Following are store definitions				*/
#ifdef MACGAME
extern owner_type *owners;
#else
extern owner_type owners[MAX_OWNERS];
#endif
#ifdef MAC
extern store_type *store;
#else
extern store_type store[MAX_STORES];
#endif
extern u16b store_choice[MAX_STORES][STORE_CHOICES];
#ifndef MAC
extern int (*store_buy[MAX_STORES])();
#endif


/*** Miscellaneous Information ***/

/* Following are treasure arrays	and variables			*/
#ifdef MACGAME
extern treasure_type *object_list;
#else
extern treasure_type object_list[MAX_OBJECTS];
#endif
extern byte object_ident[OBJECT_IDENT_SIZE];
extern s16b t_level[MAX_OBJ_LEVEL+1];
extern inven_type t_list[MAX_TALLOC];
extern inven_type inventory[INVEN_ARRAY_SIZE];
extern cptr special_names[SN_ARRAY_SIZE];
extern s16b sorted_objects[MAX_DUNGEON_OBJ];
extern s16b inven_ctr;			/* Total different obj's	*/
extern s16b inven_weight;		/* Cur carried weight	*/
extern s16b equip_ctr;			/* Cur equipment ctr	*/
extern s16b tcptr;				/* Cur treasure heap ptr	*/

/* Following are creature arrays and variables			*/
#ifdef MACGAME
extern creature_type *c_list;
#else
extern creature_type c_list[MAX_CREATURES];
#endif
extern describe_mon_type desc_list[MAX_CREATURES];
extern monster_type m_list[MAX_MALLOC];
extern s16b m_level[MAX_MONS_LEVEL+1];
extern monster_attack monster_attacks[N_MONS_ATTS];
#ifdef MAC
extern monster_lore *c_recall;
#else
extern monster_lore c_recall[MAX_CREATURES];	/* Monster memories. -CJS- */
#endif
extern monster_type blank_monster; /* Blank monster values	*/
extern s16b mfptr;				   /* Cur free monster ptr	*/
extern s16b mon_tot_mult;		   /* # of repro's of creature	*/

/* Following are arrays for descriptive pieces			*/
#ifdef MACGAME
extern cptr *colors;
extern cptr *mushrooms;
extern cptr *woods;
extern cptr *metals;
extern cptr *rocks;
extern cptr *amulets;
extern cptr *syllables;
#else
extern cptr colors[MAX_COLORS];
extern cptr mushrooms[MAX_MUSH];
extern cptr woods[MAX_WOODS];
extern cptr metals[MAX_METALS];
extern cptr rocks[MAX_ROCKS];
extern cptr amulets[MAX_AMULETS];
extern cptr syllables[MAX_SYLLABLES];
#endif

extern byte blows_table[11][12];

extern u16b normal_table[NORMAL_TABLE_SIZE];

/* Initialized data which had to be moved from some other file */
/* Since these get modified, macrsrc.c must be able to access them */
/* Otherwise, game cannot be made restartable */
/* dungeon.c */
extern char last_command;		/* Memory of previous command. */
/* moria1.c */
/* Track if temporary light about player.  */
extern int light_flag;

#ifdef MSDOS
extern byte	floorsym, wallsym;
extern int	ansi, saveprompt;
extern char	moriatop[], moriasav[];
#endif


/* If you use NEW_FILEPATHS, we need these externs; if you use OLD_FILEPATHS */
/* these are #define'd to something, so they aren't variables.         [cjh] */
#ifdef NEW_FILEPATHS
extern char *ANGBAND_TST;		/* was LIBDIR(test)					*/
extern char *ANGBAND_HOU;		/* was LIBDIR(files/hours)			*/
extern char *ANGBAND_MOR;		/* was LIBDIR(files/news)			*/
extern char *ANGBAND_TOP;		/* was LIBDIR(files/newscores)		*/
extern char *ANGBAND_BONES;		/* was LIBDIR(bones)				*/
extern char *ANGBAND_HELP;		/* was LIBDIR(files/roglcmds.hlp)	*/
extern char *ANGBAND_ORIG_HELP;	/* was LIBDIR(files/origcmds.hlp)	*/
extern char *ANGBAND_WIZ_HELP;	/* was LIBDIR(files/rwizcmds.hlp)	*/
extern char *ANGBAND_OWIZ_HELP;	/* was LIBDIR(files/owizcmds.hlp)	*/
extern char *ANGBAND_WELCOME;	/* was LIBDIR(files/welcome.hlp)	*/
extern char *ANGBAND_LOG;		/* was LIBDIR(files/ANGBAND.log)	*/
extern char *ANGBAND_VER;		/* was LIBDIR(files/version.hlp)	*/
extern char *ANGBAND_LOAD;		/* was LIBDIR(files/loadcheck)		*/
extern char *ANGBAND_WIZ;		/* was LIBDIR(files/wizards)		*/
extern char *ANGBAND_SAV;		/* was LIBDIR(save)					*/
#endif /* NEW_FILEPATHS */


/*
 * function return values
 * only extern functions declared here, static functions declared
 * inside the file that defines them.  Duh...
 */


#ifdef __STDC__

/* birth.c */
void create_character(void);

/* creature.c */
void update_mon(int);
int movement_rate(int);
int multiply_monster(int, int, int, int);
void creatures(int);

/* death.c */
void exit_game(void);
void display_scores(int, int);
void delete_entry(int);
long total_points(void);
int look_line(int);

/* desc.c */
int is_a_vowel(int);
void magic_init(void);
void known1(inven_type *);
int known1_p(inven_type *);
void known2(inven_type *);
int known2_p(inven_type *);
void clear_known2(inven_type *);
void clear_empty(inven_type *);
void store_bought(inven_type *);
int store_bought_p(inven_type *);
void sample(struct inven_type *);
void identify(int *);
void unmagic_name(inven_type *);
void objdes(char *, struct inven_type *, int);
void scribe_object(void);
void add_inscribe(inven_type *, int);
void inscribe(inven_type *, cptr);
void invcopy(inven_type *, int);
void desc_charges(int);
void desc_remain(int);
s16b object_offset(inven_type *);

/* dungeon.c */
void dungeon(void);
int special_check(inven_type *);
int is_quest(int);
void rerate(void);
int ruin_stat(int);

/* effects.c */
void eat(void);

/* files.c */
void init_scorefile(void);
void init_files(void);
void read_times(void);
void helpfile(cptr);
void print_objects(void);
#ifdef MAC
int file_character(void)
#else
int file_character(char *);
#endif

/* generate.c */
void generate_cave(void);

/* help.c */
void ident_char(void);

/* io.c */

#ifdef SIGTSTP
int suspend(void);
#endif
void init_curses(void);
void moriaterm(void);
void put_buffer(cptr, int, int);
void put_qio(void);
void restore_term(void);
void shell_out(void);
char inkey(void);
void flush(void);
void erase_line(int, int);
void clear_screen(void);
void clear_from(int);
void move_cursor_relative(int, int);
void count_msg_print(cptr);
void prt(cptr, int, int);
void move_cursor(int, int);
void msg_print(cptr);
int get_check(cptr);
int get_com(cptr, char *);
int get_string(char *, int, int, int);
void pause_line(int);
void pause_exit(int, int);
void save_screen(void);
void restore_screen(void);
void bell(void);
void screen_map(void);
void print(int, int, int);

/* magic.c */
void cast(void);

/* arrays.c */
#ifdef NEW_FILEPATHS
void get_file_paths( void );
#endif

/* main.c */
int main(int, char **);

/* misc1.c */
void init_seeds(void);
void set_seed(u32b);
void reset_seed(void);
int check_time(void);
int randnor(int, int);
int bit_pos(u32b *);
void panel_bounds(void);
int get_panel(int, int, int);
int next_to_wall(int, int);
int next_to_corr(int, int);
int damroll(int, int);
int pdamroll(byte *);
int los(int, int, int, int);
unsigned char loc_symbol(int, int);
int test_light(int, int);
void prt_map(void);
void add_food(int);

/* misc2.c */
int popm(void);
int max_hp(byte *);
int place_monster(int, int, int, int);
int place_win_monster(void);
void place_group(int, int, int, int);
int get_mons_num(int);
void alloc_monster(int, int, int);
int summon_monster(int * ,int *, int);
int summon_undead(int *, int *);
int summon_demon(int, int *, int *);
int summon_dragon(int *, int *);
int summon_wraith(int *, int *);
int summon_reptile(int *, int *);
int summon_spider(int *, int *);
int summon_angel(int *, int *);
int summon_ant(int *, int *);
int summon_unique(int *, int *);
int summon_jabberwock(int *, int *);
int summon_gundead(int *, int *);
int summon_ancientd(int *, int *);
int summon_hound(int *, int *);
int summon_jelly(int *, int *);
int popt(void);
void pusht(int);
int magik(int);
int m_bonus(int, int, int);
void magic_treasure(int, int, int, int);
void set_options(void);
int compact_monsters(void);
int next_to_walls(int, int);
int get_nmons_num(int);
int distance(int, int, int, int);

/* misc3.c */
void place_trap(int, int, int);
void place_rubble(int, int);
void place_gold(int, int);
int get_obj_num(int,int);
void place_object(int, int);
void alloc_object(int (*)(), int, int);
void random_object(int, int, int);
void cnv_stat(int, char *);
void prt_stat(int);
void prt_field(cptr, int, int);
int stat_adj(int);
int chr_adj(void);
int con_adj(void);
cptr title_string(void);
void prt_title(void);
void prt_level(void);
void prt_cmana(void);
void prt_mhp(void);
void prt_chp(void);
void prt_pac(void);
void prt_gold(void);
void prt_depth(void);
void prt_hunger(void);
void prt_blind(void);
void prt_confused(void);
void prt_afraid(void);
void prt_poisoned(void);
void prt_state(void);
void prt_speed(void);
void prt_study(void);
void prt_winner(void);
u16b modify_stat(int, int);
void set_use_stat(int);
int inc_stat(int);
int dec_stat(int);
int res_stat(int);
void bst_stat(int, int);
int tohit_adj(void);
int toac_adj(void);
int todis_adj(void);
int todam_adj(void);
void prt_stat_block(void);
void draw_cave(void);
void put_character(void);
void put_stats(void);
cptr likert(int, int);
void put_misc1(void);
void put_misc2(void);
void put_misc3(void);
void display_char(void);
void get_name(void);
void change_name(void);
void inven_destroy(int);
void take_one_item(struct inven_type *, struct inven_type *);
void inven_drop(int, int);
int inven_damage(int (*)(), int);
int weight_limit(void);
int inven_check_num(inven_type *);
int inven_check_weight(struct inven_type *);
void check_strength(void);
int inven_carry(struct inven_type *);
int spell_chance(int);
void print_spells(int *, int, int, int);
int get_spell(int *, int, int *, int *, cptr, int);
void calc_spells(int);
void gain_spells(void);
void calc_mana(int);
void prt_experience(void);
void calc_hitpoints(void);
void insert_str(char *, cptr, cptr);
void insert_lnum(char *, cptr, s32b, int);
int enter_wiz_mode(void);
int attack_blows(int, int *);
int tot_dam(struct inven_type *, int, int);
int critical_blow(int, int, int, int);
int mmove(int, int *, int *);
int player_saves(void);
int find_range(int, int, int *, int *);
void teleport(int);
void check_view(void);
void place_special(int, int, u32b);
int place_ghost(void);
void prt_cut(void);
void prt_stun(void);
void special_random_object(int, int, int);
void cut_player(int);
void stun_player(int);
void prt_equippy_chars(void);
void get_coin_type(creature_type *);

/* monsters.c */

/* moria1.c */
void change_speed(int);
void py_bonuses(struct inven_type *, int);
void calc_bonuses(void);
int show_inven(int, int, int, int, int ());
cptr describe_use(int);
int show_equip(int, int);
void takeoff(int, int);
int verify(cptr, int);
void inven_command(int);
int get_item(int *, cptr, int, int, int ());
int no_light(void);
int get_dir(cptr, int *);
int get_alldir(cptr, int *);
void move_rec(int, int, int, int);
void light_room(int, int);
void move_light(int, int, int, int);
void disturb(int, int);
void search_on(void);
void search_off(void);
void rest(void);
void rest_off(void);
int test_hit(int, int, int, int, int);
void take_hit(int, cptr);
void change_trap(int, int);
void search(int, int, int);
void find_init(int);
void find_run(void);
void end_find(void);
void area_affect(int, int, int);
int minus_ac(u32b);
void corrode_gas(cptr);
void poison_gas(int, cptr);
void fire_dam(int, cptr);
void cold_dam(int, cptr);
void light_dam(int, cptr);
void acid_dam(int, cptr);
void darken_player(int, int);

/* moria2.c */
int cast_spell(cptr ,int, int *, int *);
void delete_monster(int);
void fix1_delete_monster(int);
void fix2_delete_monster(int);
int delete_object(int, int);
u32b monster_death(int, int, u32b, u32b, u32b);
int mon_take_hit(int, int, int);
void move_char(int, int);
void openobject(void);
void closeobject(void);
int twall(int, int, int, int);
void tunnel(int);
void disarm_trap(void);
void look(void);
void throw_object(void);
void bash(void);
void delete_unique(void);
void carry(int, int, int);
void check_unique(monster_type *);
void target(void); /* target fns stolen from Morgul -CFT */
int at_target(int, int); /* target fns stolen from Morgul -CFT */
void mmove2(int *, int *, int, int, int, int);

#ifdef MSDOS
/* ms_misc.c */
char *getlogin(void);
#ifdef __TURBOC__
void sleep(int);
#else
unsigned int sleep(int );
#endif
void error(char *, ...);
void warn(char *, ...);
void msdos_init(void);
void msdos_raw(void);
void msdos_noraw(void);
int bios_getch(void);
int msdos_getch(void);
void bios_clear(void);
void msdos_intro(void);
void bios_clear(void);
#endif

/* potions.c */
void quaff(void);

/* prayer.c */
void pray(void);

/* recall.c */
int bool_roff_recall(int);
int roff_recall(int);

/* rnd.c is unused now -CWS */
/* random.c */
#if !(defined(linux) || defined(__osf__))
long random(void);
#if !(defined(__MINT__) || defined(__386BSD__))
void srandom(int);
#endif
char *initstate(unsigned int, char *, int);
char *setstate(char *);
#endif

/* rods.c */
void activate_rod(void);

/* save.c */
#ifdef MAC
int save_char(int);
#else
int save_char(void);
#endif
int _save_char(char *);
int get_char(int *);

/* scrolls.c */
void read_scroll(void);

/* sets.c */
int set_room(int);
int set_corr(int);
int set_floor(int);
int set_corrodes(inven_type *);
int set_flammable(inven_type *);
int set_frost_destroy(inven_type *);
int set_acid_affect(inven_type *);
int set_lightning_destroy(inven_type *);
int set_null(inven_type *);
int set_acid_destroy(inven_type *);
int set_fire_destroy(inven_type *);
int set_plasma_destroy(inven_type *);
int set_meteor_destroy(inven_type *);
int set_holy_destroy(inven_type *);
int set_mana_destroy(inven_type *);
int general_store(int);
int armory(int);
int weaponsmith(int);
int temple(int);
int alchemist(int);
int magic_shop(int);

#ifdef MAC
int store_buy(int, int);
#endif

/* signals.c */
void nosignals(void);
void signals(void);
void init_signals(void);
void ignore_signals(void);
void default_signals(void);
void restore_signals(void);

/* spells.c */
void monster_name(char *, struct monster_type *, struct creature_type *);
void lower_monster_name(char *, struct monster_type *, struct creature_type *);
int sleep_monsters1(int, int);
int detect_treasure(void);
int detect_object(void);
int detect_trap(void);
int detect_sdoor(void);
int detect_invisible(void);
int light_area(int, int, int, int);
int unlight_area(int, int);
void map_area(void);
int ident_spell(void);
int aggravate_monster(int);
int trap_creation(void);
int door_creation(void);
int td_destroy(void);
int detect_monsters(void);
void mon_light_dam(int, int, int);
void light_line(int, int, int);
void frost_line(int, int, int, int);
void starlite(int, int);
int disarm_all(int, int, int);
void get_flags(int, u32b *, u32b *, int (**)());
void fire_bolt(int, int, int, int, int);
void fire_ball(int, int, int, int, int, int);
void breath(int, int, int, int, char *, int);
int recharge(int);
int hp_monster(int, int, int, int);
int drain_life(int, int, int, int);
int speed_monster(int, int, int, int);
int confuse_monster(int, int, int, int);
int sleep_monster(int, int, int);
int wall_to_mud(int, int, int);
int td_destroy2(int, int, int);
int poly_monster(int, int, int);
int build_wall(int, int, int);
int clone_monster(int, int, int);
void teleport_away(int, int);
void teleport_to(int, int);
int teleport_monster(int, int, int);
int mass_genocide(int);
int genocide(int);
int speed_monsters(int);
int sleep_monsters2(void);
int mass_poly(void);
int detect_evil(void);
int hp_player(int);
int cure_confusion(void);
int cure_blindness(void);
int cure_poison(void);
int remove_fear(void);
void earthquake(void);
int protect_evil(void);
void create_food(void);
int dispel_creature(int, int);
int turn_undead(void);
void warding_glyph(void);
void lose_str(void);
void lose_int(void);
void lose_wis(void);
void lose_dex(void);
void lose_con(void);
void lose_chr(void);
void lose_exp(s32b);
int slow_poison(void);
void bless(int);
void detect_inv2(int);
void destroy_area(int, int);
int enchant(inven_type *, int, byte);
void elemental_brand(void);
int remove_curse(void);
int restore_level(void);
void self_knowledge(void);
int probing(void);
int detection(void);
void starball(int,int);
void bolt(int, int, int, int, char *, monster_type *, int);
int lose_all_info(void);
void tele_level(void);
void identify_pack(void);
int fear_monster(int, int, int, int);
int banish_creature(u32b, int);
int remove_all_curse(void);
void darken_room(int, int);
void lite_spot(int, int);
cptr pain_message(int, int);
void line_spell(int, int, int, int, int);

/* staffs.c */
void use(void);

/* store1.c */
s32b item_value(struct inven_type *);
s32b sell_price(int, s32b *, s32b *, struct inven_type *);
int store_check_num(inven_type *, int);
void store_carry(int, int *, struct inven_type *);
void store_destroy(int, int, int);
void store_init(void);
void store_maint(void);
int noneedtobargain(int, s32b);
void updatebargain(int, s32b, s32b);

/* store2.c */
void enter_store(int);

/* treasur1.c */

/* treasur2.c */

/* undef.c */
void init_files(void);
int _new_log(void);

#ifdef unix
/* unix.c */
int check_input(int);
#if 0
int system_cmd(char *);
#endif
void user_name(char *, int);
int tilde(cptr, char *);
FILE *my_tfopen(cptr, cptr);
int my_topen(cptr, int, int);
#endif

/* util.c */
#ifdef NEEDS_STRICMP
int my_stricmp(cptr, cptr);
#endif
#ifdef NEEDS_USLEEP
int microsleep(unsigned long);
#endif

/* variable.c */

/* wands.c */
void aim(void);

/* wizard.c */
void wizard_light(int);
void change_character(void);
void wizard_create(void);
void artifact_check(void);
void artifact_check_no_file(void);
void check_uniques(void);
int is_wizard(int);

#else

/* create.c */
void create_character();

/* creature.c */
void update_mon();
int movement_rate();
int multiply_monster();
void creatures();

/* death.c */
void exit_game();
void display_scores();
void delete_entry();
long total_points();
int look_line();

/* desc.c */
int is_a_vowel();
void magic_init();
void known1();
int known1_p();
void known2();
int known2_p();
void clear_known2();
void clear_empty();
void store_bought();
int store_bought_p();
void sample();
void identify();
void unmagic_name();
void objdes();
void scribe_object();
void add_inscribe(); 
void inscribe();
void invcopy();
void desc_charges();
void desc_remain();
s16b object_offset();

/* dungeon.c */
void dungeon();
int special_check();
int is_quest();
void rerate();
int ruin_stat();

/* eat.c */
void eat();

/* files.c */
void init_scorefile();
void init_files();
void read_times();
void helpfile();
void print_objects();
int file_character();
#ifdef NEW_FILEPATHS
void get_file_paths();
#endif

/* generate.c */
void generate_cave();

/* help.c */
void ident_char();

/* io.c */
#ifdef SIGTSTP
int suspend();
#endif
void init_curses();
void moriaterm();
void put_buffer();
void put_qio();
void restore_term();
void shell_out();
char inkey();
void flush();
void erase_line();
void clear_screen();
void clear_from();
void move_cursor_relative();
void count_msg_print();
void prt();
void move_cursor();
void msg_print();
int get_check();
int get_com();
int get_string();
void pause_line();
void pause_exit();
void save_screen();
void restore_screen();
void bell();
void screen_map();
void print();

/* magic.c */
void cast();

/* main.c */
int main();

/* misc1.c */
void init_seeds();
void set_seed();
void reset_seed();
int check_time();
int randnor();
int bit_pos();
int next_to_walls();
int next_to_corr();
int damroll();
int pdamroll();
int los();
unsigned char loc_symbol();
int test_light();
void prt_map();
void add_food();
int popm();
int max_hp();
int place_monster();
int place_win_monster();
int get_mons_num();
void place_group();
void alloc_monster();
int summon_monster();
int summon_undead();
int popt();
void pusht();
int magik();
int m_bonus();
void magic_treasure();
int distance();

/* misc2.c */
void place_trap();
void place_rubble();
void place_gold();
int get_obj_num();
void place_object();
void alloc_object();
void random_object();
void cnv_stat();
void prt_stat();
void prt_field();
int stat_adj();
int chr_adj();
int con_adj();
char *title_string();
void prt_title();
void prt_level();
void prt_cmana();
void prt_mhp();
void prt_chp();
void prt_pac();
void prt_gold();
void prt_depth();
void prt_hunger();
void prt_blind();
void prt_confused();
void prt_afraid();
void prt_poisoned();
void prt_state();
void prt_speed();
void prt_study();
void prt_winner();
u16b modify_stat();
void set_use_stat();
int inc_stat();
int dec_stat();
int res_stat();
void bst_stat();
int tohit_adj();
int toac_adj();
int todis_adj();
int todam_adj();
void prt_stat_block();
void draw_cave();
void put_character();
void put_stats();
char *likert();
void put_misc1();
void put_misc2();
void put_misc3();
void display_char();
void get_name();
void change_name();
void inven_destroy();
void take_one_item();
void inven_drop();
int inven_damage();
int weight_limit();
int inven_check_num();
int inven_check_weight();
void check_strength();
int inven_carry();
int spell_chance();
void print_spells();
int get_spell();
void calc_spells();
void gain_spells();
void calc_mana();
void prt_experience();
void calc_hitpoints();
void insert_str();
void insert_lnum();
int enter_wiz_mode();
int attack_blows();
int tot_dam();
int critical_blow();
int mmove();
int player_saves();
int find_range();
void teleport();
void check_view();
void place_special();
int place_ghost();
void prt_cut();
void prt_stun();
void special_random_object();
void cut_player();
void stun_player();
extern void prt_equippy_chars();
void get_coin_type();

/* monsters.c */

/* moria1.c */
void change_speed();
void py_bonuses();
void calc_bonuses();
int show_inven();
char *describe_use();
int show_equip();
void takeoff();
void check_strength();
int verify();
void inven_command();
int get_item();
void panel_bounds();
int get_panel();
int no_light();
int get_dir();
int get_alldir();
void move_rec();
void light_room();
void move_light();
void disturb();
void search_on();
void search_off();
void rest();
void rest_off();
int test_hit();
void take_hit();
void change_trap();
void search();
void set_options();
void find_init();
void find_run();
void end_find();
void area_affect();
int minus_ac();
void corrode_gas();
void poison_gas();
void fire_dam();
void cold_dam();
void light_dam();
void acid_dam();
void lite_spot();
void darken_player();

/* moria2.c */
int cast_spell();
void delete_monster();
void fix1_delete_monster();
void fix2_delete_monster();
int multiply_monster();
int delete_object();
u32b monster_death();
int mon_take_hit();
void move_char();
void openobject();
void closeobject();
int twall();
void tunnel();
void disarm_trap();
void look();
void throw_object();
void bash();
void target();					/* target fns stolen from Morgul -CFT */
int at_target();				/* target fns stolen from Morgul -CFT */
void mmove2();					/* target fns stolen from Morgul -CFT */

#ifdef MSDOS
/* ms_misc.c */
char *getlogin();
#ifdef __TURBOC__
void sleep();
#else
unsigned int sleep();
#endif
void error();
void warn();
void msdos_init();
void msdos_raw();
void msdos_noraw();
int bios_getch();
int msdos_getch();
void bios_clear();
void msdos_intro();
void msdos_print_map();
void bios_clear();
#endif

/* potions.c */
void quaff();

/* prayer.c */
void pray();

/* recall.c */
int bool_roff_recall();
int roff_recall();

/* rnd.c is unused now -CWS */
/* random.c */
#if !(defined(linux) || defined(__osf__))
long random();
void srandom();
char *initstate();
char *setstate();
#endif

/* save.c */
int save_char();
int _save_char();
int get_char();

/* scrolls.c */
void read_scroll();

/* sets.c */
int set_room();
int set_corr();
int set_floor();
int set_corrodes();
int set_flammable();
int set_frost_destroy();
int set_acid_affect();
int set_lightning_destroy();
int set_null();
int set_acid_destroy();
int set_fire_destroy();
int set_plasma_destroy();
int set_meteor_destroy();
int set_holy_destroy();
int set_mana_destroy();
int general_store();
int armory();
int weaponsmith();
int temple();
int alchemist();
int magic_shop();
#ifdef MAC
int store_buy();
#endif

/* signals.c */
void nosignals();
void signals();
void init_signals();
void ignore_signals();
void default_signals();
void restore_signals();

/* spells.c */
void monster_name();
void lower_monster_name();
int sleep_monsters1();
int detect_treasure();
int detect_object();
int detect_trap();
int detect_sdoor();
int detect_invisible();
int light_area();
int unlight_area();
void map_area();
int ident_spell();
int aggravate_monster();
int trap_creation();
int door_creation();
int td_destroy();
int detect_monsters();
void light_line();
void starlite();
int disarm_all();
void get_flags();
void fire_bolt();
void fire_ball();
void breath();
int recharge();
int hp_monster();
int drain_life();
int speed_monster();
int confuse_monster();
int sleep_monster();
int wall_to_mud();
int td_destroy2();
int poly_monster();
int build_wall();
int clone_monster();
void teleport_away();
void teleport_to();
int teleport_monster();
int mass_genocide();
int genocide();
int speed_monsters();
int sleep_monsters2();
int mass_poly();
int detect_evil();
int hp_player();
int cure_confusion();
int cure_blindness();
int cure_poison();
int remove_fear();
void earthquake();
int protect_evil();
void create_food();
int dispel_creature();
int turn_undead();
void warding_glyph();
void lose_str();
void lose_int();
void lose_wis();
void lose_dex();
void lose_con();
void lose_chr();
void lose_exp();
int slow_poison();
void bless();
void detect_inv2();
void destroy_area();
int enchant();
int remove_curse();
int restore_level();
void self_knowledge();
char *pain_message()
void line_spell();

/* staffs.c */
void use();

/* store1.c */
s32b item_value();
s32b sell_price();
int store_check_num();
void store_carry();
void store_destroy();
void store_init();
void store_maint();
int noneedtobargain();
void updatebargain();

/* store2.c */
void enter_store();

/* treasur1.c */

/* treasur2.c */

/* undef.c */
void init_files();
int _new_log();

#ifdef unix
/* unix.c */
int check_input();
#if 0
int system_cmd();
#endif
void user_name();
int tilde();
/* only declare this if stdio.h has been previously included, which will
 * be true if stdin is defined
 */
#ifdef stdin
FILE *my_tfopen();
#endif
int my_topen();
#endif

/* util.c */
#ifdef NEEDS_STRICMP
int my_stricmp();
#endif
#ifdef NEEDS_USLEEP
int microsleep();
#endif

/* variable.c */

/* wands.c */
void aim();

/* wizard.c */
void wizard_light();
void change_character();
void wizard_create();
void artifact_check();
void artifact_check_no_file();
void check_uniques();
int is_wizard();
#endif      /* __STDC__ */
