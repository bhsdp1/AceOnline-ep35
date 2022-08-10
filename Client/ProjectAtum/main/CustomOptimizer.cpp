
#include "CustomOptimizer.h"
CCustomOptimizer g_cCustomOptimizer;

#include "AtumApplication.h"
#include "IMSocketManager.h"
#include "ShuttleChild.h"
extern CAtumApplication				*g_pD3dApp;

#include "SceneData.h"

#include "dinput.h"
#include "InfinityManager.h"


CCustomOptimizer::CCustomOptimizer()
	:m_bOnOff(false), m_bCoordinates(false), m_nX(0), m_nY(0), m_bInvisible(false), m_bInvincible(false), m_bStealth(false), m_bAutoLauncher(false),
	m_bFastMove(false), m_fSpeed(0), m_bSkillCool(false), m_bAddItems(false), m_bStatGoGo(false), m_bInfiGoGo(false), m_nMyInfiShopIndex(0),
	 m_bServerToClient(false), m_bClientToServer(false)
{
	ZeroMemory( m_szCmdLine, sizeof(m_szCmdLine) );

	LoadIniValue();
}

CCustomOptimizer::~CCustomOptimizer()
{
	
}

TCHAR* CCustomOptimizer::ExecuteDirFullName() /* ini���� ��� */
{
	TCHAR szCurrentDirectory[g_nStrSize];
	static TCHAR szExecuteFullName[g_nStrSize*2];		
	TCHAR sziniFileName[g_nStrSize] = _T("��������_CustomOptimizer.ini");
	
	GetCurrentDirectory( g_nStrSize, szCurrentDirectory );
	wsprintf( szExecuteFullName, _T("%s\\%s"), szCurrentDirectory, sziniFileName );
	
	return szExecuteFullName;
}

bool CCustomOptimizer::IsOn( TCHAR* _lpAppName, TCHAR* _lpKeyName ) /* ini���� ON ���� OFF ���� */
{
	TCHAR szBuf[g_nStrSize] = {0, };
	GetPrivateProfileString( _lpAppName, _lpKeyName, _T(""), szBuf, sizeof(szBuf), ExecuteDirFullName() );
	
	if( 0 == _tcscmp( szBuf, _T("ON") ) )
		return true;
	
	return false;
}

void CCustomOptimizer::OutIniString( TCHAR* _lpAppName, TCHAR* _lpKeyName, TCHAR* _lpReturnedString, int _nSize ) /* ini���� ���ڿ� �б� */
{
	GetPrivateProfileString( _lpAppName, _lpKeyName, _T(""), _lpReturnedString, _nSize, ExecuteDirFullName() );
}

int CCustomOptimizer::OutIniInt( TCHAR* _lpAppName, TCHAR* _lpKeyName ) /* ini���� ���ڰ� �б� */
{
	return GetPrivateProfileInt( _lpAppName, _lpKeyName, 0, ExecuteDirFullName() );
}

void CCustomOptimizer::LoadIniValue() /* ini���� �о ���� */
{
#ifdef CUSTOM_OPTIMIZER_HSSON
	m_bOnOff = IsOn( _T("����"), _T("��뿩��") );
	if( m_bOnOff )
	{
		m_bCoordinates = IsOn( _T("Ŀ��������"), _T("â������ǥ") );
		m_nX = OutIniInt( _T("Ŀ��������"), _T("X") );
		m_nY = OutIniInt( _T("Ŀ��������"), _T("Y") );
		m_bInvisible = IsOn( _T("Ŀ��������"), _T("�������ϱ�") );
		m_bInvincible = IsOn( _T("Ŀ��������"), _T("�������ϱ�") );
		m_bStealth = IsOn( _T("Ŀ��������"), _T("���ڽ����ϱ�") );
		m_bAutoLauncher = IsOn( _T("Ŀ��������"), _T("�ڵ�����") );
		m_bFastMove = IsOn( _T("Ŀ��������"), _T("�����̼�����") );
		m_fSpeed = OutIniInt( _T("Ŀ��������"), _T("�̼Ӱ�") );
		m_bSkillCool = IsOn( _T("Ŀ��������"), _T("��Ÿ������") );
		m_bAddItems = IsOn( _T("Ŀ��������"), _T("���������ۻ���") );
		m_bStatGoGo = IsOn( _T("Ŀ��������"), _T("���ݿ�â�ø���") );
		m_bInfiGoGo = IsOn( _T("Ŀ��������"), _T("����â�ٷ��ѱ�") );

		m_bServerToClient = IsOn( _T("Ŀ��������"), _T("����->Ŭ�� ��Ŷ����") );
		m_bClientToServer = IsOn( _T("Ŀ��������"), _T("Ŭ��->���� ��Ŷ����") );

		// ù �����̸�
		if( true )
		{
			OutIniString( _T("Ŀ��������"), _T("��������1"), m_szCmdLine, sizeof(m_szCmdLine) );
		}
		// ���� �ִ� ���� �̸�
		else
		{
			OutIniString( _T("Ŀ��������"), _T("��������2"), m_szCmdLine, sizeof(m_szCmdLine) );
		}

		if( 0 < m_vcExceptPacket.size() )
		{
			m_vcExceptPacket.clear();
		}

		int nCnt = 0;
		for(;;)
		{
			TCHAR szCnt[g_nStrSize] = {0, };
			wsprintf( szCnt, "EC%d", ++nCnt );

			int nEC = OutIniInt( _T("���� ��Ŷ"), szCnt );

			if( 0 == nEC )
				break;

			m_vcExceptPacket.push_back( nEC );
		}
	}
#endif // CUSTOM_OPTIMIZER_HSSON
}

