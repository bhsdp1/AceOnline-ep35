#include "stdafx.h"
#include "FieldSkillManager.h"
#include "FieldIOCPSocket.h"
#include "AtumError.h"
#include "FieldParty.h"
#include "FieldGlobal.h"		// 2009-12-11 by cmkwon, 데미지 어그로로 타겟을 변경하는 몬스터 구현 - 

////////////////////////////////////////
// class CChargingSkillApplier
////////////////////////////////////////
CChargingSkillApplier::CChargingSkillApplier(ITEM **o_ppItemforProcessSplashAttack, BOOL i_bSplashAttack, ITEM *i_pAttackItem, CFieldIOCPSocket *i_pFieldIOCPSocket)
{
	m_pFieldIOCPSocket		= i_pFieldIOCPSocket;
	m_bChargingSkillApplied = i_pFieldIOCPSocket->m_SkillManager.ApplyChargingSkill(o_ppItemforProcessSplashAttack, i_bSplashAttack, i_pAttackItem, &m_ChargingSkillParamFactor);
}

CChargingSkillApplier::~CChargingSkillApplier()
{
	if (m_bChargingSkillApplied)
	{
		m_pFieldIOCPSocket->m_SkillManager.ReleaseChargingSkill(&m_ChargingSkillParamFactor);
	}
}

////////////////////////////////////////
// class CFieldSkillManager
////////////////////////////////////////
CFieldSkillManager::CFieldSkillManager()
{
	m_vectorAtivatedSkill.reserve(10);
	m_vecChargingSkill.reserve(10);

	m_bChargingSkill	= FALSE;		// 2007-04-02 by cmkwon
	m_bHyperShot		= FALSE;		// 2007-03-29 by cmkwon
	m_bPrepareUseBigBoom= FALSE;		// 2007-07-13 by dhjin

	m_nSkillConfirmUseUID	= 1;		// 2009-04-06 by cmkwon, 콜오브 히어로 스킬 시스템 변경 - 초기값
}

void CFieldSkillManager::InitFieldDataManager(CFieldIOCPSocket *i_pFieldIOCPSocket)
{
	m_pItemManager = &i_pFieldIOCPSocket->m_ItemManager;

	CFieldDataManager::InitFieldDataManager(i_pFieldIOCPSocket);
}

