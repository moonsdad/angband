
#define NO_RES 0
#define SOME_RES 1
#define RESIST 2
#define IMMUNE 3
#define SUSCEPT 4
#define CHANGED 5
#define CONFUSED 6
#define MORE_CONF 7
#define DAZED 8
#define MORE_DAZED 16
#define DEAD 32

/*
 * This function will process a bolt/ball/breath spell hitting a monster.
 * It checks for resistances, and reduces damage accordingly, and also
 * adds in what "special effects" apply to the monsters.  'rad' is used to
 * indicate the distance from "ground 0" for ball spells.  For bolts, rad
 * should be a 0 (this flags off some of the messages).  dam is changed
 * to reflect resistances and range. -CFT
 */

static void spell_hit_monster(monster_type *m_ptr, int typ, int *dam, int rad, int *y, int *x, byte by_player)
{
    register monster_race *r_ptr;
    int blind = (p_ptr->flags.status & PY_BLIND) ? 1 : 0;
    int res;			/* controls messages, using above #defines -CFT */
    vtype cdesc, outval;

    if (rad)
	*dam /= rad;		/* adjust damage for range... */

    *y = m_ptr->fy;		/* these only change if mon gets teleported */
    *x = m_ptr->fx; 
    r_ptr = &r_list[m_ptr->r_idx];
    if (m_ptr->ml){
	if (r_ptr->cflags2 & MF2_UNIQUE)
	    sprintf(cdesc, "%s ", r_ptr->name);
	else
	    sprintf(cdesc, "The %s ", r_ptr->name);
    }
    else
	strcpy(cdesc, "It ");

    res = NO_RES;		/* assume until we know different -CFT */
    switch ( typ ){		/* check for resists... */
      case GF_MISSILE:	/* pure damage, no resist possible */
	break;
      case GF_ELEC:
	if (r_ptr->cflags2 & MF2_IM_ELEC) {
	    res = RESIST;
	    *dam /= 9;
	    if (m_ptr->ml)
		l_list[m_ptr->r_idx].r_cflags2 |= MF2_IM_ELEC;
        }
	break;
      case GF_POIS:
	if (r_ptr->cflags2 & MF2_IM_POIS) {
	    res = RESIST;
	    *dam /= 9;
	    if (m_ptr->ml)
		l_list[m_ptr->r_idx].r_cflags2 |= MF2_IM_POIS;
        }
	break;
      case GF_ACID:
	if (r_ptr->cflags2 & MF2_IM_ACID) {
	    res = RESIST;
	    *dam /= 9;
	    if (m_ptr->ml)
		l_list[m_ptr->r_idx].r_cflags2 |= MF2_IM_ACID;
        }
	break;
      case GF_COLD:
	if (r_ptr->cflags2 & MF2_IM_COLD) {
	    res = RESIST;
	    *dam /= 9;
	    if (m_ptr->ml)
		l_list[m_ptr->r_idx].r_cflags2 |= MF2_IM_COLD;
        }
	break;
      case GF_FIRE:
	if (r_ptr->cflags2 & MF2_IM_FIRE) {
	    res = RESIST;
	    *dam /= 9;
	    if (m_ptr->ml)
		l_list[m_ptr->r_idx].r_cflags2 |= MF2_IM_FIRE;
        }
	break;
      case GF_HOLY_ORB:
	if (r_ptr->cflags2 & MF2_EVIL) {
	    *dam *= 2;
	    res = SUSCEPT;
	    if (m_ptr->ml)
		l_list[m_ptr->r_idx].r_cflags2 |= MF2_EVIL;
        }
	break;
      case GF_ARROW:		/* for now, no defense... maybe it should have a
				   chance of missing? -CFT */
	break;
      case GF_PLASMA:		/* maybe MF2_IM_ELEC (ball lightning is supposed
				   to be plasma) or MF2_IM_FIRE (since it's hot)? -CFT */
	if (!strncmp("Plasma", r_ptr->name, 6) ||
	    (r_ptr->spells3 & MS3_BR_PLAS)){ /* if is a "plasma" monster,
					      or can breathe plasma, then
					      we assume it should be immune.
					      plasma bolts don't count, since
					      mage-types could have them, and
					      not deserve plasma-resist -CFT */
	    res = RESIST;
	    *dam *= 3;		/* these 2 lines give avg dam of .33, ranging */
	    *dam /= (randint(6)+6); /* from .427 to .25 -CFT */
	}
	break;
      case GF_NETHER:		/* I assume nether is an evil, necromantic force,
				   so it doesn't hurt undead, and hurts evil less -CFT */
	if (r_ptr->cflags2 & MF2_UNDEAD) {
	    res = IMMUNE;
	    *dam = 0;
	    if (m_ptr->ml)
		l_list[m_ptr->r_idx].r_cflags2 |= MF2_UNDEAD;
        }
	else if (r_ptr->spells2 & MS2_BR_LIFE) { /* if can breath nether, should get
						  good resist to damage -CFT */
	    res = RESIST;
	    *dam *= 3;  /* these 2 lines give avg dam of .33, ranging */
	    *dam /= (randint(6)+6); /* from .427 to .25 -CFT */
	}
	else if (r_ptr->cflags2 & MF2_EVIL) {
	    *dam /= 2;	/* evil takes *2 for holy, so /2 for this... -CFT */
	    res = SOME_RES;
	    if (m_ptr->ml)
		l_list[m_ptr->r_idx].r_cflags2 |= MF2_EVIL;
        }
	break;
      case GF_WATER:	/* water elementals should resist.  anyone else? -CFT */
	if ((r_ptr->r_char == 'E') && (r_ptr->name[0] == 'W')){
	    res = IMMUNE;
	    *dam = 0; /* water spirit, water ele, and Waldern -CFT */
        }
	break;
      case GF_CHAOS:
	if (r_ptr->spells2 & MS2_BR_CHAO){ /* assume anything that breathes
					    choas is chaotic enough to deserve resistance... -CFT */
	    res = RESIST;
	    *dam *= 3;  /* these 2 lines give avg dam of .33, ranging */
	    *dam /= (randint(6)+6); /* from .427 to .25 -CFT */
        }
	if ((*dam <= m_ptr->hp) && /* don't bother if it's gonna die */
	    !(r_ptr->spells2 & MS2_BR_CHAO) &&
	    !(r_ptr->cflags2 & MF2_UNIQUE) &&
	    (randint(90) > r_ptr->level)) { /* then we'll polymorph it -CFT */
	    res = CHANGED;
	    if (poly(cave[*y][*x].cptr))
		*dam = 0; /* new monster was not hit by choas breath.  This also
			     makes things easier to handle */
	} /* end of choas-poly.  If was poly-ed don't bother confuse... it's
	     too hectic to keep track of... -CFT */
	else if (!(r_ptr->cflags2 & MF2_CHARM_SLEEP) &&
		 !(r_ptr->spells2 & MS2_BR_CHAO) && /* choatics hard to confuse */
		 !(r_ptr->spells2 & MS2_BR_CONF)){   /* so are bronze dragons */
	    if (m_ptr->confused > 0) { 
		res = MORE_CONF;
		if (m_ptr->confused < 240){ /* make sure not to overflow -CFT */
		    m_ptr->confused += 7/(rad>0 ? rad : 1);
		}
	    }
	    else {
		res = CONFUSED;
		m_ptr->confused = (randint(11)+5)/(rad>0 ? rad : 1);
	    }
	}
	break;
      case GF_SHARDS:
	if (r_ptr->spells2 & MS2_BR_SHAR){ /* shard breathers resist -CFT */
	    res = RESIST;
	    *dam *= 3;  /* these 2 lines give avg dam of .33, ranging */
	    *dam /= (randint(6)+6); /* from .427 to .25 -CFT */
        }
	break;
      case GF_SOUND:
      if (r_ptr->spells2 & MS2_BR_SOUN){ /* ditto for sound -CFT */
	  res = RESIST;
	  *dam *= 2;
	  *dam /= (randint(6)+6);
      }
	if ((*dam <= m_ptr->hp) && /* don't bother if it's dead */
	    !(r_ptr->spells2 & MS2_BR_SOUN) &&
	    !(r_ptr->spells3 & MS3_BR_WALL)) { /* sound and impact breathers
	  					should not stun -CFT */
	    if (m_ptr->confused > 0) { 
		res = MORE_DAZED;
		if (m_ptr->confused < 220){ /* make sure not to overflow -CFT */
		    m_ptr->confused += (randint(5)*2)/(rad>0 ? rad : 1);
		}
	    }
	    else {
		res = DAZED;
		m_ptr->confused = (randint(15)+10)/(rad>0 ? rad : 1);
	    }
  	}
	break;
      case GF_CONFUSION:
	if (r_ptr->spells2 & MS2_BR_CONF){ 
	    res = RESIST;
	    *dam *= 2;
	    *dam /= (randint(6)+6);
        }
	else if (r_ptr->cflags2 & MF2_CHARM_SLEEP){
	    res = SOME_RES;
	    *dam /= 2; /* only some resist, but they also avoid confuse -CFT */
        }
	if ((*dam <= m_ptr->hp) && /* don't bother if it's dead */
	    !(r_ptr->cflags2 & MF2_CHARM_SLEEP) &&
	    !(r_ptr->spells2 & MS2_BR_CHAO) && /* choatics hard to confuse */
	    !(r_ptr->spells2 & MS2_BR_CONF)) {  /* so are bronze dragons */
	    if (m_ptr->confused > 0) { 
		res = MORE_CONF;
		if (m_ptr->confused < 240){ /* make sure not to overflow -CFT */
		    m_ptr->confused += 7/(rad>0 ? rad : 1);
		}
	    }
	    else {
		res = CONFUSED;
		m_ptr->confused = (randint(11)+5)/(rad>0 ? rad : 1);
	    }
	}
        break;
      case GF_DISENCHANT:
	if ((r_ptr->spells2 & MS2_BR_DISE) ||
	    !strncmp("Disen", r_ptr->name, 5)) {
	    res = RESIST;
	    *dam *= 3;  /* these 2 lines give avg dam of .33, ranging */
	    *dam /= (randint(6)+6); /* from .427 to .25 -CFT */
        }
	break;
      case GF_NEXUS:
	if ((r_ptr->spells2 & MS2_BR_NETH) ||
	    !strncmp("Nexus", r_ptr->name, 5)) {
	    res = RESIST;
	    *dam *= 3;  /* these 2 lines give avg dam of .33, ranging */
	    *dam /= (randint(6)+6); /* from .427 to .25 -CFT */
        }
	break;
      case GF_FORCE:
	if (r_ptr->spells3 & MS3_BR_WALL){ /* breath ele force resists
					    ele force -CFT */
	    res = RESIST;
	    *dam *= 3;  /* these 2 lines give avg dam of .33, ranging */
	    *dam /= (randint(6)+6); /* from .427 to .25 -CFT */
        }
	if ((*dam <= m_ptr->hp) &&
	    !(r_ptr->spells2 & MS2_BR_SOUN) &&
	    !(r_ptr->spells3 & MS3_BR_WALL)){ /* sound and impact breathers
					       should not stun -CFT */
	    if (m_ptr->confused > 0) { 
		res = MORE_DAZED;
		if (m_ptr->confused < 220){ /* make sure not to overflow -CFT */
		    m_ptr->confused += (randint(5)+1)/(rad>0 ? rad : 1);
		}
	    }
	    else {
		res = DAZED;
		m_ptr->confused = randint(15)/(rad>0 ? rad : 1);
	    }
	}
	break;
      case GF_INERTIA:
	if (r_ptr->spells3 & MS3_BR_SLOW){ /* if can breath inertia, then
					    resist it. */
	    res = RESIST;
	    *dam *= 3;  /* these 2 lines give avg dam of .33, ranging */
	    *dam /= (randint(6)+6); /* from .427 to .25 -CFT */
        }
	break;
      case GF_LITE:
	if (r_ptr->spells3 & MS3_BR_LITE){ /* breathe light to res light */
	    res = RESIST;
	    *dam *= 2;
	    *dam /= (randint(6)+6);
        }
	else if (r_ptr->cflags2 & MF2_HURT_LITE){
	    res = SUSCEPT;
	    *dam *= 2; /* hurt bad by light */
        }
	else if (r_ptr->spells3 & MS3_BR_DARK){ /* breathe dark gets hurt */
	    res = SUSCEPT;
	    *dam = (*dam * 3)/2;
        }
	break;
      case GF_DARK:
	if (r_ptr->spells2 & MS3_BR_DARK){ /* shard breathers resist -CFT */
	    res = RESIST;
	    *dam *= 2;
	    *dam /= (randint(6)+6);
        }
	else if (r_ptr->cflags2 & MF2_HURT_LITE){
	    res = SOME_RES;
	    *dam /= 2; /* hurt bad by light, so not hurt bad by dark */
        }
	else if (r_ptr->spells3 & MS3_BR_LITE){ /* breathe light gets hurt */
	    res = SUSCEPT;
	    *dam = (*dam * 3)/2;
        }
	break;
      case GF_TIME:
	if (r_ptr->spells3 & MS3_BR_TIME){ /* time breathers resist -CFT */
	    res = RESIST;
	    *dam *= 3;  /* these 2 lines give avg dam of .33, ranging */
	    *dam /= (randint(6)+6); /* from .427 to .25 -CFT */
        }
	break;
      case GF_GRAVITY:
	if (r_ptr->spells3 & MS3_BR_GRAV){ /* breathers resist -CFT */
	    res = RESIST;
	    *dam *= 3;  /* these 2 lines give avg dam of .33, ranging */
	    *dam /= (randint(6)+6); /* from .427 to .25 -CFT */
        }
	else {
	    if (*dam <= m_ptr->hp) {
		teleport_away(cave[m_ptr->fy][m_ptr->fx].cptr, 5);
		*y = m_ptr->fy; /* teleported, so let outside world know monster moved! */
		*x = m_ptr->fx; 
	    }
        }
	break;
      case GF_MANA: /* raw blast of power. no way to resist, is there? */
	break;
      case GF_METEOR: /* GF_METEOR is basically a powerful magic-missile
			 ball spell.  I only made it a different type
			 so I could make it a different color -CFT */
	break;
      case GF_ICE: /* ice is basically frost + cuts + stun -CFT */
	if (r_ptr->cflags2 & MF2_IM_COLD) {
	    res = RESIST;
	    *dam /= 9;
	    if (m_ptr->ml)
		l_list[m_ptr->r_idx].r_cflags2 |= MF2_IM_COLD;
        }
	if ((*dam <= m_ptr->hp) &&
	    !(r_ptr->spells2 & MS2_BR_SOUN) &&
	    !(r_ptr->spells3 & MS3_BR_WALL)){  /* sound and impact breathers
	  					should not stun -CFT */
	    if (m_ptr->confused > 0) { 
		res += MORE_DAZED;
		if (m_ptr->confused < 220){ /* make sure not to overflow -CFT */
		    m_ptr->confused += (randint(5)+1)/(rad>0 ? rad : 1);
		}
	    }
	    else {
		res += DAZED;
		m_ptr->confused = randint(15)/(rad>0 ? rad : 1);
	    }
	}
	break;
      default:
	msg_print("Unknown typ in spell_hit_monster.  This may mean trouble.");
    } /* end switch for saving throws and extra effects */
    
    if (res == CHANGED)
	sprintf(outval, "%schanges!",cdesc);
    else if ((*dam > m_ptr->hp) &&
	     (by_player || !(r_list[m_ptr->r_idx].cflags2 & MF2_UNIQUE))) {
	res = DEAD;
	if ((r_list[m_ptr->r_idx].cflags2 & (MF2_DEMON|MF2_UNDEAD|MF2_MINDLESS)) ||
	    (r_list[m_ptr->r_idx].r_char == 'E') ||
	    (r_list[m_ptr->r_idx].r_char == 'v') ||
	    (r_list[m_ptr->r_idx].r_char == 'g') ||
	    (r_list[m_ptr->r_idx].r_char == 'X'))
	    sprintf(outval, "%sis destroyed.", cdesc);
	else
	    sprintf(outval, "%sdies.", cdesc);
    }
    else switch (res) {
      case NO_RES:
	sprintf(outval, "%sis hit.",cdesc);
	break;
      case SOME_RES:
	sprintf(outval, "%sresists somewhat.",cdesc);
	break;
      case RESIST:
	sprintf(outval, "%sresists.",cdesc);
	break;
      case IMMUNE:
	sprintf(outval, "%sis immune.",cdesc);
	break;
      case SUSCEPT:
	sprintf(outval, "%sis hit hard.",cdesc);
	break;
      case CONFUSED:
	sprintf(outval, "%sis confused.",cdesc);
	break;
      case MORE_CONF:
	sprintf(outval, "%sis more confused.",cdesc);
	break;
      case DAZED:
	sprintf(outval, "%sis dazed.",cdesc);
	break;
      case MORE_DAZED:
	sprintf(outval, "%sis more dazed.",cdesc);
	break;
      case (DAZED+RESIST):
	  sprintf(outval, "%sresists, but is dazed anyway.",cdesc);
	break;
      case (MORE_DAZED+RESIST):
	  sprintf(outval, "%sresists, but still is more dazed.",cdesc);
	break;
      default:
	sprintf(outval,"%sis affected in a mysterious way.",cdesc);
    }
    if (rad || (res != NO_RES)) { /* don't show normal hit msgs for bolts -CFT */
	if (!blind)
	    msg_print(outval);
    }	
}




