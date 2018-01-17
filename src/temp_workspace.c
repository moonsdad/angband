
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
    r_ptr = &r_list[m_ptr->mptr];
    if (m_ptr->ml){
	if (r_ptr->cdefense & MF2_UNIQUE)
	    sprintf(cdesc, "%s ", r_ptr->name);
	else
	    sprintf(cdesc, "The %s ", r_ptr->name);
    }
    else
	strcpy(cdesc, "It ");

    res = NO_RES;		/* assume until we know different -CFT */
    switch ( typ ){		/* check for resists... */
      case GF_MAGIC_MISSILE:	/* pure damage, no resist possible */
	break;
      case GF_ELEC:
	if (r_ptr->cdefense & MF2_IM_ELEC) {
	    res = RESIST;
	    *dam /= 9;
	    if (m_ptr->ml)
		l_list[m_ptr->mptr].r_cdefense |= MF2_IM_ELEC;
        }
	break;
      case GF_POIS:
	if (r_ptr->cdefense & MF2_IM_POIS) {
	    res = RESIST;
	    *dam /= 9;
	    if (m_ptr->ml)
		l_list[m_ptr->mptr].r_cdefense |= MF2_IM_POIS;
        }
	break;
      case GF_ACID:
	if (r_ptr->cdefense & MF2_IM_ACID) {
	    res = RESIST;
	    *dam /= 9;
	    if (m_ptr->ml)
		l_list[m_ptr->mptr].r_cdefense |= MF2_IM_ACID;
        }
	break;
      case GF_COLD:
	if (r_ptr->cdefense & MF2_IM_COLD) {
	    res = RESIST;
	    *dam /= 9;
	    if (m_ptr->ml)
		l_list[m_ptr->mptr].r_cdefense |= MF2_IM_COLD;
        }
	break;
      case GF_FIRE:
	if (r_ptr->cdefense & MF2_IM_FIRE) {
	    res = RESIST;
	    *dam /= 9;
	    if (m_ptr->ml)
		l_list[m_ptr->mptr].r_cdefense |= MF2_IM_FIRE;
        }
	break;
      case GF_HOLY_ORB:
	if (r_ptr->cdefense & EVIL) {
	    *dam *= 2;
	    res = SUSCEPT;
	    if (m_ptr->ml)
		l_list[m_ptr->mptr].r_cdefense |= EVIL;
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
	if (r_ptr->cdefense & UNDEAD) {
	    res = IMMUNE;
	    *dam = 0;
	    if (m_ptr->ml)
		l_list[m_ptr->mptr].r_cdefense |= UNDEAD;
        }
	else if (r_ptr->spells2 & MS2_BR_LIFE) { /* if can breath nether, should get
						  good resist to damage -CFT */
	    res = RESIST;
	    *dam *= 3;  /* these 2 lines give avg dam of .33, ranging */
	    *dam /= (randint(6)+6); /* from .427 to .25 -CFT */
	}
	else if (r_ptr->cdefense & EVIL) {
	    *dam /= 2;	/* evil takes *2 for holy, so /2 for this... -CFT */
	    res = SOME_RES;
	    if (m_ptr->ml)
		l_list[m_ptr->mptr].r_cdefense |= EVIL;
        }
	break;
      case GF_WATER:	/* water elementals should resist.  anyone else? -CFT */
	if ((r_ptr->cchar == 'E') && (r_ptr->name[0] == 'W')){
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
	    !(r_ptr->cdefense & MF2_UNIQUE) &&
	    (randint(90) > r_ptr->level)) { /* then we'll polymorph it -CFT */
	    res = CHANGED;
	    if (poly(cave[*y][*x].cptr))
		*dam = 0; /* new monster was not hit by choas breath.  This also
			     makes things easier to handle */
	} /* end of choas-poly.  If was poly-ed don't bother confuse... it's
	     too hectic to keep track of... -CFT */
	else if (!(r_ptr->cdefense & MF2_CHARM_SLEEP) &&
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
	else if (r_ptr->cdefense & MF2_CHARM_SLEEP){
	    res = SOME_RES;
	    *dam /= 2; /* only some resist, but they also avoid confuse -CFT */
        }
	if ((*dam <= m_ptr->hp) && /* don't bother if it's dead */
	    !(r_ptr->cdefense & MF2_CHARM_SLEEP) &&
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
      case GF_LIGHT:
	if (r_ptr->spells3 & MS3_BR_LITE){ /* breathe light to res light */
	    res = RESIST;
	    *dam *= 2;
	    *dam /= (randint(6)+6);
        }
	else if (r_ptr->cdefense & MF2_HURT_LITE){
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
	else if (r_ptr->cdefense & MF2_HURT_LITE){
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
	if (r_ptr->cdefense & MF2_IM_COLD) {
	    res = RESIST;
	    *dam /= 9;
	    if (m_ptr->ml)
		l_list[m_ptr->mptr].r_cdefense |= MF2_IM_COLD;
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
	     (by_player || !(r_list[m_ptr->mptr].cdefense & MF2_UNIQUE))) {
	res = DEAD;
	if ((r_list[m_ptr->mptr].cdefense & (DEMON|UNDEAD|MF2_MINDLESS)) ||
	    (r_list[m_ptr->mptr].cchar == 'E') ||
	    (r_list[m_ptr->mptr].cchar == 'v') ||
	    (r_list[m_ptr->mptr].cchar == 'g') ||
	    (r_list[m_ptr->mptr].cchar == 'X'))
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
      case GF_MAGIC_MISSILE:
      case GF_POIS:
      case GF_ARROW:
      case GF_NETHER:
      case GF_WATER:
      case GF_CHAOS:
      case GF_CONFUSION:
      case GF_DISENCHANT:
      case GF_NEXUS:
      case GF_INERTIA:
      case GF_LIGHT:
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
    if (c_ptr->cptr > 1) {
	m_ptr = &m_list[c_ptr->cptr];
	r_ptr = &r_list[m_ptr->mptr];
	monster_name(m_name, m_ptr, r_ptr);
	m_ptr->csleep = 0;
	if (MF2_HURT_LITE & r_ptr->cdefense) {
	    if (m_ptr->ml)
		l_list[m_ptr->mptr].r_cdefense |= MF2_HURT_LITE;
	    i = mon_take_hit((int)c_ptr->cptr, dam, FALSE);
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
	    tv = i_list[c_ptr->tptr].tval;
	    if ((c_ptr->tptr != 0) && !test_lite(i, j) &&
		( ((tv > TV_MAX_ENCHANT) && (tv < TV_FLASK)) || /* misc items */
		 (tv == TV_MAGIC_BOOK) || (tv == TV_PRAYER_BOOK) || /* books */
		 ((tv >= TV_MIN_WEAR) && (tv <= TV_MAX_ENCHANT) && /* armor/weap */
		  ((i_list[c_ptr->tptr].flags2 & TR_ARTIFACT) || /* if Art., or */
		   (i_list[c_ptr->tptr].tohit>0) || /* has pluses, then show */
		   (i_list[c_ptr->tptr].todam>0) ||
		   (i_list[c_ptr->tptr].toac>0))) )){
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
    else if (inventory[INVEN_AUX].tval == TV_BOW) i_ptr=&inventory[INVEN_AUX];

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
    t_ptr->p1 = missile_ctr;

    /* always show tohit/todam values if identified */
    t_ptr->ident |= ID_SHOW_HITDAM;
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
	t_ptr->flags |= TR_CURSED;
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

    amount = t_ptr->p1 * factor;
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
		     objeci_list[t_ptr->index].name) &&
	    (t_ptr->p1 > 0))
	    if ((inventory[INVEN_RIGHT].tval == TV_RING) &&
		!stricmp("Speed",
			 objeci_list[inventory[INVEN_RIGHT].index].name) &&
		(inventory[INVEN_RIGHT].p1 > 0) &&
		(inventory[INVEN_LEFT].tval == TV_RING) &&
		!stricmp("Speed",
			 objeci_list[inventory[INVEN_LEFT].index].name) &&
		(inventory[INVEN_RIGHT].p1 > 0))
		return;
	change_speed(-amount);
    }
    if (TR_INFRA & t_ptr->flags)
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
		else if (!c_ptr->fm && c_ptr->tptr != 0) {
		    tval = i_list[c_ptr->tptr].tval;
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
    t_ptr = &i_list[c_ptr->tptr];
    if (t_ptr->tval == TV_INVIS_TRAP) {
	t_ptr->tval = TV_VIS_TRAP;
	lite_spot(y, x);
    } else if (t_ptr->tval == TV_SECRET_DOOR) {
    /* change secret door to closed door */
	t_ptr->index = OBJ_CLOSED_DOOR;
	t_ptr->tval = objeci_list[OBJ_CLOSED_DOOR].tval;
	t_ptr->tchar = objeci_list[OBJ_CLOSED_DOOR].tchar;
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
    if (t_ptr->flags & TR_CURSED)
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
	(t_ptr->flags & TR_TUNNEL))
	return 1;

    return 0;
}


static void do_command(char com_val)
{
    int                    dir_val, do_pickup;
    int                    y, x, i, j = 0;
    vtype                  out_val, tmp_str;
    register struct flags *f_ptr;
    char                   prt1[80];

/* hack for move without pickup.  Map '-' to a movement command. */
    if (com_val == '-') {
	do_pickup = FALSE;
	i = command_count;
#ifdef TARGET
	{
/* If in target_mode, player will not be given a chance to pick a direction.
 * So we save it, force it off, and then ask for the direction -CFT
 */
	int temp = target_mode;
	target_mode = FALSE;
#endif
	if (get_dir(NULL, &dir_val)) {
	    command_count = i;
	    switch (dir_val) {
	      case 1:
		com_val = 'b';
		break;
	      case 2:
		com_val = 'j';
		break;
	      case 3:
		com_val = 'n';
		break;
	      case 4:
		com_val = 'h';
		break;
	      case 6:
		com_val = 'l';
		break;
	      case 7:
		com_val = 'y';
		break;
	      case 8:
		com_val = 'k';
		break;
	      case 9:
		com_val = 'u';
		break;
	      default:
		com_val = '(';
		break;
	    }
	} else
	    com_val = ' ';
#ifdef TARGET
	target_mode = temp;
	}
#endif
    } else
	do_pickup = TRUE;

    switch (com_val) {
      case 'Q':			/* (Q)uit		(^K)ill */
	flush();
	if ((!total_winner) ? get_check("Do you really want to quit?")
	    : get_check("Do you want to retire?")) {
	    new_level_flag = TRUE;
	    death = TRUE;
	    (void)strcpy(died_from, "Quitting");
	}
	free_turn_flag = TRUE;
	break;
      case CTRL('P'):		/* (^P)revious message. */
	if (command_count > 0) {
	    i = command_count;
	    if (i > MAX_SAVE_MSG)
		i = MAX_SAVE_MSG;
	    command_count = 0;
	} else if (last_command != 16)
	    i = 1;
	else
	    i = MAX_SAVE_MSG;
	j = last_msg;
	if (i > 1) {
	    save_screen();
	    x = i;
	    while (i > 0) {
		i--;
		prt(old_msg[j], i, 0);
		if (j == 0)
		    j = MAX_SAVE_MSG - 1;
		else
		    j--;
	    }
	    erase_line(x, 0);
	    pause_line(x);
	    restore_screen();
	} else {
	/* Distinguish real and recovered messages with a '>'. -CJS- */
	    put_str(">", 0, 0);
	    prt(old_msg[j], 0, 1);
	}
	free_turn_flag = TRUE;
	break;
      case CTRL('F'):		/* Repeat (^F)eeling */
	free_turn_flag = TRUE;
	do_cmd_feeling();
	break;
      case CTRL('W'):		/* (^W)izard mode */
	if (wizard) {
	    wizard = FALSE;
	    msg_print("Wizard mode off.");
	} else if (enter_wiz_mode())
	    msg_print("Wizard mode on.");
	prt_winner();
	free_turn_flag = TRUE;
	break;
      case CTRL('X'):		/* e(^X)it and save */
	if (total_winner) {
	    msg_print("You are a Total Winner,  your character must be retired.");
	    if (rogue_like_commands)
		msg_print("Use 'Q' to when you are ready to retire.");
	    else
		msg_print("Use <Control>-K when you are ready to retire.");
	} else {
	    (void)strcpy(died_from, "(saved)");
	    msg_print("Saving game...");
	    if (save_player())
		exit_game();
	    msg_print("Save failed...");
	    (void)strcpy(died_from, "(alive and well)");
	}
	free_turn_flag = TRUE;
	break;
      case CTRL('R'):
	if (p_ptr->flags.image > 0)
	    msg_print("You cannot be sure what is real and what is not!");
	else {
	    draw_cave();
	    process_monsters(FALSE);	  /* draw monsters */
	    prt_equippy_chars();  /* redraw equippy chars */
	}
	free_turn_flag = TRUE;
	break;
#ifdef TARGET
/* select a target (sorry, no intuitive letter keys were left: a/A for aim,
 * t/T for target, f/F for focus, s/S for select, c/C for choose and p/P for pick
 *  were all already taken.  Wiz light command moved to '$', which was unused. -CFT
 */
      case '*':
	target();		/* target code taken from Morgul -CFT */
	free_turn_flag = TRUE;
	break;    			
#endif
      case '=':			/* (=) set options */
	save_screen();
	set_options();
	restore_screen();
	free_turn_flag = TRUE;
	break;
      case '{':			/* ({) inscribe an object    */
	scribe_object();
	free_turn_flag = TRUE;
	break;
      case '!':			/* (!) escape to the shell */
	if (!wizard)
#ifdef MSDOS			/* Let's be a little more accurate... */
	    msg_print("Sorry, Angband doesn't leave enough free memory for a subshell.");
#else
	    msg_print("Sorry, inferior shells are not allowed from ANGBAND.");
#endif
	else
	    rerate();
	free_turn_flag = TRUE;
	break;
      case ESCAPE:		/* (ESC)   do nothing. */
      case ' ':			/* (space) do nothing. */
	free_turn_flag = TRUE;
	break;
      case 'b':			/* (b) down, left	(1) */
	move_player(1, do_pickup);
	break;
      case 'j':			/* (j) down		(2) */
	move_player(2, do_pickup);
	break;
      case 'n':			/* (n) down, right	(3) */
	move_player(3, do_pickup);
	break;
      case 'h':			/* (h) left		(4) */
	move_player(4, do_pickup);
	break;
      case 'l':			/* (l) right		(6) */
	move_player(6, do_pickup);
	break;
      case 'y':			/* (y) up, left		(7) */
	move_player(7, do_pickup);
	break;
      case 'k':			/* (k) up		(8) */
	move_player(8, do_pickup);
	break;
      case 'u':			/* (u) up, right	(9) */
	move_player(9, do_pickup);
	break;
      case 'B':			/* (B) run down, left	(. 1) */
	find_init(1);
	break;
      case 'J':			/* (J) run down		(. 2) */
	find_init(2);
	break;
      case 'N':			/* (N) run down, right	(. 3) */
	find_init(3);
	break;
      case 'H':			/* (H) run left		(. 4) */
	find_init(4);
	break;
      case 'L':			/* (L) run right	(. 6) */
	find_init(6);
	break;
      case 'Y':			/* (Y) run up, left	(. 7) */
	find_init(7);
	break;
      case 'K':			/* (K) run up		(. 8) */
	find_init(8);
	break;
      case 'U':			/* (U) run up, right	(. 9) */
	find_init(9);
	break;
      case '/':			/* (/) identify a symbol */
	ident_char();
	free_turn_flag = TRUE;
	break;
      case '.':			/* (.) stay in one place (5) */
	move_player(5, do_pickup);
	if (command_count > 1) {
	    command_count--;
	    rest();
	}
	break;
      case '<':			/* (<) go down a staircase */
	do_cmd_go_up();
	break;
      case '>':			/* (>) go up a staircase */
	do_cmd_go_down();
	break;
      case '?':			/* (?) help with commands */
	if (rogue_like_commands)
	    helpfile(ANGBAND_HELP);
	else
	    helpfile(ANGBAND_ORIG_HELP);
	free_turn_flag = TRUE;
	break;
#ifdef ALLOW_SCORE
      case 'v':   /* score patch originally by Mike Welsh mikewe@acacia.cs.pdx.edu */
	sprintf(prt1,"Your current score is: %ld", total_points());
	msg_print(prt1);
	break;
#endif
      case 'f':			/* (f)orce		(B)ash */
	bash();
	break;
      case 'A':			/* (A)ctivate		(A)ctivate */
	activate();
	break;
      case 'C':			/* (C)haracter description */
	save_screen();
	change_name();
	restore_screen();
	free_turn_flag = TRUE;
	break;
      case 'D':			/* (D)isarm trap */
	do_cmd_disarm();
	break;
      case 'E':			/* (E)at food */
	eat();
	break;
      case 'F':			/* (F)ill lamp */
	do_cmd_refill_lamp();
	break;
      case 'G':			/* (G)ain magic spells */
	gain_spells();
	break;
      case 'g':			/* (g)et an object... */
	if (prompt_carry_flag) {
	    if (cave[char_row][char_col].tptr != 0)	/* minor change -CFT */
		carry(char_row, char_col, TRUE);
	} else
	    free_turn_flag = TRUE;
	break;
      case 'W':			/* (W)here are we on the map	(L)ocate on map */
	if ((p_ptr->flags.blind > 0) || no_lite())
	    msg_print("You can't see your map.");
	else {
	    int                 cy, cx, p_y, p_x;
#ifdef TARGET
/* If in target_mode, player will not be given a chance to pick a direction.
 * So we save it, force it off, and then ask for the direction -CFT
 */
	    int temp = target_mode;
	    target_mode = FALSE;
#endif

	    y = char_row;
	    x = char_col;
	    if (get_panel(y, x, TRUE))
		prt_map();
	    cy = panel_row;
	    cx = panel_col;
	    for (;;) {
		p_y = panel_row;
		p_x = panel_col;
		if (p_y == cy && p_x == cx)
		    tmp_str[0] = '\0';
		else
		    (void)sprintf(tmp_str, "%s%s of",
			     p_y < cy ? " North" : p_y > cy ? " South" : "",
			      p_x < cx ? " West" : p_x > cx ? " East" : "");
		(void)sprintf(out_val,
      "Map sector [%d,%d], which is%s your sector. Look which direction?",
			      p_y, p_x, tmp_str);
		if (!get_dir(out_val, &dir_val))
		    break;

/* -CJS- Should really use the move function, but what the hell. This is nicer,
 * as it moves exactly to the same place in another section. The direction
 * calculation is not intuitive. Sorry.
 */
		for (;;) {
		    x += ((dir_val - 1) % 3 - 1) * SCREEN_WIDTH / 2;
		    y -= ((dir_val - 1) / 3 - 1) * SCREEN_HEIGHT / 2;
		    if (x < 0 || y < 0 || x >= cur_width || y >= cur_width) {
			msg_print("You've gone past the end of your map.");
			x -= ((dir_val - 1) % 3 - 1) * SCREEN_WIDTH / 2;
			y += ((dir_val - 1) / 3 - 1) * SCREEN_HEIGHT / 2;
			break;
		    }
		    if (get_panel(y, x, TRUE)) {
			prt_map();
			break;
		    }
		}
	    }
	/* Move to a new panel - but only if really necessary. */
	    if (get_panel(char_row, char_col, FALSE))
		prt_map();
#ifdef TARGET
	    target_mode = temp; /* restore target mode... */
#endif
	}
	free_turn_flag = TRUE;
	break;
      case 'R':			/* (R)est a while */
	rest();
	break;
      case '#':			/* (#) search toggle	(S)earch toggle */
	if (p_ptr->flags.status & PY_SEARCH)
	    search_off();
	else
	    search_on();
	free_turn_flag = TRUE;
	break;
      case CTRL('B'):		/* (^B) tunnel down left	(T 1) */
	tunnel(1);
	break;
      case CTRL('M'):		/* cr must be treated same as lf. */
      case CTRL('J'):		/* (^J) tunnel down		(T 2) */
	tunnel(2);
	break;
      case CTRL('N'):		/* (^N) tunnel down right	(T 3) */
	tunnel(3);
	break;
      case CTRL('H'):		/* (^H) tunnel left		(T 4) */
	tunnel(4);
	break;
      case CTRL('L'):		/* (^L) tunnel right		(T 6) */
	tunnel(6);
	break;
      case CTRL('Y'):		/* (^Y) tunnel up left		(T 7) */
	tunnel(7);
	break;
      case CTRL('K'):		/* (^K) tunnel up		(T 8) */
	tunnel(8);
	break;
      case CTRL('U'):		/* (^U) tunnel up right		(T 9) */
	tunnel(9);
	break;
      case 'z':			/* (z)ap a wand		(a)im a wand */
	aim();
	break;
      case 'a':			/* (a)ctivate a rod	(z)ap a rod */
	activate_rod();
	break;
      case 'M':
	screen_map();
	free_turn_flag = TRUE;
	break;
      case 'P':			/* (P)eruse a book	(B)rowse in a book */
	do_cmd_browse();
	free_turn_flag = TRUE;
	break;
      case 'c':			/* (c)lose an object */
	do_cmd_close();
	break;
      case 'd':			/* (d)rop something */
	inven_command('d');
	break;
      case 'e':			/* (e)quipment list */
	inven_command('e');
	break;
      case 't':			/* (t)hrow something	(f)ire something */
	do_cmd_fire();
	break;
      case 'i':			/* (i)nventory list */
	inven_command('i');
	break;
      case 'S':			/* (S)pike a door	(j)am a door */
	do_cmd_spike();
	break;
      case 'x':			/* e(x)amine surrounds	(l)ook about */
	look();
	free_turn_flag = TRUE;
	break;
      case 'm':			/* (m)agic spells */
	cast();
	break;
      case 'o':			/* (o)pen something */
	do_cmd_open();
	break;
      case 'p':			/* (p)ray */
	pray();
	break;
      case 'q':			/* (q)uaff */
	quaff();
	break;
      case 'r':			/* (r)ead */
	read_scroll();
	break;
      case 's':			/* (s)earch for a turn */
	search(char_row, char_col, p_ptr->misc.srh);
	break;
      case 'T':			/* (T)ake off something	(t)ake off */
	inven_command('t');
	break;
      case 'Z':			/* (Z)ap a staff	(u)se a staff */
	use();
	break;
      case 'V':			/* (V)ersion of game */
	helpfile(ANGBAND_VER);
	free_turn_flag = TRUE;
	break;
      case 'w':			/* (w)ear or wield */
	inven_command('w');
	break;
      case 'X':			/* e(X)change weapons	e(x)change */
	inven_command('x');
	break;
#ifdef ALLOW_CHECK_ARTIFACTS /* -CWS */
      case '~':
	if ((!wizard) && (dun_level != 0)) {
	    msg_print("You need to be on the town level to check artifacts!");
	    msg_print(NULL);		/* make sure can see the message -CWS */
	} else
	    artifact_check_no_file();
	break;
#endif
#ifdef ALLOW_CHECK_UNIQUES /* -CWS */
      case '|':
	do_cmd_check_uniques();
	break;
#endif
      default:
	if (wizard) {
	    free_turn_flag = TRUE; /* Wizard commands are free moves */
	    switch (com_val) {
	      case '\\':	   /* \ wizard help */
		helpfile(ANGBAND_WIZ_HELP);
	      case CTRL('A'):	   /* ^A = Cure all */
		(void)remove_all_curse();
		(void)cure_blindness();
		(void)cure_confusion();
		(void)cure_poison();
		(void)remove_fear();
		(void)res_stat(A_STR);
		(void)res_stat(A_INT);
		(void)res_stat(A_WIS);
		(void)res_stat(A_CON);
		(void)res_stat(A_DEX);
		(void)res_stat(A_CHR);
		(void)restore_level();
		(void)hp_player(2000);
		p_ptr->flags.food = PLAYER_FOOD_MAX;
		f_ptr = &p_ptr->flags;
		if (f_ptr->slow > 1)
		    f_ptr->slow = 1;
		if (f_ptr->image > 1)
		    f_ptr->image = 1;
		if (f_ptr->cut > 1)
		    f_ptr->cut = 1;
		if (f_ptr->stun > 1)
		    f_ptr->stun = 1;
		break;
	      case CTRL('D'):	/* ^D = up/down */
		if (command_count > 0) {
		    if (command_count > 99)
			i = 0;
		    else
			i = command_count;
		    command_count = 0;
		} else {
		    prt("Go to which level (0-10000) ? ", 0, 0);
		    i = (-1);
		    if (get_string(tmp_str, 0, 27, 10))
			i = atoi(tmp_str);
		    if (i > 10000)
			i = 10000;
		}
		if (i > -1) {
		    dun_level = i;
		    if (dun_level > 10000)
			dun_level = 10000;
		    new_level_flag = TRUE;
		} else
		    erase_line(MSG_LINE, 0);
		break;
	      case CTRL('E'):	/* ^E = wizchar */
		change_character();
		erase_line(MSG_LINE, 0); /* from um55 -CFT */
		break;
	      case CTRL('G'):	/* ^G = treasure */
		if (command_count > 0) {
		    i = command_count;
		    command_count = 0;
		} else
		    i = 1;
		random_object(char_row, char_col, i);
		prt_map();
		break;
	      case CTRL('I'):	/* ^I = identify */
		(void)ident_spell();
		break;
	      case CTRL('O'):	/* ^O = objects */
		print_objects();
		break;
	      case CTRL('T'):	/* ^T = teleport */
		teleport(100);
		break;
	      case CTRL('V'):	/* ^V special treasure */
		if (command_count > 0) {
		    i = command_count;
		    command_count = 0;
		} else
		    i = 1;
		special_random_object(char_row, char_col, i);
		prt_map();
		break;
	      case CTRL('Z'):	/* ^Z = genocide */
		(void)mass_genocide(FALSE);
		break;
	      case ':':
		map_area();
		break;
	      case '~':
		artifact_check_no_file();
		break;
	      case '|':
		do_cmd_check_uniques();
		break;
	      case '@':
		wizard_create();
		break;
	      case '$':	   /* $ = wiz light */
		wiz_lite(TRUE);
		break;
	      case '%':	   /* self-knowledge */
		self_knowledge();
		break;
	      case '&':	   /* & = summon  */
		y = char_row;
		x = char_col;
		(void)summon_monster(&y, &x, TRUE);
		process_monsters(FALSE);
		break;
	      case '*':		/* '*' = identify all up to a level */
		prt("Identify objects upto which level (0-200) ? ", 0, 0);
		i = (-1);
		if (get_string(tmp_str, 0, 47, 10))
		    i = atoi(tmp_str);
		if (i > 200)
		    i = 200;
		if (i > -1) {
		    int                 temp;
		    inven_type          inv;

		    for (temp = 0; temp < MAX_DUNGEON_OBJ; temp++) {
			if (objeci_list[temp].level <= i) {
			    invcopy(&inv, temp);
			    known1(&inv);
			}
		    }
		}
		erase_line(MSG_LINE, 0);
		break;
	      case '+':
		if (command_count > 0) {
		    p_ptr->misc.exp = command_count;
		    command_count = 0;
		} else if (p_ptr->misc.exp == 0)
		    p_ptr->misc.exp = 1;
		else
		    p_ptr->misc.exp = p_ptr->misc.exp * 2;
		prt_experience();
		break;
	      default:
		prt("Type '?' or '\\' for help.", 0, 0);
	    }
	} else {
	    prt("Type '?' for help.", 0, 0);
	    free_turn_flag = TRUE;
	}
    }
    last_command = com_val;
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



