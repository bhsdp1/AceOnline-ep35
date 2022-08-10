typedef union
{
	int					BOOLEAN;
	int					integer;
	char				string2048[2048];
	MONSTER_W_COUNT		monster_w_count;
	ITEM_W_COUNT		item_w_count;
	ITEM_W_COUNT_PROB	item_w_count_prob;
	QUEST_PAY_ITEM		quest_pay_item;
	INT_RANGE			integer_range;
	MAP_AREA			map_area;
} YYSTYPE;
#define	QUEST_TOK	258
#define	BASIC_INFO_TOK	259
#define	QUEST_INDEX_TOK	260
#define	QUEST_NAME_TOK	261
#define	QUEST_EPISODE_TYPE_TOK	262
#define	QUEST_PARTYTYPE_TOK	263
#define	QUEST_KIND_TOK	264
#define	QUEST_TYPE_TOK	265
#define	QUEST_START_TYPE_TOK	266
#define	QUEST_END_TYPE_TOK	267
#define	QUEST_PARTY_MEMBERS_TOK	268
#define	QUEST_NPC_INFO_TOK	269
#define	NPC_INDEX_TOK	270
#define	NPC_NAME_TOK	271
#define	PRE_TALK_TOK	272
#define	MID_TALK_TOK	273
#define	FAIL_TALK_TOK	274
#define	SUCCESS_TALK_TOK	275
#define	DESCRIPTION_TOK	276
#define	EVENT_MAP_TOK	277
#define	NEXT_QUEST_INDEX_TOK	278
#define	PARTY_QUEST_MEMBER_COUNT_TOK	279
#define	DROP_ITEM_TO_ALL_ATTACK_CHARACTER_TOK	280
#define	TIME_LIMIT_IN_MINUTES_TOK	281
#define	DISCARDABLE_QUEST_TOK	282
#define	BI_QUEST_ATTRI_IS_CITYWAR_TOK	283
#define	BI_QUEST_ATTRI_NEVER_COMPLETION_BY_USER_TOK	284
#define	DROP_ITEM_TO_ALL_PARTY_MEMBERS_TOK	285
#define	START_REQ_TOK	286
#define	S_REQ_UNIT_KIND_TOK	287
#define	S_REQ_LEVEL_TOK	288
#define	S_REQ_ATTACK_PART_TOK	289
#define	S_REQ_DEFENSE_PART_TOK	290
#define	S_REQ_FUEL_PART_TOK	291
#define	S_REQ_SOUL_PART_TOK	292
#define	S_REQ_SHIELD_PART_TOK	293
#define	S_REQ_DODGE_PART_TOK	294
#define	S_REQ_ITEM_TOK	295
#define	S_REQ_QUEST_INDEX_TOK	296
#define	S_REQ_RECV_QUEST_INDEX_TOK	297
#define	S_REQ_GUILD_TOK	298
#define	S_REQ_INFLUENCE_TYPE_TOK	299
#define	S_REQ_START_AREA_TOK	300
#define	START_ACTION_TOK	301
#define	S_PAY_ITEM_TOK	302
#define	S_DEL_ITEM_TOK	303
#define	S_START_MAP_INDEX_TOK	304
#define	S_START_PARTY_WARP_FLAG_TOK	305
#define	COMPLETION_REQ_TOK	306
#define	C_REQ_MEET_NPC_INFO_TOK	307
#define	C_REQ_ITEM_TOK	308
#define	C_REQ_MEET_OBJ_INDEX_TOK	309
#define	C_REQ_DEAD_BY_MONSTER_TOK	310
#define	C_REQ_MEET_MAP_AREA_TOK	311
#define	C_REQ_MONSTER_TOK	312
#define	C_REQ_MIX_ITEM_TARGET_TOK	313
#define	COMPLETION_ACTION_TOK	314
#define	C_PAY_BONUS_STAT_TOK	315
#define	C_PAY_BONUS_SKILL_POINT_TOK	316
#define	C_PAY_FAME_TOK	317
#define	C_PAY_STATUS_TOK	318
#define	C_PAY_LEVEL_TOK	319
#define	C_PAY_EXPERIENCE_TOK	320
#define	C_PAY_WARP_MAP_INDEX_TOK	321
#define	C_DEL_ITEM_TOK	322
#define	C_PAY_ITEM_TOK	323
#define	C_PAY_ITEM_MISSIONMASTER_TOK	324
#define	C_RANDOM_PAY_ITEM_TOK	325
#define	C_MIX_ITEM_TOK	326
#define	C_PAY_PROPENSITY_TOK	327
#define	C_CINEMA_NUM_TOK	328
#define	C_PAY_INFLUENCE_TYPE_TOK	329
#define	FROM_TOK	330
#define	TO_TOK	331
#define	MONSTER_TOK	332
#define	CHARACTER_TOK	333
#define	ALL_TOK	334
#define	ATTACH_TOK	335
#define	AREA_TOK	336
#define	X_TOK	337
#define	Y_TOK	338
#define	R_TOK	339
#define	PER_TOK	340
#define	PROB_100_TOK	341
#define	PROB_1K_TOK	342
#define	PROB_10K_TOK	343
#define	PROB_100K_TOK	344
#define	PROB_256_TOK	345
#define	AND_TOK	346
#define	OR_TOK	347
#define	TRUE_TOK	348
#define	FALSE_TOK	349
#define	R_ARROW_TOK	350
#define	GT_TOK	351
#define	LT_TOK	352
#define	GE_TOK	353
#define	LE_TOK	354
#define	EQUAL_TOK	355
#define	USER_VARIABLE_TOK	356
#define	INTEGER_TOK	357
#define	WORD_TOK	358
#define	QSTRING_TOK	359


extern YYSTYPE QuestLoaderlval;