/*
 * add new destroys?  maybe GF_FORCE destroy potions, GF_PLASMA as lightning,
 * GF_SHARDS and GF_ICE maybe break things (potions?), and GF_METEOR breaks
 * potions and burns scrolls?  not yet, but it's an idea... -CFT 
 */
static void ball_destroy(int typ, int (**destroy) ())
{
    switch (typ) {
      case GF_FIRE:
	*destroy = set_fire_destroy;
	break;
      case GF_ACID:
	*destroy = set_acid_destroy;
	break;
      case GF_COLD:
      case GF_SHARDS:
      case GF_ICE:
      case GF_FORCE:
      case GF_SOUND:
	*destroy = set_frost_destroy;	/* just potions and flasks -DGK */
	break;
      case GF_ELEC:
	*destroy = set_lightning_destroy;
	break;
      case GF_PLASMA:		   /* DGK */
	*destroy = set_plasma_destroy;	/* fire+lightning -DGK */
	break;
      case GF_METEOR:		   /* DGK */
	*destroy = set_meteor_destroy;	/* fire+shards -DGK */
	break;
      case GF_MANA:		   /* DGK */
	*destroy = set_mana_destroy;	/* everything -DGK */
	break;
      case GF_HOLY_ORB:	   /* DGK */
	*destroy = set_holy_destroy;	/* cursed stuff -DGK */
	break;
      case GF_MISSILE:
      case GF_POIS:
      case GF_ARROW:
      case GF_NETHER:
      case GF_WATER:
      case GF_CHAOS:
      case GF_CONFUSION:
      case GF_DISENCHANT:
      case GF_NEXUS:
      case GF_INERTIA:
      case GF_LITE:
      case GF_DARK:
      case GF_TIME:
      case GF_GRAVITY:
	*destroy = set_null;
	break;
      default:
	msg_print("Unknown typ in ball_destroy().  This may mean trouble.");
	*destroy = set_null;
	break;
    }
}




