#include "stdafx.h"
#include "MessageBoxUIF.h"
#include "N3BASE/AddressFunctionLogger.h"

CUIMessageBox::CUIMessageBox(MsgBoxTypes type, ParentTypes parentType)
{
	m_Type = type;
	m_ParentType = parentType;

	m_txtTitle = NULL;
	m_txtMessage = NULL;

	m_editTextbox = NULL;

	m_btnNo = NULL;
	m_btnYes = NULL;
	m_btnOk = NULL;
}

CUIMessageBox::~CUIMessageBox()
{
}
 
bool CUIMessageBox::Load(HANDLE hFile)
{
	if (CN3UIBase::Load(hFile) == false) return false;

	// Standart MessageBox ID'leri
	std::string find = xorstr("Text_title");
	m_txtTitle = (CN3UIString*)GetChildByID(find);
	
	// Fallback: text_title (küçük harf - re_reconnect_fm.uif için)
	if (m_txtTitle == NULL)
	{
		find = xorstr("text_title");
		m_txtTitle = (CN3UIString*)GetChildByID(find);
	}
	
	find = xorstr("Text_Message");
	m_txtMessage = (CN3UIString*)GetChildByID(find);
	
	// Fallback: text_msg (re_reconnect_fm.uif için)
	if (m_txtMessage == NULL)
	{
		find = xorstr("text_msg");
		m_txtMessage = (CN3UIString*)GetChildByID(find);
	}

	find = xorstr("Edit_Common");
	m_editTextbox = (CN3UIEdit*)GetChildByID(find);

	find = xorstr("Btn_Yes");
	m_btnYes = (CN3UIButton*)GetChildByID(find);
	
	// Fallback: btn_reconnect (re_reconnect_fm.uif için)
	if (m_btnYes == NULL)
	{
		find = xorstr("btn_reconnect");
		m_btnYes = (CN3UIButton*)GetChildByID(find);
	}
	
	find = xorstr("Btn_No");
	m_btnNo = (CN3UIButton*)GetChildByID(find);
	
	// Fallback: btn_exit (re_reconnect_fm.uif için)
	if (m_btnNo == NULL)
	{
		find = xorstr("btn_exit");
		m_btnNo = (CN3UIButton*)GetChildByID(find);
	}
	
	find = xorstr("btn_ok");
	m_btnOk = (CN3UIButton*)GetChildByID(find);

	// NULL kontrolü ile SetVisible çağır (crash önleme)
	if (m_Type == Ok || m_Type == OkWithEdit)
	{
		if (m_btnYes) m_btnYes->SetVisible(false);
		if (m_btnNo) m_btnNo->SetVisible(false);
		if (m_btnOk) m_btnOk->SetVisible(true);
		SetVisibleTextbox(m_Type == OkWithEdit);
	}
	else if (m_Type == YesNo || m_Type == YesNoWithEdit)
	{
		if (m_btnYes) m_btnYes->SetVisible(true);
		if (m_btnNo) m_btnNo->SetVisible(true);
		if (m_btnOk) m_btnOk->SetVisible(false);
		SetVisibleTextbox(m_Type == YesNoWithEdit);
	}
	else if (m_Type == Cancel)
	{
		if (m_btnYes) m_btnYes->SetVisible(false);
		if (m_btnNo) m_btnNo->SetVisible(false);
		if (m_btnOk) m_btnOk->SetVisible(false);
		SetVisibleTextbox(false);
	}

	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, Engine->m_UiMgr->GetScreenCenter(this).y);

	return true;
}

void CUIMessageBox::Update(MsgBoxTypes type, ParentTypes parentType)
{
	m_Type = type;
	m_ParentType = parentType;

	// NULL kontrolü ile SetVisible çağır (crash önleme)
	if (m_Type == Ok || m_Type == OkWithEdit)
	{
		if (m_btnYes) m_btnYes->SetVisible(false);
		if (m_btnNo) m_btnNo->SetVisible(false);
		if (m_btnOk) m_btnOk->SetVisible(true);
		SetVisibleTextbox(m_Type == OkWithEdit);
	}
	else if (m_Type == YesNo || m_Type == YesNoWithEdit)
	{
		if (m_btnYes) m_btnYes->SetVisible(true);
		if (m_btnNo) m_btnNo->SetVisible(true);
		if (m_btnOk) m_btnOk->SetVisible(false);
		SetVisibleTextbox(m_Type == YesNoWithEdit);
	}
	else if (m_Type == Cancel)
	{
		if (m_btnYes) m_btnYes->SetVisible(false);
		if (m_btnNo) m_btnNo->SetVisible(false);
		SetVisibleTextbox(false);
		if (m_btnOk) m_btnOk->SetVisible(false);
	}

	SetPos(Engine->m_UiMgr->GetScreenCenter(this).x, Engine->m_UiMgr->GetScreenCenter(this).y);
}

