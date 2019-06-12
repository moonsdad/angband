/* File: desc.c */

/* Purpose: handle object descriptions, mostly string handling code */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke 
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies. 
 */

#include "angband.h"


/*
 * Hack -- note that "TERM_MULTI" is now just "TERM_VIOLET"
 * We will have to find a cleaner method for "MULTI_HUED" later.
 * There were only two multi-hued "flavors" (one potion, one food).
 * Plus five multi-hued "base-objects" (3 dragon scales, one blade
 * of chaos, and one something else).
 */
#define TERM_MULTI	TERM_VIOLET


/* Following are arrays for descriptive pieces
 *
 * Color adjectives and colors, for potions.
 * Hack -- The first three are hard-coded for slime mold juice,
 * apple juice, and water, so do not scramble them.
 */

static cptr potion_adj[MAX_COLORS] = {
    "Icky Green", "Light Brown", "Clear","Azure","Blue",
    "Blue Speckled","Black","Brown","Brown Speckled","Bubbling",
    "Chartreuse","Cloudy","Copper Speckled","Crimson","Cyan",
    "Dark Blue","Dark Green","Dark Red","Gold Speckled","Green",
    "Green Speckled","Grey","Grey Speckled","Hazy","Indigo",
    "Light Blue","Light Green","Magenta","Metallic Blue","Metallic Red",
    "Metallic Green","Metallic Purple","Misty","Orange","Orange Speckled",
    "Pink","Pink Speckled","Puce","Purple","Purple Speckled",
    "Red","Red Speckled","Silver Speckled","Smoky","Tangerine",
    "Violet","Vermilion","White","Yellow", "Purple Speckled",
    "Pungent","Clotted Red","Viscous Pink","Oily Yellow","Gloopy Green",
    "Shimmering","Coagulated Crimson"
};

static byte potion_col[MAX_COLORS] = {
    TERM_GREEN,TERM_L_UMBER,TERM_WHITE,TERM_L_BLUE,TERM_BLUE,
    TERM_BLUE,TERM_D_GRAY,TERM_UMBER,TERM_UMBER,TERM_L_GRAY,
    TERM_RED,TERM_WHITE,TERM_L_UMBER,TERM_RED,TERM_L_BLUE,
    TERM_BLUE,TERM_GREEN,TERM_RED,TERM_YELLOW,TERM_GREEN,
    TERM_GREEN,TERM_GRAY,TERM_GRAY,TERM_L_GRAY,TERM_VIOLET,
    TERM_L_BLUE,TERM_L_GREEN,TERM_RED,TERM_BLUE,TERM_RED,
    TERM_GREEN,TERM_VIOLET,TERM_L_GRAY,TERM_ORANGE,TERM_ORANGE,
    TERM_L_RED,TERM_L_RED,TERM_VIOLET,TERM_VIOLET,TERM_VIOLET,
    TERM_RED,TERM_RED,TERM_L_GRAY,TERM_D_GRAY,TERM_ORANGE,
    TERM_VIOLET,TERM_RED,TERM_WHITE,TERM_YELLOW,TERM_VIOLET,
    TERM_L_RED,TERM_RED,TERM_L_RED,TERM_YELLOW,TERM_GREEN,
    TERM_MULTI,TERM_RED
};


/*
 * Color adjectives and colors, for mushrooms and molds
 */

static cptr food_adj[MAX_SHROOM] = {
    "Blue","Black","Black Spotted","Brown","Dark Blue",
    "Dark Green","Dark Red","Ecru","Furry","Green",
    "Grey","Light Blue","Light Green","Plaid","Red",
    "Slimy","Tan","White","White Spotted","Wooden",
    "Wrinkled",/*"Yellow","Shaggy","Red Spotted","Pale Blue","Dark Orange"*/
};

static byte food_col[MAX_SHROOM] = {
    TERM_BLUE,TERM_D_GRAY,TERM_D_GRAY,TERM_UMBER,TERM_BLUE,
    TERM_GREEN,TERM_RED,TERM_YELLOW,TERM_L_GRAY,TERM_GREEN,
    TERM_GRAY,TERM_L_BLUE,TERM_L_GREEN,TERM_MULTI,TERM_RED,
    TERM_GRAY,TERM_L_UMBER,TERM_WHITE,TERM_WHITE,TERM_UMBER,
    TERM_UMBER,/*TERM_YELLOW,???,TERM_RED,TERM_L_BLUE,TERM_ORANGE*/
};


/*
 * Wood adjectives and colors, for staffs
 */

static cptr staff_adj[MAX_WOODS] = {
    "Aspen","Balsa","Banyan","Birch","Cedar",
    "Cottonwood","Cypress","Dogwood","Elm","Eucalyptus",
    "Hemlock","Hickory","Ironwood","Locust","Mahogany",
    "Maple","Mulberry","Oak","Pine","Redwood",
    "Rosewood","Spruce","Sycamore","Teak","Walnut",
    "Mistletoe","Hawthorn","Bamboo","Silver","Runed",
    "Golden","Ashen"/*,"Gnarled","Ivory","Decorative","Willow"*/
};

