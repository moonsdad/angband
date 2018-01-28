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


/*
 * Here's some functions that've been macroized rather than being called
 * from everywhere.  They're short enough so that inlining them will probably
 * result in a smaller executable, and speed things up, to boot. -CWS
 */


/*
 * Simple integer math functions
 * Should just use the real ones.
 */
#define MY_MAX	MAX
#define MY_MIN	MIN
#define MY_ABS	ABS
#define MY_POM	POM


/*
 * Determines if a map location is fully inside the outer walls -RAK-
 */
#define in_bounds(y, x) \
   ((((y) > 0) && ((x) > 0) && ((y) < cur_height-1) && ((x) < cur_width-1)) ? \
    (TRUE) : (FALSE))

/*
 * Determines if a map location is on or inside the outer walls
 * Checks if we can see this point -CWS
 */
#define in_bounds2(y, x) \
   ((((y) >= 0) && ((x) >= 0) && ((y) < cur_height) && ((x) < cur_width)) ? \
    (TRUE) : (FALSE))


    
/*
 * Determines if a map location is currently "on screen" -RAK-
 * Note that "panel_contains(y,x)" always implies "in_bounds2(y,x)".
 */
#define panel_contains(y, x) \
  ((((y) >= panel_row_min) && ((y) <= panel_row_max) && \
    ((x) >= panel_col_min) && ((x) <= panel_col_max)) ? (TRUE) : (FALSE))

/*
 * Determine if a given object is "wearable"
 */
#define wearable_p(T) \
	(((T)->tval >= TV_MIN_WEAR) && ((T)->tval <= TV_MAX_WEAR))

/*
 * Only wearable items can be cursed.
 */
#define cursed_p(T) \
	(wearable_p(T) && ((T)->flags3 & TR3_CURSED))

/*
 * Artifacts use the "name1" field
 */
#define artifact_p(T) \
	((T)->name1 ? TRUE : FALSE)

/**** Available macros ****/

/*
 * Generates a random long integer X where O<=X<M.
 * The integer X falls along a uniform distribution.
 * For example, if M is 100, you get "percentile dice"
 */
#define rand_int(M) (random() % (M))

/*
 * Generates a random long integer X where A<=X<=B
 * The integer X falls along a uniform distribution.
 * Note: rand_range(0,N-1) == rand_int(N)
 */
#define rand_range(A,B) ((A) + (rand_int(1+(B)-(A))))

/*
 * Generate a random long integer X where A-D<=X<=A+D
 * The integer X falls along a uniform distribution.
 * Note: rand_spread(A,D) == rand_range(A-D,A+D)
 */
#define rand_spread(A,D) ((A) + (rand_int(1+(D)+(D))) - (D))


/*
 * Generate a random long integer X where 1<=X<=M
 * Also, "correctly" handle the case of M<=1
 */
#define randint(M) (((M) <= 1) ? (1) : (rand_int(M) + 1))




/*****************************************************************************/


/*
 * Variable access to the version
 */
extern int cur_version_maj;
extern int cur_version_min;
extern int cur_patch_level;

extern int NO_SAVE;


/*
 * Note that "hack_m_idx" is not nearly as horrible a hack as before.
 * Needed because compact_monster() can be called from within
 * process_monsters() via place_monster() and summon_monster() 
 */

extern int hack_m_idx;			/* The "current" monster, if any */

extern int player_uid;			/* The player's uid, or zero */

extern vtype savefile;		/* The save file name */

extern vtype died_from;		/* Cause of death */

extern char days[7][29];		/* The load check info */

extern s16b log_index;		/* Index to log file. -CJS- */


/*
 * These are options, set with via "do_cmd_options()" -CJS-
 */

/* Option set 1 */

extern int rogue_like_commands;	/* Pick initial keyset */
extern int quick_messages;		/* Quick messages -CWS */
extern int prompt_carry_flag;		/* Require "g" key to pick up */
extern int carry_query_flag;		/* Prompt for pickup */
extern int always_repeat;		/* Always repeat commands */

extern int new_screen_layout;	/* Use the new screen layout */
extern int equippy_chars;	/* do equipment characters -CWS */
extern int depth_in_feet;	/* Display the depth in "feet" */
extern int notice_seams;	/* Highlight mineral seams */

extern int use_color;		/* Use color if possible */


/* Option Set 2 -- Disturbance */