/* Split out of lite_line.       -DGK */
void mon_elec_dam(int y, int x, int dam)
{
    register cave_type     *c_ptr;
    register monster_type  *m_ptr;
    register monster_race *r_ptr;
    vtype                   out_val, m_name;
    int                     i;

    c_ptr = &cave[y][x];
    if (c_ptr->m_idx > 1) {
	m_ptr = &m_list[c_ptr->m_idx];
	r_ptr = &r_list[m_ptr->r_idx];
	monster_name(m_name, m_ptr, r_ptr);
	m_ptr->csleep = 0;
	if (MF2_HURT_LITE & r_ptr->cflags2) {
	    if (m_ptr->ml)
		l_list[m_ptr->r_idx].r_cflags2 |= MF2_HURT_LITE;
	    i = mon_take_hit((int)c_ptr->m_idx, dam, FALSE);
	    if (i >= 0) {
		(void)sprintf(out_val, "%s shrivels away in the light!", m_name);
		msg_print(out_val);
		prt_experience();
	    } else {
		(void)sprintf(out_val, "%s cringes from the light!", m_name);
		msg_print(out_val);
	    }
	}
    }
}




/* this fn only exists to avoid duplicating this code in the selfknowledge fn. -CFT */
static void pause_if_screen_full(int *i, int j)
{
    int t;

    if (*i == 22) {		   /* is screen full? */
	prt("-- more --", *i, j);
	inkey();
	for (t = 2; t < 23; t++)
	    erase_line(t, j);	   /* don't forget to erase extra */
	prt("Your Attributes: (continued)", 1, j + 5);
	*i = 2;
    }
}