static byte staff_col[MAX_WOODS] = {
    TERM_L_UMBER,TERM_L_UMBER,TERM_L_UMBER,TERM_L_UMBER,TERM_L_UMBER,
    TERM_L_UMBER,TERM_L_UMBER,TERM_L_UMBER,TERM_L_UMBER,TERM_L_UMBER,
    TERM_L_UMBER,TERM_L_UMBER,TERM_UMBER,TERM_L_UMBER,TERM_UMBER,
    TERM_L_UMBER,TERM_L_UMBER,TERM_L_UMBER,TERM_L_UMBER,TERM_RED,
    TERM_RED,TERM_L_UMBER,TERM_L_UMBER,TERM_L_UMBER,TERM_UMBER,
    TERM_GREEN,TERM_L_UMBER,TERM_L_UMBER,TERM_L_GRAY,TERM_UMBER,
    TERM_YELLOW,TERM_GRAY,/*???,???,???,???*/
};


/*
 * Metal adjectives and colors, for wands
 */

static cptr wand_adj[MAX_METALS] = {
    "Aluminum","Cast Iron","Chromium","Copper","Gold",
    "Iron","Magnesium","Molybdenum","Nickel","Rusty",
    "Silver","Steel","Tin","Titanium","Tungsten",
    "Zirconium","Zinc","Aluminum-Plated","Copper-Plated","Gold-Plated",
    "Nickel-Plated","Silver-Plated","Steel-Plated","Tin-Plated","Zinc-Plated",
    "Mithril-Plated","Mithril","Runed","Bronze","Brass",
    "Platinum","Lead"/*,"Lead-Plated","Ivory","Pewter"*/
};

static byte wand_col[MAX_METALS] = {
    TERM_L_BLUE,TERM_D_GRAY,TERM_WHITE,TERM_L_UMBER,TERM_YELLOW,
    TERM_GRAY,TERM_L_GRAY,TERM_L_GRAY,TERM_L_UMBER,TERM_RED,
    TERM_L_GRAY,TERM_L_GRAY,TERM_L_GRAY,TERM_WHITE,TERM_WHITE,
    TERM_L_GRAY,TERM_L_GRAY,TERM_L_BLUE,TERM_L_UMBER,TERM_YELLOW,
    TERM_L_UMBER,TERM_L_GRAY,TERM_L_GRAY,TERM_L_GRAY,TERM_L_GRAY,
    TERM_L_BLUE,TERM_L_BLUE,TERM_UMBER,TERM_L_UMBER,TERM_L_UMBER,
    TERM_WHITE,TERM_GRAY,/*TERM_GRAY,TERM_WHITE,TERM_GRAY*/
};


/*
 * Another copy of the metal adjectives and colors, for rods
 * We do not want rods and wands to be identical, its too easy to "cheat".
 * However, the two arrays start out identical.  They are scrambled later.
 * We could actually copy them directly, but this lets us change the
 * bounds on the arrays, and change the distributions, etc.
 */

static cptr rod_adj[MAX_METALS] = {
    "Aluminum","Cast Iron","Chromium","Copper","Gold",
    "Iron","Magnesium","Molybdenum","Nickel","Rusty",
    "Silver","Steel","Tin","Titanium","Tungsten",
    "Zirconium","Zinc","Aluminum-Plated","Copper-Plated","Gold-Plated",
    "Nickel-Plated","Silver-Plated","Steel-Plated","Tin-Plated","Zinc-Plated",
    "Mithril-Plated","Mithril","Runed","Bronze","Brass",
    "Platinum","Lead"/*,"Lead-Plated","Ivory","Pewter"*/
};

static byte rod_col[MAX_METALS] = {
    TERM_L_BLUE,TERM_D_GRAY,TERM_WHITE,TERM_L_UMBER,TERM_YELLOW,
    TERM_GRAY,TERM_L_GRAY,TERM_L_GRAY,TERM_L_UMBER,TERM_RED,
    TERM_L_GRAY,TERM_L_GRAY,TERM_L_GRAY,TERM_WHITE,TERM_WHITE,
    TERM_L_GRAY,TERM_L_GRAY,TERM_L_BLUE,TERM_L_UMBER,TERM_YELLOW,
    TERM_L_UMBER,TERM_L_GRAY,TERM_L_GRAY,TERM_L_GRAY,TERM_L_GRAY,
    TERM_L_BLUE,TERM_L_BLUE,TERM_UMBER,TERM_L_UMBER,TERM_L_UMBER,
    TERM_WHITE,TERM_GRAY,/*TERM_GRAY,TERM_WHITE,TERM_GRAY*/
};


/*
 * Rock adjectives and colors, for rings
 */

static cptr ring_adj[MAX_ROCKS] = {
    "Alexandrite","Amethyst","Aquamarine","Azurite","Beryl",
    "Bloodstone","Calcite","Carnelian","Corundum","Diamond",
    "Emerald","Fluorite","Garnet","Granite","Jade",
    "Jasper","Lapis Lazuli","Malachite","Marble","Moonstone",
    "Onyx","Opal","Pearl","Quartz","Quartzite",
    "Rhodonite","Ruby","Sapphire","Tiger Eye","Topaz",
    "Turquoise","Zircon","Platinum","Bronze","Gold",
    "Obsidian","Silver","Tortoise Shell","Mithril","Jet",
    "Engagement","Adamantite"
};

