#include "stdafx.h"
#include "UITaskbarSub.h"
#include "N3BASE/AddressFunctionLogger.h"

// Exit butonu için global flag (FMGAME Engine.cpp'den)
extern bool g_bExitButtonPressed;
extern bool g_bDisconnectBlocked; // Disconnect engelleme flag'i (reconnect sonrası exit butonu için)

CUITaskbarSubPlug::CUITaskbarSubPlug()
{
	vector<int>offsets;
	offsets.push_back(0x3DC);
	offsets.push_back(0);

	m_dVTableAddr = Engine->rdword(KO_DLG, offsets);
	m_btnPowerUPStore = 0;
	m_mobdrop = 0;
	btn_wof = 0;
	m_dailyquest = 0;
	m_btnbug = NULL;
	m_btnQuestion = 0;

	ParseUIElements();
	InitReceiveMessage();
}

CUITaskbarSubPlug::~CUITaskbarSubPlug()
{

}

void CUITaskbarSubPlug::ParseUIElements()
{
	m_btnPowerUPStore = Engine->GetChildByID(m_dVTableAddr, "btns_pus");
	m_btnHotkeyH = Engine->GetChildByID(m_dVTableAddr, "btn_hotkey");
	m_btnGlobalMap = Engine->GetChildByID(m_dVTableAddr, "btn_globalmap");
	m_btnInviteParty = Engine->GetChildByID(m_dVTableAddr, "btn_party");
	m_btnExitGame = Engine->GetChildByID(m_dVTableAddr, "btn_exit");	
}

DWORD uiTaskbarSubVTable;
bool CUITaskbarSubPlug::ReceiveMessage(DWORD * pSender, uint32_t dwMsg)
{
	uiTaskbarSubVTable = m_dVTableAddr;
	if (!pSender || pSender == 0)
		return false;

	// DEBUG: Tüm mesajları logla (exit butonu tespiti için - NORMAL LOGIN SÜRECİ ANALİZİ)
	char logMsg[512];
	sprintf_s(logMsg, sizeof(logMsg), "UITaskbarSub::ReceiveMessage called | pSender: 0x%08X | m_btnExitGame: 0x%08X | dwMsg: 0x%08X (UIMSG_BUTTON_CLICK: 0x%08X) | Match: %s", 
		(DWORD)pSender, (DWORD)m_btnExitGame, dwMsg, UIMSG_BUTTON_CLICK, 
		(pSender == (DWORD*)m_btnExitGame && dwMsg == UIMSG_BUTTON_CLICK) ? "YES" : "NO");
	CAddressFunctionLogger::LogReconnectProcess("UITaskbarSub_ReceiveMessage_DEBUG", logMsg);

	// Exit Game butonuna basıldığında: g_bExitButtonPressed flag'ini set et ve g_bDisconnectBlocked flag'ini reset et
	// Bu sayede hkEndGame içinde exit butonu tespit edilecek ve normal kapanma yapılacak
	// Reconnect sonrası grace period aktif olsa bile exit butonu çalışacak
	if (pSender == (DWORD*)m_btnExitGame && dwMsg == UIMSG_BUTTON_CLICK)
	{
		// Exit butonuna basıldı - flag'leri set et (normal kapanma için)
		bool oldExitValue = g_bExitButtonPressed;
		bool oldDisconnectValue = g_bDisconnectBlocked;
		g_bExitButtonPressed = true;
		g_bDisconnectBlocked = false;  // Exit butonuna basıldığında disconnect engellemesini kaldır (reconnect sonrası exit butonu için)
		CAddressFunctionLogger::LogFlagChange("g_bExitButtonPressed", oldExitValue, true, "UITaskbarSub - Exit button pressed");
		CAddressFunctionLogger::LogFlagChange("g_bDisconnectBlocked", oldDisconnectValue, false, "UITaskbarSub - Exit button pressed (disconnect unblocked)");
		
		// WIZ_LOGOUT paketi gönder (server'a logout bilgisi gönder)
		if (Engine && Engine->m_bGameStart)
		{
			Packet logoutPkt(0x0F); // WIZ_LOGOUT = 0x0F
			Engine->Send(&logoutPkt);
			CAddressFunctionLogger::LogReconnectProcess("EXIT_BUTTON", "WIZ_LOGOUT packet sent to server (normal login exit)");
		}
		else
		{
			char noSendMsg[256];
			sprintf_s(noSendMsg, sizeof(noSendMsg), "WIZ_LOGOUT packet NOT sent | Engine: %s | m_bGameStart: %s", 
				Engine ? "VALID" : "NULL", Engine && Engine->m_bGameStart ? "YES" : "NO");
			CAddressFunctionLogger::LogReconnectProcess("EXIT_BUTTON", noSendMsg);
		}
	}

	if (pSender == (DWORD*)m_mobdrop)
	{
		if (Engine->uiSearchMonster != NULL)
			Engine->m_UiMgr->OpenMonsterSearch();

	}
	else if (pSender == (DWORD*)m_dailyquest) {
		if (Engine->uiQuestPage != NULL)
		{

			Engine->uiQuestPage->InitQuests(Engine->uiQuestPage->page);
			Engine->SetVisible(Engine->uiQuestPage->m_dVTableAddr, true);
			Engine->UIScreenCenter(Engine->uiQuestPage->m_dVTableAddr);
		}
	}
	if (pSender == (DWORD*)m_btnbug)
	{
		if (Engine->uiState == NULL)
		{
			Engine->m_UiMgr->ShowMessageBox(xorstr("Error"), xorstr("Support is available only after the game has started."), Ok);
			return true;
		}
		
		if (Engine->uiSupport)
			Engine->uiSupport->OpenSupport();
	}
	else if (pSender == (DWORD*)btn_wof)
	{
		Engine->uiWheel->OpenWheel();
	}

	return true;
}