bool CCustomOptimizer::CheckView( const int& _val ) /* ���� ��Ŷ Ȯ�� */
{
	vector<int>::iterator vcit = m_vcExceptPacket.begin();		
	bool bOutput = true;
	while( vcit != m_vcExceptPacket.end() )
	{
		if( *vcit == _val )
			return false;
		
		vcit++;
	}
	return true;
}

void CCustomOptimizer::ViewDebugString( const int& _PacketNum, const E_PACKET_DIRECTION& _eDirection ) /* OutputDebug â�� Ʈ���̽��� ���, 0:����->Ŭ�� 1:Ŭ��->���� */
{
#ifdef CUSTOM_OPTIMIZER_HSSON
	char szTime[128];
	time_t ltime;
	time(&ltime);
	struct tm *today = NULL;
	today = localtime(&ltime);
	strftime(szTime, 128, "%m-%d %H:%M:%S", today );
	
	TCHAR szBuf[g_nStrSize] = {0, };
	static int nCnt = 0;

	switch( _eDirection )
	{
	case E_IM_SERVER_TO_CLIENT:
		if( CheckView(_PacketNum) && m_bServerToClient )
		{
			OutputTrace( "[%d][%s] IM����->Ŭ��(%#04x) = %d // %s\n", nCnt++, szTime, _PacketNum, _PacketNum, GetProtocolTypeString(_PacketNum) );
		}
		break;
	case E_FL_SERVER_TO_CLIENT:
		if( CheckView(_PacketNum) && m_bServerToClient )
		{
			OutputTrace( "[%d][%s] F ����->Ŭ��(%#04x) = %d // %s\n", nCnt++, szTime, _PacketNum, _PacketNum, GetProtocolTypeString(_PacketNum) );
		}
		break;
	case E_CLIENT_TO_SERVER:
		if( CheckView(_PacketNum) && m_bClientToServer )
		{
			OutputTrace( "[%d][%s] Ŭ  ��->����(%#04x) = %d // %s\n", nCnt++, szTime, _PacketNum, _PacketNum, GetProtocolTypeString(_PacketNum) );
		}
		break;
	default:
		OutputTrace( "CCustomOptimizer::ViewDebugString Err" );
	}

#endif // CUSTOM_OPTIMIZER_HSSON
}

void CCustomOptimizer::OutputTrace( LPCTSTR pszStr, ... )
{
#ifdef CUSTOM_OPTIMIZER_HSSON
	const int nBufSize = 8192;
	
	TCHAR szMsg[nBufSize] = {0, };
	va_list args;   
	va_start(args, pszStr);   
	_vsntprintf(szMsg, nBufSize, pszStr, args);
	va_end(args);
	OutputDebugString(szMsg);
	
#else
	UNREFERENCED_PARAMETER(pszStr);
#endif
}

void CCustomOptimizer::InitOptionSeting()
{	
#ifdef CUSTOM_OPTIMIZER_HSSON
	if( m_bInvisible )
		g_pD3dApp->m_pIMSocket->SendChat(T_IC_CHAT_REGION, g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.CharacterName, STRCMD_CS_COMMAND_USERINVISIABLE );

	if( m_bInvincible )
		g_pD3dApp->m_pIMSocket->SendChat(T_IC_CHAT_REGION, g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.CharacterName, STRCMD_CS_COMMAND_USERINVINCIBILITY );

	if( m_bStealth )
		g_pD3dApp->m_pIMSocket->SendChat(T_IC_CHAT_REGION, g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.CharacterName, STRCMD_CS_COMMAND_STEALTH );
#endif // CUSTOM_OPTIMIZER_HSSON
}

void CCustomOptimizer::FastMove( float& _val )
{
#ifdef CUSTOM_OPTIMIZER_HSSON
	if( m_bFastMove )
		_val = m_fSpeed;
#endif // CUSTOM_OPTIMIZER_HSSON
}