static byte ring_col[MAX_ROCKS] = {
    TERM_GREEN,TERM_VIOLET,TERM_L_BLUE,TERM_L_BLUE,TERM_L_GREEN,
    TERM_RED,TERM_WHITE,TERM_RED,TERM_GRAY,TERM_WHITE,
    TERM_GREEN,TERM_L_GREEN,TERM_RED,TERM_L_GRAY,TERM_L_GREEN,
    TERM_UMBER,TERM_BLUE,TERM_GREEN,TERM_WHITE,TERM_L_GRAY,
    TERM_L_RED,TERM_L_GRAY,TERM_WHITE,TERM_L_GRAY,TERM_L_GRAY,
    TERM_L_RED,TERM_RED,TERM_BLUE,TERM_YELLOW,TERM_YELLOW,
    TERM_L_BLUE,TERM_L_UMBER,TERM_WHITE,TERM_L_UMBER,TERM_YELLOW,
    TERM_D_GRAY,TERM_L_GRAY,TERM_UMBER,TERM_L_BLUE,TERM_D_GRAY,
    TERM_YELLOW,TERM_L_GREEN
};


/*
 * Amulet adjectives and colors, for amulets
 */

static cptr amulet_adj[MAX_AMULETS] = {
    "Amber","Driftwood","Coral","Agate","Ivory",
    "Obsidian","Bone","Brass","Bronze","Pewter",
    "Tortoise Shell","Golden","Azure","Crystal","Silver",
    "Copper"
};

static byte amulet_col[MAX_AMULETS] = {
    TERM_YELLOW,TERM_L_UMBER,TERM_WHITE,TERM_L_GRAY,TERM_WHITE,
    TERM_D_GRAY,TERM_WHITE,TERM_L_UMBER,TERM_L_UMBER,TERM_GRAY,
    TERM_UMBER,TERM_YELLOW,TERM_L_BLUE,TERM_WHITE,TERM_L_GRAY,
    TERM_L_UMBER
};


/*
 * Syllables for scrolls
 */

static cptr syllables[MAX_SYLLABLES] = {
  "a","ab","ag","aks","ala","an","ankh","app",
  "arg","arze","ash","aus","ban","bar","bat","bek",
  "bie","bin","bit","bjor","blu","bot","bu",
  "byt","comp","con","cos","cre","dalf","dan",
  "den","der","doe","dok","eep","el","eng","er","ere","erk",
  "esh","evs","fa","fid","flit","for","fri","fu","gan",
  "gar","glen","gop","gre","ha","he","hyd","i",
  "ing","ion","ip","ish","it","ite","iv","jo",
  "kho","kli","klis","la","lech","man","mar",
  "me","mi","mic","mik","mon","mung","mur","nag","nej",
  "nelg","nep","ner","nes","nis","nih","nin","o",
  "od","ood","org","orn","ox","oxy","pay","pet",
  "ple","plu","po","pot","prok","re","rea","rhov",
  "ri","ro","rog","rok","rol","sa","san","sat",
  "see","sef","seh","shu","ski","sna","sne","snik",
  "sno","so","sol","sri","sta","sun","ta","tab",
  "tem","ther","ti","tox","trol","tue","turs","u",
  "ulk","um","un","uni","ur","val","viv","vly",
  "vom","wah","wed","werg","wex","whon","wun","x",
  "yerg","yp","zun","tri","blaa"
};


/*
 * Hold the titles of scrolls, ten characters each
 * Also keep an array of scroll colors, all WHITE for now
 */

static char scroll_adj[MAX_TITLES][10];

static byte scroll_col[MAX_TITLES];



/*
 * The "color"/"metal"/"type" of an item is its "flavor".
 *
 * Initialize descriptions for the "colored" objects, including:
 * Food, Scrolls, Potions, Wands, Staffs, Rods, Rings, Amulets.
 *
 */
void flavor_init(void)
{
    register int        h, i, j, k;
    register cptr		tmp1;
    vtype               string;


    /* Hack -- Play games with the R.N.G. */
    set_seed(randes_seed);

    /* The first 3 entries for potions are fixed */
    /* That is, slime mold juice, apple juice, water */
    for (i = 3; i < MAX_COLORS; i++) {
	j = rand_int(MAX_COLORS - 3) + 3;
	tmp1 = potion_adj[i];
	potion_adj[i] = potion_adj[j];
	potion_adj[j] = tmp1;
    }

    /* Woods are used for staffs */
    for (i = 0; i < MAX_WOODS; i++) {
	j = rand_int(MAX_WOODS);
	tmp1 = staff_adj[i];
	staff_adj[i] = staff_adj[j];
	staff_adj[j] = tmp1;
    }

    /* Wands are made of metal */
    for (i = 0; i < MAX_METALS; i++) {
	j = rand_int(MAX_METALS);
	tmp1 = wand_adj[i];
	wand_adj[i] = wand_adj[j];
	wand_adj[j] = tmp1;
    }

    /* Rocks are used for rings */
    for (i = 0; i < MAX_ROCKS; i++) {
	j = rand_int(MAX_ROCKS);
	tmp1 = ring_adj[i];
	ring_adj[i] = ring_adj[j];
	ring_adj[j] = tmp1;
    }

    /* Rocks are used for amulets */
    for (i = 0; i < MAX_AMULETS; i++) {
	j = rand_int(MAX_AMULETS);
	tmp1 = amulet_adj[i];
	amulet_adj[i] = amulet_adj[j];
	amulet_adj[j] = tmp1;
    }

    /* Hack -- Molds and Mushrooms (not normal foods) have colors */
    for (i = 0; i < MAX_SHROOM; i++) {
	j = rand_int(MAX_SHROOM);
	tmp1 = food_adj[i];
	food_adj[i] = food_adj[j];
	food_adj[j] = tmp1;
    }

    /* Hack -- Scrolls have titles, and are always white */
    for (h = 0; h < MAX_TITLES; h++) {
	string[0] = '\0';

	/* Construct a two or three word title */
	k = rand_range(2,3);
	for (i = 0; i < k; i++) {

	    /* Add a one or two syllable word */
	    for (j = rand_range(1,2); j > 0; j--) {
		(void)strcat(string, syllables[rand_int(MAX_SYLLABLES)]);
	    }

	    /* Add a space */
	    if (i < k - 1) {
		(void)strcat(string, " ");
	    }
	}

	/* Hack -- chop off part of the title */
	if (string[8] == ' ') {
	    string[8] = '\0';
	}
	else if (string[7] == ' ') {
	    string[7] = '\0';
	}
	else {
	    string[9] = '\0';
	}

	/* Save the title */
	(void)strcpy(scroll_adj[h], string);
    }
    reset_seed();
}

