int unique_weapon(inven_type *t_ptr)
{
    cptr name;

    if (be_nasty)
	return 0;
    name = objeci_list[t_ptr->index].name;
    if (!stricmp("& Longsword", name)) {
	switch (randint(15)) {
	  case 1:
	    if (RINGIL)
		return 0;
	    if (wizard || peek)
		msg_print("Ringil");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_RINGIL;
	    t_ptr->tohit = 22;
	    t_ptr->todam = 25;
	    t_ptr->dd = 4;
	    t_ptr->flags = (TR3_SEE_INVIS | TR1_SLAY_UNDEAD | TR1_SLAY_EVIL | TR3_REGEN |
		     TR1_SPEED | TR2_RES_COLD | TR1_BRAND_COLD | TR2_FREE_ACT |
			    TR3_SLOW_DIGEST);
	    t_ptr->flags2 |= (TR1_SLAY_DEMON | TR_SLAY_TROLL | TR_LIGHT | TR_ACTIVATE
			      | TR2_RES_LITE | TR_ARTIFACT);
	    t_ptr->p1 = 1;
	    t_ptr->cost = 300000L;
	    RINGIL = 1;
	    return 1;
	  case 2:
	  case 3:
	  case 4:
	    if (ANDURIL)
		return 0;
	    if (wizard || peek)
		msg_print("Anduril");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_ANDURIL;
	    t_ptr->tohit = 10;
	    t_ptr->todam = 15;
	    t_ptr->flags = (TR3_SEE_INVIS | TR1_SLAY_EVIL | TR2_FREE_ACT |
		     TR_SUST_STAT | TR1_STR | TR2_RES_FIRE | TR1_BRAND_FIRE);
	    t_ptr->flags2 |= (TR1_SLAY_TROLL | TR_ACTIVATE | TR_SLAY_MF2_ORC | TR_ARTIFACT);
	    t_ptr->p1 = 4;
	    t_ptr->toac = 5;
	    t_ptr->cost = 80000L;
	    ANDURIL = 1;
	    return 1;
	  case 5:
	  case 6:
	  case 7:
	  case 8:
	    if (ANGUIREL)
		return 0;
	    if (wizard || peek)
		msg_print("Anguirel");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_ANGUIREL;
	    t_ptr->tohit = 8;
	    t_ptr->todam = 12;
	    t_ptr->flags = (TR3_SEE_INVIS | TR1_SLAY_EVIL | TR2_FREE_ACT | TR2_RES_LIGHT
			    | TR1_STR | TR1_CON);
	    t_ptr->flags2 |= (TR_ARTIFACT |
		       TR_LIGHTNING | TR_LIGHT | TR1_SLAY_DEMON | TR2_RES_LITE);
	    t_ptr->p1 = 2;
	    t_ptr->cost = 40000L;
	    ANGUIREL = 1;
	    return 1;
	  default:
	    if (ELVAGIL)
		return 0;
	    if (wizard || peek)
		msg_print("Elvagil");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_ELVAGIL;
	    t_ptr->ident |= ID_NOSHOW_TYPE;
	    t_ptr->tohit = 2;
	    t_ptr->todam = 7;
	    t_ptr->flags |= (TR3_SEE_INVIS | TR1_CHR | TR1_DEX | TR1_STEALTH | TR_FFALL);
	    t_ptr->flags2 |= (TR1_SLAY_TROLL | TR_SLAY_MF2_ORC | TR_ARTIFACT);
	    t_ptr->p1 = 2;
	    t_ptr->cost = 30000L;
	    ELVAGIL = 1;
	    return 1;
	}
    } else if (!stricmp("& Two-Handed Sword", name)) {
	switch (randint(8)) {
	  case 1:
	  case 2:
	    if (GURTHANG)
		return 0;
	    if (wizard || peek)
		msg_print("Gurthang");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_GURTHANG;
	    t_ptr->tohit = 13;
	    t_ptr->todam = 17;
	    t_ptr->flags = (TR3_REGEN | TR1_KILL_DRAGON | TR1_STR |
			    TR2_FREE_ACT | TR3_SLOW_DIGEST);
	    t_ptr->flags2 |= (TR1_SLAY_TROLL | TR_ARTIFACT);
	    t_ptr->p1 = 2;
	    t_ptr->cost = 100000L;
	    GURTHANG = 1;
	    return 1;
	  case 3:
	    if (ZARCUTHRA)
		return 0;
	    if (randint(3) > 1)
		return 0;
	    if (wizard || peek)
		msg_print("Zarcuthra");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_ZARCUTHRA;
	    t_ptr->ident |= ID_NOSHOW_TYPE;
	    t_ptr->tohit = 19;
	    t_ptr->todam = 21;
	    t_ptr->flags = (TR1_KILL_DRAGON | TR1_STR | TR1_SLAY_EVIL | TR1_SLAY_ANIMAL |
		  TR1_SLAY_UNDEAD | TR3_AGGRAVATE | TR1_CHR | TR1_BRAND_FIRE |
		  TR3_SEE_INVIS | TR2_RES_FIRE | TR2_FREE_ACT | TR1_INFRA);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR1_SLAY_TROLL | TR_SLAY_MF2_ORC | TR_SLAY_GIANT
			      | TR1_SLAY_DEMON | TR2_RES_CHAOS);
	    t_ptr->p1 = 4;
	    t_ptr->dd = 6;
	    t_ptr->ds = 4;
	    t_ptr->cost = 200000L;
	    ZARCUTHRA = 1;
	    return 1;
	  default:
	    if (MORMEGIL)
		return 0;
	    if (wizard || peek)
		msg_print("Mormegil");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_MORMEGIL;
	    t_ptr->tohit = -40;
	    t_ptr->todam = -60;
	    t_ptr->flags = (TR1_SPEED | TR3_AGGRAVATE | TR_CURSED);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->p1 = -1;
	    t_ptr->toac = -50;
	    t_ptr->cost = 10000L;
	    MORMEGIL = 1;
	    return 1;
	}
    } else if (!stricmp("& Broadsword", name)) {
	switch (randint(12)) {
	  case 1:
	  case 2:
	    if (ARUNRUTH)
		return 0;
	    if (wizard || peek)
		msg_print("Arunruth");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_ARUNRUTH;
	    t_ptr->tohit = 20;
	    t_ptr->todam = 12;
	    t_ptr->dd = 3;
	    t_ptr->flags = (TR_FFALL | TR1_DEX |
			    TR2_FREE_ACT | TR3_SLOW_DIGEST);
	    t_ptr->flags2 |= (TR1_SLAY_DEMON | TR_SLAY_MF2_ORC | TR_ACTIVATE | TR_ARTIFACT);
	    t_ptr->p1 = 4;
	    t_ptr->cost = 50000L;
	    ARUNRUTH = 1;
	    return 1;
	  case 3:
	  case 4:
	  case 5:
	  case 6:
	    if (GLAMDRING)
		return 0;
	    if (wizard || peek)
		msg_print("Glamdring");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_GLAMDRING;
	    t_ptr->tohit = 10;
	    t_ptr->todam = 15;
	    t_ptr->flags = (TR1_SLAY_EVIL | TR3_SLOW_DIGEST | TR1_SEARCH | TR1_BRAND_FIRE |
			    TR2_RES_FIRE);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR1_SLAY_ORC | TR_LIGHT | TR2_RES_LITE);
	    t_ptr->p1 = 3;
	    t_ptr->cost = 40000L;
	    GLAMDRING = 1;
	    return 1;
	  case 7:
	    if (AEGLIN)
		return 0;
	    if (wizard || peek)
		msg_print("Aeglin");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_AEGLIN;
	    t_ptr->tohit = 12;
	    t_ptr->todam = 16;
	    t_ptr->flags = (TR3_SLOW_DIGEST | TR1_SEARCH | TR2_RES_LIGHT);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR1_SLAY_ORC | TR_LIGHT | TR_LIGHTNING);
	    t_ptr->p1 = 4;
	    t_ptr->cost = 45000L;
	    AEGLIN = 1;
	    return 1;
	  default:
	    if (ORCRIST)
		return 0;
	    if (wizard || peek)
		msg_print("Orcrist");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_ORCRIST;
	    t_ptr->tohit = 10;
	    t_ptr->todam = 15;
	    t_ptr->flags = (TR1_SLAY_EVIL | TR3_SLOW_DIGEST | TR1_STEALTH | TR1_BRAND_COLD |
			    TR2_RES_COLD);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR1_SLAY_ORC | TR_LIGHT);
	    t_ptr->p1 = 3;
	    t_ptr->cost = 40000L;
	    ORCRIST = 1;
	    return 1;
	}
    } else if (!stricmp("& Bastard Sword", name)) {
	if (CALRIS)
	    return 0;
	if (wizard || peek)
	    msg_print("Calris");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_CALRIS;
	t_ptr->tohit = -20;
	t_ptr->todam = 20;
	t_ptr->dd = 3;
	t_ptr->ds = 7;
	t_ptr->flags = (TR1_KILL_DRAGON | TR1_CON | TR3_AGGRAVATE |
			TR_CURSED | TR1_SLAY_EVIL);
	t_ptr->flags2 |= (TR1_SLAY_DEMON | TR_SLAY_TROLL | TR2_RES_DISEN
			  | TR_ARTIFACT);
	t_ptr->p1 = 5;
	t_ptr->cost = 100000L;
	CALRIS = 1;
	return 1;
    } else if (!stricmp("& Main Gauche", name)) {
	if (randint(4) > 1)
	    return 0;
	if (MAEDHROS)
	    return 0;
	if (wizard || peek)
	    msg_print("Maedhros");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_MAEDHROS;
	t_ptr->tohit = 12;
	t_ptr->todam = 15;
	t_ptr->dd = 2;
	t_ptr->ds = 6;
	t_ptr->flags = (TR1_DEX | TR1_INT | TR2_FREE_ACT | TR3_SEE_INVIS);
	t_ptr->flags2 |= (TR_ARTIFACT | TR1_SLAY_GIANT | TR_SLAY_TROLL);
	t_ptr->p1 = 3;
	t_ptr->cost = 20000L;
	MAEDHROS = 1;
	return 1;
    } else if (!stricmp("& Glaive", name)) {
	if (randint(3) > 1)
	    return 0;
	if (PAIN)
	    return 0;
	if (wizard || peek)
	    msg_print("Pain!");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_PAIN;
	t_ptr->tohit = 0;
	t_ptr->todam = 30;
	t_ptr->dd = 10;
	t_ptr->ds = 6;
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->cost = 50000L;
	PAIN = 1;
	return 1;
    } else if (!stricmp("& Halberd", name)) {
	if (OSONDIR)
	    return 0;
	if (wizard || peek)
	    msg_print("Osondir");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_OSONDIR;
	t_ptr->tohit = 6;
	t_ptr->todam = 9;
	t_ptr->flags = (TR1_BRAND_FIRE | TR1_SLAY_UNDEAD | TR2_RES_FIRE |
			TR_FFALL | TR1_CHR | TR3_SEE_INVIS);
	t_ptr->flags2 |= (TR_ARTIFACT | TR2_RES_SOUND | TR1_SLAY_GIANT);
	t_ptr->p1 = 3;
	t_ptr->cost = 22000L;
	OSONDIR = 1;
	return 1;
    } else if (!stricmp("& Lucerne Hammer", name)) {
	if (randint(2) > 1)
	    return 0;
	if (TURMIL)
	    return 0;
	if (wizard || peek)
	    msg_print("Turmil");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_TURMIL;
	t_ptr->ident |= ID_NOSHOW_TYPE;
	t_ptr->tohit = 10;
	t_ptr->todam = 6;
	t_ptr->flags = (TR1_WIS | TR3_REGEN | TR1_BRAND_COLD | TR2_RES_COLD | TR1_INFRA);
	t_ptr->flags2 |= (TR_ARTIFACT | TR1_SLAY_ORC | TR_LIGHT |
			  TR_ACTIVATE | TR2_RES_LITE);
	t_ptr->p1 = 4;
	t_ptr->cost = 30000L;
	t_ptr->toac = 8;
	TURMIL = 1;
	return 1;
    } else if (!stricmp("& Pike", name)) {
	if (randint(2) > 1)
	    return 0;
	if (TIL)
	    return 0;
	if (wizard || peek)
	    msg_print("Til-i-arc");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_TIL;
	t_ptr->tohit = 10;
	t_ptr->todam = 12;
	t_ptr->toac = 10;
	t_ptr->flags = (TR1_BRAND_COLD | TR1_BRAND_FIRE | TR2_RES_FIRE | TR_RES_COLD |
			TR3_SLOW_DIGEST | TR1_INT | TR_SUST_STAT);
	t_ptr->flags2 |= (TR_ARTIFACT | TR1_SLAY_DEMON | TR_SLAY_GIANT | TR_SLAY_TROLL);
	t_ptr->p1 = 2;
	t_ptr->cost = 32000L;
	TIL = 1;
	return 1;
    } else if (!stricmp("& Mace of Disruption", name)) {
	if (randint(5) > 1)
	    return 0;
	if (DEATHWREAKER)
	    return 0;
	if (wizard || peek)
	    msg_print("Deathwreaker");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_DEATHWREAKER;
	t_ptr->tohit = 18;
	t_ptr->todam = 18;
	t_ptr->ds = 12;
	t_ptr->flags = (TR1_STR | TR1_BRAND_FIRE | TR1_SLAY_EVIL | TR1_SLAY_DRAGON |
		   TR1_SLAY_ANIMAL | TR1_TUNNEL | TR3_AGGRAVATE | TR2_RES_FIRE);
	t_ptr->flags2 |= (TR_ARTIFACT | TR2_IM_FIRE | TR2_RES_CHAOS
			  | TR2_RES_DISEN | TR_RES_DARK);
	t_ptr->p1 = 6;
	t_ptr->cost = 400000L;
	DEATHWREAKER = 1;
	return 1;
    } else if (!stricmp("& Scythe", name)) {
	if (AVAVIR)
	    return 0;
	if (wizard || peek)
	    msg_print("Avavir");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_AVAVIR;
	t_ptr->tohit = 8;
	t_ptr->todam = 8;
	t_ptr->toac = 10;
	t_ptr->flags = (TR1_DEX | TR1_CHR | TR2_FREE_ACT | TR2_RES_FIRE | TR_RES_COLD |
			TR3_SEE_INVIS | TR1_BRAND_FIRE | TR1_BRAND_COLD);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_LIGHT | TR_ACTIVATE | TR2_RES_LITE);
	t_ptr->p1 = 3;
	t_ptr->cost = 18000L;
	AVAVIR = 1;
	return 1;
    } else if (!stricmp("& Mace", name)) {
	if (randint(2) > 1)
	    return 0;
	if (TARATOL)
	    return 0;
	if (wizard || peek)
	    msg_print("Taratol");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_TARATOL;
	t_ptr->tohit = 12;
	t_ptr->todam = 12;
	t_ptr->weight = 200;
	t_ptr->ds = 7;
	t_ptr->flags = (TR1_KILL_DRAGON | TR2_RES_LIGHT);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_LIGHTNING | TR_ACTIVATE | TR2_RES_DARK);
	t_ptr->cost = 20000L;
	TARATOL = 1;
	return 1;
    } else if (!stricmp("& Lance", name)) {
	if (randint(3) > 1)
	    return 0;
	if (EORLINGAS)
	    return 0;
	if (wizard || peek)
	    msg_print("Lance of Eorlingas");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_EORLINGAS;
	t_ptr->tohit = 3;
	t_ptr->todam = 21;
	t_ptr->weight = 360;
	t_ptr->flags |= (TR3_SEE_INVIS | TR1_SLAY_EVIL | TR1_DEX);
	t_ptr->flags2 |= (TR1_SLAY_TROLL | TR_SLAY_MF2_ORC | TR_ARTIFACT);
	t_ptr->p1 = 2;
	t_ptr->ds = 12;
	t_ptr->cost = 55000L;
	EORLINGAS = 1;
	return 1;
    } else if (!stricmp("& Broad Axe", name)) {
	if (BARUKKHELED)
	    return 0;
	if (wizard || peek)
	    msg_print("Barukkheled");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_BARUKKHELED;
	t_ptr->tohit = 13;
	t_ptr->todam = 19;
	t_ptr->flags |= (TR3_SEE_INVIS | TR1_SLAY_EVIL | TR1_CON);
	t_ptr->flags2 |= (TR1_SLAY_ORC | TR_SLAY_TROLL | TR_SLAY_GIANT | TR_ARTIFACT);
	t_ptr->p1 = 3;
	t_ptr->cost = 50000L;
	BARUKKHELED = 1;
	return 1;
    } else if (!stricmp("& Trident", name)) {
	switch (randint(3)) {
	  case 1:
	  case 2:
	    if (randint(3) > 1)
		return 0;
	    if (WRATH)
		return 0;
	    if (wizard || peek)
		msg_print("Wrath");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_WRATH;
	    t_ptr->tohit = 16;
	    t_ptr->todam = 18;
	    t_ptr->weight = 300;
	    t_ptr->dd = 3;
	    t_ptr->ds = 9;
	    t_ptr->flags |= (TR3_SEE_INVIS | TR1_SLAY_EVIL | TR1_STR | TR1_DEX |
			     TR1_SLAY_UNDEAD);
	    t_ptr->flags2 |= (TR2_RES_DARK | TR_RES_LT | TR_ARTIFACT | TR_BLESS_BLADE);
	    t_ptr->p1 = 2;
	    t_ptr->cost = 90000L;
	    WRATH = 1;
	    return 1;
	  case 3:
	    if (randint(4) > 1)
		return 0;
	    if (ULMO)
		return 0;
	    if (wizard || peek)
		msg_print("Ulmo");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_ULMO;
	    t_ptr->tohit = 15;
	    t_ptr->todam = 19;
	    t_ptr->dd = 4;
	    t_ptr->ds = 10;
	    t_ptr->flags = (TR3_SEE_INVIS | TR2_FREE_ACT | TR1_DEX | TR3_REGEN |
			    TR3_SLOW_DIGEST | TR1_SLAY_ANIMAL | TR_SLAY_DRAGON |
			    TR2_RES_ACID);
	    t_ptr->flags2 |= (TR2_IM_ACID | TR_HOLD_LIFE | TR_ACTIVATE
			    | TR2_RES_NETHER | TR_ARTIFACT | TR_BLESS_BLADE);
	    t_ptr->p1 = 4;
	    t_ptr->cost = 120000L;
	    ULMO = 1;
	    return 1;
	}
    } else if (!stricmp("& Scimitar", name)) {
	if (HARADEKKET)
	    return 0;
	if (wizard || peek)
	    msg_print("Haradekket");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_HARADEKKET;
	t_ptr->tohit = 9;
	t_ptr->todam = 11;
	t_ptr->flags |= (TR3_SEE_INVIS | TR1_SLAY_EVIL | TR1_DEX | TR1_SLAY_UNDEAD
			 | TR1_SLAY_ANIMAL);
	t_ptr->flags2 |= (TR_ARTIFACT | TR1_ATTACK_SPD);
	t_ptr->p1 = 2;
	t_ptr->cost = 30000L;
	HARADEKKET = 1;
	return 1;
    } else if (!stricmp("& Lochaber Axe", name)) {
	if (MUNDWINE)
	    return 0;
	if (wizard || peek)
	    msg_print("Mundwine");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_MUNDWINE;
	t_ptr->tohit = 12;
	t_ptr->todam = 17;
	t_ptr->flags |= (TR1_SLAY_EVIL | TR2_RES_FIRE | TR_RES_COLD
			 | TR2_RES_LIGHT | TR2_RES_ACID);
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->cost = 30000L;
	MUNDWINE = 1;
	return 1;
    } else if (!stricmp("& Cutlass", name)) {
	if (GONDRICAM)
	    return 0;
	if (wizard || peek)
	    msg_print("Gondricam");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_GONDRICAM;
	t_ptr->ident |= ID_NOSHOW_TYPE;
	t_ptr->tohit = 10;
	t_ptr->todam = 11;
	t_ptr->flags |= (TR3_SEE_INVIS | TR_FFALL | TR3_REGEN | TR1_STEALTH | TR2_RES_FIRE |
			 TR2_RES_COLD | TR2_RES_ACID | TR_RES_LIGHT | TR1_DEX);
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->p1 = 3;
	t_ptr->cost = 28000L;
	GONDRICAM = 1;
	return 1;
    } else if (!stricmp("& Sabre", name)) {
	if (CARETH)
	    return 0;
	if (wizard || peek)
	    msg_print("Careth Asdriag");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_CARETH;
	t_ptr->tohit = 6;
	t_ptr->todam = 8;
	t_ptr->flags |= (TR1_SLAY_DRAGON | TR_SLAY_ANIMAL);
	t_ptr->flags2 |= (TR1_SLAY_GIANT | TR_SLAY_MF2_ORC | TR_SLAY_TROLL | TR_ARTIFACT |
			  TR1_ATTACK_SPD);
	t_ptr->p1 = 1;
	t_ptr->cost = 25000L;
	CARETH = 1;
	return 1;
    } else if (!stricmp("& Rapier", name)) {
	if (FORASGIL)
	    return 0;
	if (wizard || peek)
	    msg_print("Forasgil");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_FORASGIL;
	t_ptr->tohit = 12;
	t_ptr->todam = 19;
	t_ptr->flags |= (TR2_RES_COLD | TR1_BRAND_COLD | TR1_SLAY_ANIMAL);
	t_ptr->flags2 |= (TR_LIGHT | TR2_RES_LITE | TR_ARTIFACT);
	t_ptr->cost = 15000L;
	FORASGIL = 1;
	return 1;
    } else if (!stricmp("& Executioner's Sword", name)) {
	if (randint(2) > 1)
	    return 0;
	if (CRISDURIAN)
	    return 0;
	if (wizard || peek)
	    msg_print("Crisdurian");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_CRISDURIAN;
	t_ptr->tohit = 18;
	t_ptr->todam = 19;
	t_ptr->flags |= (TR3_SEE_INVIS | TR1_SLAY_EVIL | TR1_SLAY_UNDEAD | TR1_SLAY_DRAGON);
	t_ptr->flags2 |= (TR1_SLAY_GIANT | TR_SLAY_MF2_ORC | TR_SLAY_TROLL | TR_ARTIFACT);
	t_ptr->cost = 100000L;
	CRISDURIAN = 1;
	return 1;
    } else if (!stricmp("& Flail", name)) {
	if (TOTILA)
	    return 0;
	if (wizard || peek)
	    msg_print("Totila");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_TOTILA;
	t_ptr->tohit = 6;
	t_ptr->todam = 8;
	t_ptr->ds = 9;
	t_ptr->flags = (TR1_STEALTH | TR2_RES_FIRE | TR1_BRAND_FIRE | TR1_SLAY_EVIL);
	t_ptr->flags2 |= (TR_ARTIFACT | TR_ACTIVATE | TR2_RES_CONF);
	t_ptr->p1 = 2;
	t_ptr->cost = 55000L;
	TOTILA = 1;
	return 1;
    } else if (!stricmp("& Short sword", name)) {
	if (GILETTAR)
	    return 0;
	if (wizard || peek)
	    msg_print("Gilettar");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_GILETTAR;
	t_ptr->tohit = 3;
	t_ptr->todam = 7;
	t_ptr->flags = (TR3_REGEN | TR3_SLOW_DIGEST | TR1_SLAY_ANIMAL);
	t_ptr->flags2 |= (TR_ARTIFACT | TR1_ATTACK_SPD);
	t_ptr->p1 = 2;
	t_ptr->cost = 15000L;
	GILETTAR = 1;
	return 1;
    } else if (!stricmp("& Katana", name)) {
	if (randint(3) > 1)
	    return 0;
	if (AGLARANG)
	    return 0;
	if (wizard || peek)
	    msg_print("Aglarang");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_AGLARANG;
	t_ptr->tohit = 0;
	t_ptr->todam = 0;
	t_ptr->dd = 6;
	t_ptr->ds = 8;
	t_ptr->weight = 50;
	t_ptr->flags = (TR1_DEX | TR_SUST_STAT);
	t_ptr->flags2 |= (TR_ARTIFACT);
	t_ptr->p1 = 5;
	t_ptr->cost = 40000L;
	AGLARANG = 1;
	return 1;
    } else if (!stricmp("& Spear", name)) {
	switch (randint(6)) {
	  case 1:
	    if (AEGLOS)
		return 0;
	    if (wizard || peek)
		msg_print("Aeglos");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_AEGLOS;
	    t_ptr->tohit = 15;
	    t_ptr->todam = 25;
	    t_ptr->dd = 1;
	    t_ptr->ds = 20;
	    t_ptr->flags = (TR1_WIS | TR1_BRAND_COLD |
			    TR2_RES_COLD | TR2_FREE_ACT | TR3_SLOW_DIGEST);
	    t_ptr->flags2 |= (TR1_SLAY_TROLL | TR_SLAY_MF2_ORC | TR_ACTIVATE | TR_ARTIFACT |
			      TR_BLESS_BLADE);
	    t_ptr->toac = 5;
	    t_ptr->p1 = 4;
	    t_ptr->cost = 140000L;
	    AEGLOS = 1;
	    return 1;
	  case 2:
	  case 3:
	  case 4:
	  case 5:
	    if (NIMLOTH)
		return 0;
	    if (wizard || peek)
		msg_print("Nimloth");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_NIMLOTH;
	    t_ptr->tohit = 11;
	    t_ptr->todam = 13;
	    t_ptr->flags = (TR1_BRAND_COLD | TR2_RES_COLD | TR1_SLAY_UNDEAD |
			    TR3_SEE_INVIS | TR1_STEALTH);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->p1 = 3;
	    t_ptr->cost = 30000L;
	    NIMLOTH = 1;
	    return 1;
	  case 6:
	    if (OROME)
		return 0;
	    if (wizard || peek)
		msg_print("Orome");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_OROME;
	    t_ptr->ident |= ID_NOSHOW_TYPE;
	    t_ptr->tohit = 15;
	    t_ptr->todam = 15;
	    t_ptr->flags = (TR1_BRAND_FIRE | TR3_SEE_INVIS | TR1_SEARCH | TR1_INT |
			    TR2_RES_FIRE | TR_FFALL | TR1_INFRA);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_LIGHT | TR1_SLAY_GIANT | TR2_RES_LITE
			      | TR_ARTIFACT | TR_BLESS_BLADE);
	    t_ptr->p1 = 4;
	    t_ptr->cost = 60000L;
	    OROME = 1;
	    return 1;
	}
    } else if (!stricmp("& Dagger", name)) {
	switch (randint(11)) {
	  case 1:
	    if (ANGRIST)
		return 0;
	    if (wizard || peek)
		msg_print("Angrist");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_ANGRIST;
	    t_ptr->tohit = 10;
	    t_ptr->todam = 15;
	    t_ptr->dd = 2;
	    t_ptr->ds = 5;
	    t_ptr->flags = (TR1_DEX | TR1_SLAY_EVIL | TR_SUST_STAT |
			    TR2_FREE_ACT);
	    t_ptr->flags2 |= (TR1_SLAY_TROLL | TR_SLAY_MF2_ORC | TR2_RES_DARK | TR_ARTIFACT);
	    t_ptr->toac = 5;
	    t_ptr->p1 = 4;
	    t_ptr->cost = 100000L;
	    ANGRIST = 1;
	    return 1;
	  case 2:
	  case 3:
	    if (NARTHANC)
		return 0;
	    if (wizard || peek)
		msg_print("Narthanc");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_NARTHANC;
	    t_ptr->tohit = 4;
	    t_ptr->todam = 6;
	    t_ptr->flags = (TR1_BRAND_FIRE | TR2_RES_FIRE);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
	    t_ptr->cost = 12000;
	    NARTHANC = 1;
	    return 1;
	  case 4:
	  case 5:
	    if (NIMTHANC)
		return 0;
	    if (wizard || peek)
		msg_print("Nimthanc");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_NIMTHANC;
	    t_ptr->tohit = 4;
	    t_ptr->todam = 6;
	    t_ptr->flags = (TR1_BRAND_COLD | TR2_RES_COLD);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
	    t_ptr->cost = 11000L;
	    NIMTHANC = 1;
	    return 1;
	  case 6:
	  case 7:
	    if (DETHANC)
		return 0;
	    if (wizard || peek)
		msg_print("Dethanc");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_DETHANC;
	    t_ptr->tohit = 4;
	    t_ptr->todam = 6;
	    t_ptr->flags = (TR2_RES_LIGHT);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_LIGHTNING | TR_ARTIFACT);
	    t_ptr->cost = 13000L;
	    DETHANC = 1;
	    return 1;
	  case 8:
	  case 9:
	    if (RILIA)
		return 0;
	    if (wizard || peek)
		msg_print("Rilia");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_RILIA;
	    t_ptr->tohit = 4;
	    t_ptr->todam = 3;
	    t_ptr->dd = 2;
	    t_ptr->ds = 4;
	    t_ptr->flags = TR_POISON;
	    t_ptr->flags2 |= (TR_ACTIVATE | TR2_RES_DISEN | TR_ARTIFACT);
	    t_ptr->cost = 15000L;
	    RILIA = 1;
	    return 1;
	  case 10:
	  case 11:
	    if (BELANGIL)
		return 0;
	    if (wizard || peek)
		msg_print("Belangil");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_BELANGIL;
	    t_ptr->tohit = 6;
	    t_ptr->todam = 9;
	    t_ptr->dd = 3;
	    t_ptr->ds = 2;
	    t_ptr->flags = (TR1_BRAND_COLD | TR2_RES_COLD | TR3_REGEN | TR3_SLOW_DIGEST |
			    TR1_DEX | TR3_SEE_INVIS);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
	    t_ptr->p1 = 2;
	    t_ptr->cost = 40000L;
	    BELANGIL = 1;
	    return 1;
	}
    } else if (!stricmp("& Small sword", name)) {
	if (STING)
	    return 0;
	if (wizard || peek)
	    msg_print("Sting");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_STING;
	t_ptr->tohit = 7;
	t_ptr->todam = 8;
	t_ptr->flags |= (TR3_SEE_INVIS | TR1_SLAY_EVIL |
			 TR1_SLAY_UNDEAD | TR1_DEX | TR1_CON | TR1_STR |
			 TR2_FREE_ACT);
	t_ptr->flags2 |= (TR_ARTIFACT | TR1_SLAY_ORC | TR_LIGHT | TR2_RES_LITE |
			  TR1_ATTACK_SPD);
	t_ptr->p1 = 2;
	t_ptr->cost = 100000L;
	STING = 1;
	return 1;
    } else if (!stricmp("& Great Axe", name)) {
	switch (randint(2)) {
	  case 1:
	    if (randint(6) > 1)
		return 0;
	    if (DURIN)
		return 0;
	    if (wizard || peek)
		msg_print("Durin");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_DURIN;
	    t_ptr->tohit = 10;
	    t_ptr->todam = 20;
	    t_ptr->toac = 15;
	    t_ptr->flags = (TR1_KILL_DRAGON | TR1_CON | TR2_FREE_ACT |
			    TR2_RES_FIRE | TR2_RES_ACID);
	    t_ptr->flags2 |= (TR1_SLAY_DEMON | TR_SLAY_TROLL | TR_SLAY_MF2_ORC | TR2_RES_DARK
			      | TR2_RES_LITE | TR_RES_CHAOS | TR_ARTIFACT);
	    t_ptr->p1 = 3;
	    t_ptr->cost = 150000L;
	    DURIN = 1;
	    return 1;
	  case 2:
	    if (randint(8) > 1)
		return 0;
	    if (EONWE)
		return 0;
	    if (wizard || peek)
		msg_print("Eonwe");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_EONWE;
	    t_ptr->tohit = 15;
	    t_ptr->todam = 18;
	    t_ptr->toac = 8;
	    t_ptr->flags = (TR_STATS | TR1_SLAY_EVIL | TR1_SLAY_UNDEAD | TR1_BRAND_COLD |
			    TR2_FREE_ACT | TR3_SEE_INVIS | TR2_RES_COLD);
	    t_ptr->flags2 |= (TR2_IM_COLD | TR1_SLAY_ORC | TR_ACTIVATE | TR_ARTIFACT |
			      TR_BLESS_BLADE);
	    t_ptr->p1 = 2;
	    t_ptr->cost = 200000L;
	    EONWE = 1;
	    return 1;
	}
    } else if (!stricmp("& Battle Axe", name)) {
	switch (randint(2)) {
	  case 1:
	    if (BALLI)
		return 0;
	    if (wizard || peek)
		msg_print("Balli Stonehand");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_BALLI;
	    t_ptr->ident |= ID_NOSHOW_TYPE;
	    t_ptr->tohit = 8;
	    t_ptr->todam = 11;
	    t_ptr->dd = 3;
	    t_ptr->ds = 6;
	    t_ptr->toac = 5;
	    t_ptr->flags = (TR_FFALL | TR2_RES_LIGHT | TR3_SEE_INVIS | TR1_STR | TR1_CON
			    | TR2_FREE_ACT | TR2_RES_COLD | TR2_RES_ACID
			    | TR2_RES_FIRE | TR3_REGEN | TR1_STEALTH);
	    t_ptr->flags2 |= (TR1_SLAY_DEMON | TR_SLAY_TROLL | TR_SLAY_MF2_ORC | TR2_RES_BLIND
			      | TR_ARTIFACT);
	    t_ptr->p1 = 3;
	    t_ptr->cost = 90000L;
	    BALLI = 1;
	    return 1;
	  case 2:
	    if (LOTHARANG)
		return 0;
	    if (wizard || peek)
		msg_print("Lotharang");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_LOTHARANG;
	    t_ptr->tohit = 4;
	    t_ptr->todam = 3;
	    t_ptr->flags = (TR1_STR | TR1_DEX);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR1_SLAY_TROLL | TR_SLAY_MF2_ORC | TR_ARTIFACT);
	    t_ptr->p1 = 1;
	    t_ptr->cost = 21000L;
	    LOTHARANG = 1;
	    return 1;
	}
    } else if (!stricmp("& War Hammer", name)) {
	if (randint(10) > 1)
	    return 0;
	if (AULE)
	    return 0;
	if (wizard || peek)
	    msg_print("Aule");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_AULE;
	t_ptr->dd = 5;
	t_ptr->ds = 5;
	t_ptr->tohit = 19;
	t_ptr->todam = 21;
	t_ptr->toac = 5;
	t_ptr->flags = (TR1_KILL_DRAGON | TR1_SLAY_EVIL | TR1_SLAY_UNDEAD |
		    TR2_RES_FIRE | TR2_RES_ACID | TR_RES_COLD | TR_RES_LIGHT |
			TR2_FREE_ACT | TR3_SEE_INVIS | TR1_WIS);
	t_ptr->flags2 |= (TR_ARTIFACT | TR1_SLAY_DEMON | TR_LIGHTNING | TR2_RES_NEXUS);
	t_ptr->p1 = 4;
	t_ptr->cost = 250000L;
	AULE = 1;
	return 1;
    } else if (!stricmp("& Beaked Axe", name)) {
	if (randint(2) > 1)
	    return 0;
	if (THEODEN)
	    return 0;
	if (wizard || peek)
	    msg_print("Theoden");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_THEODEN;
	t_ptr->tohit = 8;
	t_ptr->todam = 10;
	t_ptr->flags = (TR1_WIS | TR1_CON | TR1_SEARCH | TR3_SLOW_DIGEST | TR1_SLAY_DRAGON);
	t_ptr->flags2 |= (TR3_TELEPATHY | TR_ACTIVATE | TR_ARTIFACT);
	t_ptr->ident |= ID_NOSHOW_TYPE;
	t_ptr->p1 = 3;
	t_ptr->cost = 40000L;
	THEODEN = 1;
	return 1;
    } else if (!stricmp("& Two-Handed Great Flail", name)) {
	if (randint(5) > 1)
	    return 0;
	if (THUNDERFIST)
	    return 0;
	if (wizard || peek)
	    msg_print("Thunderfist");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_THUNDERFIST;
	t_ptr->tohit = 5;
	t_ptr->todam = 18;
	t_ptr->flags = (TR1_SLAY_ANIMAL | TR1_STR | TR1_BRAND_FIRE |
			TR2_RES_FIRE | TR_RES_LIGHT);
	t_ptr->flags2 |= (TR_ARTIFACT | TR1_SLAY_TROLL | TR_SLAY_MF2_ORC
			  | TR_LIGHTNING | TR2_RES_DARK);
	t_ptr->p1 = 4;
	t_ptr->cost = 160000L;
	THUNDERFIST = 1;
	return 1;
    } else if (!stricmp("& Morningstar", name)) {
	switch (randint(2)) {
	  case 1:
	    if (randint(2) > 1)
		return 0;
	    if (BLOODSPIKE)
		return 0;
	    if (wizard || peek)
		msg_print("Bloodspike");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_BLOODSPIKE;
	    t_ptr->tohit = 8;
	    t_ptr->todam = 22;
	    t_ptr->flags = (TR1_SLAY_ANIMAL | TR1_STR | TR3_SEE_INVIS);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR1_SLAY_TROLL | TR_SLAY_MF2_ORC | TR2_RES_NEXUS);
	    t_ptr->p1 = 4;
	    t_ptr->cost = 30000L;
	    BLOODSPIKE = 1;
	    return 1;
	  case 2:
	    if (FIRESTAR)
		return 0;
	    if (wizard || peek)
		msg_print("Firestar");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_FIRESTAR;
	    t_ptr->tohit = 5;
	    t_ptr->todam = 7;
	    t_ptr->flags = (TR1_BRAND_FIRE | TR2_RES_FIRE);
	    t_ptr->flags2 |= (TR_ACTIVATE | TR_ARTIFACT);
	    t_ptr->toac = 2;
	    t_ptr->cost = 35000L;
	    FIRESTAR = 1;
	    return 1;
	}
    } else if (!stricmp("& Blade of Chaos", name)) {
	if (DOOMCALLER)
	    return 0;
	if (randint(3) > 1)
	    return 0;
	if (wizard || peek)
	    msg_print("Doomcaller");
	else
	    good_item_flag = TRUE;
	t_ptr->name2 = ART_DOOMCALLER;
	t_ptr->tohit = 18;
	t_ptr->todam = 28;
	t_ptr->flags = (TR1_CON | TR1_SLAY_ANIMAL | TR1_KILL_DRAGON |
		TR1_BRAND_COLD | TR1_SLAY_EVIL | TR2_FREE_ACT | TR3_SEE_INVIS |
		    TR2_RES_FIRE | TR_RES_COLD | TR_RES_LIGHT | TR2_RES_ACID |
			TR3_AGGRAVATE);
	t_ptr->flags2 |= (TR1_SLAY_TROLL | TR_SLAY_MF2_ORC | TR3_TELEPATHY | TR_ARTIFACT);
	t_ptr->p1 = -5;
	t_ptr->cost = 200000L;
	DOOMCALLER = 1;
	return 1;
    } else if (!stricmp("& Quarterstaff", name)) {
	switch (randint(7)) {
	  case 1:
	  case 2:
	  case 3:
	    if (NAR)
		return 0;
	    if (wizard || peek)
		msg_print("Nar-i-vagil");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_NAR;
	    t_ptr->tohit = 10;
	    t_ptr->todam = 20;
	    t_ptr->flags = (TR1_INT | TR1_SLAY_ANIMAL | TR1_BRAND_FIRE | TR2_RES_FIRE);
	    t_ptr->flags2 |= (TR_ARTIFACT);
	    t_ptr->p1 = 3;
	    t_ptr->cost = 70000L;
	    NAR = 1;
	    return 1;
	  case 4:
	  case 5:
	  case 6:
	    if (ERIRIL)
		return 0;
	    if (wizard || peek)
		msg_print("Eriril");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_ERIRIL;
	    t_ptr->tohit = 3;
	    t_ptr->todam = 5;
	    t_ptr->flags = (TR1_SLAY_EVIL | TR3_SEE_INVIS | TR1_INT | TR1_WIS);
	    t_ptr->flags2 |= (TR_LIGHT | TR_ACTIVATE | TR2_RES_LITE | TR_ARTIFACT);
	    t_ptr->p1 = 4;
	    t_ptr->cost = 20000L;
	    ERIRIL = 1;
	    return 1;
	  case 7:
	    if (OLORIN)
		return 0;
	    if (randint(2) > 1)
		return 0;
	    if (wizard || peek)
		msg_print("Olorin");
	    else
		good_item_flag = TRUE;
	    t_ptr->name2 = ART_OLORIN;
	    t_ptr->tohit = 10;
	    t_ptr->todam = 13;
	    t_ptr->dd = 2;
	    t_ptr->ds = 10;
	    t_ptr->flags = (TR1_SLAY_EVIL | TR3_SEE_INVIS | TR1_WIS | TR1_INT | TR1_CHR
			    | TR1_BRAND_FIRE | TR2_RES_FIRE);
	    t_ptr->flags2 |= (TR_ARTIFACT | TR_HOLD_LIFE | TR1_SLAY_ORC | TR_SLAY_TROLL
			      | TR_ACTIVATE | TR2_RES_NETHER);
	    t_ptr->p1 = 4;
	    t_ptr->cost = 130000L;
	    OLORIN = 1;
	    return 1;
	}
    }
    return 0;
}