int detect_enchantment()
{
    register int i, j, detect, tv;
    register cave_type *c_ptr;
    
    detect = FALSE;
    for (i = panel_row_min; i <= panel_row_max; i++)
	for (j = panel_col_min; j <= panel_col_max; j++) {
	    c_ptr = &cave[i][j];
	    tv = i_list[c_ptr->i_idx].tval;
	    if ((c_ptr->i_idx != 0) && !test_lite(i, j) &&
		( ((tv > TV_MAX_ENCHANT) && (tv < TV_FLASK)) || /* misc items */
		 (tv == TV_MAGIC_BOOK) || (tv == TV_PRAYER_BOOK) || /* books */
		 ((tv >= TV_MIN_WEAR) && (tv <= TV_MAX_ENCHANT) && /* armor/weap */
		  ((i_list[c_ptr->i_idx].flags2 & TR_ARTIFACT) || /* if Art., or */
		   (i_list[c_ptr->i_idx].tohit>0) || /* has pluses, then show */
		   (i_list[c_ptr->i_idx].todam>0) ||
		   (i_list[c_ptr->i_idx].toac>0))) )){
		c_ptr->fm = TRUE;
		lite_spot(i, j);
		detect = TRUE;
	    }
	}

    /* Return result */    
    return (detect);
}



