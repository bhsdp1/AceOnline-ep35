// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.


#include "stdafx.h"
#include "scdialer1.h"

/////////////////////////////////////////////////////////////////////////////
// CSCDialer1

IMPLEMENT_DYNCREATE(CSCDialer1, CWnd)

/////////////////////////////////////////////////////////////////////////////
// CSCDialer1 properties

CString CSCDialer1::GetPCCallAgentIP()
{
	CString result;
	GetProperty(0x1, VT_BSTR, (void*)&result);
	return result;
}

void CSCDialer1::SetPCCallAgentIP(LPCTSTR propVal)
{
	SetProperty(0x1, VT_BSTR, propVal);
}

long CSCDialer1::GetPCCallAgentPort()
{
	long result;
	GetProperty(0x2, VT_I4, (void*)&result);
	return result;
}

void CSCDialer1::SetPCCallAgentPort(long propVal)
{
	SetProperty(0x2, VT_I4, propVal);
}

CString CSCDialer1::GetUserID()
{
	CString result;
	GetProperty(0x3, VT_BSTR, (void*)&result);
	return result;
}

void CSCDialer1::SetUserID(LPCTSTR propVal)
{
	SetProperty(0x3, VT_BSTR, propVal);
}

CString CSCDialer1::GetPhoneNumber()
{
	CString result;
	GetProperty(0x4, VT_BSTR, (void*)&result);
	return result;
}

void CSCDialer1::SetPhoneNumber(LPCTSTR propVal)
{
	SetProperty(0x4, VT_BSTR, propVal);
}

CString CSCDialer1::GetPcn()
{
	CString result;
	GetProperty(0x5, VT_BSTR, (void*)&result);
	return result;
}

void CSCDialer1::SetPcn(LPCTSTR propVal)
{
	SetProperty(0x5, VT_BSTR, propVal);
}

BOOL CSCDialer1::GetAutoAcceptCall()
{
	BOOL result;
	GetProperty(0x6, VT_BOOL, (void*)&result);
	return result;
}

void CSCDialer1::SetAutoAcceptCall(BOOL propVal)
{
	SetProperty(0x6, VT_BOOL, propVal);
}

BOOL CSCDialer1::GetEnableTelBox()
{
	BOOL result;
	GetProperty(0x7, VT_BOOL, (void*)&result);
	return result;
}

void CSCDialer1::SetEnableTelBox(BOOL propVal)
{
	SetProperty(0x7, VT_BOOL, propVal);
}

long CSCDialer1::GetProxyMode()
{
	long result;
	GetProperty(0x8, VT_I4, (void*)&result);
	return result;
}

void CSCDialer1::SetProxyMode(long propVal)
{
	SetProperty(0x8, VT_I4, propVal);
}

short CSCDialer1::GetStatus()
{
	short result;
	GetProperty(0x9, VT_I2, (void*)&result);
	return result;
}

void CSCDialer1::SetStatus(short propVal)
{
	SetProperty(0x9, VT_I2, propVal);
}

CString CSCDialer1::GetPhoneBookList()
{
	CString result;
	GetProperty(0xa, VT_BSTR, (void*)&result);
	return result;
}

void CSCDialer1::SetPhoneBookList(LPCTSTR propVal)
{
	SetProperty(0xa, VT_BSTR, propVal);
}

short CSCDialer1::GetAuthType()
{
	short result;
	GetProperty(0xb, VT_I2, (void*)&result);
	return result;
}

void CSCDialer1::SetAuthType(short propVal)
{
	SetProperty(0xb, VT_I2, propVal);
}

BOOL CSCDialer1::GetDialButtonClicked()
{
	BOOL result;
	GetProperty(0xc, VT_BOOL, (void*)&result);
	return result;
}

void CSCDialer1::SetDialButtonClicked(BOOL propVal)
{
	SetProperty(0xc, VT_BOOL, propVal);
}

short CSCDialer1::GetMyStatus()
{
	short result;
	GetProperty(0xd, VT_I2, (void*)&result);
	return result;
}

void CSCDialer1::SetMyStatus(short propVal)
{
	SetProperty(0xd, VT_I2, propVal);
}

BOOL CSCDialer1::GetDTMFWave()
{
	BOOL result;
	GetProperty(0xe, VT_BOOL, (void*)&result);
	return result;
}