extern int find_cut;		/* Cut corners */
extern int find_examine;	/* Examine corners */
extern int find_prself;		/* Print self on a run (slower) */
extern int find_bound;		/* Stop on borders when the map shifts */
extern int find_ignore_doors;	/* Run through doors */


/* Option set 3 -- Gameplay */

extern int no_haggle_flag;	/* Cancel haggling */

extern int show_inven_weight;	/* Show weights in inven */
extern int show_equip_weight;	/* Show weights in equip */
extern int plain_descriptions;	/* Plain descriptions don't add color */


/*
 * More options
 */
extern int delay_spd;		/* 1-9 for delays, or zero */
extern int hitpoint_warn;	/* 1-9 for hitpoint warning, or zero */


/*
 * More flags
 */
extern int in_store_flag;		/* Currently in a store */
extern int peek;			/* should we display additional msgs */
extern int coin_type;			/* Hack -- creeping coin treasure type */
extern int opening_chest;		/* Hack -- do not generate another chest */

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

/*
 * Term window info
 */
extern term *term_screen;	/* The main screen */
extern term *term_recall;	/* The recall window */
extern term *term_choice;	/* The choice window */


/* The current dungeon level */
/* Was: extern cave_type cave[MAX_HEIGHT][MAX_WIDTH]; */
extern cave_type *cave[MAX_HEIGHT];


/*
 * global flags
 */

extern int LOAD;
extern int closing_flag;		/* Used for closing   */

extern int in_store_flag;		/* Notice when in stores */
extern int good_item_flag;		/* Notice artifact created... */
extern int feeling;			/* level feeling */
extern int rating;			/* level rating */
extern int unfelt;
extern int new_level_flag;		/* Next level when true  */
extern int teleport_flag;		/* Handle teleport traps  */
extern int eof_flag;			/* Used to handle eof/HANGUP */
extern int player_light;		/* Player carrying light */
extern int cur_lite, old_lite;          /* Light radius */
extern int find_flag;			/* Are we running */
extern int free_turn_flag;		/* Is this turn free */

extern int weapon_heavy;		/* Flag if the weapon too heavy -CJS- */
extern int pack_heavy;			/* Flag if the pack too heavy -CJS- */

extern char doing_inven;		/* Track inventory commands */
extern int screen_change;		/* Notice disturbing of inventory */

extern int character_generated;		/* Character generation complete */
extern int character_saved;		/* prevents save on kill after save_player() */
extern int peek;			/* Peek like a wizard */
extern int be_nasty;
extern int monster_is_afraid;	/* redo monster fear messages -CWS */

extern int command_cmd;			/* Current command */
extern int command_old;			/* Last command */
extern int command_rep;			/* Repetition of commands. -CJS- */
extern int command_dir;			/* Use last direction in repeated commands */

extern s16b noscore;			/* Don't score this game. -CJS- */

extern u32b randes_seed;		/* Hack -- consistent object colors */
extern u32b town_seed;		/* Hack -- consistent town layout */


extern s16b dun_level;			/* Cur dungeon level   */
extern s16b object_level;		/* used to generate objects -CWS */
extern s16b missile_ctr;		/* Counter for missiles */
extern int msg_flag;			/* Set with first msg  */
extern vtype old_msg[MAX_SAVE_MSG];	/* Last messages -CJS- */
extern s16b last_msg;			/* Where in the array is the last */
extern int death;			/* True if died	      */
extern s32b turn;			/* Current game turn */
extern s32b old_turn;			/* Last turn dungeon() started */
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
extern u16b target_mon;
extern u16b target_row;
extern u16b target_col;
#endif

/* A pointer to the main player record */
extern player_type *p_ptr;

extern player_race race[MAX_RACES];
extern player_class class[MAX_CLASS];
extern player_background background[MAX_BACKGROUND];

extern cptr player_title[MAX_CLASS][MAX_PLAYER_LEVEL];

extern u32b player_exp[MAX_PLAYER_LEVEL];
extern u16b player_hp[MAX_PLAYER_LEVEL];

extern s16b char_row;
extern s16b char_col;

extern byte rgold_adj[MAX_RACES][MAX_RACES];

extern s16b class_level_adj[MAX_CLASS][MAX_LEV_ADJ];
extern u16b player_init[MAX_CLASS][5]; /* used to check if player knows all spells knowable to him -CFT */

extern s16b total_winner;