/* 
 * I may have written the town level code, but I'm not exactly	 */
/* proud of it.	 Adding the stores required some real slucky	 */
/* hooks which I have not had time to re-think.		 -RAK-	 */


static void magic_ammo(inven_type *t_ptr, int good, int chance, int special, int cursed, int level)
{
    register inven_type *i_ptr = NULL;
    register int         i;

    /* if wielding a bow as main/aux weapon, then ammo will be "right" ammo
     * more often than not of the time -CFT */
    if (inventory[INVEN_WIELD].tval == TV_BOW) i_ptr=&inventory[INVEN_WIELD];
    else if (inventory[INVEN_BOW].tval == TV_BOW) i_ptr=&inventory[INVEN_BOW];

    if (i_ptr && (randint(2)==1)){
	if ((t_ptr->tval == TV_SHOT) &&
	    (i_ptr->sval >= 20) && (i_ptr->subval <= 21));
	/* right type, do nothing */
	else if ((t_ptr->tval == TV_ARROW) &&
		 (i_ptr->sval >= 1) && (i_ptr->subval <= 4));
	/* right type, do nothing */
	else if ((t_ptr->tval == TV_BOLT) &&
		 (i_ptr->sval >= 10) && (i_ptr->subval <= 12));
	/* right type, do nothing */
	else if ((i_ptr->sval >= 20) && (i_ptr->subval <= 21))
	    invcopy(t_ptr, 83); /* this should be treasure list index of shots -CFT */
	else if ((i_ptr->sval >= 1) && (i_ptr->subval <= 4))
	    invcopy(t_ptr, 78); /* this should be index of arrows -CFT */
	else			/* xbow */
	    invcopy(t_ptr, 80); /* this should be index of bolts -CFT */
    }

    t_ptr->number = 0;
    for (i = 0; i < 7; i++)
	t_ptr->number += randint(6);
    if (missile_ctr == MAX_SHORT)
	missile_ctr = -MAX_SHORT - 1;
    else
	missile_ctr++;
    t_ptr->pval = missile_ctr;

    /* always show tohit/todam values if identified */
    t_ptr->ident |= TR3_SHOW_MODS;
    if (magik(chance)||good) {
	t_ptr->tohit = randint(5) + m_bonus(1, 15, level);
	t_ptr->todam = randint(5) + m_bonus(1, 15, level);
	/* see comment for weapons */
	if (magik(5*special/2)||(good==666))
	    switch(randint(11)) {
	      case 1: case 2: case 3:
		t_ptr->name2 = EGO_WOUNDING; /* swapped with slaying -CFT */
		t_ptr->tohit += 5;
		t_ptr->todam += 5;
		t_ptr->dd ++; /* added -CFT */
		t_ptr->cost += 30;
		rating += 5;
		break;
	      case 4: case 5:
		t_ptr->flags |= (TR1_BRAND_FIRE|TR2_RES_FIRE); /* RF so won't burn */
		t_ptr->tohit += 2;
		t_ptr->todam += 4;
		t_ptr->name2 = EGO_FIRE;
		t_ptr->cost += 25;
		rating += 6;
		break;
	      case 6: case 7:
		t_ptr->flags |= TR1_SLAY_EVIL;
		t_ptr->tohit += 3;
		t_ptr->todam += 3;
		t_ptr->name2 = EGO_SLAY_EVIL;
		t_ptr->cost += 25;
		rating += 7;
		break;
	      case 8: case 9:
		t_ptr->flags |= TR1_SLAY_ANIMAL;
		t_ptr->tohit += 2;
		t_ptr->todam += 2;
		t_ptr->name2 = EGO_SLAY_ANIMAL;
		t_ptr->cost += 30;
		rating += 5;
		break;
	      case 10:
		t_ptr->flags |= TR1_SLAY_DRAGON;
		t_ptr->tohit += 3;
		t_ptr->todam += 3;
		t_ptr->name2 = EGO_DRAGON_SLAYING;
		t_ptr->cost += 35;
		rating += 9;
		break;
	      case 11:
		t_ptr->tohit += 10; /* reduced because of dice bonus -CFT */
		t_ptr->todam += 10;
		t_ptr->name2 = EGO_SLAYING; /* swapped w/ wounding -CFT */
		t_ptr->dd += 2; /* added -CFT */
		t_ptr->cost += 45;
		rating += 10;
		break;
	    }
	while (magik(special)) { /* added -CFT */
	    t_ptr->dd++;
	    t_ptr->cost += t_ptr->dd*5;
	}
    }
    else if (magik(cursed)) {
	t_ptr->tohit = (-randint(10)) - m_bonus(5, 25, level);
	t_ptr->todam = (-randint(10)) - m_bonus(5, 25, level);
	t_ptr->flags |= TR3_CURSED;
	t_ptr->cost = 0;
	if (randint(5)==1) {
	    t_ptr->name2 = EGO_BACKBITING;
	    t_ptr->tohit -= 20;
	    t_ptr->todam -= 20;
	}
    }
}


