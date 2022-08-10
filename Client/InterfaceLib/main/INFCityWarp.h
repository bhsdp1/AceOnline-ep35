// INFCityWarp.h: interface for the CINFCityWarp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INFCITYWARP_H__8C8FFF17_5918_4808_A782_6368A036D467__INCLUDED_)
#define AFX_INFCITYWARP_H__8C8FFF17_5918_4808_A782_6368A036D467__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "INFBase.h"

#define CITY_WARP_BUTTON_NUMBER			3
#define CITY_WARP_BUTTON_MOVE			0
#define CITY_WARP_BUTTON_CANCEL			1

#define CITY_WARP_LIST_NUMBER			6
#define CITY_WARP_LIST_STRING_LENGTH	64

class CAtumNode;
class CINFImage;
class CD3DHanFont;
class CINFScrollBar;
class CINFCityWarp : public CINFBase  
{
public:
	CINFCityWarp(CAtumNode* pParent);
	virtual ~CINFCityWarp();

	void Reset();
	void AddWarpTargetInfoList(WARP_TARGET_MAP_INFO_4_EXCHANGE* pInfo);
	void RecvWarpListDone();
	WARP_TARGET_MAP_INFO_4_EXCHANGE *GetCurrentWarpInfo();

	virtual HRESULT InitDeviceObjects();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	void Render();
	virtual void Tick();
	virtual int WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnButtonClicked(int nButton);
	void SendFieldSocketRequestShopWarp();//int nMapIndex, int nTargetIndex);

protected:
	BOOL		m_bRestored;
	CINFScrollBar*	m_pScroll;
	CINFImage*	m_pImgBack;
	CINFImage*	m_pImgTitle;
	CINFImage*	m_pButtonMove[CITY_WARP_BUTTON_NUMBER];
	CINFImage*	m_pButtonCancel[CITY_WARP_BUTTON_NUMBER];
	CINFImage*	m_pImgHightLight;

	int			m_nButtonState[2];		// ��ư ����, CITY_WARP_BUTTON_MOVE, CITY_WARP_BUTTON_CANCEL
	CD3DHanFont *m_pFontWarpList[CITY_WARP_LIST_NUMBER];
	CD3DHanFont *m_pFontWarpPrice[CITY_WARP_LIST_NUMBER];
	char		m_szWarpList[CITY_WARP_LIST_NUMBER][CITY_WARP_LIST_STRING_LENGTH];
	int			m_nWarpListLineNumber;		// ����Ʈ ��ü ����
	int			m_nMapIndex;				// ������ �� �ε���
	int			m_nTargetIndex;				// ������ ���� Ÿ�� �ε���
	
	CVectorWarpTargetInfo m_vecWarpTargetInfo;

	CD3DHanFont*	m_pInfluenceTex;
};

#endif // !defined(AFX_INFCITYWARP_H__8C8FFF17_5918_4808_A782_6368A036D467__INCLUDED_)