/*** Spell Information ***/

/* Warriors don't have spells, so there is no entry for them. */
extern spell_type magic_spell[MAX_CLASS-1][63];
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

extern owner_type owners[MAX_OWNERS];
extern store_type store[MAX_STORES];
extern u16b store_choice[MAX_STORES][STORE_CHOICES];
extern int (*store_buy[MAX_STORES])();


/*** Miscellaneous Information ***/

extern cptr ego_names[EGO_MAX];		/* Ego-item Names */

extern monster_attack a_list[MAX_A_IDX];	/* Monster attacks */

extern byte blows_table[11][12];

extern u16b normal_table[NORMAL_TABLE_SIZE];

/*** Inventory ***/

/* Inventory information */
extern s16b inven_weight;		/* Total carried weight */
extern s16b inven_ctr;			/* Number of obj's in inven */
extern s16b equip_ctr;			/* Number of obj's in equip */

/* Player inventory (inven+equip) */
extern inven_type inventory[INVEN_TOTAL];


/*** Item Information ***/

/* Treasure heap pointer (used with i_list) */
extern s16b i_max;

extern inven_type i_list[MAX_I_IDX];

extern inven_kind k_list[MAX_OBJECTS];

extern byte object_ident[OBJECT_IDENT_SIZE];

extern s16b t_level[MAX_K_LEV+1];

/* The "artifact" template array */
extern inven_very *v_list;

extern s16b sorted_objects[MAX_DUNGEON_OBJ];


/*** Monster Information ***/

/* Monster heap pointer (used with m_list) */
extern s16b m_max;

/* Actual array of physical monsters */
extern monster_type m_list[MAX_M_IDX];

/* The array of monster races */
extern monster_race r_list[MAX_R_IDX];

/* Monster memories. -CJS- */
extern monster_lore l_list[MAX_R_IDX];

extern describe_mon_type desc_list[MAX_R_IDX];

extern s16b r_level[MAX_R_LEV+1];

/* Brand new extra effecient and kind way to add unique monsters... HOORAY!! */
extern struct unique_mon u_list[MAX_R_IDX];

extern int quests[MAX_QUESTS];

extern monster_type blank_monster; /* Blank monster values	*/

/* moria1.c */
/* Track if temporary light about player.  */
extern int light_flag;

#ifdef MSDOS
extern byte	floorsym, wallsym;
extern int	ansi, saveprompt;
extern char	moriatop[], moriasav[];
#endif


/*
 * The FILEPATH's to various files, see "arrays.c"
 */

extern cptr ANGBAND_DIR_FILES;		/* Dir: ascii files  */
extern cptr ANGBAND_DIR_BONES;		/* Dir: ascii bones files was LIBDIR(bones) */
extern cptr ANGBAND_DIR_SAVE;		/* Dir: binary save files was LIBDIR(save) */
extern cptr ANGBAND_TST;		/* was LIBDIR(test) */
extern cptr ANGBAND_DIR_DATA;		/* Dir: system dependant files */

extern cptr ANGBAND_NEWS;		/* News file  was LIBDIR(files/news) */
extern cptr ANGBAND_WELCOME;		/* Player generation help was LIBDIR(files/welcome.hlp) */
extern cptr ANGBAND_VERSION;		/* Version information was LIBDIR(files/version.hlp) */

extern cptr ANGBAND_WIZ;		/* Acceptable wizard uid's was LIBDIR(files/wizards) */
extern cptr ANGBAND_HOURS;		/* Hours of operation was LIBDIR(files/hours) */
extern cptr ANGBAND_LOAD;		/* Load information was LIBDIR(files/loadcheck) */
extern cptr ANGBAND_LOG;		/* Log file of some form was LIBDIR(files/ANGBAND.log) */

extern cptr ANGBAND_R_HELP;		/* Roguelike command help was LIBDIR(files/roglcmds.hlp) */
extern cptr ANGBAND_O_HELP;		/* Original command help was LIBDIR(files/origcmds.hlp) */
extern cptr ANGBAND_W_HELP;		/* Wizard command help was LIBDIR(files/rwizcmds.hlp) */
extern cptr ANGBAND_OWIZ_HELP;		/* was LIBDIR(files/owizcmds.hlp) */

extern cptr ANGBAND_TOP;		/* was LIBDIR(files/newscores) */