/*
 * Boost a stat artificially (by wearing something). If the display argument
 * is TRUE, then increase is shown on the screen. 
 */
void bst_stat(int stat, int amount)
{
    p_ptr->stats.mod_stat[stat] += amount;

    set_use_stat(stat);
/* can not call prt_stat() here, may be in store, may be in inven_command */
    p_ptr->flags.status |= (PY_STR << stat);
}




/*
 * Changes speed of monsters relative to player		-RAK-
 * Note: When the player is sped up or slowed down, I simply change the
 * speed of all the monsters.  This greatly simplified the logic.
 *****
 * No LONGER!  A change in player speed only affect player's speed.  The new
 * code in movement_rate() allows monsters to have attacks in correct
 * proportions, and still uses only int math -CFT 
 */
void change_speed(register int num)
{
    p_ptr->flags.speed += num;
    p_ptr->flags.status |= PY_SPEED;
}


/*
 * Player bonuses					-RAK-	 
 * When an item is worn or taken off, this re-adjusts the player
 * bonuses.  Factor=1 : wear; Factor=-1 : removed  
 *****
 * Only calculates properties with cumulative effect.  Properties that depend
 * on everything being worn are recalculated by calc_bonuses() -CJS - 
 */
void py_bonuses(register inven_type *t_ptr, register int factor)
{
    register int i, amount;

    amount = t_ptr->pval * factor;
    if (t_ptr->flags & TR_STATS) {
	for (i = 0; i < 6; i++)
	    if ((1 << i) & t_ptr->flags)
		bst_stat(i, amount);
    }
    if (TR1_SEARCH & t_ptr->flags) {
	p_ptr->misc.srh += amount;
	p_ptr->misc.fos -= amount;
    }
    if (TR1_STEALTH & t_ptr->flags)
	p_ptr->misc.stl += amount;
    if (TR1_SPEED & t_ptr->flags) {
	if ((t_ptr->tval == TV_RING) &&
	    !stricmp("Speed",
		     k_list[t_ptr->index].name) &&
	    (t_ptr->pval > 0))
	    if ((inventory[INVEN_RIGHT].tval == TV_RING) &&
		!stricmp("Speed",
			 k_list[inventory[INVEN_RIGHT].index].name) &&
		(inventory[INVEN_RIGHT].pval > 0) &&
		(inventory[INVEN_LEFT].tval == TV_RING) &&
		!stricmp("Speed",
			 k_list[inventory[INVEN_LEFT].index].name) &&
		(inventory[INVEN_RIGHT].pval > 0))
		return;
	change_speed(-amount);
    }
    if (TR1_INFRA & t_ptr->flags)
	p_ptr->flags.see_infra += amount;
}


/* 
 * map rogue_like direction commands into numbers */
static char map_roguedir(register int my_comval)
{
    char comval = (char)my_comval;

    switch (comval) {
      case 'h':
	comval = '4';
	break;
      case 'y':
	comval = '7';
	break;
      case 'k':
	comval = '8';
	break;
      case 'u':
	comval = '9';
	break;
      case 'l':
	comval = '6';
	break;
      case 'n':
	comval = '3';
	break;
      case 'j':
	comval = '2';
	break;
      case 'b':
	comval = '1';
	break;
      case '.':
	comval = '5';
	break;
    }
    return (comval);
}


static void flood_light(int y,int x)
{
    register cave_type *c_ptr;
    register int temp;
    c_ptr = &cave[y][x];
    if (c_ptr->lr) {
	temp=c_ptr->tl;
	c_ptr->tl = TRUE;
	if (c_ptr->fval<MIN_CLOSED_SPACE && temp==FALSE) {
	    flood_light(y+1,x);
	    flood_light(y-1,x);
	    flood_light(y,x+1);
	    flood_light(y,x-1);
	    flood_light(y+1,x+1);
	    flood_light(y-1,x-1);
	    flood_light(y-1,x+1);
	    flood_light(y+1,x-1);
	}
    }
}


static void flood_permanent(int y,int x)
{
    register cave_type *c_ptr;
    c_ptr = &cave[y][x];
    if (c_ptr->tl) {
	c_ptr->tl = FALSE;
	c_ptr->pl = TRUE;
	if (c_ptr->fval==NT_DARK_FLOOR)
	    c_ptr->fval=NT_LIGHT_FLOOR;
	else if (c_ptr->fval==DARK_FLOOR)
	    c_ptr->fval=LIGHT_FLOOR;
#ifdef MSDOS
	lite_spot(y,x); /* this does all that; plus color-safe -CFT */
#else
	if ((y-panel_row_prt)<23 && (y-panel_row_prt)>0 &&
	    (x-panel_col_prt)>12 && (x-panel_col_prt)<80)
	    print(loc_symbol(y, x), y, x);
#endif
	if (c_ptr->fval<MIN_CLOSED_SPACE) {
	    flood_permanent(y+1,x);
      flood_permanent(y-1,x);
	    flood_permanent(y,x+1);
	    flood_permanent(y,x-1);
	    flood_permanent(y+1,x+1);
	    flood_permanent(y-1,x-1);
	    flood_permanent(y-1,x+1);
	    flood_permanent(y+1,x-1);
	}
    }
}