void CSCDialer1::SetDTMFWave(BOOL propVal)
{
	SetProperty(0xe, VT_BOOL, propVal);
}

BOOL CSCDialer1::GetRingWave()
{
	BOOL result;
	GetProperty(0xf, VT_BOOL, (void*)&result);
	return result;
}

void CSCDialer1::SetRingWave(BOOL propVal)
{
	SetProperty(0xf, VT_BOOL, propVal);
}

BOOL CSCDialer1::GetInBand()
{
	BOOL result;
	GetProperty(0x10, VT_BOOL, (void*)&result);
	return result;
}

void CSCDialer1::SetInBand(BOOL propVal)
{
	SetProperty(0x10, VT_BOOL, propVal);
}

CString CSCDialer1::GetLKey()
{
	CString result;
	GetProperty(0x11, VT_BSTR, (void*)&result);
	return result;
}

void CSCDialer1::SetLKey(LPCTSTR propVal)
{
	SetProperty(0x11, VT_BSTR, propVal);
}

CString CSCDialer1::GetWebCallAgentIP()
{
	CString result;
	GetProperty(0x12, VT_BSTR, (void*)&result);
	return result;
}

void CSCDialer1::SetWebCallAgentIP(LPCTSTR propVal)
{
	SetProperty(0x12, VT_BSTR, propVal);
}

long CSCDialer1::GetWebCallAgentPort()
{
	long result;
	GetProperty(0x13, VT_I4, (void*)&result);
	return result;
}

void CSCDialer1::SetWebCallAgentPort(long propVal)
{
	SetProperty(0x13, VT_I4, propVal);
}

short CSCDialer1::GetCallType()
{
	short result;
	GetProperty(0x14, VT_I2, (void*)&result);
	return result;
}

void CSCDialer1::SetCallType(short propVal)
{
	SetProperty(0x14, VT_I2, propVal);
}

short CSCDialer1::GetDeviceType()
{
	short result;
	GetProperty(0x15, VT_I2, (void*)&result);
	return result;
}

void CSCDialer1::SetDeviceType(short propVal)
{
	SetProperty(0x15, VT_I2, propVal);
}

short CSCDialer1::GetSerialPort()
{
	short result;
	GetProperty(0x16, VT_I2, (void*)&result);
	return result;
}

void CSCDialer1::SetSerialPort(short propVal)
{
	SetProperty(0x16, VT_I2, propVal);
}

short CSCDialer1::GetAutoRetryLoginInterval()
{
	short result;
	GetProperty(0x17, VT_I2, (void*)&result);
	return result;
}

void CSCDialer1::SetAutoRetryLoginInterval(short propVal)
{
	SetProperty(0x17, VT_I2, propVal);
}

CString CSCDialer1::GetPlayDevice()
{
	CString result;
	GetProperty(0x18, VT_BSTR, (void*)&result);
	return result;
}

void CSCDialer1::SetPlayDevice(LPCTSTR propVal)
{
	SetProperty(0x18, VT_BSTR, propVal);
}

CString CSCDialer1::GetRecordDevice()
{
	CString result;
	GetProperty(0x19, VT_BSTR, (void*)&result);
	return result;
}

void CSCDialer1::SetRecordDevice(LPCTSTR propVal)
{
	SetProperty(0x19, VT_BSTR, propVal);
}

CString CSCDialer1::GetWaveDevice()
{
	CString result;
	GetProperty(0x1a, VT_BSTR, (void*)&result);
	return result;
}

void CSCDialer1::SetWaveDevice(LPCTSTR propVal)
{
	SetProperty(0x1a, VT_BSTR, propVal);
}

long CSCDialer1::GetNATServerPort()
{
	long result;
	GetProperty(0x1b, VT_I4, (void*)&result);
	return result;
}

void CSCDialer1::SetNATServerPort(long propVal)
{
	SetProperty(0x1b, VT_I4, propVal);
}

long CSCDialer1::GetNATFireTalkPort()
{
	long result;
	GetProperty(0x1c, VT_I4, (void*)&result);
	return result;
}

void CSCDialer1::SetNATFireTalkPort(long propVal)
{
	SetProperty(0x1c, VT_I4, propVal);
}