/*
 * only extern functions declared here, static functions declared
 * inside the file that defines them.  Duh...
 */


#ifdef __STDC__

/* birth.c */
void player_birth(void);

/* creature.c */
void update_mon(int);
int movement_rate(int);
int multiply_monster(int, int, int, int);
void update_monsters(void);
void process_monsters(void);

/* death.c */
void init_scorefile(void);
void display_scores(int, int);
void delete_entry(int);
long total_points(void);
void exit_game(void);
int look_line(int);

/* desc.c */
void flavor_init(void);
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
void scribe_object(void);
void add_inscribe(inven_type *, int);
void inscribe(inven_type *, cptr);
void objdes(char *, struct inven_type *, int);
void invcopy(inven_type *, int);
void inven_item_charges(int);
void desc_remain(int);
s16b object_offset(inven_type *);

/* command.c */
void process_command(void);
void request_command(void);

/* dungeon.c */
void dungeon(void);
int special_check(inven_type *);
int is_quest(int);
void rerate(void);

/* effects.c */
void eat(void);
void do_cmd_aim_wand(void);
void do_cmd_zap_rod(void);

/* files.c */
void init_files(void);
void read_times(void);
void helpfile(cptr);
int file_character(cptr);
int mac_file_character(void);

/* generate.c */
void generate_cave(void);

/* cave.c */

int los(int, int, int, int);
int test_lite(int, int);
int no_lite(void);

void lite_room(int, int);
void unlite_room(int, int);

void move_cursor_relative(int, int);

void lite_spot(int, int);

void screen_map(void);

/* help.c */
void ident_char(void);

/* io.c */

void bell(void);

void move_cursor(int, int);

void text_to_ascii(char *buf, cptr str);
void ascii_to_text(char *buf, cptr str);

void flush(void);

char inkey(void);

void msg_print(cptr);

void erase_line(int, int);
void clear_screen(void);
void clear_from(int);

void c_put_str(byte, cptr, int, int);
void put_str(cptr, int, int);

void c_prt(byte, cptr, int, int);
void prt(cptr, int, int);

void save_screen(void);
void restore_screen(void);

int get_check(cptr);
int get_com(cptr, char *);
int get_string(char *, int, int, int);
int askfor(char *, int);
void pause_line(int);
void pause_exit(int, int);
void print(int, int, int);

/* magic.c */
int door_creation(void);
void stair_creation(void);
void cast(void);

/* arrays.c */
void get_file_paths(void);

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
int damroll(int, int);
int pdamroll(byte *);
unsigned char loc_symbol(int, int);
void prt_map(void);
void add_food(int);

/* misc2.c */
void delete_monster(int);
int compact_monsters(void);
int m_pop(void);
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
int next_to_walls(int, int);
int get_nmons_num(int);
int distance(int, int, int, int);

/* misc3.c */
int i_pop(void);
int delete_object(int, int);
void pusht(int);
int magik(int);
int m_bonus(int, int, int);
void place_trap(int, int, int);
void magic_treasure(int, int, int, int);
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
void display_player(void);
void get_name(void);
void change_name(void);
void inven_destroy(int);
void take_one_item(struct inven_type *, struct inven_type *);
void inven_drop(int, int);
int inven_damage(int (*)(), int);
int inven_check_num(inven_type *);
int inven_carry(struct inven_type *);
int weight_limit(void);
int inven_check_weight(struct inven_type *);
void check_strength(void);
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
void place_good(int, int, u32b);
int place_ghost(void);
void prt_cut(void);
void prt_stun(void);
void special_random_object(int, int, int);
void cut_player(int);
void stun_player(int);
void prt_equippy_chars(void);
void get_coin_type(monster_race *);

/* moria1.c */
int is_a_vowel(int);
void change_speed(int);
void py_bonuses(struct inven_type *, int);
cptr describe_use(int);
void calc_bonuses(void);
int show_inven(int, int, int, int, int ());
int show_equip(int, int);
void inven_takeoff(int, int);
int verify(cptr, int);
void inven_command(int);
int get_item(int *, cptr, int, int, int ());
void move_light(int, int, int, int);
int test_hit(int, int, int, int, int);
void take_hit(int, cptr);
void change_trap(int, int);
void area_affect(int, int, int);
int minus_ac(u32b);
void corrode_gas(cptr);
void poison_gas(int, cptr);
void fire_dam(int, cptr);
void cold_dam(int, cptr);
void elec_dam(int, cptr);
void acid_dam(int, cptr);
void darken_player(int, int);