void __stdcall UITaskbarSubReceiveMessage_Hook(DWORD * pSender, uint32_t dwMsg)
{
	// DEBUG: Hook çağrıldığını logla (exit butonu tespiti için - NORMAL LOGIN SÜRECİ ANALİZİ)
	char hookLogMsg[512];
	sprintf_s(hookLogMsg, sizeof(hookLogMsg), "UITaskbarSubReceiveMessage_Hook called | pSender: 0x%08X | dwMsg: 0x%08X | Engine: %s | uiTaskbarSub: %s", 
		(DWORD)pSender, dwMsg, Engine ? "VALID" : "NULL", Engine && Engine->uiTaskbarSub ? "VALID" : "NULL");
	CAddressFunctionLogger::LogReconnectProcess("UITaskbarSub_HOOK_DEBUG", hookLogMsg);
	
	Engine->uiTaskbarSub->ReceiveMessage(pSender, dwMsg);
	__asm
	{
		MOV ECX, uiTaskbarSubVTable
		PUSH dwMsg
		PUSH pSender
		MOV EAX, KO_UI_TASKBAR_SUB_RECEIVE_MESSAGE_FUNC
		CALL EAX
	}
}

void CUITaskbarSubPlug::InitReceiveMessage()
{
	
	*(DWORD*)KO_UI_TASKBAR_SUB_RECEIVE_MESSAGE_PTR = (DWORD)UITaskbarSubReceiveMessage_Hook;
}
void CUITaskbarSubPlug::OpenGlobalMap()
{
	UITaskbarSubReceiveMessage_Hook((DWORD*)m_btnGlobalMap, UIMSG_BUTTON_CLICK);
}
void CUITaskbarSubPlug::OpenInviteParty()
{
	UITaskbarSubReceiveMessage_Hook((DWORD*)m_btnInviteParty, UIMSG_BUTTON_CLICK);
}
void CUITaskbarSubPlug::OpenHotkeyH()
{
	UITaskbarSubReceiveMessage_Hook((DWORD*)m_btnHotkeyH, UIMSG_BUTTON_CLICK);
}
void CUITaskbarSubPlug::OpenExitGame()
{
	UITaskbarSubReceiveMessage_Hook((DWORD*)m_btnExitGame, UIMSG_BUTTON_CLICK);
}