s16b flavor_p(inven_type *t_ptr)
{
    switch (t_ptr->tval) {
      case TV_ROD:
	return (7);		   /* -CFT */
      case TV_AMULET:
	return (0);
      case TV_RING:
	return (1);
      case TV_STAFF:
	return (2);
      case TV_WAND:
	return (3);
      case TV_SCROLL1:
      case TV_SCROLL2:
	return (4);
      case TV_POTION1:
      case TV_POTION2:
	return (5);
      case TV_FOOD:
	if ((t_ptr->sval & (ITEM_SINGLE_STACK_MIN - 1)) < MAX_SHROOM)
	    return (6);
	return (-1);
      default:
	return (-1);
    }
}


/*
 * Is a given item "fully identified"?
 */
int known2_p(inven_type *i_ptr)
{
    return (i_ptr->ident & ID_KNOWN);
}



/*
 * Remove "Secret" symbol for identity of plusses.
 */
void known2(inven_type *i_ptr)
{
    /* Remove an automatically generated inscription.	-CJS- */
    /* used to clear ID_DAMD flag, but I think it should remain set */
    i_ptr->ident &= ~ID_MAGIK;

    /* Clear the "Empty" info */
    i_ptr->ident &= ~ID_EMPTY;

    /* Now we know all about it */
    i_ptr->ident |= ID_KNOWN;
}




/*
 */
int inven_aware_p(inven_type *i_ptr)
{
    s16b offset;
    byte indexx;

/* Items which don't have a 'color' are always known, so that they can be
 * carried in order in the inventory.  
 */
    if ((offset = flavor_p(i_ptr)) < 0)
	return (TRUE);
    if (store_bought_p(i_ptr))
	return (TRUE);
    offset <<= 6;
    indexx = i_ptr->sval & (ITEM_SINGLE_STACK_MIN - 1);

    /* Check the "x_list" */
    return (x_list[offset + indexx].aware);
}


/*
 * Remove "Secret" symbol for identity of object.
 */
void inven_aware(inven_type *i_ptr)
{
    s16b offset;
    byte indexx;

    if ((offset = flavor_p(i_ptr)) < 0)
	return;
    offset <<= 6;
    indexx = i_ptr->sval & (ITEM_SINGLE_STACK_MIN - 1);
    /* Fully aware of the effects */
    x_list[offset + indexx].aware = TRUE;
    /* clear the tried flag, since it is now known */
    x_list[offset + indexx].tried = FALSE;
}


void store_bought(inven_type *i_ptr)
{
    i_ptr->ident |= ID_STOREBOUGHT;
    known2(i_ptr);
}


int store_bought_p(inven_type *i_ptr)
{
    return (i_ptr->ident & ID_STOREBOUGHT);
}



/* 
 * Somethings been sampled -CJS-
 */
void inven_tried(inven_type *i_ptr)
{
    s16b offset;
    byte indexx;

    if ((offset = flavor_p(i_ptr)) < 0)
	return;
    offset <<= 6;
    indexx = i_ptr->sval & (ITEM_SINGLE_STACK_MIN - 1);
    /* Mark it as tried (even if "aware") */
    x_list[offset + indexx].tried = TRUE;
}

/*
 * Somethings been identified					 
 *
 * extra complexity by CJS so that it can merge store/dungeon objects when
 * appropriate 
 */
void identify(int *item)
{
    register int         i, x1, x2;
    int                  j;
    register inven_type *i_ptr, *t_ptr;

    i_ptr = &inventory[*item];

    if ((i_ptr->flags3 & TR3_CURSED) && (i_ptr->tval != TV_MAGIC_BOOK) &&
	(i_ptr->tval != TV_PRAYER_BOOK))
	add_inscribe(i_ptr, ID_DAMD);

    if (!inven_aware_p(i_ptr)) {
	inven_aware(i_ptr);
	x1 = i_ptr->tval;
	x2 = i_ptr->sval;
	if (x2 < ITEM_SINGLE_STACK_MIN || x2 >= ITEM_GROUP_MIN)
	/* no merging possible */
	    ;
	else
	    for (i = 0; i < inven_ctr; i++) {
		t_ptr = &inventory[i];
		if (t_ptr->tval == x1 && t_ptr->sval == x2 && i != *item
		    && ((int)t_ptr->number + (int)i_ptr->number < 256)) {
		/* make *item the smaller number */
		    if (*item > i) {
			j = *item;
			*item = i;
			i = j;
		    }
		    msg_print("You combine similar objects from the shop and dungeon.");

		    inventory[*item].number += inventory[i].number;
		    inven_ctr--;
		    for (j = i; j < inven_ctr; j++)
			inventory[j] = inventory[j + 1];
		    invcopy(&inventory[j], OBJ_NOTHING);
		}
	    }
    }
}