void CUIMessageBox::SetTitle(std::string title)
{
	if (m_txtTitle) m_txtTitle->SetString(title);
}

void CUIMessageBox::SetMessage(std::string msg)
{
	if (m_txtMessage) m_txtMessage->SetString(msg);
}

void CUIMessageBox::SetVisibleTextbox(bool bVisible)
{
	if (NULL == m_editTextbox) return;

	m_editTextbox->SetString("");
	m_editTextbox->SetVisible(bVisible);
	if (bVisible) m_editTextbox->SetFocus();
	else m_editTextbox->KillFocus();
}

bool CUIMessageBox::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		if (pSender == m_btnYes)
		{
			if (m_ParentType == PARENT_PAGE_STATE)
			{
				Packet result(XSafe);
				result << uint8_t(XSafeOpCodes::RESET) << uint8(2);
				Engine->Send(&result);
			}
			else if (m_ParentType == PARENT_SKILL_TREE)
			{
				Packet result(XSafe);
				result << uint8_t(XSafeOpCodes::RESET) << uint8(1);
				Engine->Send(&result);
			}
			else if (m_ParentType == PARENT_PERK_RESET) {
				Packet result(XSafe, uint8(XSafeOpCodes::PERKS));
				result << uint8(perksSub::perkReset);
				Engine->Send(&result);
			}
			else if (m_ParentType == PARENT_MONSTER_STONE)
			{
				// Monster Stone confirmation - Send WIZ_EVENT MONSTER_STONE with item ID
				// TempleOpCodes::MONSTER_STONE = 6
				// Normal Monster Stone item ID = 900144023
				Packet result(WIZ_EVENT, uint8(6));
				result << uint32(900144023); // Normal Monster Stone item ID
				Engine->Send(&result);
			}
			else if (m_ParentType == PARENT_PARTY_MONSTER_STONE)
			{
				// Party Monster Stone confirmation - Send WIZ_EVENT MONSTER_STONE with item ID
				// TempleOpCodes::MONSTER_STONE = 6
				// Party Monster Stone item ID = 300145039
				Packet result(WIZ_EVENT, uint8(6));
				result << uint32(300145039); // Party Monster Stone item ID
				Engine->Send(&result);
			}
			else if (m_ParentType == PARENT_DISCONNECT_RECONNECT)
			{
				// Disconnect Reconnect - Confirm butonu: Reconnect işlemini başlat
				// Detaylı loglama yap
				CAddressFunctionLogger::LogReconnectButtonPressed();
				
				// Oyun durumunu detaylı logla (Engine bilgileri)
				if(Engine)
				{
					char gameStateInfo[512];
					sprintf_s(gameStateInfo, "Reconnect button clicked | Engine: 0x%08X | GameStart: %s | SelectedCharacter: %s | ZoneID: %d | ConnectedIP: %s", 
						(DWORD)Engine,
						Engine->m_bGameStart ? "YES" : "NO",
						Engine->m_bSelectedCharacter ? "YES" : "NO",
						Engine->Player.ZoneID,
						Engine->m_connectedIP.c_str());
					CAddressFunctionLogger::LogReconnectProcess("BUTTON_PRESSED", gameStateInfo);
					char gameStateDetails[256];
					sprintf_s(gameStateDetails, "ZoneID: %d", Engine->Player.ZoneID);
					CAddressFunctionLogger::LogGameState("RECONNECT_BUTTON", gameStateDetails);
					CAddressFunctionLogger::LogSocketState("RECONNECT_BUTTON", Engine->m_connectedIP.c_str(), 0, true);
				}
				else
				{
					CAddressFunctionLogger::LogReconnectProcess("BUTTON_PRESSED", "Reconnect button clicked but Engine is NULL");
				}
				
				// PerformReconnect fonksiyonunu çağır
				extern void PerformReconnect();
				PerformReconnect();
			}
		}
		else if (pSender == m_btnNo)
		{
			if (m_ParentType == PARENT_DISCONNECT_RECONNECT)
			{
				// Disconnect Reconnect - Cancel butonu: Oyunu kapat
				// g_bDisconnectBlocked flag'ini false yap ve normal shutdown işlemini tetikle
				extern bool g_bDisconnectBlocked;
				
				// Disconnect engelleme flag'ini kaldır (oyunu kapatmaya izin ver) - flag değişikliğini logla
				bool oldValue = g_bDisconnectBlocked;
				g_bDisconnectBlocked = false;
				CAddressFunctionLogger::LogFlagChange("g_bDisconnectBlocked", oldValue, false, "MessageBoxUIF - Cancel button pressed");
				
				// MessageBox'ı kapat
				if (Engine->m_UiMgr && Engine->m_UiMgr->uiMsgBox)
				{
					Engine->m_UiMgr->uiMsgBox->Close();
					Engine->m_UiMgr->uiMsgBox = NULL;
				}
				
				// Normal shutdown işlemini tetikle (PostQuitMessage çağır)
				// g_bDisconnectBlocked false olduğu için hook normal şekilde çalışacak ve oyun kapanacak
				PostQuitMessage(0);
			}
		}
		else if (pSender == m_btnOk)
		{
			if (m_ParentType == PARENT_LOGIN)
			{
				exit(0);
				FreeLibrary(GetModuleHandle(NULL));
				TerminateProcess(GetCurrentProcess(), 0);
			}
		}

		Close();
	}

	return true;
}