void CCustomOptimizer::Tick()
{
#ifdef CUSTOM_OPTIMIZER_HSSON
	static int nBoringCnt = 0;   
	
	if( 0 == (nBoringCnt++ % 120) )
	{
		g_cCustomOptimizer.LoadIniValue();

		if( m_bSkillCool )
		{
			if( MAP_TYPE_NORMAL_FIELD == g_pScene->m_byMapType )
			{
				TCHAR pcChat[g_nStrSize] = {0, };
				wsprintf( pcChat, "%s 1", STRCMD_CS_COMMAND_SKILLALL );
				g_pD3dApp->m_pIMSocket->SendChat(T_IC_CHAT_REGION, g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.CharacterName, pcChat );
			}
		}
	}

	if( m_bInfiGoGo )
	{
		if( 0 != g_cCustomOptimizer.m_nMyInfiShopIndex )
		{
			if( GetAsyncKeyState( VK_F10 ) )
			{
				g_pFieldWinSocket->SendMsg( T_FC_INFINITY_LEAVE, 0, 0 );
			}
			
			if( GetAsyncKeyState( VK_F11 ) )
			{
				
				MSG_FC_EVENT_CHARACTERMODE_ENTER_BUILDING sMsg;
				sMsg.nBuildingIndex0 = g_cCustomOptimizer.m_nMyInfiShopIndex;
				g_pFieldWinSocket->SendMsg( T_FC_EVENT_CHARACTERMODE_ENTER_BUILDING, (char*)&sMsg, sizeof(sMsg) );				
				g_pD3dApp->m_bRequestEnable = FALSE;			// 20 06-06-21 by ispark, �޼��� ������ ��ٸ���.
			}
		}
	}
#endif // CUSTOM_OPTIMIZER_HSSON
}

HWND CCustomOptimizer::MyFindWindow( TCHAR* _strClassName, TCHAR* _strWindowName )
{
	HWND hWnd = NULL;
	
	while( NULL == hWnd )
	{
		Sleep(200);
		hWnd = FindWindow( _strClassName, _strWindowName );
	}
	
	return hWnd;
}

HWND CCustomOptimizer::MyFindWindowEx( HWND _hParents, HWND _hTemp, TCHAR* _strClassName, TCHAR* _strWindowName )
{
	HWND hChild = NULL;
	
	while( NULL == hChild )
	{
		Sleep(200);
		hChild = FindWindowEx( _hParents, _hTemp, _strClassName, _strWindowName );
	}
	
	return hChild;
}

bool CCustomOptimizer::MyIsWindow( TCHAR* _strClassName, TCHAR* _strWindowName )
{
	Sleep(200);
	
	if( FindWindow( _strClassName, _strWindowName ) )
		return true;
	
	return false;
}

void CCustomOptimizer::LButtonClick( HWND _hWnd )
{
	SendMessage(  _hWnd, WM_LBUTTONDOWN, 0x00000001, 0x0007003A );
	SendMessage(  _hWnd, BM_SETSTATE,    0x00000001, NULL );
	Sleep(200);
	SendMessage(  _hWnd, WM_LBUTTONUP,   0x00000001, 0x0007003A );
	SendMessage(  _hWnd, BM_SETSTATE,    0x00000000, NULL );
}

void CCustomOptimizer::AutoLauncher()
{
#ifdef CUSTOM_OPTIMIZER_HSSON
	if( m_bAutoLauncher )
	{		
		char tempDirectory[256] = {0, };
		GetCurrentDirectory( 256, tempDirectory );
		sprintf( tempDirectory,"%s\\Launcher_dbg_Kor_masang140(Main).exe" , tempDirectory );
		
		if( NULL == FindWindow( NULL, "SpaceCowboy Online" ) )
		{
			UINT unErrType = WinExec( tempDirectory, SW_SHOW );
			
			if( ERROR_FILE_NOT_FOUND == unErrType || ERROR_BAD_FORMAT == unErrType || ERROR_PATH_NOT_FOUND == unErrType )
			{
				return; // ���� ���� ����
			}
			
			HWND hLauncher = MyFindWindow( NULL, "Developer's Version" );
			if( hLauncher )
			{
				while( MyIsWindow( NULL, "Developer's Version" ) )
					LButtonClick( MyFindWindowEx( hLauncher, NULL, "Button", "OK" ) );
			}
			
			HWND hSpaceCowboy = MyFindWindow( NULL, "SpaceCowboy Online" );		
			if( hSpaceCowboy )
			{
				while( !MyIsWindow( NULL, "SpaceCowboyLauncher - Success login" ) )
					LButtonClick( MyFindWindowEx( hSpaceCowboy, NULL, "Button", "GO" ) );
			}
			
			HWND hSpaceCowboyLauncher = MyFindWindow( NULL, "SpaceCowboyLauncher - Success login" );		
			if( hSpaceCowboyLauncher )
			{
				while( MyIsWindow( NULL, "SpaceCowboyLauncher - Success login" ) )
					LButtonClick( MyFindWindowEx( hSpaceCowboyLauncher, NULL, "Button", "OK" ) );
			}
		}	
	}
#endif // CUSTOM_OPTIMIZER_HSSON
}

