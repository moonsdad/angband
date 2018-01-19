int unique_armour(inven_type *t_ptr)
{
    cptr name;

    if (be_nasty)
	return 0;
    name = k_list[t_ptr->index].name;
    if (!strncmp("Adamantite", name, 10)) {
	if (SOULKEEPER)
	    return 0;
	if (randint(3) > 1)
	    return 0;
	if (wizard || peek)
	    msg_print("Soulkeeper");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR2_RES_ACID | TR2_RES_COLD);
	t_ptr->flags2 |= (TR_HOLD_LIFE | TR_ACTIVATE | TR2_RES_CHAOS | TR_RES_DARK |
			  TR2_RES_NEXUS | TR_RES_NETHER | TR_ARTIFACT);
	t_ptr->name2 = ART_SOULKEEPER;
	t_ptr->toac = 20;
	t_ptr->cost = 300000L;
	SOULKEEPER = 1;
	return 1;
    }
     /* etc..... */ 
    else if (!strncmp("Multi-Hued", name, 10)) {
	if (RAZORBACK)
	    return 0;
	if (randint(3) > 1)
	    return 0;
	if (wizard || peek)
	    msg_print("Razorback");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR2_RES_FIRE | TR_RES_COLD | TR2_RES_ACID | TR2_RES_POIS |
			 TR2_RES_ELEC | TR2_FREE_ACT | TR3_SEE_INVIS | TR1_INT |
			 TR1_WIS | TR1_STEALTH | TR3_AGGRAVATE);
	t_ptr->flags2 |= (TR_ACTIVATE | TR3_LITE | TR2_IM_ELEC | TR2_RES_LITE |
			  TR_ARTIFACT);
	t_ptr->ident |= ID_NOSHOW_TYPE;
	t_ptr->toac = 25;
	t_ptr->pval = -2;
	t_ptr->weight = 400;
	t_ptr->ac = 30;
	t_ptr->tohit = -3;
	t_ptr->cost = 400000L;
	t_ptr->name2 = ART_RAZORBACK;
	RAZORBACK = 1;
	return 1;
    } else if (!strncmp("Power Drag", name, 10)) {
	if (BLADETURNER)
	    return 0;
	if (wizard || peek)
	    msg_print("Bladeturner");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR2_RES_FIRE | TR_RES_COLD | TR2_RES_ACID | TR2_RES_POIS |
			 TR2_RES_ELEC | TR1_DEX | TR1_SEARCH | TR3_REGEN);
	t_ptr->flags2 |= (TR_HOLD_LIFE | TR2_RES_CONF | TR_RES_SOUND | TR_RES_LT
	      | TR2_RES_DARK | TR_RES_CHAOS | TR_RES_DISENCHANT | TR_ARTIFACT
	       | TR2_RES_SHARDS | TR_RES_BLIND | TR_RES_NEXUS | TR_RES_NETHER
			  | TR_ACTIVATE);
	t_ptr->ident |= ID_NOSHOW_TYPE;
	t_ptr->toac = 35;
	t_ptr->pval = -3;
	t_ptr->ac = 50;
	t_ptr->tohit = -4;
	t_ptr->weight = 500;
	t_ptr->cost = 500000L;
	t_ptr->name2 = ART_BLADETURNER;
	BLADETURNER = 1;
	return 1;
    } else if (!stricmp("& Pair of Hard Leather Boots", name)) {
	if (FEANOR)
	    return 0;
	if (randint(5) > 1)
	    return 0;
	if (wizard || peek)
	    msg_print("Feanor");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR2_RES_ACID | TR1_SPEED | TR1_STEALTH);
	t_ptr->flags2 |= (TR_ACTIVATE | TR2_RES_NEXUS | TR_ARTIFACT);
	t_ptr->name2 = ART_FEANOR;
	t_ptr->pval = 1;
	t_ptr->toac = 20;
	t_ptr->cost = 130000L;
	FEANOR = 1;
	return 1;
    } else if (!stricmp("& Pair of Soft Leather Boots", name)) {
	if (DAL)
	    return 0;
	if (wizard || peek)
	    msg_print("Dal-i-thalion");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR2_FREE_ACT | TR1_DEX | TR_SUST_STAT | TR2_RES_ACID);
	t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT | TR2_RES_NETHER | TR_RES_CHAOS);
	t_ptr->name2 = ART_DAL;
	t_ptr->pval = 5;
	t_ptr->toac = 15;
	t_ptr->cost = 40000L;
	DAL = 1;
	return 1;
    } else if (!stricmp("& Small Metal Shield", name)) {
	if (THORIN)
	    return 0;
	if (randint(2) > 1)
	    return 0;
	if (wizard || peek)
	    msg_print("Thorin");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR1_CON | TR2_FREE_ACT | TR1_STR |
			 TR2_RES_ACID | TR1_SEARCH);
	t_ptr->flags2 |= (TR2_RES_SOUND | TR_RES_CHAOS | TR_ARTIFACT | TR2_IM_ACID);
	t_ptr->name2 = ART_THORIN;
	t_ptr->ident |= ID_NOSHOW_TYPE;
	t_ptr->tohit = 0;
	t_ptr->pval = 4;
	t_ptr->toac = 25;
	t_ptr->cost = 60000L;
	THORIN = 1;
	return 1;
    } else if (!stricmp("Full Plate Armour", name)) {
	if (ISILDUR)
	    return 0;
	if (wizard || peek)
	    msg_print("Isildur");
	else
	    good_item_flag = TRUE;
	t_ptr->weight = 300;
	t_ptr->flags |= (TR2_RES_ACID | TR2_RES_FIRE | TR_RES_COLD | TR_RES_LIGHT);
	t_ptr->flags2 |= (TR2_RES_SOUND | TR_ARTIFACT | TR_RES_NEXUS);
	t_ptr->name2 = ART_ISILDUR;
	t_ptr->tohit = 0;
	t_ptr->toac = 25;
	t_ptr->cost = 40000L;
	ISILDUR = 1;
	return 1;
    } else if (!stricmp("Metal Brigandine Armour", name)) {
	if (ROHAN)
	    return 0;
	if (wizard || peek)
	    msg_print("Rohirrim");
	else
	    good_item_flag = TRUE;
	t_ptr->weight = 200;
	t_ptr->flags |= (TR2_RES_ACID | TR2_RES_FIRE | TR_RES_COLD | TR_RES_LIGHT |
			 TR1_STR | TR1_DEX);
	t_ptr->flags2 |= (TR2_RES_SOUND | TR_RES_CONF | TR_ARTIFACT);
	t_ptr->name2 = ART_ROHIRRIM;
	t_ptr->tohit = 0;
	t_ptr->pval = 2;
	t_ptr->toac = 15;
	t_ptr->cost = 30000L;
	ROHAN = 1;
	return 1;
    } else if (!stricmp("& Large Metal Shield", name)) {
	if (ANARION)
	    return 0;
	if (randint(3) > 1)
	    return 0;
	else
	    good_item_flag = TRUE;
	if (wizard || peek)
	    msg_print("Anarion");
	t_ptr->flags |= (TR2_RES_ACID | TR2_RES_FIRE | TR_RES_COLD | TR_RES_LIGHT |
			 TR_SUST_STAT);
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->name2 = ART_ANARION;
	t_ptr->pval = 10;
	t_ptr->ident |= ID_NOSHOW_P1;
	t_ptr->tohit = 0;
	t_ptr->toac = 20;
	t_ptr->cost = 160000L;
	ANARION = 1;
	return 1;
    } else if (!stricmp("& Set of Cesti", name)) {
	if (FINGOLFIN)
	    return 0;
	if (randint(3) > 1)
	    return 0;
	if (wizard || peek)
	    msg_print("Fingolfin");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR2_RES_ACID | TR1_DEX | TR2_FREE_ACT);
	t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
	t_ptr->name2 = ART_FINGOLFIN;
	t_ptr->ident |= ID_SHOW_HITDAM;
	t_ptr->pval = 4;
	t_ptr->tohit = 10;
	t_ptr->todam = 10;
	t_ptr->toac = 20;
	t_ptr->cost = 110000L;
	FINGOLFIN = 1;
	return 1;
    } else if (!stricmp("& Set of Leather Gloves", name)) {
	if (randint(3) == 1) {
	    if (CAMBELEG)
		return 0;
	    if (wizard || peek)
		msg_print("Cambeleg");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags |= (TR1_STR | TR1_CON | TR2_FREE_ACT);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->name2 = ART_CAMBELEG;
	    t_ptr->ident |= ID_SHOW_HITDAM;
	    t_ptr->pval = 2;
	    t_ptr->tohit = 8;
	    t_ptr->todam = 8;
	    t_ptr->toac = 15;
	    t_ptr->cost = 36000L;
	    CAMBELEG = 1;
	    return 1;
	} else {
	    if (CAMMITHRIM)
		return 0;
	    if (wizard || peek)
		msg_print("Cammithrim");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags |= (TR_SUST_STAT | TR2_FREE_ACT);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR3_LITE | TR2_RES_LITE | TR_ARTIFACT);
	    t_ptr->name2 = ART_CAMMITHRIM;
	    t_ptr->ident |= ID_NOSHOW_P1;
	    t_ptr->pval = 5;
	    t_ptr->toac = 10;
	    t_ptr->cost = 30000L;
	    CAMMITHRIM = 1;
	    return 1;
	}
    } else if (!stricmp("& Set of Gauntlets", name)) {
	switch (randint(6)) {
	  case 1:
	    if (PAURHACH)
		return 0;
	    if (wizard || peek)
		msg_print("Paurhach");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags |= TR2_RES_FIRE;
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
	    t_ptr->name2 = ART_PAURHACH;
	    t_ptr->toac = 15;
	    t_ptr->cost = 15000L;
	    PAURHACH = 1;
	    return 1;
	  case 2:
	    if (PAURNIMMEN)
		return 0;
	    if (wizard || peek)
		msg_print("Paurnimmen");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags |= TR2_RES_COLD;
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
	    t_ptr->name2 = ART_PAURNIMMEN;
	    t_ptr->toac = 15;
	    t_ptr->cost = 13000L;
	    PAURNIMMEN = 1;
	    return 1;
	  case 3:
	    if (PAURAEGEN)
		return 0;
	    if (wizard || peek)
		msg_print("Pauraegen");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags |= TR2_RES_ELEC;
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
	    t_ptr->name2 = ART_PAURAEGEN;
	    t_ptr->toac = 15;
	    t_ptr->cost = 11000L;
	    PAURAEGEN = 1;
	    return 1;
	  case 4:
	    if (PAURNEN)
		return 0;
	    if (wizard || peek)
		msg_print("Paurnen");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags |= TR2_RES_ACID;
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
	    t_ptr->name2 = ART_PAURNEN;
	    t_ptr->toac = 15;
	    t_ptr->cost = 12000L;
	    PAURNEN = 1;
	    return 1;
	  default:
	    if (CAMLOST)
		return 0;
	    if (wizard || peek)
		msg_print("Camlost");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags |= (TR1_STR | TR1_DEX | TR3_AGGRAVATE | TR_CURSED);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->name2 = ART_CAMLOST;
	    t_ptr->toac = 0;
	    t_ptr->pval = -5;
	    t_ptr->tohit = -11;
	    t_ptr->todam = -12;
	    t_ptr->ident |= (ID_SHOW_HITDAM/* | ID_SHOW_P1*/);
	    t_ptr->cost = 0L;
	    CAMLOST = 1;
	    return 1;
	}
    } else if (!stricmp("Mithril Chain Mail", name)) {
	if (BELEGENNON)
	    return 0;
	if (wizard || peek)
	    msg_print("Belegennon");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR2_RES_ACID | TR2_RES_FIRE | TR_RES_COLD |
			 TR2_RES_ELEC | TR1_STEALTH);
	t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
	t_ptr->name2 = ART_BELEGENNON;
	t_ptr->pval = 4;
	t_ptr->toac = 20;
	t_ptr->cost = 105000L;
	BELEGENNON = 1;
	return 1;
    } else if (!stricmp("Mithril Plate Mail", name)) {
	if (CELEBORN)
	    return 0;
	if (wizard || peek)
	    msg_print("Celeborn");
	else
	    good_item_flag = TRUE;
	t_ptr->weight = 250;
	t_ptr->flags |= (TR2_RES_ACID | TR2_RES_FIRE | TR_RES_COLD | TR_RES_LIGHT |
			 TR1_STR | TR1_CHR);
	t_ptr->flags2 |= (TR_ACTIVATE | TR2_RES_DISEN | TR_RES_DARK | TR_ARTIFACT);
	t_ptr->name2 = ART_CELEBORN;
	t_ptr->pval = 4;
	t_ptr->toac = 25;
	t_ptr->cost = 150000L;
	CELEBORN = 1;
	return 1;
    } else if (!stricmp("Augmented Chain Mail", name)) {
	if (randint(3) > 1)
	    return 0;
	if (CASPANION)
	    return 0;
	if (wizard || peek)
	    msg_print("Caspanion");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR2_RES_ACID | TR2_RES_POIS | TR1_CON | TR1_WIS | TR1_INT);
	t_ptr->flags2 |= (TR2_RES_CONF | TR_ACTIVATE | TR_ARTIFACT);
	t_ptr->name2 = ART_CASPANION;
	t_ptr->pval = 3;
	t_ptr->toac = 20;
	t_ptr->cost = 40000L;
	CASPANION = 1;
	return 1;
    } else if (!stricmp("Soft Leather Armour", name)) {
	if (HITHLOMIR)
	    return 0;
	if (wizard || peek)
	    msg_print("Hithlomir");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR2_RES_ACID | TR2_RES_FIRE | TR_RES_COLD | TR_RES_LIGHT |
			 TR1_STEALTH);
	t_ptr->flags2 |= (TR_ARTIFACT | TR2_RES_DARK);
	t_ptr->name2 = ART_HITHLOMIR;
	t_ptr->pval = 4;
	t_ptr->toac = 20;
	t_ptr->cost = 45000L;
	HITHLOMIR = 1;
	return 1;
    } else if (!stricmp("Leather Scale Mail", name)) {
	if (THALKETTOTH)
	    return 0;
	if (wizard || peek)
	    msg_print("Thalkettoth");
	else
	    good_item_flag = TRUE;
	t_ptr->weight = 60;
	t_ptr->flags |= (TR2_RES_ACID | TR1_DEX);
	t_ptr->flags2 |= (TR_ARTIFACT | TR2_RES_SHARDS);
	t_ptr->name2 = ART_THALKETTOTH;
	t_ptr->toac = 25;
	t_ptr->pval = 3;
	t_ptr->cost = 25000L;
	THALKETTOTH = 1;
	return 1;
    } else if (!stricmp("Chain Mail", name)) {
	if (ARVEDUI)
	    return 0;
	if (wizard || peek)
	    msg_print("Arvedui");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR2_RES_ACID | TR2_RES_FIRE | TR_RES_COLD | TR_RES_LIGHT |
			 TR1_STR | TR1_CHR);
	t_ptr->flags2 |= (TR_ARTIFACT | TR2_RES_NEXUS | TR_RES_SHARDS);
	t_ptr->name2 = ART_ARVEDUI;
	t_ptr->pval = 2;
	t_ptr->toac = 15;
	t_ptr->cost = 32000L;
	ARVEDUI = 1;
	return 1;
    } else if (!stricmp("& Hard Leather Cap", name)) {
	if (THRANDUIL)
	    return 0;
	if (wizard || peek)
	    msg_print("Thranduil");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR2_RES_ACID | TR1_INT | TR1_WIS);
	t_ptr->flags2 |= (TR3_TELEPATHY | TR2_RES_BLIND | TR_ARTIFACT);
	t_ptr->name2 = ART_THRANDUILM;
	t_ptr->pval = 2;
	t_ptr->toac = 10;
	t_ptr->cost = 50000L;
	THRANDUIL = 1;
	return 1;
    } else if (!stricmp("& Metal Cap", name)) {
	if (THENGEL)
	    return 0;
	if (wizard || peek)
	    msg_print("Thengel");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR2_RES_ACID | TR1_WIS | TR1_CHR);
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->name2 = ART_THENGEL;
	t_ptr->pval = 3;
	t_ptr->toac = 12;
	t_ptr->cost = 22000L;
	THENGEL = 1;
	return 1;
    } else if (!stricmp("& Steel Helm", name)) {
	if (HAMMERHAND)
	    return 0;
	if (wizard || peek)
	    msg_print("Hammerhand");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR1_STR | TR1_CON | TR1_DEX | TR2_RES_ACID);
	t_ptr->flags2 |= (TR_ARTIFACT | TR2_RES_NEXUS);
	t_ptr->name2 = ART_HAMMERHAND;
	t_ptr->pval = 3;
	t_ptr->toac = 20;
	t_ptr->cost = 45000L;
	HAMMERHAND = 1;
	return 1;
    } else if (!stricmp("& Large Leather Shield", name)) {
	if (CELEGORM)
	    return 0;
	if (wizard || peek)
	    msg_print("Celegorm");
	else
	    good_item_flag = TRUE;
	t_ptr->weight = 60;
	t_ptr->flags |= (TR2_RES_ACID | TR2_RES_FIRE | TR_RES_COLD | TR_RES_LIGHT);
	t_ptr->flags2 |= (TR2_RES_LITE | TR_RES_DARK | TR_ARTIFACT);
	t_ptr->name2 = ART_CELEGORM;
	t_ptr->toac = 20;
	t_ptr->cost = 12000L;
	CELEGORM = 1;
	return 1;
    } else if (!stricmp("& Pair of Metal Shod Boots", name)) {
	if (THROR)
	    return 0;
	if (wizard || peek)
	    msg_print("Thror");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR1_CON | TR1_STR | TR2_RES_ACID);
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->name2 = ART_THROR;
	t_ptr->pval = 3;
	t_ptr->toac = 20;
	t_ptr->cost = 12000L;
	THROR = 1;
	return 1;
    } else if (!stricmp("& Iron Helm", name)) {
	if (randint(6) == 1) {
	    if (DOR_LOMIN)
		return 0;
	    if (wizard || peek)
		msg_print("Dor-Lomin");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags |= (TR2_RES_ACID | TR2_RES_FIRE | TR_RES_COLD | TR_RES_LIGHT |
			     TR1_CON | TR1_DEX | TR1_STR | TR3_SEE_INVIS);
	    t_ptr->flags2 |= (TR3_TELEPATHY | TR3_LITE | TR2_RES_LITE | TR_RES_BLIND
			      | TR_ARTIFACT);
	    t_ptr->name2 = ART_DOR;
	    t_ptr->pval = 4;
	    t_ptr->toac = 20;
	    t_ptr->cost = 300000L;
	    DOR_LOMIN = 1;
	    return 1;
	} else if (randint(2) == 1) {
	    if (HOLHENNETH)
		return 0;
	    if (wizard || peek)
		msg_print("Holhenneth");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags |= (TR1_INT | TR1_WIS | TR3_SEE_INVIS | TR1_SEARCH | TR2_RES_ACID);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR2_RES_BLIND | TR_ARTIFACT);
	    t_ptr->name2 = ART_HOLHENNETH;
	    t_ptr->ident |= ID_NOSHOW_TYPE;
	    t_ptr->pval = 2;
	    t_ptr->toac = 10;
	    t_ptr->cost = 100000L;
	    HOLHENNETH = 1;
	    return 1;
	} else {
	    if (GORLIM)
		return 0;
	    if (wizard || peek)
		msg_print("Gorlim");
	    else
		good_item_flag = TRUE;
	    t_ptr->flags |= (TR1_INT | TR1_WIS | TR3_SEE_INVIS | TR1_SEARCH | TR_CURSED
			     | TR3_AGGRAVATE);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->name2 = ART_GORLIM;
	    t_ptr->ident |= ID_NOSHOW_TYPE;
	    t_ptr->pval = -125;
	    t_ptr->toac = 10;
	    t_ptr->cost = 0L;
	    GORLIM = 1;
	    return 1;
	}
    } else if (!stricmp("& Golden Crown", name)) {
	if (randint(3) > 1)
	    return 0;
	if (GONDOR)
	    return 0;
	if (wizard || peek)
	    msg_print("Gondor");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_GONDOR;
	t_ptr->flags = (TR1_STR | TR1_CON | TR1_WIS | TR3_SEE_INVIS | TR3_REGEN
			| TR2_RES_ACID | TR2_RES_FIRE);
	t_ptr->flags2 = (TR_ACTIVATE | TR3_LITE | TR2_RES_LITE | TR_RES_BLIND |
			 TR_ARTIFACT);
	t_ptr->pval = 3;
	t_ptr->toac = 15;
	t_ptr->cost = 100000L;
	GONDOR = 1;
	return 1;
    } else if (!stricmp("& Iron Crown", name)) {
	if (BERUTHIEL)
	    return 0;
	if (wizard || peek)
	    msg_print("Beruthiel");
	else
	    good_item_flag = TRUE;
	t_ptr->flags |= (TR1_STR | TR1_DEX | TR1_CON |
		      TR2_RES_ACID | TR3_SEE_INVIS | TR2_FREE_ACT | TR_CURSED);
	t_ptr->flags2 |= (TR3_TELEPATHY | TR_ARTIFACT);
	t_ptr->name2 = ART_BERUTHIEL;
	t_ptr->pval = -125;
	t_ptr->toac = 20;
	t_ptr->cost = 10000L;
	BERUTHIEL = 1;
	return 1;
    }
    return 0;
}