/* moria2.c */
int ruin_stat(int);
void move_rec(int, int, int, int);
int cast_spell(cptr ,int, int *, int *);
u32b monster_death(int, int, u32b, u32b, u32b);
int mon_take_hit(int, int, int);
void check_unique(monster_type *);
void delete_unique(void);
void fix1_delete_monster(int);
void fix2_delete_monster(int);


/* moria3.c */
int twall(int, int, int, int);
void do_cmd_look(void);
void do_cmd_rest(void); /* SIC RMHV */
void do_cmd_open(void);
void do_cmd_close(void);
void do_cmd_tunnel(int);
void do_cmd_disarm(void);
void do_cmd_bash(void);
void do_cmd_spike(void);
void do_cmd_fire(void);
void do_cmd_rest(void);
void do_cmd_feeling(void);
void artifact_check(void);
void artifact_check_no_file(void);
void do_cmd_check_uniques(void);

/* moria4.c */
int target_at(int, int); /* target fns stolen from Morgul -CFT */
void target(void); /* target fns stolen from Morgul -CFT */
void mmove2(int *, int *, int, int, int, int);
int get_alldir(cptr, int *);
int get_dir(cptr, int *);
void disturb(int, int);
void search_on(void);
void search_off(void);
void search(int, int, int);
void rest_off(void);
void carry(int, int, int);
void move_player(int, int);
void find_step(void);
void find_init(void);
void end_find(void);

/* potions.c */
void quaff(void);

/* prayer.c */
void pray(void);

/* recall.c */
int roff_recall(int);
int bool_roff_recall(int);

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

/* save.c */
int save_player(void);
int _save_player(char *);
int load_player(int *);

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

#ifdef MACINTOSH
int store_buy(int, int);
#endif

/* signals.c */
void signals_ignore_tstp(void);
void signals_handle_tstp(void);
void signals_init(void);
void ignore_signals(void);
void default_signals(void);
void restore_signals(void);

/* spells.c */
void monster_name(char *, struct monster_type *, struct monster_race *);
int sleep_monsters1(int, int);
int detect_treasure(void);
int detect_magic(void);
int detect_object(void);
int detect_trap(void);
int detect_sdoor(void);
int detect_invisible(void);
int lite_area(int, int, int, int);
int unlite_area(int, int);
void map_area(void);
int ident_spell(void);
void identify_pack(void);
int aggravate_monster(int);
int trap_creation(void);
int door_creation(void);
void stair_creation(void);
int td_destroy(void);
int detect_monsters(void);
void lite_line(int, int, int);
void starlite(int, int);
int disarm_all(int, int, int);
void bolt(int, int, int, int, char *, monster_type *, int);
void get_flags(int, u32b *, u32b *, int (**)());
void breath(int, int, int, int, char *, int);
void fire_bolt(int, int, int, int, int);
void fire_ball(int, int, int, int, int, int);
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
void spell_hit_monster(monster_type *, int, int *, int, int *, int *, int);
void wiz_lite(int);
int lose_all_info(void);
void tele_level(void);
void identify_pack(void);
int fear_monster(int, int, int, int);
int banish_creature(u32b, int);
int remove_all_curse(void);
cptr pain_message(int, int);
void line_spell(int, int, int, int, int);

/* staffs.c */
void use(void);

/* store.c */
s32b item_value(struct inven_type *);
s32b sell_price(int, s32b *, s32b *, struct inven_type *);
int store_check_num(inven_type *, int);
void store_carry(int, int *, struct inven_type *);
void store_destroy(int, int, int);
void store_init(void);
void store_maint(void);
int noneedtobargain(int, s32b);
void updatebargain(int, s32b, s32b);
void enter_store(int);

/* treasur1.c */

/* treasur2.c */

/* undef.c */
int _new_log(void);

#ifdef unix
/* unix.c */
int check_input(int);
#endif

/* util.c */
#ifndef HAS_USLEEP
int usleep(unsigned long);
#endif
void delay(int);
void user_name(char *buf, int id);
int my_topen(cptr, int, int);
FILE *my_tfopen(cptr, cptr);

/* variable.c */

/* wizard.c */
void change_character(void);
void wizard_create(void);
int is_wizard(int);

#endif      /* __STDC__ */