static void flood_permanent_dark(int y,int x)
{
    register cave_type *c_ptr;
    c_ptr = &cave[y][x];
    if (c_ptr->tl) {
	c_ptr->tl = FALSE;
	if (c_ptr->fval==NT_LIGHT_FLOOR)
	    c_ptr->fval=NT_DARK_FLOOR;
	else if (c_ptr->fval==LIGHT_FLOOR)
	    c_ptr->fval=DARK_FLOOR;
#ifdef MSDOS
	if (panel_contains(y,x)) {
	    if (c_ptr->fval < MIN_CLOSED_SPACE){
		c_ptr->pl = FALSE;
		flood_permanent_dark(y+1,x);
		flood_permanent_dark(y-1,x);
		flood_permanent_dark(y,x+1);
		flood_permanent_dark(y,x-1);
		flood_permanent_dark(y+1,x+1);
		flood_permanent_dark(y-1,x-1);
		flood_permanent_dark(y-1,x+1);
		flood_permanent_dark(y+1,x-1);
	    }
	    lite_spot(y,x);
	}
#else
	if ((y-panel_row_prt)<23 && (y-panel_row_prt)>0 &&
	    (x-panel_col_prt)>12 && (x-panel_col_prt)<80)
	    if (c_ptr->fval<MIN_CLOSED_SPACE) {
		c_ptr->pl = FALSE;
		flood_permanent_dark(y+1,x);
		flood_permanent_dark(y-1,x);
		flood_permanent_dark(y,x+1);
		flood_permanent_dark(y,x-1);
		flood_permanent_dark(y+1,x+1);
		flood_permanent_dark(y-1,x-1);
		flood_permanent_dark(y-1,x+1);
		flood_permanent_dark(y+1,x-1);
	    }
	print(loc_symbol(y, x), y, x);
#endif
    }
}


/* 
 * Normal movement					 */
/* When FIND_FLAG,  light only permanent features	 */
static void sub1_move_light(int y1, register int x1, int y2, register int x2)
{
    register int        i, j;
    register cave_type *c_ptr;
    int                 tval, top, left, bottom, right;
    int          min_i, max_i, min_j, max_j;

    if (light_flag) {
	darken_player(y1, x1);
	if (find_flag && !find_prself)
	    light_flag = FALSE;
    } else if (!find_flag || find_prself)
	light_flag = TRUE;

/* replace a check for in_bounds2 every loop with 4 quick computations -CWS */
    min_i = MY_MAX(0, (y2 - cur_lite));
    max_i = MY_MIN(cur_height, (y2 + cur_lite));
    min_j = MY_MAX(0, (x2 - cur_lite));
    max_j = MY_MIN(cur_width, (x2 + cur_lite));
    for (i = min_i; i <= max_i; i++)
	for (j = min_j; j <= max_j; j++)
	    if (los(y2, x2, i, j) && distance(i, j, y2, x2) <= cur_lite) {
		c_ptr = &cave[i][j];
	    /* only light up if normal movement */
		if (light_flag)
		    c_ptr->tl = TRUE;
		if (c_ptr->fval >= MIN_CAVE_WALL)
		    c_ptr->pl = TRUE;
		else if (!c_ptr->fm && c_ptr->i_idx != 0) {
		    tval = i_list[c_ptr->i_idx].tval;
		    if ((tval >= TV_MIN_VISIBLE) && (tval <= TV_MAX_VISIBLE))
			c_ptr->fm = TRUE;
		}
	    }
/* From uppermost to bottom most lines player was on.	 */
    if (y1 < y2) {
	top = y1 - cur_lite;
	bottom = y2 + cur_lite;
    } else {
	top = y2 - cur_lite;
	bottom = y1 + cur_lite;
    }
    if (x1 < x2) {
	left = x1 - cur_lite;
	right = x2 + cur_lite;
    } else {
	left = x2 - cur_lite;
	right = x1 + cur_lite;
    }
    for (i = top; i <= bottom; i++)
	for (j = left; j <= right; j++)	/* Leftmost to rightmost do */
	    lite_spot(i, j);	   /* this does that, plus panel check +
				    * color safe */
}


/* 
 * When blinded,  move only the player symbol.		 */
/* With no light,  movement becomes involved.		 */
static void sub3_move_light(register int y1, register int x1, int y2, int x2)
{
    if (light_flag) {
	darken_player(y1, x1);
	light_flag = FALSE;
    } else if (!find_flag || find_prself)	/* um55 change -CFT */
	lite_spot(y1, x1);

#ifdef TC_COLOR
    if (!no_color_flag)
	textcolor(LIGHTGRAY);
#endif

    if (!find_flag || find_prself)
	print('@', y2, x2);
}