/*
 * defines for pval_use, determine how the pval field is printed 
 */
#define IGNORED     0		/* never show (+x) */
#define Z_PLUSSES   1		/* always show pval as (+x), even if x==0 -CWS */
#define PLUSSES     2		/* show pval as (+x) only */
#define FLAGS       3		/* show pval as (+x of yyy) */
#define CHARGES     5		/* show pval as charges */
#define USE_LITE    7		/* show pval as turns of light */


/*
 * Creates a description of the item "i_ptr", and stores it in "out_val".
 *
 * If "pref" is TRUE, the description is verbose, and has an article (or number,
 * or "no more") prefixed to the description.
 *
 * Note that since out_val can easily exceed 80 characters, objdes must
 * always be called with a bigvtype as the first paramter 
 *****
 * Note that objdes now never returns a description ending with punctuation
 * (ie, "."'s) -CWS 
 */
void objdes(char *out_val, inven_type *i_ptr, int pref)
{
    /* base name, modifier string */
    register cptr basenm, modstr;
    int modify, append_name;
    int indexx, pval_use;
    vtype                tmp_str, damstr;
    bigvtype             tmp_val;

    /* Hack -- Extract the sub-type "indexx" */
    indexx = i_ptr->sval & (ITEM_SINGLE_STACK_MIN - 1);

    /* Extract the (default) "base name" */
    basenm = k_list[i_ptr->k_idx].name;

    /* Assume we will NOT append the "kind" name */
    append_name = FALSE;

    /* Assume no modifier string */
    modstr = NULL;

    /* Start with no damage string */
    damstr[0] = '\0';

    /* Assume no display of "pval" */
    pval_use = IGNORED;

    modify = (inven_aware_p(i_ptr) ? FALSE : TRUE);

    /* Analyze the object */
    switch (i_ptr->tval) {

      /* Some objects are easy to describe */
      case TV_MISC:
      case TV_SPIKE:
      case TV_FLASK:
	break;

      case TV_CHEST:

	break;


      /* Weapons have a damage string, and flags */
      case TV_HAFTED:
      case TV_POLEARM:
      case TV_SWORD:
	(void)sprintf(damstr, " (%dd%d)", i_ptr->dd, i_ptr->ds);
	pval_use = FLAGS;
	break;
      case TV_DIGGING:
	pval_use = Z_PLUSSES;
	(void)sprintf(damstr, " (%dd%d)", i_ptr->dd, i_ptr->ds);
	break;

      /* Bows get a special "damage string" */
      case TV_BOW:
	switch(i_ptr->sval) { /* whole new code -CFT */
	  case 20: case 1: /* sling, sh. bow */
	    strcpy(damstr, " (x2)");
	    break;
	  case 21: case 2: case 10: /* sling of M, s bow of M, l bow, l xbow */
	    strcpy(damstr, " (x3)");
	    break;
	  case 3: case 11: /* l bow of M, l xbow of M, h xbow, BARD, CUBRAGOL */
	    strcpy(damstr, " (x4)");
	    break;
	  case 4: case 12:        /* h xbow of M, BELEG */
	    strcpy(damstr, " (x5)");
	    break;
	  default:        /* just in case... */
	    strcpy(damstr, " (unknown mult.)");
	}
	if (i_ptr->flags2 & TR_ARTIFACT)	/* only show pval for artifacts... */
	    pval_use = FLAGS;
	break;


      /* Missiles have a damage string */
      case TV_SHOT:
      case TV_BOLT:
      case TV_ARROW:
	(void)sprintf(damstr, " (%dd%d)", i_ptr->dd, i_ptr->ds);
	break;


      /* Armour uses flags */
      case TV_BOOTS:
      case TV_GLOVES:
      case TV_CLOAK:
      case TV_HELM:
      case TV_SHIELD:
      case TV_SOFT_ARMOR:
      case TV_HARD_ARMOR:
	pval_use = FLAGS;
	break;


      /* Lites (including a few "Specials") */
      case TV_LITE:
	pval_use = LIGHT;
	if (!stricmp("The Phial of Galadriel", basenm) && !known2_p(i_ptr))
	    basenm = "a Shining Phial";
	if (!stricmp("The Star of Elendil", basenm) && !known2_p(i_ptr))
	    basenm = "a Shining Gem";
	if (!stricmp("The Arkenstone of Thrain", basenm) && !known2_p(i_ptr))
	    basenm = "a Shining Gem";
	break;


      /* Amulets (including a few "Specials") */
      case TV_AMULET:

	pval_use = FLAGS;

	if (modify || !(plain_descriptions || store_bought_p(i_ptr))) {
	    basenm = "& %s Amulet";
	    modstr = amulet_adj[indexx];
	    if (!modify)
	    append_name = TRUE;
	}
	else {
	    basenm = "& Amulet";
	    append_name = TRUE;
	}
	break;


      /* Rings (including a few "Specials") */
      case TV_RING:

	pval_use = PLUSSES;

	if (!stricmp("Power", basenm)) { /* name this "the One Ring" -CWS */
	    append_name = FALSE;
	    if (!known2_p(i_ptr)) basenm = "a plain gold Ring";
	    else basenm = "The One Ring";
	} else if (modify || !(plain_descriptions || store_bought_p(i_ptr))) {
	    basenm = "& %s Ring";
	    modstr = ring_adj[indexx];
	    if (!modify)
	    append_name = TRUE;
	}
	else {
	    basenm = "& Ring";
	    append_name = TRUE;
	}
	break;

      case TV_STAFF:
	pval_use = CHARGES;
	if (modify || !(plain_descriptions || store_bought_p(i_ptr))) {
	    basenm = "& %s Staff";
	    modstr = staff_adj[indexx];
	    if (!modify)
	    append_name = TRUE;
	}
	else {
	    basenm = "& Staff";
	    append_name = TRUE;
	}
	break;

      case TV_WAND:
	pval_use = CHARGES;
	if (modify || !(plain_descriptions || store_bought_p(i_ptr))) {
	    basenm = "& %s Wand";
	    modstr = wand_adj[k_idxx];
	    if (!modify)
	    append_name = TRUE;
	}
	else {
	    basenm = "& Wand";
	    append_name = TRUE;
	}
	break;

      case TV_ROD:
	if (modify || !(plain_descriptions || store_bought_p(i_ptr))) {
	    basenm = "& %s Rod";
	    modstr = wand_adj[k_idxx];
	    if (!modify)
	    append_name = TRUE;
	}
	else {
	    basenm = "& Rod";
	    append_name = TRUE;
	}
	break;

      case TV_SCROLL1:
      case TV_SCROLL2:
	if (modify || !(plain_descriptions || store_bought_p(i_ptr))) {
	    basenm = "& Scroll~ titled \"%s\"";
	    modstr = scroll_adj[indexx];
	    if (!modify)
	    append_name = TRUE;
	}
	else {
	    basenm = "& Scroll~";
	    append_name = TRUE;
	}
	break;

      case TV_POTION1:
      case TV_POTION2:
	if (modify || !(plain_descriptions || store_bought_p(i_ptr))) {
	    basenm = "& %s Potion~";
	    modstr = potion_adj[indexx];
	    if (!modify)
	    append_name = TRUE;
	}
	else {
	    basenm = "& Potion~";
	    append_name = TRUE;
	}
	break;

      case TV_FOOD:

	if (modify || !(plain_descriptions || store_bought_p(i_ptr))) {
	    if (!modify)
		append_name = TRUE;
	    if (indexx <= 15)
		basenm = "& %s Mushroom~";
	    else if (indexx <= 20)
		basenm = "& Hairy %s Mold~";
	    else
		append_name = FALSE;	/* Ordinary food has no name appended. */
	    if (indexx <= 20)
		modstr = food_adj[indexx];
	}
	else {
	    append_name = TRUE;
	    if (indexx <= 15) {
		basenm = "& Mushroom~";
	    }
	    else if (indexx <= 20) {
		basenm = "& Hairy Mold~";
	    }
	    else {
	    /* Ordinary food does not have a name appended.  */
		append_name = FALSE;
	    }
	}
	break;


      /* Magic Books */
      case TV_MAGIC_BOOK:
	modstr = basenm;
	basenm = "& Book~ of Magic Spells %s";
	break;

      /* Prayer Books */
      case TV_PRAYER_BOOK:
	modstr = basenm;
	basenm = "& Holy Book~ of Prayers %s";
	break;


      /* Things in the dungeon */
      case TV_OPEN_DOOR:
      case TV_CLOSED_DOOR:
      case TV_SECRET_DOOR:
      case TV_RUBBLE:
	break;
      case TV_GOLD:
      case TV_INVIS_TRAP:
      case TV_VIS_TRAP:
      case TV_UP_STAIR:
      case TV_DOWN_STAIR:
	(void)strcpy(out_val, k_list[i_ptr->k_idx].name);
    /* (void) strcat(out_val, "."); avoid ".." bug -CWS */
	return;

      case TV_STORE_DOOR:
	sprintf(out_val, "the entrance to the %s", k_list[i_ptr->k_idx].name);
	return;

      /* Used in the "inventory" routine */
      default:
	strcpy(out_val, "Error in objdes()");
	return;
    }


    /* Insert the modifier */
    if (modstr) {
	sprintf(tmp_val, basenm, modstr);
    }
    else {
	strcpy(tmp_val, basenm);
    }


    /* Append the "kind name" to the "base name" */
    if (append_name) {
	(void)strcat(tmp_val, " of ");
	(void)strcat(tmp_val, k_list[i_ptr->k_idx].name);
    }


    /* Attempt to pluralize somewhat correctly */
    if (i_ptr->number != 1) {
	insert_str(tmp_val, "ch~", "ches");
	insert_str(tmp_val, "~", "s");
    }

    /* Delete the plural, if any */
    else {
	insert_str(tmp_val, "~", NULL);
    }


    /* Hack -- No prefixes requested */
    if (!pref) {

	if (!strncmp("some", tmp_val, 4))
	    (void)strcpy(out_val, &tmp_val[5]);
	else if (tmp_val[0] == '&')
	/* eliminate the '& ' at the beginning */
	    (void)strcpy(out_val, &tmp_val[2]);
	else
	    (void)strcpy(out_val, tmp_val);
    } else {

    /* Hack -- Append "Artifact" or "Special" names */
    if (known2_p(i_ptr)) {

	/* Hack -- grab the artifact name */
	if (i_ptr->name1) {
	    (void)strcat(tmp_val, " ");
	    (void)strcat(tmp_val, v_list[i_ptr->name1].name);
	}

	/* Otherwise, grab the "ego-item" name */
	else if (i_ptr->name2) {
	    (void)strcat(tmp_val, " ");
	    (void)strcat(tmp_val, ego_names[i_ptr->name2]);
	}
    }

    /* Append the "damage info", if any */
    if (damstr[0]) {
	(void)strcat(tmp_val, damstr);
    }


    /* We know it, describe it */	
    if (known2_p(i_ptr)) {

	/* originally used %+d, but several machines don't support it */
	if (i_ptr->flags3 & TR3_SHOW_MODS) {
	    (void)sprintf(tmp_str, " (%c%d,%c%d)",
			  (i_ptr->tohit < 0) ? '-' : '+', MY_ABS( i_ptr->tohit),
			 (i_ptr->todam < 0) ? '-' : '+', MY_ABS(i_ptr->todam));
	}

	/* Show the tohit if needed */
	else if (i_ptr->tohit ) {
	    (void)sprintf(tmp_str, " (%c%d)",
			 (i_ptr->tohit < 0) ? '-' : '+', MY_ABS(i_ptr->tohit));
	}

	/* Show the todam if needed */
	else if (i_ptr->todam != 0) {
	    (void)sprintf(tmp_str, " (%c%d)",
			 (i_ptr->todam < 0) ? '-' : '+', MY_ABS(i_ptr->todam));
	}
	else tmp_str[0] = '\0';
	(void)strcat(tmp_val, tmp_str);
    }

    /* Hack -- Crowns have a zero base AC, so make a special test for them. */
    if (i_ptr->ac || (i_ptr->tval == TV_HELM)) {
	    (void)sprintf(tmp_str, " [%d", i_ptr->ac);
	    (void)strcat(tmp_val, tmp_str);
	    if (known2_p(i_ptr)) {
	    /* originally used %+d, but several machines don't support it */
		(void)sprintf(tmp_str, ",%c%d",
			   (i_ptr->toac < 0) ? '-' : '+', MY_ABS(i_ptr->toac));
		(void)strcat(tmp_val, tmp_str);
	    }
	    (void)strcat(tmp_val, "]");
	} else if ((i_ptr->toac != 0) && known2_p(i_ptr)) {
	/* originally used %+d, but several machines don't support it */
	    (void)sprintf(tmp_str, " [%c%d]",
			  (i_ptr->toac < 0) ? '-' : '+', MY_ABS(i_ptr->toac));
	    (void)strcat(tmp_val, tmp_str);
	}

	tmp_str[0] = '\0';

    /* override defaults, check for pval flags in the ident field */
	if (pval_use != IGNORED) {
	    if (pval_use == LIGHT);
	    else if (i_ptr->ident & ID_NOSHOW_P1)
		pval_use = IGNORED;
	    else if (i_ptr->ident & TR3_HIDE_TYPE)
		pval_use = PLUSSES;
	}

	if (pval_use == IGNORED);
	else if ((pval_use == LIGHT) && !(i_ptr->flags2 & TR_ARTIFACT))
	    (void)sprintf(tmp_str, " with %d turns of light", i_ptr->pval);

	else if (known2_p(i_ptr)) {

	    if (pval_use == CHARGES)
		(void)sprintf(tmp_str, " (%d charge%s", i_ptr->pval,
			      (i_ptr->pval == 1 ? ")" : "s)"));

	    else if (pval_use == Z_PLUSSES) /* (+0) digging implements -CWS */
		    (void)sprintf(tmp_str, " (%c%d)",
				  (i_ptr->pval < 0) ? '-' : '+', MY_ABS(i_ptr->pval));

	    else if (i_ptr->pval != 0) {
		if (pval_use == PLUSSES)
		    (void)sprintf(tmp_str, " (%c%d)",
				  (i_ptr->pval < 0) ? '-' : '+', MY_ABS(i_ptr->pval));
		else if (i_ptr->ident & TR3_HIDE_TYPE)
		    (void)sprintf(tmp_str, " (%c%d)",
				  (i_ptr->pval < 0) ? '-' : '+', MY_ABS(i_ptr->pval));

		else if (pval_use == FLAGS) {
		    if ((i_ptr->flags1 & TR1_SPEED) &&
			     (i_ptr->name2 != EGO_SPEED))
			(void)sprintf(tmp_str, " (%c%d to speed)",
				      (i_ptr->pval < 0) ? '-' : '+', MY_ABS(i_ptr->pval));
		    else if (i_ptr->flags1 & TR1_SEARCH)
			/*			&& (i_ptr->name2 != EGO_SLAY_EARCH)) */
			(void)sprintf(tmp_str, " (%c%d to searching)",
				      (i_ptr->pval < 0) ? '-' : '+', MY_ABS(i_ptr->pval));
		    else if ((i_ptr->flags1 & TR1_STEALTH) &&
			     (i_ptr->name2 != EGO_STEALTH))
			(void)sprintf(tmp_str, " (%c%d to stealth)",
				      (i_ptr->pval < 0) ? '-' : '+', MY_ABS(i_ptr->pval));
		    else if ((i_ptr->flags1 & TR1_INFRA) &&
			     (i_ptr->name2 != EGO_INFRAVISION))
			(void)sprintf(tmp_str, " (%c%d to infravision)",
				      (i_ptr->pval < 0) ? '-' : '+', MY_ABS(i_ptr->pval));
		    else if (i_ptr->flags2 & TR1_ATTACK_SPD) {
			if (MY_ABS(i_ptr->pval) == 1)
			    (void)sprintf(tmp_str, " (%c%d attack)",
					  (i_ptr->pval < 0) ? '-' : '+', MY_ABS(i_ptr->pval));
			else
			    (void)sprintf(tmp_str, " (%c%d attacks)",
					  (i_ptr->pval < 0) ? '-' : '+', MY_ABS(i_ptr->pval));
		    } /* attack speed */
		    else
			(void)sprintf(tmp_str, " (%c%d)",
				      (i_ptr->pval < 0) ? '-' : '+', MY_ABS(i_ptr->pval));
		}     /* pval_use == FLAGS */
	    }         /* pval != 0 */
	}             /* if known2_p (fully identified) */

	(void)strcat(tmp_val, tmp_str);

    /* ampersand is always the first character */
	if (tmp_val[0] == '&') {
	/* use &tmp_val[1], so that & does not appear in output */
	    if (i_ptr->number > 1)
		(void)sprintf(out_val, "%d%s", (int)i_ptr->number, &tmp_val[1]);
	    else if (i_ptr->number < 1)
		(void)sprintf(out_val, "%s%s", "no more", &tmp_val[1]);
	    else if (known2_p(i_ptr) && (i_ptr->tval >= TV_MIN_WEAR)
		     && (i_ptr->tval <= TV_MAX_WEAR) &&
		     (i_ptr->flags2 & TR_ARTIFACT))
		(void)sprintf(out_val, "The%s", &tmp_val[1]);
	    else if (is_a_vowel(tmp_val[2]))
		(void)sprintf(out_val, "an%s", &tmp_val[1]);
	    else
		(void)sprintf(out_val, "a%s", &tmp_val[1]);
	}
    /* handle 'no more' case specially */
	else if (i_ptr->number < 1) {
	/* check for "some" at start */
	    if (!strncmp("some", tmp_val, 4))
		(void)sprintf(out_val, "no more %s", &tmp_val[5]);
	/* here if no article */
	    else
		(void)sprintf(out_val, "no more %s", tmp_val);
	} else
	    (void)strcpy(out_val, tmp_val);

	tmp_str[0] = '\0';
	if ((indexx = flavor_p(i_ptr)) >= 0) {
	    indexx = (indexx <<= 6) +
		(i_ptr->sval & (ITEM_SINGLE_STACK_MIN - 1));
	/* don't print tried string for store bought items */
	    if ((x_list[indexx].tried) && !store_bought_p(i_ptr))
		(void)strcat(tmp_str, "tried ");
	}
	if ((i_ptr->ident & (ID_MAGIK | ID_EMPTY | ID_DAMD)) &&
	    i_ptr->tval != TV_MAGIC_BOOK && i_ptr->tval != TV_PRAYER_BOOK) {
	    if (i_ptr->ident & ID_MAGIK)
		(void)strcat(tmp_str, "blessed ");
	    if (i_ptr->ident & ID_EMPTY)
		(void)strcat(tmp_str, "empty ");
	    if (i_ptr->ident & ID_DAMD)
		(void)strcat(tmp_str, "cursed ");
	}
	if ((known2_p(i_ptr) || store_bought_p(i_ptr)) &&
	    ((!strncmp(i_ptr->inscrip, "average", 7)) ||
	     (!strncmp(i_ptr->inscrip, "good", 4)) ||
	     (!strncmp(i_ptr->inscrip, "excellent", 9)) ||
	     (!strncmp(i_ptr->inscrip, "special", 7))))
	    i_ptr->inscrip[0] = '\0';
	if (i_ptr->inscrip[0] != '\0')
	    (void)strcat(tmp_str, i_ptr->inscrip);
	else if ((indexx = strlen(tmp_str)) > 0)
	/* remove the extra blank at the end */
	    tmp_str[indexx - 1] = '\0';
	if (tmp_str[0]) {
	    (void)sprintf(tmp_val, " {%s}", tmp_str);
	    (void)strcat(out_val, tmp_val);
	}
    /* (void) strcat(out_val, "."); avoid ".." bug -CWS */
    }
}