BOOL CSCDialer1::GetManualRoute()
{
	BOOL result;
	GetProperty(0x1d, VT_BOOL, (void*)&result);
	return result;
}

void CSCDialer1::SetManualRoute(BOOL propVal)
{
	SetProperty(0x1d, VT_BOOL, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// CSCDialer1 operations

BOOL CSCDialer1::Hangup(short Mode)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x1e, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		Mode);
	return result;
}

BOOL CSCDialer1::RegisterServer(short Mode)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x1f, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		Mode);
	return result;
}

BOOL CSCDialer1::UnRegisterServer()
{
	BOOL result;
	InvokeHelper(0x20, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL CSCDialer1::AcceptCall(short IsAccept)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x21, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		IsAccept);
	return result;
}

BOOL CSCDialer1::SetLicenseKey(LPCTSTR LicenseKey)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x22, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		LicenseKey);
	return result;
}

void CSCDialer1::SpkVolumeUp()
{
	InvokeHelper(0x23, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void CSCDialer1::SpkVolumeDown()
{
	InvokeHelper(0x24, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void CSCDialer1::MicVolumeUp()
{
	InvokeHelper(0x25, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void CSCDialer1::MicVolumeDown()
{
	InvokeHelper(0x26, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void CSCDialer1::PushButton(LPCTSTR button)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x27, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 button);
}

BOOL CSCDialer1::IsRegistered()
{
	BOOL result;
	InvokeHelper(0x28, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL CSCDialer1::RetrievePhoneBook()
{
	BOOL result;
	InvokeHelper(0x29, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

void CSCDialer1::GetAudioDeviceList()
{
	InvokeHelper(0x2a, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void CSCDialer1::Forwarding(short Type, LPCTSTR Destination)
{
	static BYTE parms[] =
		VTS_I2 VTS_BSTR;
	InvokeHelper(0x2b, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Type, Destination);
}

void CSCDialer1::SendVideoInfo(LPCTSTR IP, long Port)
{
	static BYTE parms[] =
		VTS_BSTR VTS_I4;
	InvokeHelper(0x2c, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 IP, Port);
}

BOOL CSCDialer1::SendMessage(LPCTSTR Destination, LPCTSTR Message)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR;
	InvokeHelper(0x2d, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		Destination, Message);
	return result;
}

long CSCDialer1::SerialMessage(long dwMsg)
{
	long result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x2e, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		dwMsg);
	return result;
}

long CSCDialer1::TimeOutTone()
{
	long result;
	InvokeHelper(0x2f, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

void CSCDialer1::Dial()
{
	InvokeHelper(0x30, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void CSCDialer1::ReDial()
{
	InvokeHelper(0x31, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

CString CSCDialer1::GetVersion()
{
	CString result;
	InvokeHelper(0x32, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return result;
}

void CSCDialer1::SendVideoPause(long Flag)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x33, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Flag);
}

void CSCDialer1::SetKeepAliveSerial(LPCTSTR KeepAlive)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x34, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 KeepAlive);
}

void CSCDialer1::MakeConference(short AttendantCount, LPCTSTR ConferenceInfo)
{
	static BYTE parms[] =
		VTS_I2 VTS_BSTR;
	InvokeHelper(0x35, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 AttendantCount, ConferenceInfo);
}

void CSCDialer1::JoinConference(LPCTSTR ConferenceID, LPCTSTR ConferenceInfo)
{
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR;
	InvokeHelper(0x36, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 ConferenceID, ConferenceInfo);
}

void CSCDialer1::InviteConference(LPCTSTR Callee)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x37, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Callee);
}

void CSCDialer1::CloseConference()
{
	InvokeHelper(0x38, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void CSCDialer1::StatusConference(short Status, LPCTSTR StatusInfo)
{
	static BYTE parms[] =
		VTS_I2 VTS_BSTR;
	InvokeHelper(0x39, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Status, StatusInfo);
}

void CSCDialer1::LeaveConference()
{
	InvokeHelper(0x3a, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

long CSCDialer1::SetSpkVolume(long i_nVolumeValueOfPercent)
{
	long result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x41, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		i_nVolumeValueOfPercent);
	return result;
}

long CSCDialer1::GetSpkVolume()
{
	long result;
	InvokeHelper(0x42, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

void CSCDialer1::AboutBox()
{
	InvokeHelper(0xfffffdd8, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}