void CFieldSkillManager::ResetSkillData()
{
	m_vectorAtivatedSkill.clearLock();
	m_vecChargingSkill.clearLock();
	m_bChargingSkill	= FALSE;		// 2007-04-02 by cmkwon
	m_bHyperShot		= FALSE;		// 2007-03-29 by cmkwon
	m_bPrepareUseBigBoom= FALSE;		// 2007-07-13 by dhjin

	m_mtvectSkillConfirmUseList.clearLock();	// 2009-04-06 by cmkwon, 콜오브 히어로 스킬 시스템 변경 - 
	m_nSkillConfirmUseUID	= 1;				// 2009-04-06 by cmkwon, 콜오브 히어로 스킬 시스템 변경 - 
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CFieldSkillManager::ResetTimeLimitSkillData()
/// \brief		아레나 통합 - 시간제한형 스킬 초기화
/// \author		dhjin
/// \date		2008-02-25 ~ 2008-02-25
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CFieldSkillManager::ResetTimeLimitSkillData()
{
	mt_auto_lock alock(&m_vectorAtivatedSkill);

	mtvectorItemID::iterator itr = m_vectorAtivatedSkill.begin();
	while (itr != m_vectorAtivatedSkill.end())
	{
		ITEM *pSkillItemInfo = m_pFieldIOCPSocket->ms_pFieldIOCP->GetItemInfo(itr->ItemNum);

		if(NULL != pSkillItemInfo
			&& SKILLTYPE_TIMELIMIT == pSkillItemInfo->SkillType)
		{
			this->UnsetSkillParamFactor(pSkillItemInfo);

			itr = m_vectorAtivatedSkill.erase(itr);
			continue;
		}

		itr++;
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CFieldSkillManager::ResetToggleSkillData()
/// \brief		아레나 통합 - 토글 스킬형 초기화
/// \author		dhjin
/// \date		2008-02-25 ~ 2008-02-25
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CFieldSkillManager::ResetToggleSkillData()
{
	mt_auto_lock alock(&m_vectorAtivatedSkill);

	mtvectorItemID::iterator itr = m_vectorAtivatedSkill.begin();
	while (itr != m_vectorAtivatedSkill.end())
	{
		ITEM *pSkillItemInfo = m_pFieldIOCPSocket->ms_pFieldIOCP->GetItemInfo(itr->ItemNum);

		if(NULL != pSkillItemInfo
			&& SKILLTYPE_TOGGLE == pSkillItemInfo->SkillType)
		{
			this->UnsetSkillParamFactor(pSkillItemInfo);

			itr = m_vectorAtivatedSkill.erase(itr);

			const MEX_TIMER_EVENT *pTimerEvent = m_pFieldIOCPSocket->m_TimerManager.GetTimerEventByItemNum(pSkillItemInfo->ItemNum);
			if (pTimerEvent != NULL)
			{
				m_pFieldIOCPSocket->m_TimerManager.StopTimerField(pTimerEvent->TimerUID);
			}
			continue;
		}

		itr++;
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CFieldSkillManager::ResetChargingSkillData()
/// \brief		아레나 통합 - 차징 스킬형 초기화
/// \author		dhjin
/// \date		2008-02-25 ~ 2008-02-25
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CFieldSkillManager::ResetChargingSkillData()
{
	mt_auto_lock mtA(&m_vecChargingSkill);
	
	m_bChargingSkill	= FALSE;
	m_bHyperShot		= FALSE;		
	m_vecChargingSkill.clear();
}

BOOL CFieldSkillManager::IsSkillActivatedByUID(UID64_t i_nSkillUID)
{
	mt_auto_lock alock(&m_vectorAtivatedSkill);

	mtvectorItemID::iterator itr = m_vectorAtivatedSkill.begin();
	while (itr != m_vectorAtivatedSkill.end())
	{
		if (i_nSkillUID == itr->ItemUID)
		{
			return TRUE;
		}

		itr++;
	}

	return FALSE;
}

BOOL CFieldSkillManager::IsSkillActivatedByItemNum(INT i_nSkillItemNum)
{
	mt_auto_lock alock(&m_vectorAtivatedSkill);

	mtvectorItemID::iterator itr = m_vectorAtivatedSkill.begin();
	while (itr != m_vectorAtivatedSkill.end())
	{
		if (i_nSkillItemNum == itr->ItemNum)
		{
			return TRUE;
		}

		itr++;
	}

	return FALSE;
}

// returns: 0이면 없음, 0이 아니연 해당 스킬의 ItemNum
INT CFieldSkillManager::IsSkillActivatedBySkillBaseNum(INT i_nSkillItemNum)
{
	mt_auto_lock alock(&m_vectorAtivatedSkill);

	mtvectorItemID::iterator itr = m_vectorAtivatedSkill.begin();
	while (itr != m_vectorAtivatedSkill.end())
	{
		if (SKILL_BASE_NUM(i_nSkillItemNum) == SKILL_BASE_NUM(itr->ItemNum))
		{
			return itr->ItemNum;
		}

		itr++;
	}

	return 0;
}

BOOL CFieldSkillManager::ApplyAllPermanentAndAtivatedSkills()
{
	// 패시브 스킬 적용
	mt_auto_lock mtAuto(&m_pItemManager->m_mapItemGeneral);
	for(int i=0; i < m_pItemManager->m_vectItemSkillPtr.size(); i++)
	{
		ITEM_SKILL *pTmpSkill = m_pItemManager->m_vectItemSkillPtr[i];
		if (pTmpSkill->ItemInfo->SkillType == SKILLTYPE_PERMANENT)
		{
			SetSkillParamFactor(pTmpSkill->ItemInfo);
		}
	}
	mtAuto.auto_unlock_cancel();

	// 사용 중 스킬 적용
	m_vectorAtivatedSkill.lock();
	mtvectorItemID::iterator itr = m_vectorAtivatedSkill.begin();
	while (itr != m_vectorAtivatedSkill.end())
	{
		ItemID_t itemID = *itr;
		ITEM *pItemInfo = ms_pFieldIOCP2->GetItemInfo(itemID.ItemNum);
		if (pItemInfo != NULL)
		{
			SetSkillParamFactor(pItemInfo);
		}

		itr++;
	}
	m_vectorAtivatedSkill.unlock();

	return TRUE;
}

// check: 쓸모 있다 싶으면 살리기
ITEM_SKILL* CFieldSkillManager::GetActivatedSkillByUID(UID64_t i_nSkillUID)
{
	ASSERT_NOT_IMPLEMENTED_YET();
	return NULL;
}

// check: 쓸모 있다 싶으면 살리기
ITEM_SKILL* CFieldSkillManager::GetActivatedSkillByItemNum(INT i_nSkillItemNum)
{
	ASSERT_NOT_IMPLEMENTED_YET();
	return NULL;
}

BOOL CFieldSkillManager::UseSkill(ITEM_SKILL* i_pItemSkill, MSG_FC_SKILL_USE_SKILL *i_pMsgUseSkill)
{
	if (i_pItemSkill == NULL)
	{
		return FALSE;
	}
	
	// get skill targets
	// 2009-12-11 by cmkwon, 멤버 변수로 처리할 필요 없음 - 아래와 같이 수정함.
	//m_vectorTargetClients.clear();
	vectClientIndex_t	vectTargetList;
	GetSkillTargets(&vectTargetList, i_pMsgUseSkill->TargetIndex, i_pMsgUseSkill->TargetCharUID, i_pItemSkill->ItemInfo);

	///////////////////////////////////////////////////////////////////////////////
	// 2009-12-11 by cmkwon, 데미지 어그로로 타겟을 변경하는 몬스터 구현 - 
	setClientIndex_t	monsterIdxListForSkillAggro;

	BOOL bRet;
	switch(i_pItemSkill->ItemInfo->SkillType)
	{
	case SKILLTYPE_CLICK:
		bRet = UseSkillTypeClick(i_pItemSkill, i_pMsgUseSkill, &vectTargetList, &monsterIdxListForSkillAggro);			// 2009-12-11 by cmkwon, 데미지 어그로로 타겟을 변경하는 몬스터 구현 - 
		break;
	case SKILLTYPE_TIMELIMIT:
		bRet = UseSkillTypeTimeLimit(i_pItemSkill, i_pMsgUseSkill, &vectTargetList, &monsterIdxListForSkillAggro);		// 2009-12-11 by cmkwon, 데미지 어그로로 타겟을 변경하는 몬스터 구현 - 
		break;
	case SKILLTYPE_TOGGLE:
		bRet = UseSkillTypeToggle(i_pItemSkill, i_pMsgUseSkill, &monsterIdxListForSkillAggro);
		break;
	case SKILLTYPE_CHARGING:
		bRet = UseSkillTypeCharging(i_pItemSkill, i_pMsgUseSkill, &monsterIdxListForSkillAggro);
		break;
	default:
#ifdef _DEBUG
		m_pFieldIOCPSocket->SendString128(STRING_128_ADMIN_CMD, "ATTACKPROBABILITY01: %d @UseSkill", m_pFieldIOCPSocket->m_ParamFactor.pfp_ATTACKPROBABILITY_01);
#endif
		m_pFieldIOCPSocket->SendErrorMessage(T_FC_SKILL_USE_SKILL, ERR_PROTOCOL_CANNOT_USE_SKILL, i_pItemSkill->ItemNum);
		return FALSE;
	}

	// 2010-04-01 by cmkwon, 스킬 사용 실패 처리 - 
	if(FALSE == bRet)
	{
		return FALSE;
	}

	///////////////////////////////////////////////////////////////////////////////
	// 2009-12-11 by cmkwon, 데미지 어그로로 타겟을 변경하는 몬스터 구현 - 
	float fSkillDamageForAggro = i_pItemSkill->ItemInfo->GetSkillDamageForAggro();
	if(0.0f < fSkillDamageForAggro
		&& false == monsterIdxListForSkillAggro.empty())
	{
// 2009-12-10 by cmkwon, TEMP 
//		g_pFieldGlobal->WriteSystemLogEX(TRUE, "[TEMP] 091210 %s monstercountforAggro(%d) before unique \r\n"
//			, GetCharacterString(m_pFieldIOCPSocket->GetCharacter(), string()), monsterIdxListForSkillAggro.size());

		CFieldMapChannel *pCurFMChann = m_pFieldIOCPSocket->GetCurrentFieldMapChannel();
		if(pCurFMChann)
		{
			int nSentCnt = 0;
			setClientIndex_t::iterator itr(monsterIdxListForSkillAggro.begin());
			for(; itr != monsterIdxListForSkillAggro.end(); itr++)
			{
				ClientIndex_t monIdx = *itr;
				CFieldMonster *pFMon = pCurFMChann->GetFieldMonster(monIdx, 10000);
				if(NULL == pFMon
					|| FALSE == pFMon->IsValidMonster()
					|| ATTACKOBJ_AGGRO != pFMon->MonsterInfoPtr->AttackObject)
				{
					continue;
				}

// 2009-12-10 by cmkwon, TEMP 
//				g_pFieldGlobal->WriteSystemLogEX(TRUE, "[TEMP] 091210 %s monstercountforAggro(%d) MonsterIndex(%d) \r\n"
//					, GetCharacterString(m_pFieldIOCPSocket->GetCharacter(), string()), monsterIdxListForSkillAggro.size(), pFMon->MonsterIndex);
			
				///////////////////////////////////////////////////////////////////////
				// Attack 정보를 NPC에게 보냄
				INIT_MSG_WITH_BUFFER(MSG_FN_BATTLE_SET_ATTACK_CHARACTER, T_FN_BATTLE_SET_ATTACK_CHARACTER, pSetAttackChar, SendBuf);
				pSetAttackChar->ChannelIndex	= pCurFMChann->GetMapChannelIndex().ChannelIndex;
				pSetAttackChar->AttackIndex		= m_pFieldIOCPSocket->GetCharacter()->ClientIndex;
				pSetAttackChar->TargetIndex		= monIdx;
				pSetAttackChar->DamageAmount	= fSkillDamageForAggro;
				pSetAttackChar->ItemKind		= i_pItemSkill->Kind;
				pCurFMChann->Send2NPCServerW(SendBuf, MSG_SIZE(MSG_FN_BATTLE_SET_ATTACK_CHARACTER));

				nSentCnt++;
				if(10 <= nSentCnt)
				{
					break;
				}
			}

// 2009-12-10 by cmkwon, TEMP 
//			g_pFieldGlobal->WriteSystemLogEX(TRUE, "[TEMP] 091210 %s monstercountforAggro(%d) after unique, nSentCnt(%d) \r\n"
//				, GetCharacterString(m_pFieldIOCPSocket->GetCharacter(), string()), monsterIdxListForSkillAggro.size(), nSentCnt);
		}
	}


#ifdef _DEBUG
	m_pFieldIOCPSocket->SendString128(STRING_128_ADMIN_CMD, "ATTACKPROBABILITY01: %d @UseSkill", m_pFieldIOCPSocket->m_ParamFactor.pfp_ATTACKPROBABILITY_01);
#endif
	
	return bRet;
}

// 클릭형 스킬 처리
// 2009-12-11 by cmkwon, 데미지 어그로로 타겟을 변경하는 몬스터 구현 - 아래의 함수에 인자추가(, setClientIndex_t *o_pMonListForSkillAggro)
BOOL CFieldSkillManager::UseSkillTypeClick(ITEM_SKILL *i_pItemSkill, MSG_FC_SKILL_USE_SKILL *i_pMsgUseSkill, vector<ClientIndex_t> *i_pVectorClientIndex, setClientIndex_t *o_pMonListForSkillAggro)
{
	INIT_MSG_OF_SIZE(MSG_FC_SKILL_USE_SKILL_OK, T_FC_SKILL_USE_SKILL_OK, msgUseSkillOK, SendBuf);
	int nSuccessCnt	= 0;
	for (int i = 0; i < i_pVectorClientIndex->size(); i++)
	{
		ClientIndex_t nTmpClientIndex = (*i_pVectorClientIndex)[i];

		CFieldIOCPSocket *pTargetSocket = ms_pFieldIOCP2->GetFieldIOCPSocket(nTmpClientIndex);

		// 2011-06-27 by shcho, 아르헨티나 좀비모드 수정 - 콜오브 히어로 관련 수정
		if(i_pItemSkill->ItemNum == 7813031)
		{
			// 2011-07-11 by shcho, 아르헨티나 갓모드 로그 추가
			DbgOut("!!!!!!!!!!!!!!CALL OF HERO START!!!!!!!!!!!!!! \r\n");
			g_pFieldGlobal->WriteSystemLogEX(TRUE,"!!!!!!!!!!!!!!CALL OF HERO START!!!!!!!!!!!!!! \r\n");
			// end 2011-07-11 by shcho, 아르헨티나 갓모드 로그 추가
			if(pTargetSocket->GetCharacter()->CurrentHP <= 0)
			{
				// 2011-07-11 by shcho, 아르헨티나 갓모드 로그 추가
				DbgOut("CALL OF HERO HP ZERO!! \r\n");
				g_pFieldGlobal->WriteSystemLogEX(TRUE,"CALL OF HERO HP ZERO!! \r\n");
				// end 2011-07-11 by shcho, 아르헨티나 갓모드 로그 추가
				continue; // 피가 0이거나
			}
			if(COMPARE_BODYCON_BIT(pTargetSocket->GetCharacter()->BodyCondition,BODYCON_DEAD_MASK))
			{
				// 2011-07-11 by shcho, 아르헨티나 갓모드 로그 추가
				DbgOut("CALL OF HERO BODYCON_DEAD_MASK!! \r\n");
				g_pFieldGlobal->WriteSystemLogEX(TRUE,"CALL OF HERO BODYCON_DEAD_MASK!! \r\n");
				// end 2011-07-11 by shcho, 아르헨티나 갓모드 로그 추가
				continue; // 바디컨디션이 데드
			}
		}
		// end 2011-06-27 by shcho, 아르헨티나 좀비모드 수정 - 콜오브 히어로 관련 수정

		// 2010-12-29 by shcho, 맵 인덱스가 다르면 사용 불가
		if (NULL == pTargetSocket
			|| FALSE == pTargetSocket->IsValidCharacter()
 			|| pTargetSocket->IsInvisibleCharacter()		// 2007-10-02 by cmkwon, 투명 상태의 유저는 스킬 사용 및 적용 불가)
			|| i_pItemSkill->ItemNum !=	7813031)	// 7813031 : 콜 오브 히어로인지 체크
		{
			// 2011-07-11 by shcho, 아르헨티나 갓모드 로그 추가
			DbgOut("!!!!NOT CALL OF HERO SKILL!!!! \r\n");
			g_pFieldGlobal->WriteSystemLogEX(TRUE,"!!!!NOT CALL OF HERO SKILL!!!! \r\n");
			// end 2011-07-11 by shcho, 아르헨티나 갓모드 로그 추가

			// 콜 오브 히어로가 아닐 경우에는 이 체크를 해야한다.
			if(m_pFieldIOCPSocket->GetCurrentFieldMapChannel()->GetMapChannelIndex() != pTargetSocket->GetCurrentFieldMapChannel()->GetMapChannelIndex())
			{
				continue;
			}
			
			if (!pTargetSocket->m_SkillManager.CheckSkillReqItemKind(i_pItemSkill->ItemInfo))
			{
				continue;
			}
			// 2011-03-28 by hsson, 이미 죽은 상태의 기체에게 힐링 타겟 써지던 버그 수정
			if(COMPARE_BODYCON_BIT(pTargetSocket->GetCharacter()->BodyCondition,BODYCON_DEAD_MASK))
			{
				continue;
			}
			// END 2011-03-28 by hsson, 이미 죽은 상태의 기체에게 힐링 타겟 써지던 버그 수정
		}
		// end 2010-12-29 by shcho, 맵 인덱스가 다르면 사용 불가


		////////////////////////////////////////////////////////////////////////////////
		// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 저항 아이템 사용 체크
		if(COMPARE_BIT_FLAG(i_pItemSkill->ItemInfo->ItemAttribute, ITEM_ATTR_CHECK_SKILL_RESISTANCE_STATE) 
			&& TRUE == this->CheckResistance(pTargetSocket)) {
			// 저항 정보를 전송하자~!!
			INIT_MSG_WITH_BUFFER(MSG_FC_INFO_APPLY_RESISTANCE_ITEM, T_FC_INFO_APPLY_RESISTANCE_ITEM, ResistanceMsg, ResistanceSendBuf);
			ResistanceMsg->ResistanceUserIdx		= pTargetSocket->m_character.ClientIndex;
			ResistanceMsg->SkillNum					= i_pItemSkill->ItemInfo->ItemNum;	// 2009-09-09 ~ 2010-01-19 by dhjin, 인피니티 - 저항된 스킬 정보 추가
			ResistanceMsg->OnceResistanceItem		= FALSE;		// 2009-09-09 ~ 2010-01-19 by dhjin, 인피니티 - 한 번만 저항하고 없어지는 저항 아이템 추가
			pTargetSocket->SendInRangeMessageAroundMe(ResistanceSendBuf, MSG_SIZE(MSG_FC_INFO_APPLY_RESISTANCE_ITEM), TRUE);
			continue;
		}
		////////////////////////////////////////////////////////////////////////////////
		// 2009-09-09 ~ 2010-01-19 by dhjin, 인피니티 - 한 번만 저항하고 없어지는 저항 아이템 추가
		if(COMPARE_BIT_FLAG(i_pItemSkill->ItemInfo->ItemAttribute, ITEM_ATTR_CHECK_SKILL_RESISTANCE_STATE) 
			&& TRUE == this->CheckOnceResistance(pTargetSocket)) {
			// 저항 정보를 전송하자~!!
			INIT_MSG_WITH_BUFFER(MSG_FC_INFO_APPLY_RESISTANCE_ITEM, T_FC_INFO_APPLY_RESISTANCE_ITEM, ResistanceMsg, ResistanceSendBuf);
			ResistanceMsg->ResistanceUserIdx		= pTargetSocket->m_character.ClientIndex;
			ResistanceMsg->SkillNum					= i_pItemSkill->ItemInfo->ItemNum;	// 2009-09-09 ~ 2010-01-19 by dhjin, 인피니티 - 저항된 스킬 정보 추가
			ResistanceMsg->OnceResistanceItem		= TRUE;			// 2009-09-09 ~ 2010-01-19 by dhjin, 인피니티 - 한 번만 저항하고 없어지는 저항 아이템 추가
			pTargetSocket->SendInRangeMessageAroundMe(ResistanceSendBuf, MSG_SIZE(MSG_FC_INFO_APPLY_RESISTANCE_ITEM), TRUE);
			continue;
		}

		// 2007-07-11 by dhjin, 케릭터 상태이면 BigBoom 사용 불가 해제
		// 2009-04-21 by cmkwon, ITEM에 DesParam 필드 개수 8개로 늘리기 - ITEM::IsExistDesParam() 함수로 통합함
		//if(IS_EXIST_DES_PARAM(i_pItemSkill->ItemInfo, DES_SKILL_BIG_BOOM))
		if(i_pItemSkill->ItemInfo->IsExistDesParam(DES_SKILL_BIG_BOOM))
		{
			if(m_pFieldIOCPSocket->m_character.CharacterMode)
			{
				return FALSE;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// 2008-05-20 by dhjin, 빅붐 중인 대상 정화 스킬 사용시 귀환 포탈 사용 안되는 버그 수정
		// 2009-04-21 by cmkwon, ITEM에 DesParam 필드 개수 8개로 늘리기 - ITEM::IsExistDesParam() 함수로 통합함
		//if(IS_EXIST_DES_PARAM(i_pItemSkill->ItemInfo, DES_SKILL_CANCELALL))
		if(i_pItemSkill->ItemInfo->IsExistDesParam(DES_SKILL_CANCELALL))
		{
			pTargetSocket->m_SkillManager.SetPrepareUseBigBoom(FALSE);
		}

		///////////////////////////////////////////////////////////////////////////////
		// 2005-11-24 by cmkwon, 편대원 소환 스킬 처리
		// 2009-04-21 by cmkwon, ITEM에 DesParam 필드 개수 8개로 늘리기 - ITEM::IsExistDesParam() 함수로 통합함
		//if(IS_EXIST_DES_PARAM(i_pItemSkill->ItemInfo, DES_SKILL_SUMMON_FORMATION_MEMBER))
		if(i_pItemSkill->ItemInfo->IsExistDesParam(DES_SKILL_SUMMON_FORMATION_MEMBER))
		{// 편대원 소환 스킬이면

			// 2009-09-09 ~ 2010-01-20 by dhjin, 인피니티 - 인피 상태 값을 메인서버로 전송한다. 인피 상태 체크
			if(FALSE == g_pFieldGlobal->IsArenaServer()
				&& INFINITY_STATE_NONE != pTargetSocket->m_InfinityPlayingInfo.InfinityState ) {
				// 메인서버에선 인피 진행중인 유저는 소환 불가능하다.
				return FALSE;
			}
			
			if(m_pFieldIOCPSocket->GetCharacter()->InfluenceType != pTargetSocket->GetCharacter()->InfluenceType)
			{// 2007-05-10 by cmkwon, 편대원 소환 스킬은 세력이 다르면 사용 불가
				return FALSE;
			}

			CFieldMapProject *pFMPro = m_pFieldIOCPSocket->GetCurrentFieldMapProject();
			if(NULL == pFMPro)
			{
				pTargetSocket->SendErrorMessage(T_FC_SKILL_USE_SKILL, ERR_INVALID_CHARACTER, m_pFieldIOCPSocket->GetCharacter()->CharacterUniqueNumber, i_pItemSkill->ItemNum);
				return FALSE;
			}

// 2008-02-12 by cmkwon, IS_CITY_MAP_INDEX() 매크로를 포괄적인 도시맵인지 여부를 리턴 - 아래와 같이 수정			
// 			if(IS_CITY_MAP_INDEX(pFMPro->m_nMapIndex)
// 				|| IS_BAZAAR_MAP_INDEX(pFMPro->m_nMapIndex)
// 				|| IS_OUTPOST_CITY_MAP_INDEX(pFMPro->m_nMapIndex)				// 2007-09-28 by cmkwon, 전진기지 도시맵에서는 콜오브히어로 불가
// 				|| IS_MAP_INFLUENCE_EVENT_AREA(pFMPro->m_nMapInfluenceType))	// 2006-11-17 by cmkwon, 이벤트맵에서는 콜오브히어로 불가
			if(IS_CITY_MAP_INDEX(pFMPro->m_nMapIndex)
				|| IS_MAP_INFLUENCE_EVENT_AREA(pFMPro->m_nMapInfluenceType))	// 2006-11-17 by cmkwon, 이벤트맵에서는 콜오브히어로 불가
			{// 2006-07-20 by cmkwon, 
				return FALSE;
			}

			//////////////////////////////////////////////////////////////////////////
			// 2007-05-02 by dhjin, 아레나 맵에서는 아레나 맵 안에서만 콜오브히어로 스킬이 사용가능하다.
			if(IS_MAP_INFLUENCE_ARENA(pFMPro->GetMapInfluenceType())
				&& m_pFieldIOCPSocket->GetCurrentFieldMapChannel() != pTargetSocket->GetCurrentFieldMapChannel())
			{
				return FALSE;
			}

			// 2010. 11. 24. by hsLee.	시전자가 버닝타임 맵일 경우 타겟의 버닝타임 맵 진입 조건 체크.
			if ( NULL != ms_pFieldIOCP2->GetBurningMapInfo ( m_pFieldIOCPSocket->GetCharacter()->MapChannelIndex.MapIndex ) 
				&& ERR_NO_ERROR != pTargetSocket->CheckPossibleBurningMapWarp ( m_pFieldIOCPSocket->GetCharacter()->MapChannelIndex.MapIndex ) )
			{
				m_pFieldIOCPSocket->SendErrorMessage ( T_FC_SKILL_USE_SKILL , ERR_PROTOCOL_NOT_WARP_BURNING_MAP , m_pFieldIOCPSocket->GetCharacter()->CharacterUniqueNumber , i_pItemSkill->ItemNum );
				return FALSE;
			}

			//////////////////////////////////////////////////////////////////////////
			// 2007-08-28 by dhjin, 전진기지 맵 체크
			if(IS_MAP_INFLUENCE_OUTPOST(pFMPro->GetMapInfluenceType()) )
			{
				if(FALSE == ms_pFieldIOCP2->m_OutPostManager.CheckOutPostWaring(m_pFieldIOCPSocket->m_character.MapChannelIndex.MapIndex))
				{
					return FALSE;
				}
			}

			Err_t nRetErr = pTargetSocket->IsEnableWarp(pFMPro->GetMapInfluenceType(), pFMPro->IsConflictAreaMap(), pFMPro->m_nQuestIndexForWarp);
			if(ERR_NO_ERROR != nRetErr)
			{// 2005-12-02 by cmkwon, 워프 불가
				pTargetSocket->SendErrorMessage(T_FC_SKILL_USE_SKILL, nRetErr, m_pFieldIOCPSocket->GetCharacter()->CharacterUniqueNumber, i_pItemSkill->ItemNum);
				return FALSE;
			}

			// 2009-04-06 by cmkwon, 콜오브 히어로 스킬 시스템 변경 - 아래와 같이 수정 함.
			//EventResult_t nRetedResult = pTargetSocket->WarpToMap(m_pFieldIOCPSocket->GetCharacter()->MapChannelIndex, &(m_pFieldIOCPSocket->m_character.PositionVector)); 
			//if(EVENT_RESULT_CONTINUE != nRetedResult)
			//{// 워프 실패
			//	return FALSE;
			//}
			// 2009-04-06 by cmkwon, 콜오브 히어로 스킬 시스템 변경 - 아래와 같이 처리 함. 유효시간 15초
			INIT_MSG_WITH_BUFFER(MSG_FC_SKILL_CONFIRM_USE, T_FC_SKILL_CONFIRM_USE, pSMsg, SendBuf);
			STRNCPY_MEMSET(pSMsg->szAttackCharacterName, m_pFieldIOCPSocket->GetCharacter()->CharacterName, SIZE_MAX_CHARACTER_NAME);
			pSMsg->AttackCharacterUID	= m_pFieldIOCPSocket->GetCharacter()->CharacterUniqueNumber;
			pSMsg->TargetCharacterUID	= pTargetSocket->GetCharacter()->CharacterUniqueNumber;
			pSMsg->UsingSkillItemNum	= i_pItemSkill->ItemNum;
			pSMsg->MapChannelIndex		= m_pFieldIOCPSocket->GetCharacter()->MapChannelIndex;
			pSMsg->SkillConfirmUseUID	= this->AddSkillConfirmUse(pSMsg->TargetCharacterUID, pSMsg->UsingSkillItemNum, 10+5, pSMsg->MapChannelIndex, m_pFieldIOCPSocket->GetCharacter()->PositionVector);
			// 2011-07-11 by shcho, 아르헨티나 갓모드 로그 추가
			DbgOut("CALL OF HERO USE!!!! Target_characterName:[ %s ]\r\n",pTargetSocket->GetCharacter()->CharacterName);
			g_pFieldGlobal->WriteSystemLogEX(TRUE,"CALL OF HERO USE!!!! Target_characterName:[ %s ]\r\n",pTargetSocket->GetCharacter()->CharacterName);
			// end 2011-07-11 by shcho, 아르헨티나 갓모드 로그 추가
			pTargetSocket->SendAddData(SendBuf, MSG_SIZE(MSG_FC_SKILL_CONFIRM_USE));
		}

		//////////////////////////////////////////////////////////////////////////
		// 2006-11-17 by dhjin, 171 - 기체의 HP, 쉴드, 연료, SP를 완전회복
		// 2009-04-21 by cmkwon, ITEM에 DesParam 필드 개수 8개로 늘리기 - ITEM::IsExistDesParam() 함수로 통합함
		//if(IS_EXIST_DES_PARAM(i_pItemSkill->ItemInfo, DES_SKILL_FULL_RECOVERY))
		if(i_pItemSkill->ItemInfo->IsExistDesParam(DES_SKILL_FULL_RECOVERY))
		{
			pTargetSocket->m_character.CurrentHP	= pTargetSocket->m_character.HP;
			pTargetSocket->m_character.CurrentDP	= pTargetSocket->m_character.DP;
			pTargetSocket->m_character.CurrentSP	= pTargetSocket->m_character.SP;
			pTargetSocket->m_character.CurrentEP	= pTargetSocket->m_character.EP;
			pTargetSocket->SendCharacterInfo(T_FC_CHARACTER_CHANGE_CURRENTHPDPSPEP);
		}

		///////////////////////////////////////////////////////////////////////////////
		// 2006-12-18 by cmkwon, DP 회복 불가
		// 2009-04-21 by cmkwon, ITEM에 DesParam 필드 개수 8개로 늘리기 - ITEM::IsExistDesParam() 함수로 통합함
		//if(IS_EXIST_DES_PARAM(i_pItemSkill->ItemInfo, DES_IMMEDIATE_DP_UP)
		if(i_pItemSkill->ItemInfo->IsExistDesParam(DES_IMMEDIATE_DP_UP)
			&& FALSE == pTargetSocket->IsEnableRepairDP())
		{
			continue;
		}
		
		nSuccessCnt++;

		// check: 클릭형 스킬은 ParamFactor 적용이 없음, 20041125, kelovon w/ dhkwon, sjmin
		// 2004-12-06 by cmkwon, 에너지업/쉴드업에서 사용(나머지 클릭형 스킬에는 필요없음-DesParam이 초기화 되어 있어야함)
		// set paramfactor
 		pTargetSocket->m_SkillManager.SetSkillParamFactor(i_pItemSkill->ItemInfo, m_pFieldIOCPSocket->GetCharacter()->ClientIndex);

		////////////////////////////////////////////////////////////////////////////////
		// 2009-09-09 ~ 2010 by dhjin, 인피니티 - OrbitSkill 적용
		if(TRUE == IS_ORBIT_SKILL(i_pItemSkill->ItemInfo->OrbitType)) {
			pTargetSocket->m_SkillManager.ApplyOrbitSkill(i_pItemSkill->ItemInfo, m_pFieldIOCPSocket->GetCharacter()->ClientIndex);
		}

		msgUseSkillOK->SkillItemID	= i_pMsgUseSkill->SkillItemID;
		msgUseSkillOK->AttackIndex	= i_pMsgUseSkill->AttackIndex;
		msgUseSkillOK->TargetIndex	= nTmpClientIndex;
		msgUseSkillOK->UseTime		= i_pItemSkill->UseTime;			// 2006-12-05 by dhjin, 스킬 사용 시간

//		// 2007-02-05 by dhjin, 스캔 스킬 10초 버프형으로 수정
//		// 자신과 주위의 아그들에게도 보냄
//		if(IS_EXIST_DES_PARAM(i_pItemSkill->ItemInfo, DES_SKILL_SCANNING))
//		{// 2006-11-27 by dhjin, 정확한 거리를 체크하여 전송
//			pTargetSocket->SendInRangeMessageExactAroundMe(SendBuf, MSG_SIZE(MSG_FC_SKILL_USE_SKILL_OK), i_pItemSkill->ItemInfo->Range, TRUE);
//		}
//		else
//		{// 2006-11-27 by dhjin
//			pTargetSocket->SendInRangeMessageAroundMe(SendBuf, MSG_SIZE(MSG_FC_SKILL_USE_SKILL_OK), TRUE);
//		}
		pTargetSocket->SendInRangeMessageAroundMe(SendBuf, MSG_SIZE(MSG_FC_SKILL_USE_SKILL_OK), TRUE);

		///////////////////////////////////////////////////////////////////////////////
		// 2009-12-11 by cmkwon, 데미지 어그로로 타겟을 변경하는 몬스터 구현 - 
		if(0.0f < i_pItemSkill->ItemInfo->GetSkillDamageForAggro())
		{
			pTargetSocket->GetAttackedMonsterIndex(o_pMonListForSkillAggro);
		}

#ifdef _DEBUG
		m_pFieldIOCPSocket->SendString128(STRING_128_ADMIN_CMD, "SKILL Target: %s",
			GetCharacterString(ms_pFieldIOCP2->GetCharacterByArrayIndex(nTmpClientIndex), string()));
#endif
	}
	// 2005-11-24 by cmkwon, 스킬이 적용된 사람이 없다면 스킬 사용 실패
	if(0 == nSuccessCnt)
	{
		return FALSE;
	}
	return TRUE;
}

// 2009-12-11 by cmkwon, 데미지 어그로로 타겟을 변경하는 몬스터 구현 - 아래의 함수에 인자추가(, setClientIndex_t *o_pMonListForSkillAggro)
BOOL CFieldSkillManager::UseSkillTypeTimeLimit(ITEM_SKILL *i_pItemSkill, MSG_FC_SKILL_USE_SKILL *i_pMsgUseSkill, vector<ClientIndex_t> *i_pVectorClientIndex, setClientIndex_t *o_pMonListForSkillAggro)
{
	// 재사용을 위해서는 클라이언트에서 항상 cancel_skill을 미리 전송함, 20041015, kelovon with jschoi
// 2006-12-28 by cmkwon, 자신이 사용한 스킬만 체크해야함
//	BOOL bSkillActivated = IsSkillActivatedByItemNum(i_pItemSkill->ItemNum);
	BOOL bSkillActivated = IsSkillActivatedByUID(i_pItemSkill->UniqueNumber);
	if (bSkillActivated)
	{
		return FALSE;
	}

	BOOL bBreak = FALSE;		// 2006-12-04 by dhjin
	INIT_MSG_OF_SIZE(MSG_FC_SKILL_USE_SKILL_OK, T_FC_SKILL_USE_SKILL_OK, msgUseSkillOK, SendBuf);
	int nSuccessCnt	= 0;
	for (int i = 0; i < i_pVectorClientIndex->size(); i++)
	{
		ClientIndex_t nTmpClientIndex = (*i_pVectorClientIndex)[i];

		CFieldIOCPSocket *pTargetSocket = ms_pFieldIOCP2->GetFieldIOCPSocket(nTmpClientIndex);
		if (pTargetSocket == NULL
			|| FALSE == pTargetSocket->IsValidCharacter()
			|| pTargetSocket->IsInvisibleCharacter())		// 2007-10-02 by cmkwon, 투명 상태의 유저는 스킬 사용 및 적용 불가
		{
			continue;
		}

		if (!pTargetSocket->m_SkillManager.CheckSkillReqItemKind(i_pItemSkill->ItemInfo))
		{
			continue;
		}

		ITEM *pItemInfo = i_pItemSkill->ItemInfo;

		// 기존에 같은 BaseNum의 스킬을 사용하고 있는지 확인
		INT nOldSkillItemNum = pTargetSocket->m_SkillManager.IsSkillActivatedBySkillBaseNum(i_pItemSkill->ItemNum);
		if (nOldSkillItemNum != 0)
		{
			if (SKILL_LEVEL(nOldSkillItemNum) <= SKILL_LEVEL(i_pItemSkill->ItemNum))
			{
				// 이미 사용하고 있는 경우
				pTargetSocket->m_SkillManager.DeactivateSkillByItemNum(nOldSkillItemNum, FALSE);
			}
			else
			{
				// check: 에러는 안 보냄, 나중에 필요하면 살림, kelovon with jschoi, 20041018
				continue;
			}
		}

		// set paramfactor
		pTargetSocket->m_SkillManager.SetSkillParamFactor(pItemInfo, m_pFieldIOCPSocket->GetCharacter()->ClientIndex);

		////////////////////////////////////////////////////////////////////////////////
		// 2009-09-09 ~ 2010 by dhjin, 인피니티 - OrbitSkill 적용
		if(TRUE == IS_ORBIT_SKILL(i_pItemSkill->ItemInfo->OrbitType)) {
			pTargetSocket->m_SkillManager.ApplyOrbitSkill(i_pItemSkill->ItemInfo, m_pFieldIOCPSocket->GetCharacter()->ClientIndex);
		}

// check: 그냥 쏘게 하기, 20041030, kelovon with ClientTeam
//		// check: shotnum을 증가시키는 스킬이면 강제 세팅하기, 20040902, kelovon
//		//        게임 시작 즉시 shotnum 증가 스킬 사용하면 총알 안 나가는 버그 수정한 것임
//		if (pItemInfo->DestParameter1 == DES_SHOTNUM_01 || pItemInfo->DestParameter2 == DES_SHOTNUM_01 ||
//			pItemInfo->DestParameter3 == DES_SHOTNUM_01 || pItemInfo->DestParameter4 == DES_SHOTNUM_01)
//		{
//			pTargetSocket->m_PrimaryShotNumCount
//				= CAtumSJ::GetTotalPrimaryShotCountPerReattackTime(&pTargetSocket->m_ItemProwOut, &pTargetSocket->m_ParamFactor);
//		}

		//////////////////////////////////////////////////////////////////////////
		// 81, 2006-12-11 by dhjin - A기어의 응급처치에 사용
		// 2009-04-21 by cmkwon, ITEM에 DesParam 필드 개수 8개로 늘리기 - ITEM::IsExistDesParam() 함수로 통합함
		//if (IS_EXIST_DES_PARAM(pItemInfo, DES_GRADUAL_DP_UP))
		// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 
		if (pItemInfo->IsExistDesParam(DES_GRADUAL_DP_UP)
			&& FALSE == pTargetSocket->m_SkillManager.m_FieldDebuff.CheckApplyingDebuff(DES_SKILL_MON_FREEZE_DP))
		{
			pTargetSocket->m_TimerManager.m_nRemainedTimeOfGradualDPUP		= pItemInfo->Time;
			pTargetSocket->m_TimerManager.StartTimerField(TE_TYPE_GRADUAL_DP_UP, TIMER_GRADUAL_UP_INTERVAL, (UINT)pItemInfo);
		}
		
		//////////////////////////////////////////////////////////////////////////
		// 124, 2006-12-04 by dhjin - B기어의 체프 사출
		// 2009-04-21 by cmkwon, ITEM에 DesParam 필드 개수 8개로 늘리기 - ITEM::IsExistDesParam() 함수로 통합함
		//if (IS_EXIST_DES_PARAM(pItemInfo, DES_SKILL_HALLUCINATION))
		if (pItemInfo->IsExistDesParam(DES_SKILL_HALLUCINATION))
		{			
			mt_auto_lock dummylock(&pTargetSocket->m_mtvectFieldDummy);
			if(false == pTargetSocket->m_mtvectFieldDummy.empty())
			{// 2006-12-04 by dhjin, 사용된것이 아직 사라지지 않음
				return FALSE;
			}
// 2007-06-21 by cmkwon, 체프 하나의 메시지로 모두 전송 - 아래와 같이 수정			
//			int nSendByte = 0;
//			INIT_MSG_WITH_BUFFER(MSG_FC_BATTLE_DROP_DUMMY_OK, T_FC_BATTLE_DROP_DUMMY_OK, pMsgDropDummyOK, SendBuf);
//			for (int i = 0; i < pItemInfo->GetParameterValue(DES_SKILL_HALLUCINATION); i++)
//			{
//				if(SIZE_MAX_PACKET < nSendByte + MSG_SIZE(MSG_FC_BATTLE_DROP_DUMMY_OK))
//				{
//					break;
//				}
//				FIELD_DUMMY *pFieldDummy = new FIELD_DUMMY(pTargetSocket->m_uiDummyItemFieldIndex++, pItemInfo);
//				pTargetSocket->m_mtvectFieldDummy.push_back(pFieldDummy);					
//				
//				pMsgDropDummyOK->ItemNum		= pItemInfo->ItemNum;
//				pMsgDropDummyOK->AttackIndex	= pTargetSocket->m_character.ClientIndex;
//				pMsgDropDummyOK->ItemFieldIndex = pFieldDummy->ItemFieldIndex;
//				
//				pTargetSocket->m_TimerManager.StartTimerField(TE_TYPE_DELETE_DUMMY, pItemInfo->Time, pFieldDummy->ItemFieldIndex);
//
//				nSendByte		+= MSG_SIZE(MSG_FC_BATTLE_DROP_DUMMY_OK);
//				*(MessageType_t*)(SendBuf+nSendByte)	= T_FC_BATTLE_DROP_DUMMY_OK;
//				pMsgDropDummyOK							= (MSG_FC_BATTLE_DROP_DUMMY_OK*)(SendBuf + nSendByte + SIZE_FIELD_TYPE_HEADER);
//			}
			pTargetSocket->m_bDummyCheck	= TRUE;
			for (int i = 0; i < pItemInfo->GetParameterValue(DES_SKILL_HALLUCINATION); i++)
			{
				FIELD_DUMMY *pFieldDummy = new FIELD_DUMMY(pTargetSocket->m_uiDummyItemFieldIndex++, pItemInfo);
				pTargetSocket->m_mtvectFieldDummy.push_back(pFieldDummy);					
				pTargetSocket->m_TimerManager.StartTimerField(TE_TYPE_DELETE_DUMMY, pItemInfo->Time, pFieldDummy->ItemFieldIndex);
			}
			int		nSendByte = 0;
			BYTE	SendBuf[SIZE_MAX_PACKET];
			if(pTargetSocket->Make_MSG_FC_BATTLE_DROP_DUMMY_OK(SendBuf, &nSendByte, SIZE_MAX_PACKET))
			{
				CFieldIOCPSocket::ms_pFieldIOCP->SendInRangeMessageAroundCharacter(pTargetSocket->m_character.ClientIndex, SendBuf, nSendByte
						, pTargetSocket->GetCurrentFieldMapChannel(), TRUE, pTargetSocket->GetCurrentFieldMapChannel()->GetUserVisibleDiameterW());
			}
			pTargetSocket->m_uiDummyItemFieldIndex += 100;			
			bBreak							= TRUE;
		}		
		pTargetSocket->m_SkillManager.m_vectorAtivatedSkill.pushBackLock(ItemID_t(i_pItemSkill->UniqueNumber, i_pItemSkill->ItemNum));

		// MSG 전송
		msgUseSkillOK->SkillItemID	= i_pMsgUseSkill->SkillItemID;
		msgUseSkillOK->AttackIndex	= i_pMsgUseSkill->AttackIndex;
		msgUseSkillOK->TargetIndex	= nTmpClientIndex;
		msgUseSkillOK->UseTime		= i_pItemSkill->UseTime;			// 2006-12-05 by dhjin, 스킬 사용 시간

		///////////////////////////////////////////////////////////////////////////////
		// 2007-02-07 by cmkwon
		// 1. 스캐닝 스킬일 경우 전체 유저에게는 한번만 보내고 타겟 유저 사용 정보는 타겟 유저에게만 전송한다.
		// 2. 나머지 스킬은 사용 정보를 모든 유저에게 전송한다.
		// 2009-04-21 by cmkwon, ITEM에 DesParam 필드 개수 8개로 늘리기 - ITEM::IsExistDesParam() 함수로 통합함
		//if(IS_EXIST_DES_PARAM(i_pItemSkill->ItemInfo, DES_SKILL_SCANNING)
		if(i_pItemSkill->ItemInfo->IsExistDesParam(DES_SKILL_SCANNING)
			&& i_pMsgUseSkill->AttackIndex != nTmpClientIndex)
		{
			pTargetSocket->SendAddData(SendBuf, MSG_SIZE(MSG_FC_SKILL_USE_SKILL_OK));
		}
		else
		{
			pTargetSocket->SendInRangeMessageAroundMe(SendBuf, MSG_SIZE(MSG_FC_SKILL_USE_SKILL_OK), TRUE);
		}

		nSuccessCnt++;		// 2006-12-04 by dhjin

		///////////////////////////////////////////////////////////////////////////////
		// 2009-12-11 by cmkwon, 데미지 어그로로 타겟을 변경하는 몬스터 구현 - 
		if(0.0f < i_pItemSkill->ItemInfo->GetSkillDamageForAggro())
		{
			pTargetSocket->GetAttackedMonsterIndex(o_pMonListForSkillAggro);
		}

		if(bBreak)
		{// 2006-12-04 by dhjin
			break;
		}
	}
	// 2005-11-24 by cmkwon, 스킬이 적용된 사람이 없다면 스킬 사용 실패
	if(0 == nSuccessCnt)
	{
		return FALSE;
	}
	return TRUE;
}

// 2009-12-11 by cmkwon, 데미지 어그로로 타겟을 변경하는 몬스터 구현 - 아래의 함수에 인자추가(, setClientIndex_t *o_pMonListForSkillAggro)
BOOL CFieldSkillManager::UseSkillTypeToggle(ITEM_SKILL *i_pItemSkill, MSG_FC_SKILL_USE_SKILL *i_pMsgUseSkill, setClientIndex_t *o_pMonListForSkillAggro)
{
	// 재사용을 위해서는 클라이언트에서 항상 cancel_skill을 미리 전송함, 20041015, kelovon with jschoi
	BOOL bSkillActivated = IsSkillActivatedByItemNum(i_pItemSkill->ItemNum);
	if (bSkillActivated)
	{
		return FALSE;
	}

	ITEM *pItemInfo = i_pItemSkill->ItemInfo;
	
	// set paramfactor
	m_pFieldIOCPSocket->m_SkillManager.SetSkillParamFactor(pItemInfo);

// check: 그냥 쏘게 하기, 20041030, kelovon with ClientTeam
//	// check: shotnum을 증가시키는 스킬이면 강제 세팅하기, 20040902, kelovon
//	//        게임 시작 즉시 shotnum 증가 스킬 사용하면 총알 안 나가는 버그 수정한 것임
//	if (pItemInfo->DestParameter1 == DES_SHOTNUM_01 || pItemInfo->DestParameter2 == DES_SHOTNUM_01 ||
//		pItemInfo->DestParameter3 == DES_SHOTNUM_01 || pItemInfo->DestParameter4 == DES_SHOTNUM_01)
//	{
//		m_pFieldIOCPSocket->m_PrimaryShotNumCount
//			= CAtumSJ::GetTotalPrimaryShotCountPerReattackTime(&m_pFieldIOCPSocket->m_ItemProwOut, &m_pFieldIOCPSocket->m_ParamFactor);
//	}

	// 타이머 동작
	m_pFieldIOCPSocket->m_TimerManager.StartTimerField(TE_TYPE_DECREASE_SP, TIMER_DECREASE_SP_INTERVAL,
				HIDWORD(i_pItemSkill->UniqueNumber), LODWORD(i_pItemSkill->UniqueNumber), pItemInfo->ItemNum, TIMER_REPEAT_INFINITE);

	m_vectorAtivatedSkill.pushBackLock(ItemID_t(i_pItemSkill->UniqueNumber, i_pItemSkill->ItemNum));

	INIT_MSG_OF_SIZE(MSG_FC_SKILL_USE_SKILL_OK, T_FC_SKILL_USE_SKILL_OK, msgUseSkillOK, SendBuf);
	msgUseSkillOK->SkillItemID	= i_pMsgUseSkill->SkillItemID;
	msgUseSkillOK->AttackIndex	= i_pMsgUseSkill->AttackIndex;
	msgUseSkillOK->TargetIndex	= i_pMsgUseSkill->TargetIndex;
	msgUseSkillOK->UseTime		= i_pItemSkill->UseTime;			// 2006-12-05 by dhjin, 스킬 사용 시간

	// 자신과 주위의 아그들에게도 보냄
	m_pFieldIOCPSocket->SendInRangeMessageAroundMe(SendBuf, MSG_SIZE(MSG_FC_SKILL_USE_SKILL_OK), TRUE);
	
	///////////////////////////////////////////////////////////////////////////////
	// 2009-12-11 by cmkwon, 데미지 어그로로 타겟을 변경하는 몬스터 구현 - 
	if(0.0f < i_pItemSkill->ItemInfo->GetSkillDamageForAggro())
	{
		m_pFieldIOCPSocket->GetAttackedMonsterIndex(o_pMonListForSkillAggro);
	}

	return TRUE;
}

// 2009-12-11 by cmkwon, 데미지 어그로로 타겟을 변경하는 몬스터 구현 - 아래의 함수에 인자추가(, setClientIndex_t *o_pMonListForSkillAggro)
BOOL CFieldSkillManager::UseSkillTypeCharging(ITEM_SKILL *i_pItemSkill, MSG_FC_SKILL_USE_SKILL *i_pMsgUseSkill, setClientIndex_t *o_pMonListForSkillAggro)
{
	// 재사용을 위해서는 클라이언트에서 항상 cancel_skill을 미리 전송함, 20041015, kelovon with jschoi
	BOOL bSkillActivated = IsSkillActivatedByItemNum(i_pItemSkill->ItemNum);
	if (bSkillActivated)
	{
		return FALSE;
	}

	mt_auto_lock mtAuto(&m_vecChargingSkill);
	for(int i=0; i < m_vecChargingSkill.size(); i++)
	{
		if(m_vecChargingSkill[i]->ItemNum == i_pItemSkill->ItemInfo->ItemNum)
		{// 이미 같은 차징스킬이 사용중이다

			return FALSE;
		}
	}

	///////////////////////////////////////////////////////////////////////////////
	// 2007-03-29 by cmkwon, MSG_FC_MOVE_OK에 전송할 비트프래그 설정
	// 2009-04-21 by cmkwon, ITEM에 DesParam 필드 개수 8개로 늘리기 - ITEM::IsExistDesParam() 함수로 통합함
	//if(IS_EXIST_DES_PARAM(i_pItemSkill->ItemInfo, DES_SKILL_HYPERSHOT))
	if(i_pItemSkill->ItemInfo->IsExistDesParam(DES_SKILL_HYPERSHOT))
	{
		m_bHyperShot		= TRUE;		// 2007-03-29 by cmkwon
	}
	else
	{
		m_bChargingSkill	= TRUE;		// 2007-04-02 by cmkwon
	}
	
	// check: charging 시간이 지났는지 확인하는 루틴 필요한데... ITEM::AttackTime에 설정됨
	m_vecChargingSkill.push_back(i_pItemSkill->ItemInfo);
	mtAuto.auto_unlock_cancel();

	INIT_MSG_OF_SIZE(MSG_FC_SKILL_USE_SKILL_OK, T_FC_SKILL_USE_SKILL_OK, msgUseSkillOK, SendBuf);
	msgUseSkillOK->SkillItemID	= i_pMsgUseSkill->SkillItemID;
	msgUseSkillOK->AttackIndex	= i_pMsgUseSkill->AttackIndex;
	msgUseSkillOK->TargetIndex	= i_pMsgUseSkill->TargetIndex;
	msgUseSkillOK->UseTime		= i_pItemSkill->UseTime;			// 2006-12-05 by dhjin, 스킬 사용 시간

	// 자신과 주위의 아그들에게도 보냄
	m_pFieldIOCPSocket->SendInRangeMessageAroundMe(SendBuf, MSG_SIZE(MSG_FC_SKILL_USE_SKILL_OK), TRUE);

	///////////////////////////////////////////////////////////////////////////////
	// 2009-12-11 by cmkwon, 데미지 어그로로 타겟을 변경하는 몬스터 구현 - 
	if(0.0f < i_pItemSkill->ItemInfo->GetSkillDamageForAggro())
	{
		m_pFieldIOCPSocket->GetAttackedMonsterIndex(o_pMonListForSkillAggro);
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CFieldSkillManager::CheckUseSkillTime(ITEM_SKILL* i_pItemSkill, ATUM_DATE_TIME *i_patCurTime)
/// \brief		
/// \author		dhjin
/// \date		2006-12-05 ~ 2006-12-05
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CFieldSkillManager::CheckUseSkillTime(ITEM_SKILL* i_pItemSkill, ATUM_DATE_TIME *i_patCurTime)
{
// 2009-04-06 by cmkwon, 스킬 관련 속성 추가안 구현(사용 시간 저장) - 아래와 같이 스킬속성으로 체크
//	if (i_pItemSkill->ItemInfo->ReAttacktime >= TERM_TICK_MUST_SERVER_CHECK_SKILL_REATTACK_TIME)  // 10분 = 10 * 60 * 1000
//	{// 재사용 시간이 10분이 이상이면 체크
	if(COMPARE_BIT_FLAG(i_pItemSkill->ItemInfo->ItemAttribute, SKILL_ATTR_STORE_USING_TIME))
	{// 2009-04-06 by cmkwon, 스킬 관련 속성 추가안 구현(사용 시간 저장) - 스킬속성으로 체크
		int GapTime = i_patCurTime->GetTimeDiffTimeInSeconds(i_pItemSkill->UseTime);
		if (GapTime > (i_pItemSkill->ItemInfo->ReAttacktime/1000))
		{
			return TRUE;
		}
		return FALSE;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CFieldSkillManager::SaveUseSkillTime(ITEM_SKILL* i_pItemSkill)
/// \brief		
/// \author		dhjin
/// \date		2006-12-06 ~ 2006-12-06
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CFieldSkillManager::SaveUseSkillTime(ITEM_SKILL* i_pItemSkill)
{
// 2009-04-06 by cmkwon, 스킬 관련 속성 추가안 구현(사용 시간 저장) - 아래와 같이 수정함.
//	if (i_pItemSkill->ItemInfo->ReAttacktime < TERM_TICK_MUST_SERVER_CHECK_SKILL_REATTACK_TIME)  // 10분 = 10 * 60 * 1000
//	{// 2006-12-20 by cmkwon, 10분 미만은 저장하지 않음
	if(FALSE == COMPARE_BIT_FLAG(i_pItemSkill->ItemInfo->ItemAttribute, SKILL_ATTR_STORE_USING_TIME))
	{// 2009-04-06 by cmkwon, 스킬 관련 속성 추가안 구현(사용 시간 저장) - 스킬 사용시간 저장 여부 체크
		return FALSE;
	}

	QPARAM_UPDATE_SKILLUSETIME *pQParam = new QPARAM_UPDATE_SKILLUSETIME;
	pQParam->ItemUID					= i_pItemSkill->UniqueNumber;
	pQParam->UseTime					= i_pItemSkill->UseTime;
	m_pFieldDBManager->MakeAndEnqueueQuery(QT_UpdateStoreItemSkillUseTime, m_pFieldIOCPSocket, m_pFieldIOCPSocket->m_character.AccountUniqueNumber, pQParam);
	return TRUE;
}

BOOL CFieldSkillManager::DeactivateSkill(ITEM* i_pItemInfo, BOOL i_bSendMSG)
{
	if (i_pItemInfo == NULL)
	{
		return FALSE;
	}

	if (i_pItemInfo->SkillType != SKILLTYPE_TIMELIMIT && i_pItemInfo->SkillType != SKILLTYPE_TOGGLE)
	{
		return FALSE;
	}

	// unset paramfactor
	m_pFieldIOCPSocket->m_SkillManager.UnsetSkillParamFactor(i_pItemInfo);

	// AtivatedSkill에서 제거
	if (!DeleteActivatedSkillByItemNum(i_pItemInfo->ItemNum))
	{
		return FALSE;
	}

	if (i_bSendMSG)
	{
		// send msg
		INIT_MSG_OF_SIZE(MSG_FC_SKILL_INVALIDATE_SKILL, T_FC_SKILL_INVALIDATE_SKILL, msgInvalidateSkill, SendBuf);
		msgInvalidateSkill->SkillItemID.ItemNum = i_pItemInfo->ItemNum;
		msgInvalidateSkill->SkillItemID.ItemUID = 0;	// 이 경우에는 클라이언트 불필요함, 필요하면 제대로 할당하기, 20041018, kelovon
		msgInvalidateSkill->ClientIndex = m_pFieldIOCPSocket->m_character.ClientIndex;
		m_pFieldIOCPSocket->SendInRangeMessageAroundMe(SendBuf, MSG_SIZE(MSG_FC_SKILL_INVALIDATE_SKILL), TRUE);
	}

	// TOGGLE형이면 타이머 제거
	if (SKILLTYPE_TOGGLE == i_pItemInfo->SkillType)
	{
		const MEX_TIMER_EVENT *pTimerEvent = m_pFieldIOCPSocket->m_TimerManager.GetTimerEventByItemNum(i_pItemInfo->ItemNum);
		if (pTimerEvent != NULL)
		{
			m_pFieldIOCPSocket->m_TimerManager.StopTimerField(pTimerEvent->TimerUID);
		}
		else
		{
			m_pFieldIOCPSocket->SendErrorMessage(T_FC_SKILL_CANCEL_SKILL, ERR_PROTOCOL_NO_SUCH_SKILL, i_pItemInfo->ItemNum);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CFieldSkillManager::DeactivateSkillByUID(UID64_t i_nSkillUID, BOOL i_bSendMSG)
{
	ASSERT_NOT_IMPLEMENTED_YET();
	return FALSE;
}

BOOL CFieldSkillManager::DeactivateSkillByItemNum(INT i_nSkillItemNum, BOOL i_bSendMSG)
{
	ITEM *pItemInfo = ms_pFieldIOCP2->GetItemInfo(i_nSkillItemNum);

	if (pItemInfo == NULL)
	{
		return FALSE;
	}

	return DeactivateSkill(pItemInfo, i_bSendMSG);
}

BOOL CFieldSkillManager::ApplyChargingSkill(ITEM **o_ppItemforProcessSplashAttack, BOOL i_bSplashAttack, ITEM *i_pAttackItem, CParamFactor *i_pParamFactorForChargingSkill)
{
	BOOL bChargingSkillApplied = FALSE;

	// Chaging Skill 적용
	if (!m_vecChargingSkill.empty())
	{
		m_vecChargingSkill.lock();
		mtvectorItemPtr::iterator itrChargingSkill = m_vecChargingSkill.begin();
		while (itrChargingSkill != m_vecChargingSkill.end()) 
		{
			ITEM *pChagingSkillItem = *itrChargingSkill;
			
			if (COMPARE_ITEMKIND(pChagingSkillItem->ReqItemKind, i_pAttackItem->Kind))
			{
				if(FALSE == i_bSplashAttack
					// 2009-04-21 by cmkwon, ITEM에 DesParam 필드 개수 8개로 늘리기 - ITEM::IsExistDesParam() 함수로 통합함
					//&& IS_EXIST_DES_PARAM(pChagingSkillItem, DES_SKILL_HYPERSHOT))
					&& pChagingSkillItem->IsExistDesParam(DES_SKILL_HYPERSHOT))
				{// 2006-12-01 by dhjin, SplashAttack 처리가 아니면서 하이퍼삿 스킬이면 ProcessSplashAttack만 TRUE로 설정한다
					*o_ppItemforProcessSplashAttack		= pChagingSkillItem;		// 2006-12-01 by dhjin
				}
				else
				{
					// 2009-04-21 by cmkwon, ITEM에 DesParam 필드 개수 8개로 늘리기 - ITEM::IsExistDesParam() 함수로 통합함
					//if(IS_EXIST_DES_PARAM(pChagingSkillItem, DES_SKILL_HYPERSHOT))
					if(pChagingSkillItem->IsExistDesParam(DES_SKILL_HYPERSHOT))
					{
						*o_ppItemforProcessSplashAttack	= pChagingSkillItem;	// 2006-12-01 by dhjin
						m_bHyperShot		= FALSE;		// 2007-03-29 by cmkwon, MSG_FC_MOVE_OK에 전송할 비트프래그 설정 해제
					}
					else
					{
						m_bChargingSkill	= FALSE;		// 2007-04-02 by cmkwon, MSG_FC_MOVE_OK에 전송할 비트프래그 설정 해제
					}

// 2009-04-21 by cmkwon, ITEM에 DesParam 필드 개수 8개로 늘리기 - 아래와 같이 수정
// 					m_pFieldIOCPSocket->SetParamFactor(i_pParamFactorForChargingSkill, pChagingSkillItem->DestParameter1, pChagingSkillItem->ParameterValue1);
// 					m_pFieldIOCPSocket->SetParamFactor(i_pParamFactorForChargingSkill, pChagingSkillItem->DestParameter2, pChagingSkillItem->ParameterValue2);
// 					m_pFieldIOCPSocket->SetParamFactor(i_pParamFactorForChargingSkill, pChagingSkillItem->DestParameter3, pChagingSkillItem->ParameterValue3);
// 					m_pFieldIOCPSocket->SetParamFactor(i_pParamFactorForChargingSkill, pChagingSkillItem->DestParameter4, pChagingSkillItem->ParameterValue4);
					for(int i=0; i < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; i++)
					{
						m_pFieldIOCPSocket->SetParamFactor(i_pParamFactorForChargingSkill, pChagingSkillItem->ArrDestParameter[i], pChagingSkillItem->ArrParameterValue[i] );
					}

					// Chaging Skill에 의한 속성 일시 변경
					m_pFieldIOCPSocket->m_ParamFactor += *i_pParamFactorForChargingSkill;
					bChargingSkillApplied = TRUE;

					itrChargingSkill = m_vecChargingSkill.erase(itrChargingSkill);
					continue;
				}
			}

			itrChargingSkill++;
		}
		m_vecChargingSkill.unlock();
	}

	return bChargingSkillApplied;
}

BOOL CFieldSkillManager::ReleaseChargingSkill(CParamFactor *i_pParamFactorForChargingSkill)
{
	m_pFieldIOCPSocket->m_ParamFactor -= *i_pParamFactorForChargingSkill;

	return TRUE;
}

// 2006-12-06 by cmkwon, 인자 추가(ITEM *i_pSkillItem/*=NULL*/), NULL이면 모두 초기화, NULL이 아니면 특정 스킬만 초기화
BOOL CFieldSkillManager::ClearChargingSkill(ITEM *i_pSkillItem/*=NULL*/)
{
	mt_auto_lock mtA(&m_vecChargingSkill);
	if (m_vecChargingSkill.empty())
	{
		m_bChargingSkill	= FALSE;		// 2007-04-02 by cmkwon
		m_bHyperShot		= FALSE;		// 2007-03-29 by cmkwon
		return FALSE;
	}

	if(NULL == i_pSkillItem)
	{// 2006-12-06 by cmkwon, 모든 차징 초기화
		m_bChargingSkill	= FALSE;		// 2007-04-02 by cmkwon
		m_bHyperShot		= FALSE;		// 2007-03-29 by cmkwon
		m_vecChargingSkill.clear();
	}
	else
	{// 2006-12-06 by cmkwon, 특정 차징만 초기화
		mtvectorItemPtr::iterator itrChargingSkill = m_vecChargingSkill.begin();
		while (itrChargingSkill != m_vecChargingSkill.end()) 
		{
			if(i_pSkillItem->ItemNum == (*itrChargingSkill)->ItemNum)
			{
				///////////////////////////////////////////////////////////////////////////////
				// 2007-03-29 by cmkwon, MSG_FC_MOVE_OK에 전송할 비트프래그 설정 해제
				// 2009-04-21 by cmkwon, ITEM에 DesParam 필드 개수 8개로 늘리기 - ITEM::IsExistDesParam() 함수로 통합함
				//if(IS_EXIST_DES_PARAM(i_pSkillItem, DES_SKILL_HYPERSHOT))
				if(i_pSkillItem->IsExistDesParam(DES_SKILL_HYPERSHOT))
				{
					m_bHyperShot		= FALSE;		// 2007-03-29 by cmkwon
				}
				else
				{
					m_bChargingSkill	= FALSE;		// 2007-04-02 by cmkwon
				}

				itrChargingSkill = m_vecChargingSkill.erase(itrChargingSkill);
				continue;
			}
			itrChargingSkill++;
		}
	}
	return TRUE;
}

void CFieldSkillManager::GetSkillTargets(vector<ClientIndex_t> *i_pVectorClientIndex
										 , ClientIndex_t i_nTargetIndex
										 , UID32_t i_nTargetCharUID
										 , ITEM *i_pItemInfo)
{
	switch(i_pItemInfo->SkillTargetType)
	{
	case SKILLTARGETTYPE_ME:
		{
			i_pVectorClientIndex->push_back(m_pFieldIOCPSocket->m_character.ClientIndex);
		}
		break;
	case SKILLTARGETTYPE_ONE:
		{
			if (IS_VALID_CLIENT_INDEX(i_nTargetIndex))
			{
				i_pVectorClientIndex->push_back(i_nTargetIndex);
			}
			else if(0 != i_nTargetCharUID)
			{
				CFieldIOCPSocket *pFSock = m_pFieldIOCPSocket->ms_pFieldIOCP->GetFieldIOCPSocketByCharacterUniqueNumber(i_nTargetCharUID);
				if(pFSock
					&& pFSock->IsValidCharacter())
				{
					i_pVectorClientIndex->push_back(pFSock->GetCharacter()->ClientIndex);
				}
			}
		}
		break;
	case SKILLTARGETTYPE_ONE_EXCLUDE_ME:
		{
			if (IS_VALID_CLIENT_INDEX(i_nTargetIndex)
				&& m_pFieldIOCPSocket->GetCharacter()->ClientIndex != i_nTargetIndex)
			{
				i_pVectorClientIndex->push_back(i_nTargetIndex);
			}
			else if(0 != i_nTargetCharUID
				&& m_pFieldIOCPSocket->GetCharacter()->CharacterUniqueNumber != i_nTargetCharUID)
			{
				CFieldIOCPSocket *pFSock = m_pFieldIOCPSocket->ms_pFieldIOCP->GetFieldIOCPSocketByCharacterUniqueNumber(i_nTargetCharUID);
				if(pFSock
					&& pFSock->IsValidCharacter())
				{
					i_pVectorClientIndex->push_back(pFSock->GetCharacter()->ClientIndex);
				}
			}
		}
		break;
	case SKILLTARGETTYPE_PARTY_WITH_ME:
		{
			CFieldParty *pFParty = m_pFieldIOCPSocket->m_pFieldParty;			// 2006-07-04 by cmkwon
			if (pFParty != NULL)
			{
				i_pVectorClientIndex->push_back(m_pFieldIOCPSocket->m_character.ClientIndex);
				pFParty->GetMembersInSameMapToVector(i_pVectorClientIndex,
								m_pFieldIOCPSocket->m_pCurrentFieldMapChannel,
								&m_pFieldIOCPSocket->m_character.PositionVector,
								i_pItemInfo->Range, m_pFieldIOCPSocket->m_character.ClientIndex);
			}
		}
		break;
	case SKILLTARGETTYPE_PARTY_ONE:	// 21	// 아무나 하나의 파티원, // 2005-12-05 by cmkwon
		{
			if (m_pFieldIOCPSocket->m_pFieldParty != NULL)
			{
				if (IS_VALID_CLIENT_INDEX(i_nTargetIndex))
				{
					CFieldIOCPSocket *pFSock = m_pFieldIOCPSocket->ms_pFieldIOCP->GetFieldIOCPSocket(i_nTargetIndex);
					if(pFSock
						&& pFSock->IsValidCharacter()
						&& pFSock->m_pFieldParty == m_pFieldIOCPSocket->m_pFieldParty)
					{
						i_pVectorClientIndex->push_back(i_nTargetIndex);
					}
				}
				else if(0 != i_nTargetCharUID)
				{
					CFieldIOCPSocket *pFSock = m_pFieldIOCPSocket->ms_pFieldIOCP->GetFieldIOCPSocketByCharacterUniqueNumber(i_nTargetCharUID);
					if(pFSock
						&& pFSock->IsValidCharacter()
						&& pFSock->m_pFieldParty == m_pFieldIOCPSocket->m_pFieldParty)
					{
						i_pVectorClientIndex->push_back(pFSock->GetCharacter()->ClientIndex);
					}
				}
			}
		}
		break;
	case SKILLTARGETTYPE_PARTY_ONE_EXCLUDE_ME:	// 22	// 자신을 제외한 하나의 파티원, // 2005-12-05 by cmkwon
		{
			if (m_pFieldIOCPSocket->m_pFieldParty != NULL)
			{
				if (IS_VALID_CLIENT_INDEX(i_nTargetIndex)
					&& m_pFieldIOCPSocket->GetCharacter()->ClientIndex != i_nTargetIndex)
				{
					CFieldIOCPSocket *pFSock = m_pFieldIOCPSocket->ms_pFieldIOCP->GetFieldIOCPSocket(i_nTargetIndex);
					if(pFSock
						&& pFSock->IsValidCharacter()
						&& pFSock->m_pFieldParty == m_pFieldIOCPSocket->m_pFieldParty)
					{
						i_pVectorClientIndex->push_back(i_nTargetIndex);
					}
				}
				else if(0 != i_nTargetCharUID
					&& m_pFieldIOCPSocket->GetCharacter()->CharacterUniqueNumber != i_nTargetCharUID)
				{
					CFieldIOCPSocket *pFSock = m_pFieldIOCPSocket->ms_pFieldIOCP->GetFieldIOCPSocketByCharacterUniqueNumber(i_nTargetCharUID);
					if(pFSock
						&& pFSock->IsValidCharacter()
						&& pFSock->m_pFieldParty == m_pFieldIOCPSocket->m_pFieldParty)
					{
						i_pVectorClientIndex->push_back(pFSock->GetCharacter()->ClientIndex);
					}
				}
			}
		}
		break;
	case SKILLTARGETTYPE_INRANGE_WITH_ME:
		{
			i_pVectorClientIndex->push_back(m_pFieldIOCPSocket->m_character.ClientIndex);
			m_pFieldIOCPSocket->m_pCurrentFieldMapChannel->GetAdjacentCharacterIndexes(
								&m_pFieldIOCPSocket->m_character.PositionVector,
								i_pItemInfo->Range,
								i_pVectorClientIndex, m_pFieldIOCPSocket->m_character.ClientIndex);

		}
		break;
	case SKILLTARGETTYPE_PARTY_WITHOUT_ME:
		{
			CFieldParty *pFParty = m_pFieldIOCPSocket->m_pFieldParty;			// 2006-07-04 by cmkwon
			if (pFParty != NULL)
			{
				pFParty->GetMembersInSameMapToVector(i_pVectorClientIndex,
								m_pFieldIOCPSocket->m_pCurrentFieldMapChannel,
								&m_pFieldIOCPSocket->m_character.PositionVector,
								i_pItemInfo->Range, m_pFieldIOCPSocket->m_character.ClientIndex);
			}
		}
		break;
	case SKILLTARGETTYPE_INRANGE_WITHOUT_ME:
		{
			m_pFieldIOCPSocket->m_pCurrentFieldMapChannel->GetAdjacentCharacterIndexes(
								&m_pFieldIOCPSocket->m_character.PositionVector,
								i_pItemInfo->Range,
								i_pVectorClientIndex, m_pFieldIOCPSocket->m_character.ClientIndex);
		}
		break;
	case SKILLTARGETTYPE_ONE_EXCEPT_OURS:		// 2006-11-23 by cmkwon, 일단 서버에서 체크는 하지 않는다, 클라이언트에서 체크 - 상대세력, PvP 상대, 파티전 상대, 여단전 상대
		{
			CFieldIOCPSocket *pFISock = m_pFieldIOCPSocket->ms_pFieldIOCP->GetFieldIOCPSocket(i_nTargetIndex);
			if(pFISock
				&& pFISock->IsValidCharacter())
			{
				i_pVectorClientIndex->push_back(i_nTargetIndex);
			}
		}
		break;
	case SKILLTARGETTYPE_ALL_ENEMY:			// 2006-11-30 by dhjin, 자신을 중심으로 한 적기
		{
			i_pVectorClientIndex->push_back(m_pFieldIOCPSocket->m_character.ClientIndex);	// 2007-02-07 by dhjin, 스킬 사용 내용 확인 후 수정
		}
		break;
	case SKILLTARGETTYPE_ALL_OURS:			// 2007-02-06 by dhjin, 자신을 중심으로 한 아군기
		{
			m_pFieldIOCPSocket->m_pCurrentFieldMapChannel->GetExactCharacterIndexes(i_pVectorClientIndex, &m_pFieldIOCPSocket->m_character.PositionVector, i_pItemInfo->Range);
			vectClientIndex_t::iterator itr = i_pVectorClientIndex->begin();
			while(itr != i_pVectorClientIndex->end())
			{
				CFieldIOCPSocket	*pTargetCSock = m_pFieldIOCPSocket->ms_pFieldIOCP->GetFieldIOCPSocket(*itr);
				if ( NULL == pTargetCSock
					|| FALSE == pTargetCSock->IsValidCharacter()
					|| COMPARE_INFLUENCE(pTargetCSock->GetCharacter()->InfluenceType, INFLUENCE_TYPE_NORMAL)
					|| FALSE == CFieldIOCPSocket::IsValidCharacterFriend(m_pFieldIOCPSocket, pTargetCSock) )
				{// 2007-03-07 by cmkwon, 일반 세력은 제외, 아군이 아니면 제거
					itr = i_pVectorClientIndex->erase(itr);
					continue;
				}
				itr++;
			}
		}
		break;
	default:
		{
		}
	}
}


/******************************************************************************
**
**	스킬의 각 파라메터 적용.
**
**	Create Info : ??. ??. ??.
**
**	Update Info :	// 2006-07-21 by cmkwon, 인자 추가함 - ClientIndex_t i_SkillUseClientIdx
**
**					// 2010. 06. 08 by hsLee. 인자 추가 - i_fIncreasePowerRatio
**
*******************************************************************************/
BOOL CFieldSkillManager::SetSkillParamFactor(ITEM *i_pItemInfo, ClientIndex_t i_SkillUseClientIdx/*=0*/ , float i_fIncreasePowerRatio /*= 1.0f*/ )
{
// 2009-04-21 by cmkwon, ITEM에 DesParam 필드 개수 8개로 늘리기 - 아래와 같이 수정
// 	m_pFieldIOCPSocket->SetParamFactor(i_pItemInfo->DestParameter1, i_pItemInfo->ParameterValue1, i_SkillUseClientIdx);
// 	m_pFieldIOCPSocket->SetParamFactor(i_pItemInfo->DestParameter2, i_pItemInfo->ParameterValue2, i_SkillUseClientIdx);
// 	m_pFieldIOCPSocket->SetParamFactor(i_pItemInfo->DestParameter3, i_pItemInfo->ParameterValue3, i_SkillUseClientIdx);
// 	m_pFieldIOCPSocket->SetParamFactor(i_pItemInfo->DestParameter4, i_pItemInfo->ParameterValue4, i_SkillUseClientIdx);
	for(int i=0; i < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; i++)
	{
		// 2010. 06. 08 by hsLee 인피니티 필드 2차 난이도 조절. (아군 동일 밸런스 적용.) - 스킬에 밸런스 적용.
		//m_pFieldIOCPSocket->SetParamFactor(i_pItemInfo->ArrDestParameter[i], i_pItemInfo->ArrParameterValue[i], i_SkillUseClientIdx);
		m_pFieldIOCPSocket->SetParamFactor ( i_pItemInfo->ArrDestParameter[i] , (float)( i_pItemInfo->ArrParameterValue[i] * i_fIncreasePowerRatio ) , i_SkillUseClientIdx );
	}

	return TRUE;
}

BOOL CFieldSkillManager::UnsetSkillParamFactor(ITEM *i_pItemInfo)
{
// 2009-04-21 by cmkwon, ITEM에 DesParam 필드 개수 8개로 늘리기 - 아래와 같이 수정
// 	m_pFieldIOCPSocket->UnsetParamFactor(i_pItemInfo->DestParameter1, i_pItemInfo->ParameterValue1);
// 	m_pFieldIOCPSocket->UnsetParamFactor(i_pItemInfo->DestParameter2, i_pItemInfo->ParameterValue2);
// 	m_pFieldIOCPSocket->UnsetParamFactor(i_pItemInfo->DestParameter3, i_pItemInfo->ParameterValue3);
// 	m_pFieldIOCPSocket->UnsetParamFactor(i_pItemInfo->DestParameter4, i_pItemInfo->ParameterValue4);
	for(int i=0; i < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; i++)
	{
		m_pFieldIOCPSocket->UnsetParamFactor(i_pItemInfo->ArrDestParameter[i], i_pItemInfo->ArrParameterValue[i]);
	}

	return TRUE;
}

BOOL CFieldSkillManager::CheckSkillReqItemKind(ITEM *i_pItemInfo)
{
	for (int i = 0; i < SIZE_MAX_POS; i++)
	{
		if (COMPARE_ITEMKIND(i_pItemInfo->ReqItemKind, m_pFieldIOCPSocket->m_arrAttachItems[i].Kind))
		{
			return TRUE;
		}
	}

	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CFieldSkillManager::GetPrepareUseBigBoom(void)
/// \brief		// 2007-07-11 by cmkwon, BigBoom이 PrepareUse 상태 여부
/// \author		cmkwon
/// \date		2007-07-11 ~ 2007-07-11
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CFieldSkillManager::GetPrepareUseBigBoom(void)
{
	return m_bPrepareUseBigBoom;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CFieldSkillManager::SetPrepareUseBigBoom(BOOL i_bPrepareUseBigBoom)
/// \brief		// 2007-07-11 by cmkwon, BigBoom이 PrepareUse 상태 여부
/// \author		cmkwon
/// \date		2007-07-11 ~ 2007-07-11
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CFieldSkillManager::SetPrepareUseBigBoom(BOOL i_bPrepareUseBigBoom)
{
	m_bPrepareUseBigBoom	= i_bPrepareUseBigBoom;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// 2009-04-06 by cmkwon, 콜오브 히어로 스킬 시스템 변경 - 
/// \author		cmkwon
/// \date		2009-04-06 ~ 2009-04-06
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
INT CFieldSkillManager::AddSkillConfirmUse(UID32_t i_targetCharcUID, int i_nUsingSkillNum, DWORD i_dwAvailableSec, MAP_CHANNEL_INDEX i_mapChannIdx, D3DXVECTOR3 i_posVec3)
{
	mt_auto_lock maA(&m_mtvectSkillConfirmUseList);

	this->CheckExpireAvaibleTick();

	SSKILL_CONFIRM_USE tmSkillConUse;
	tmSkillConUse.SkillConfirmUseUID	= m_nSkillConfirmUseUID++;
	tmSkillConUse.ExpireTick			= timeGetTime() + i_dwAvailableSec * 1000;
	tmSkillConUse.TargetCharacterUID	= i_targetCharcUID;
	tmSkillConUse.UsingSkillItemNum		= i_nUsingSkillNum;
	tmSkillConUse.MapChannelIndex		= i_mapChannIdx;
	tmSkillConUse.PositionVec3			= i_posVec3;

	m_mtvectSkillConfirmUseList.push_back(tmSkillConUse);

	return tmSkillConUse.SkillConfirmUseUID;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// 2009-04-06 by cmkwon, 콜오브 히어로 스킬 시스템 변경 - 
/// \author		cmkwon
/// \date		2009-04-06 ~ 2009-04-06
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CFieldSkillManager::PopSkillConfirmUse(INT i_skillConfirmUseUID, UID32_t i_targetCharcUID, int i_nUsingSkillNum, SSKILL_CONFIRM_USE *o_pSkillConfirmUse)
{
	mt_auto_lock maA(&m_mtvectSkillConfirmUseList);

	mtvectSSKILL_CONFIRM_USE::iterator itr(m_mtvectSkillConfirmUseList.begin());
	for(; itr != m_mtvectSkillConfirmUseList.end(); itr++)
	{
		if(itr->SkillConfirmUseUID == i_skillConfirmUseUID
			&& itr->TargetCharacterUID == i_targetCharcUID
			&& itr->UsingSkillItemNum == i_nUsingSkillNum)
		{
			*o_pSkillConfirmUse		= *itr;
			m_mtvectSkillConfirmUseList.erase(itr);
			return TRUE;
		}
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// 2009-04-06 by cmkwon, 콜오브 히어로 스킬 시스템 변경 - 
/// \author		cmkwon
/// \date		2009-04-06 ~ 2009-04-06
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CFieldSkillManager::CheckExpireAvaibleTick(void)
{
	mt_auto_lock maA(&m_mtvectSkillConfirmUseList);
	DWORD dwCurTick = timeGetTime();

	mtvectSSKILL_CONFIRM_USE::iterator itr(m_mtvectSkillConfirmUseList.begin());
	while(itr != m_mtvectSkillConfirmUseList.end())
	{
		if(itr->ExpireTick < dwCurTick)
		{
			itr = m_mtvectSkillConfirmUseList.erase(itr);
			continue;
		}
		itr++;
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// 2009-08-03 by cmkwon, EP3-4 편대 대형 스킬 구현 - 
///					CFieldIOCPSocket::Process_FC_SKILL_CANCEL_SKILL()에서 처리하던 것을 함수로 만듬
/// \author		cmkwon
/// \date		2009-08-05 ~ 2009-08-05
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
Err_t CFieldSkillManager::CancelSkill(ITEM *i_pSkillInfo, MSG_FC_SKILL_CANCEL_SKILL *i_pCancelSkill)
{
	// CANCEL_SKILL은 시간형(SKILLTYPE_TIMELIMIT), 토글형(SKILLTYPE_TOGGLE) 스킬에만 사용할 수 있다.
	switch(i_pSkillInfo->SkillType)
	{
	case SKILLTYPE_TIMELIMIT:
		{
			if (this->IsSkillActivatedByItemNum(i_pCancelSkill->SkillItemID.ItemNum))
			{
				if (!this->DeactivateSkill(i_pSkillInfo, TRUE))
				{					
					return ERR_PROTOCOL_NO_SUCH_SKILL;
				}
			}
		}
		break;
	case SKILLTYPE_TOGGLE:
		{
			if (this->IsSkillActivatedByItemNum(i_pCancelSkill->SkillItemID.ItemNum))
			{
				if (!this->DeactivateSkill(i_pSkillInfo, TRUE))
				{
					return ERR_PROTOCOL_NO_SUCH_SKILL;
				}
			}
		}
		break;
	case SKILLTYPE_CLICK:
		{
			if (SKILL_KIND(i_pSkillInfo->ItemNum) == SKILL_KIND_CONTROL)
			{
				// 자신을 제외한 주위 유닛에게 뿌려주기만 한다.
				INIT_MSG_OF_SIZE(MSG_FC_SKILL_INVALIDATE_SKILL, T_FC_SKILL_INVALIDATE_SKILL, msgInvalidateSkill, msgInvalidateSkillBuf);
				msgInvalidateSkill->SkillItemID.ItemNum = i_pSkillInfo->ItemNum;
				msgInvalidateSkill->SkillItemID.ItemUID = 0;	// 클라이언트 불필요함
				msgInvalidateSkill->ClientIndex = m_pCharacter->ClientIndex;
				m_pFieldIOCPSocket->SendInRangeMessageAroundMe(msgInvalidateSkillBuf, MSG_SIZE(MSG_FC_SKILL_INVALIDATE_SKILL), TRUE);
			}
		}
		break;
	case SKILLTYPE_CHARGING:
		{// 2006-11-30 by dhjin, 차징 취소도 클라이언트로 전송한다
			this->ClearChargingSkill(i_pSkillInfo);			// 2006-12-06 by cmkwon, 취소된 차징스킬을 초기화 한다.
		}
		break;
	default:
		{
			return ERR_PROTOCOL_INVALID_SKILLTYPE;
		}
	}

	return ERR_NO_ERROR;
}


BOOL CFieldSkillManager::DeleteActivatedSkillByItemNum(INT i_nSkillItemNum)
{
	mt_auto_lock alock(&m_vectorAtivatedSkill);

	mtvectorItemID::iterator itr = m_vectorAtivatedSkill.begin();
	while (itr != m_vectorAtivatedSkill.end())
	{
		if (i_nSkillItemNum == itr->ItemNum)
		{
			m_vectorAtivatedSkill.erase(itr);
			return TRUE;
		}

		itr++;
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CFieldSkillManager::ApplyMonSkill(ITEM *i_pSkill, MSG_FN_BATTLE_ATTACK_SKILL * i_pMsg)
/// \brief		인피니티 - 적용 당할 스킬이 있다면 적용
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CFieldSkillManager::ApplyMonSkill(ITEM *i_pSkill, MSG_FN_BATTLE_ATTACK_SKILL * i_pMsg) 
{
	if ( TRUE == IS_ORBIT_SKILL ( i_pSkill->OrbitType ) ) 
	{
		// OrbitSkill 적용
		this->ApplyOrbitSkill ( i_pSkill , i_pMsg->ClientIndex );
		return;		// 2010-03-31 by dhjin, 인피니티(기지방어) -
	}

	// 2010. 06. 08 by hsLee 인피니티 필드 2차 난이도 조절. (아군 동일 밸런스 적용.) - 스킬 밸런스 적용.
	//this->SetSkillParamFactor(i_pSkill, i_pMsg->ClientIndex);		// 2010-03-31 by dhjin, 인피니티(기지방어) -
	this->SetSkillParamFactor ( i_pSkill , i_pMsg->ClientIndex , i_pMsg->fIncreasePowerRatio );		// 2010-03-31 by dhjin, 인피니티(기지방어) -

}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CFieldSkillManager::ApplyOrbitSkill(ITEM *i_pSkill, ClientIndex_t i_ClientIdx)
/// \brief		인피니티 - OrbitSkill 적용
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
// 2010. 06. 08 by hsLee 인피니티 필드 2차 난이도 조절. (아군 동일 밸런스 적용.) - (i_fIncreasePowerRatio) 인자 추가.
void CFieldSkillManager::ApplyOrbitSkill( ITEM *i_pSkill , ClientIndex_t i_ClientIdx , float i_fIncreasePowerRatio /*= 1.0f*/ ) 
{
	switch(i_pSkill->OrbitType) 
	{
		case ORBIT_SKILL_DEBUFF_TIMELIMIT: 
			{
				this->ApplyDebuffSkill(i_pSkill , i_fIncreasePowerRatio );
			}
			break;

		case ORBIT_SKILL_DOT_TIMELIMIT: 
			{
				this->ApplyDotSkill(i_pSkill, i_ClientIdx , i_fIncreasePowerRatio );
			}
			break;
	}	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CFieldSkillManager::ApplyDebuffSkill(ITEM *i_pSkill)
/// \brief		인피니티 - Debuff 적용
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
// 2010. 06. 08 by hsLee 인피니티 필드 2차 난이도 조절. (아군 동일 밸런스 적용.) - (i_fIncreasePowerRatio) 인자 추가.
void CFieldSkillManager::ApplyDebuffSkill(ITEM *i_pSkill , float i_fIncreasePowerRatio /*= 1.0f*/ ) {
	for(int i=0; i < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; i++) {
		switch(i_pSkill->ArrDestParameter[i]) {
			case DES_SKILL_NO_WARNING: {
				m_FieldDebuff.SetDebuff(i_pSkill, i , i_fIncreasePowerRatio );
			}break;
			case DES_SKILL_SLOWMOVING: {
				m_FieldDebuff.SetDebuff(i_pSkill, i , i_fIncreasePowerRatio );
			}break;
			case DES_SKILL_SHIELD_PARALYZE: {
				m_FieldDebuff.SetDebuff(i_pSkill, i , i_fIncreasePowerRatio );
			}break;
			case DES_SKILL_MON_SILENCE_PRIMARY: {
				m_FieldDebuff.SetDebuff(i_pSkill, i , i_fIncreasePowerRatio );
			}break;
			case DES_SKILL_MON_SILENCE_SECOND: {
				m_FieldDebuff.SetDebuff(i_pSkill, i , i_fIncreasePowerRatio );
			}break;
			case DES_SKILL_MON_FREEZE_HP: {
				m_FieldDebuff.SetDebuff(i_pSkill, i , i_fIncreasePowerRatio );
			}break;
			case DES_SKILL_MON_FREEZE_DP: {
				m_FieldDebuff.SetDebuff(i_pSkill, i , i_fIncreasePowerRatio );
			}break;
			case DES_SKILL_MON_FREEZE_SP: {
				m_FieldDebuff.SetDebuff(i_pSkill, i , i_fIncreasePowerRatio );
			}break;
			case DES_SKILL_MON_HOLD: {
				m_FieldDebuff.SetDebuff(i_pSkill, i , i_fIncreasePowerRatio );
			}break;
			case DES_SKILL_MON_SILENCE_SKILL: {
				m_FieldDebuff.SetDebuff(i_pSkill, i , i_fIncreasePowerRatio );
			}break;		
		}	
	}	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CFieldSkillManager::ApplyDotSkill(ITEM *i_pSkill, ClientIndex_t i_ClientIdx)
/// \brief		인피니티 - Dot 적용
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
// 2010. 06. 08 by hsLee 인피니티 필드 2차 난이도 조절. (아군 동일 밸런스 적용.) - (i_fIncreasePowerRatio) 인자 추가.
void CFieldSkillManager::ApplyDotSkill(ITEM *i_pSkill, ClientIndex_t i_ClientIdx , float i_fIncreasePowerRatio /*= 1.0f*/ ) 
{
	for(int i=0; i < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; i++) 
	{
		switch(i_pSkill->ArrDestParameter[i]) 
		{
			case DES_SKILL_MON_STEALING: 
				{
					UID32_t TimerUID = m_pFieldIOCPSocket->m_TimerManager.m_TimerUIDGenerator.GetNext();
					m_pFieldIOCPSocket->m_TimerManager.StartTimerFieldNotCreateTimerUID(TE_TYPE_DOTSKILL_STEALING, TIMER_DOT_INTERVAL, TimerUID, TimerUID);

					// 2010. 06. 08 by hsLee 인피니티 필드 2차 난이도 조절. (아군 동일 밸런스 적용.) - 스킬 밸런스 적용.
					//m_FieldDot.SetDot(i_pSkill, i, i_ClientIdx, TimerUID );
					m_FieldDot.SetDot( i_pSkill , i , i_ClientIdx , TimerUID , i_fIncreasePowerRatio );
				}
				break;

			case DES_SKILL_MON_DRAIN: 
				{
					UID32_t TimerUID = m_pFieldIOCPSocket->m_TimerManager.m_TimerUIDGenerator.GetNext();
					m_pFieldIOCPSocket->m_TimerManager.StartTimerFieldNotCreateTimerUID(TE_TYPE_DOTSKILL_DRAIN, TIMER_DOT_INTERVAL, TimerUID, TimerUID);

					// 2010. 06. 08 by hsLee 인피니티 필드 2차 난이도 조절. (아군 동일 밸런스 적용.) - 스킬 밸런스 적용.
					//m_FieldDot.SetDot(i_pSkill, i, i_ClientIdx, TimerUID);
					m_FieldDot.SetDot( i_pSkill , i , i_ClientIdx , TimerUID , i_fIncreasePowerRatio );
				}
				break;

			case DES_TIME_BOMB: 
				{	// 2010-03-31 by dhjin, 인피니티(기지방어) -
					UID32_t TimerUID = m_pFieldIOCPSocket->m_TimerManager.m_TimerUIDGenerator.GetNext();
					m_pFieldIOCPSocket->m_TimerManager.StartTimerFieldNotCreateTimerUID(TE_TYPE_DOTSKILL_TIME_BOMB, TIMER_DOT_INTERVAL, TimerUID, TimerUID);
					
					// 2010. 06. 08 by hsLee 인피니티 필드 2차 난이도 조절. (아군 동일 밸런스 적용.) - 스킬 밸런스 적용.
					//m_FieldDot.SetDot(i_pSkill, i, i_ClientIdx, TimerUID);
					m_FieldDot.SetDot( i_pSkill , i , i_ClientIdx , TimerUID , i_fIncreasePowerRatio );
				}
				break;
		}
	}	
}

void CFieldSkillManager::ResetDebuffSkill() {
	this->m_FieldDebuff.InitFieldDebuff();
}

void CFieldSkillManager::ResetDotSkill() {
	this->m_FieldDot.InitFieldDot();
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CFieldSkillManager::ReleaseOrbitSkill(ITEM *i_pSkill, BOOL i_bTimeCheck/* = TRUE*/)
/// \brief		인피니티 - OrbitSkill 해제
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CFieldSkillManager::ReleaseOrbitSkill(ITEM *i_pSkill, BOOL i_bTimeCheck/* = TRUE*/) {
	switch(i_pSkill->OrbitType) {
		case ORBIT_SKILL_DEBUFF_TIMELIMIT: {
			this->ReleaseDebuffSkill(i_pSkill, i_bTimeCheck);
		}break;
		case ORBIT_SKILL_DOT_TIMELIMIT: {
			this->ReleaseDotSkill(i_pSkill);
		}break;
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CFieldSkillManager::ReleaseDebuffSkill(ITEM *i_pSkill, BOOL i_bTimeCheck/* = TRUE*/)
/// \brief		인피니티 - DebuffSkill 해제
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CFieldSkillManager::ReleaseDebuffSkill(ITEM *i_pSkill, BOOL i_bTimeCheck/* = TRUE*/) {
	for(int i=0; i < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; i++) {	
		switch(i_pSkill->ArrDestParameter[i]) {
			case DES_SKILL_NO_WARNING: {
				m_FieldDebuff.ReleaseDebuff(i_pSkill->ItemNum, DES_SKILL_NO_WARNING, i_bTimeCheck);
			}break;
			case DES_SKILL_SLOWMOVING: {
				m_FieldDebuff.ReleaseDebuff(i_pSkill->ItemNum, DES_SKILL_SLOWMOVING, i_bTimeCheck);
			}break;
			case DES_SKILL_SHIELD_PARALYZE: {
				m_FieldDebuff.ReleaseDebuff(i_pSkill->ItemNum, DES_SKILL_SHIELD_PARALYZE, i_bTimeCheck);
			}break;
			case DES_SKILL_MON_SILENCE_PRIMARY: {
				m_FieldDebuff.ReleaseDebuff(i_pSkill->ItemNum, DES_SKILL_MON_SILENCE_PRIMARY, i_bTimeCheck);
			}break;
			case DES_SKILL_MON_SILENCE_SECOND: {
				m_FieldDebuff.ReleaseDebuff(i_pSkill->ItemNum, DES_SKILL_MON_SILENCE_SECOND, i_bTimeCheck);
			}break;
			case DES_SKILL_MON_FREEZE_HP: {
				m_FieldDebuff.ReleaseDebuff(i_pSkill->ItemNum, DES_SKILL_MON_FREEZE_HP, i_bTimeCheck);
			}break;
			case DES_SKILL_MON_FREEZE_DP: {
				m_FieldDebuff.ReleaseDebuff(i_pSkill->ItemNum, DES_SKILL_MON_FREEZE_DP, i_bTimeCheck);
			}break;
			case DES_SKILL_MON_FREEZE_SP: {
				m_FieldDebuff.ReleaseDebuff(i_pSkill->ItemNum, DES_SKILL_MON_FREEZE_SP, i_bTimeCheck);
			}break;
			case DES_SKILL_MON_HOLD: {
				m_FieldDebuff.ReleaseDebuff(i_pSkill->ItemNum, DES_SKILL_MON_HOLD, i_bTimeCheck);
			}break;
			case DES_SKILL_MON_SILENCE_SKILL: {
				m_FieldDebuff.ReleaseDebuff(i_pSkill->ItemNum, DES_SKILL_MON_SILENCE_SKILL, i_bTimeCheck);
			}break;	
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CFieldSkillManager::ReleaseDotSkill(ITEM *i_pSkill)
/// \brief		인피니티 - DotSkill 해제
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CFieldSkillManager::ReleaseDotSkill(ITEM *i_pSkill) {
	for(int i=0; i < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; i++) {	
		switch(i_pSkill->ArrDestParameter[i]) {
			case DES_SKILL_MON_STEALING: {
				m_FieldDot.ReleaseDotByCancelSkill(i_pSkill, DES_SKILL_MON_STEALING);
			}break;
			case DES_SKILL_MON_DRAIN: {
				m_FieldDot.ReleaseDotByCancelSkill(i_pSkill, DES_SKILL_MON_DRAIN);
			}break;	
			case DES_TIME_BOMB: {	// 2010-03-31 by dhjin, 인피니티(기지방어) -
				m_FieldDot.ReleaseDotByCancelSkill(i_pSkill, DES_TIME_BOMB);
			}break;	
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CFieldSkillManager::CheckResistance(CFieldIOCPSocket * i_pTargetUser)
/// \brief		인피니티 - 저항 아이템 사용 체크
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CFieldSkillManager::CheckResistance(CFieldIOCPSocket * i_pTargetUser) {
	if(NULL == i_pTargetUser) {
		return FALSE;
	}

	ParamValue_t	ResistanceValue = i_pTargetUser->GetParamResistance();
	if(RAND100() <= ResistanceValue) {
		return TRUE;
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CFieldSkillManager::CheckResistance(CFieldIOCPSocket * i_pTargetUser)
/// \brief		인피니티 - 한 번만 저항하고 없어지는 저항 아이템 추가
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CFieldSkillManager::CheckOnceResistance(CFieldIOCPSocket * i_pTargetUser) {
	if(NULL == i_pTargetUser) {
		return FALSE;
	}
	
	ParamValue_t	ResistanceValue = i_pTargetUser->GetParamOnceResistance();
	if(RAND100() <= ResistanceValue) {
		return TRUE;
	}
	
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CFieldSkillManager::ProcessingTimeBombSkill(float i_DamageRate)
/// \brief		인피니티(기지방어) - 타임밤스킬 처리
/// \author		dhjin
/// \date		2010-03-31
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CFieldSkillManager::ProcessingTimeBombSkill(float i_DamageRate)
{
	if (m_pFieldIOCPSocket->m_bNoDamage
		|| FALSE == m_pFieldIOCPSocket->IsValidCharacter())
	{	// 무적 상태
		return;
	}

	float Damage = m_pFieldIOCPSocket->m_character.CurrentHP * i_DamageRate;
	if(0 >= (int)Damage)
	{	// 타임밤 스킬로는 절대 죽일수없다!!
		return;
	}
	m_pFieldIOCPSocket->ChangeCurrentHP(-Damage);
}