/*
 * Make "i_ptr" a "clean" copy of the given "kind" of object
 */
void invcopy(inven_type *i_ptr, int k_idx)
{
    register inven_kind *k_ptr;

    /* Get the object template */
    k_ptr = &k_list[k_idx];

    /* Save the kind k_idx */
    i_ptr->k_idx = k_idx;

    /* Quick access to tval/sval */
    i_ptr->tval = k_ptr->tval;
    i_ptr->sval = k_ptr->subval;

    i_ptr->tchar = k_ptr->tchar;

    /* Save the default "pval" */
    i_ptr->pval = k_ptr->pval;

    /* Default number and weight */
    i_ptr->number = k_ptr->number;
    i_ptr->weight = k_ptr->weight;

    /* Default magic */
    i_ptr->tohit = k_ptr->tohit;
    i_ptr->todam = k_ptr->todam;
    i_ptr->toac = k_ptr->toac;
    i_ptr->ac = k_ptr->ac;
    i_ptr->dd = k_ptr->damage[0];
    i_ptr->ds = k_ptr->damage[1];

    /* Default cost and flags */
    i_ptr->cost = k_ptr->cost;
    i_ptr->flags1 = k_ptr->flags1;
    i_ptr->flags2 = k_ptr->flags2;
    i_ptr->flags3 = k_ptr->flags3;

    /* Wipe the inscription */
    i_ptr->inscrip[0] = '\0';

    /* No artifact name */
    i_ptr->name1 = 0;

    /* No special name */
    i_ptr->name2 = 0;

    /* No ident info yet */
    i_ptr->ident = 0;

    i_ptr->level = k_ptr->level;
}