bool CUIMessageBox::OnKeyPress(int iKey)
{
	if (!IsVisible())
		return CN3UIBase::OnKeyPress(iKey);

	if (m_Type == Ok || m_Type == OkWithEdit)
	{
		if (iKey == DIK_ESCAPE || iKey == DIK_RETURN)
		{
			ReceiveMessage(m_btnOk, UIMSG_BUTTON_CLICK);
			return true;
		}
	}
	else if (m_Type == YesNo || m_Type == YesNoWithEdit)
	{
		if (iKey == DIK_ESCAPE)
		{
			ReceiveMessage(m_btnNo, UIMSG_BUTTON_CLICK);
			return true;
		}
		else if (iKey == DIK_RETURN)
		{
			ReceiveMessage(m_btnYes, UIMSG_BUTTON_CLICK);
			return true;
		}
	}
	else if (m_Type == Cancel)
	{
		if (iKey == DIK_ESCAPE)
		{
			ReceiveMessage(m_btnNo, UIMSG_BUTTON_CLICK);
			return true;
		}
	}

	return CN3UIBase::OnKeyPress(iKey);
}

void CUIMessageBox::SimulateYesClick()
{
	if (m_btnYes)
		ReceiveMessage(m_btnYes, UIMSG_BUTTON_CLICK);
}

void CUIMessageBox::Close()
{

	m_Type = None;
	m_ParentType = PARENT_NONE;

	this->SetVisible(false);
	this->SetMessage("");
	this->SetTitle("");
	this->SetVisibleTextbox(false);

	Engine->m_UiMgr->RemoveChild(this);
	Engine->m_UiMgr->uiMsgBox->Release();
	Engine->m_UiMgr->uiMsgBox = NULL;
}


uint8 FMGAMEEngine::GetNation()
{
	return *(uint8*)(*(DWORD*)KO_PTR_CHR + KO_OFF_NATION);
}

void CUIMessageBox::OpenMessageBox(MsgBoxTypes type, ParentTypes parent)
{
#if (HOOK_SOURCE_VERSION == 1098)
	uint8 Nation = *(uint8*)(*(DWORD*)KO_PTR_CHR + KO_OFF_NATION);
	if (Nation == 1)
	{
		std::string name = Engine->dcpUIF(xorstr("FMGAME\\Ka_MessageBox.fmgame"));
		Engine->m_UiMgr->uiMsgBox = new CUIMessageBox(type, parent);
		Engine->m_UiMgr->uiMsgBox->Init(Engine->m_UiMgr->uiBase);
		Engine->m_UiMgr->uiMsgBox->LoadFromFile(Engine->m_BasePath + name.c_str(), N3FORMAT_VER_1068);
		Engine->m_UiMgr->AddChild(Engine->m_UiMgr->uiMsgBox);
		remove(name.c_str());
}
	else if (Nation == 2)
	{
		std::string name = Engine->dcpUIF(xorstr("FMGAME\\El_MessageBox.fmgame"));
		Engine->m_UiMgr->uiMsgBox = new CUIMessageBox(type, parent);
		Engine->m_UiMgr->uiMsgBox->Init(Engine->m_UiMgr->uiBase);
		Engine->m_UiMgr->uiMsgBox->LoadFromFile(Engine->m_BasePath + name.c_str(), N3FORMAT_VER_1068);
		Engine->m_UiMgr->AddChild(Engine->m_UiMgr->uiMsgBox);
		remove(name.c_str());
	}
#else
	std::string name = Engine->dcpUIF(xorstr("FMGAME\\re_message_box.fmgame"));
	Engine->m_UiMgr->uiMsgBox = new CUIMessageBox(type, parent);
	Engine->m_UiMgr->uiMsgBox->Init(Engine->m_UiMgr->uiBase);
	Engine->m_UiMgr->uiMsgBox->LoadFromFile(Engine->m_BasePath + name.c_str(), N3FORMAT_VER_1068);
	Engine->m_UiMgr->AddChild(Engine->m_UiMgr->uiMsgBox);
	remove(name.c_str());
#endif
}