void darken_player(int y1, int x1)
{
    int min_i, max_i, min_j, max_j, rad, i, j;

    rad = MY_MAX(cur_lite, old_lite);

/* replace a check for in_bounds2 every loop with 4 quick computations -CWS */
    min_i = MY_MAX(0, (y1 - rad));
    max_i = MY_MIN(cur_height, (y1 + rad));
    min_j = MY_MAX(0, (x1 - rad));
    max_j = MY_MIN(cur_width, (x1 + rad));

    for (i = min_i; i <= max_i; i++)
        for (j = min_j; j <= max_j; j++) {
	    cave[i][j].tl = FALSE;  /* Turn off light */
	    lite_spot(i, j);
        }
}


/* 
 * Package for moving the character's light about the screen	 */
/* Four cases : Normal, Finding, Blind, and Nolight	 -RAK-	 */
void move_light(int y1, int x1, int y2, int x2)
{
    if (p_ptr->flags.blind > 0 || !player_light)
	sub3_move_light(y1, x1, y2, x2);
    else
	sub1_move_light(y1, x1, y2, x2);
}

    
/* 
 * Change a trap from invisible to visible		-RAK-	 */
/* Note: Secret doors are handled here				 */
void change_trap(register int y, register int x)
{
    register cave_type  *c_ptr;
    register inven_type *t_ptr;

    c_ptr = &cave[y][x];
    t_ptr = &i_list[c_ptr->i_idx];
    if (t_ptr->tval == TV_INVIS_TRAP) {
	t_ptr->tval = TV_VIS_TRAP;
	lite_spot(y, x);
    } else if (t_ptr->tval == TV_SECRET_DOOR) {
    /* change secret door to closed door */
	t_ptr->index = OBJ_CLOSED_DOOR;
	t_ptr->tval = k_list[OBJ_CLOSED_DOOR].tval;
	t_ptr->tchar = k_list[OBJ_CLOSED_DOOR].tchar;
	lite_spot(y, x);
    }
}


/* Is an item an enchanted weapon or armor and we don't know?  -CJS- */
/* returns positive if it is a good enchantment */
/* returns negative if a bad enchantment... */
int special_check(register inven_type *t_ptr)
{
    if (t_ptr->tval == TV_NOTHING)
	return 0;
    if (known2_p(t_ptr))
	return 0;
    if (store_bought_p(t_ptr))
	return 0;
    if (t_ptr->ident & ID_MAGIK)
	return 0;
    if (t_ptr->ident & ID_DAMD)
	return 0;
    if (t_ptr->flags & TR3_CURSED)
	return -1;
    if (t_ptr->tval != TV_HARD_ARMOR && t_ptr->tval != TV_SWORD &&
	t_ptr->tval != TV_SOFT_ARMOR && t_ptr->tval != TV_SHIELD &&
	t_ptr->tval != TV_CLOAK && t_ptr->tval != TV_GLOVES &&
	t_ptr->tval != TV_BOOTS && t_ptr->tval != TV_HELM &&
	t_ptr->tval != TV_DIGGING && t_ptr->tval != TV_SPIKE &&
	t_ptr->tval != TV_SHOT && t_ptr->tval != TV_BOLT &&
	t_ptr->tval != TV_ARROW && t_ptr->tval != TV_BOW &&
	t_ptr->tval != TV_POLEARM && t_ptr->tval != TV_HAFTED)
	return 0;
    if (t_ptr->tohit > 0 || t_ptr->todam > 0 || t_ptr->toac > 0)
	return 1;
    if ((t_ptr->tval == TV_DIGGING) && /* digging tools will pseudo ID, either
					  as {good} or {average} -CFT */
	(t_ptr->flags & TR1_TUNNEL))
	return 1;

    return 0;
}





/* Check whether this command will accept a count.     -CJS-  */
static int valid_countcommand(char c)
{
    switch (c) {
      case 'Q':
      case CTRL('W'):
      case CTRL('X'):
      case '=':
      case '{':
      case '/':
      case '<':
      case '>':
      case '?':
      case 'A':
      case 'C':
      case 'E':
      case 'F':
      case 'G':
      case '#':
      case 'z':
      case 'P':
      case 'c':
      case 'd':
      case 'e':
      case 't':
      case 'i':
      case 'x':
      case 'm':
      case 'p':
      case 'q':
      case 'r':
      case 'T':
      case 'Z':
      case 'V':
      case 'w':
      case 'W':
      case 'X':
      case CTRL('A'):
      case '\\':
      case CTRL('I'):
      case CTRL('^'):
      case '$':
      case '*':
      case ':':
      case CTRL('T'):
      case CTRL('E'):
      case CTRL('F'):
      case CTRL('Z'):
      case CTRL('S'):
      case CTRL('Q'):
      case CTRL('R'):
	return FALSE;
      case CTRL('P'):
      case ESCAPE:
      case ' ':
      case '-':
      case 'b':
      case 'f':
      case 'j':
      case 'n':
      case 'h':
      case 'l':
      case 'y':
      case 'k':
      case 'u':
      case '.':
      case 'B':
      case 'J':
      case 'N':
      case 'H':
      case 'L':
      case 'Y':
      case 'K':
      case 'U':
      case 'D':
      case 'R':
      case CTRL('Y'):
      case CTRL('K'):
      case CTRL('U'):
      case CTRL('L'):
      case CTRL('N'):
      case CTRL('J'):
      case CTRL('B'):
      case CTRL('H'):
      case 'S':
      case 'o':
      case 's':
      case CTRL('D'):
      case CTRL('G'):
      case '+':
	return TRUE;
      default:
	return FALSE;
    }
}



