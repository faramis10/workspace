#ifndef _LocalInput_H_
#define _LocalInput_H_
class CLocalInput;
#include "stdafx.h"
#include <DInput.h>
#include "FMGAME Engine.h"

const int DK_NONE = 0;
const int DK_RELEASE = 1;
const int DK_PRESS = 2;
const int DK_REPEAT = 4;
const int NUMDIKEYS = 256;

// ??? ??? - ?? ??? ???? OR ???? ???? ??..
const int MOUSE_LBCLICK = 0x1;
const int MOUSE_LBCLICKED = 0x2;
const int MOUSE_LBDOWN = 0x4;
const int MOUSE_MBCLICK = 0x8;
const int MOUSE_MBCLICKED = 0x10;
const int MOUSE_MBDOWN = 0x20;
const int MOUSE_RBCLICK = 0x40;
const int MOUSE_RBCLICKED = 0x80;
const int MOUSE_RBDOWN = 0x100;
const int MOUSE_LBDBLCLK = 0x200;
const int MOUSE_MBDBLCLK = 0x400;
const int MOUSE_RBDBLCLK = 0x800;

//////////////////////////////////////////////////////////////////////////////////
// CLocalInput is a class wrapper for DirectInput and contains functions to receive 
// data from the mouse, keyboard
//////////////////////////////////////////////////////////////////////////////////
class CLocalInput
{
private:
	void AcquireMouse();
	void UnacquireMouse();
	void AcquireKeyboard();
	void UnacquireKeyboard();

protected:
	LPDIRECTINPUT8			m_lpDI;
	LPDIRECTINPUTDEVICE8	m_lpDIDKeyboard;
	//	LPDIRECTINPUTDEVICE8	m_lpDIDMouse;

	HWND m_hWnd;

	//	BOOL m_bMouse;
	//	BOOL m_bKeyboard;

	int m_nMouseFlag, m_nMouseFlagOld; // ??? ?? ?? ???
	DWORD m_dwTickLBDown; // ??? ?? ?? ?? ?? ???
	DWORD m_dwTickRBDown; // ??? ??? ?? ?? ?? ???

	POINT	m_ptCurMouse; // ?? ??? ???
	POINT	m_ptOldMouse; // ?? ??? ???

	RECT	m_rcLBDrag; // ??? ??
	RECT	m_rcMBDrag; // ??? ??
	RECT	m_rcRBDrag; // ??? ??

	RECT m_rcMLimit; // ??? ??? ?? ??
	BYTE m_byCurKeys[NUMDIKEYS]; // ?? ? ??
	BYTE m_byOldKeys[NUMDIKEYS]; // ?? ? ??
	BOOL m_bKeyPresses[NUMDIKEYS]; // ?? ?? ????
	BOOL m_bKeyPresseds[NUMDIKEYS]; // ?? ??? ?? ????
	BOOL m_bNoKeyDown; // ?? ???? ???

	DWORD m_dwTickKeyPress[NUMDIKEYS];

public:
	void KeyboardClearInput(int iIndex = -1) // ??? ??? ??? ???.. ???? ?? ?????..
	{
		if (-1 == iIndex)
		{
			memset(m_byOldKeys, 0, sizeof(m_byOldKeys));
			memset(m_byCurKeys, 0, sizeof(m_byCurKeys));
			memset(m_bKeyPresses, 0, sizeof(m_bKeyPresses));
			memset(m_bKeyPresseds, 0, sizeof(m_bKeyPresseds));
		}
		else if (iIndex >= 0 && iIndex < NUMDIKEYS) // ??? ?? ???..
		{
			m_byCurKeys[iIndex] = m_byOldKeys[iIndex] = m_bKeyPresses[iIndex] = m_bKeyPresseds[iIndex] = 0;
		}
	}
	BOOL IsNoKeyDown() { return m_bNoKeyDown; }
	BOOL IsKeyDown(int iIndex) { if (iIndex < 0 || iIndex >= NUMDIKEYS) return FALSE; return m_byCurKeys[iIndex]; } // ???? ?????... "DInput.h" ? ?? ?? ?? DIK_???? ????? ??..
	BOOL IsKeyPress(int iIndex) { if (iIndex < 0 || iIndex >= NUMDIKEYS) return FALSE; return m_bKeyPresses[iIndex]; } // ???? ??? ??... "DInput.h" ? ?? ?? ?? DIK_???? ????? ??..
	BOOL IsKeyPressed(int iIndex) { if (iIndex < 0 || iIndex >= NUMDIKEYS) return FALSE; return m_bKeyPresseds[iIndex]; } // ???? ????? ?? ??... "DInput.h" ? ?? ?? ?? DIK_???? ????? ??..

	BOOL Init(HINSTANCE hInst, HWND hWnd, BOOL bActivateKeyboard = TRUE, BOOL bActivateMouse = TRUE, BOOL ExclusiveMouseAccess = TRUE);

	void Tick(void);
	void KeyboardFlushData();
	void RunMouseControlPanel(HWND hWnd);
	void MouseSetLimits(int x1, int y1, int x2, int y2);
	void SetActiveDevices(BOOL bKeyboard, BOOL bMouse);
	void MouseSetPos(int x, int y);
	BOOL CreateMouseWheelWindow(HWND hParent, RECT rect);
	static LRESULT APIENTRY MouseWheelWndProc(HWND hWnd, uint16_t Message, WPARAM wParam, LPARAM lParam);
	int m_iMouseWheelDelta;
	int GetMouseWheelDelta() { return m_iMouseWheelDelta; }
	void SetMouseWheelDelta(int delta) { m_iMouseWheelDelta = delta; };

	BOOL KeyboardGetKeyState(int nDIKey); // ?? ??? ? ??..

	const POINT MouseGetPos() { return m_ptCurMouse; }
	const POINT MouseGetPosOld() { return m_ptOldMouse; }

	RECT MouseGetLBDragRect() { return m_rcLBDrag; }
	RECT MouseGetMBDragRect() { return m_rcMBDrag; }
	RECT MouseGetRBDragRect() { return m_rcRBDrag; }

	int MouseGetFlag() { return m_nMouseFlag; } // Mouse Flag ? or ???? ???? ??.
	int MouseGetFlagOld() { return m_nMouseFlagOld; }
	void MouseRemoveFlag(int nFlag = -1) { if (-1 == nFlag) m_nMouseFlag = m_nMouseFlagOld = 0; else m_nMouseFlag &= (~nFlag); } // ??? Mouse Flag ??

	CLocalInput(void);
	~CLocalInput(void);
};

#endif // end of _LocalInput_H_
