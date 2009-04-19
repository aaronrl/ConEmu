#include "Header.h"

CConEmuChild::CConEmuChild()
{
	mn_MsgTabChanged = RegisterWindowMessage(CONEMUTABCHANGED);
}

CConEmuChild::~CConEmuChild()
{
}

HWND CConEmuChild::Create()
{
	//WNDCLASS wc = {CS_OWNDC|CS_DBLCLKS/*|CS_SAVEBITS*/, CConEmuChild::ChildWndProc, 0, 0, 
	//		g_hInstance, NULL, LoadCursor(NULL, IDC_ARROW), 
	//		NULL /*(HBRUSH)COLOR_BACKGROUND*/, 
	//		NULL, szClassName};// | CS_DROPSHADOW
	//if (!RegisterClass(&wc)) {
	//	ghWndDC = (HWND)-1; // ����� �������� �� �������
	//	MBoxA(_T("Can't register DC window class!"));
	//	return NULL;
	//}
	DWORD style = WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS /*| WS_CLIPCHILDREN | (gSet.Buffer Height ? WS_VSCROLL : 0)*/;
	//RECT rc = gConEmu.DCClientRect();
	RECT rcMain; GetClientRect(ghWnd, &rcMain);
	RECT rc = gConEmu.CalcRect(CER_DC, rcMain, CER_MAINCLIENT);
	ghWndDC = CreateWindow(szClassName, 0, style, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, ghWnd, NULL, (HINSTANCE)g_hInstance, NULL);
	if (!ghWndDC) {
		ghWndDC = (HWND)-1; // ����� �������� �� �������
		MBoxA(_T("Can't create DC window!"));
		return NULL; //
	}
	//SetClassLong(ghWndDC, GCL_HBRBACKGROUND, (LONG)gConEmu.m_Back.mh_BackBrush);
	SetWindowPos(ghWndDC, HWND_TOP, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
	//gConEmu.dcWindowLast = rc; //TODO!!!
	return ghWndDC;
}

LRESULT CALLBACK CConEmuChild::ChildWndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

	if (messg == WM_SYSCHAR)
		return TRUE;

    switch (messg)
    {
    case WM_COPYDATA:
		// ���� �� ������ ���� - ��������� ���� ����
		result = gConEmu.WndProc ( ghWnd, messg, wParam, lParam );
		break;

    case WM_ERASEBKGND:
		result = 0;
		break;
		
    case WM_PAINT:
		result = gConEmu.m_Child.OnPaint(wParam, lParam);
		break;

    case WM_SIZE:
		result = gConEmu.m_Child.OnSize(wParam, lParam);
        break;

    case WM_CREATE:
        break;

    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_MOUSEWHEEL:
    case WM_ACTIVATE:
    case WM_ACTIVATEAPP:
    case WM_KILLFOCUS:
    case WM_SETFOCUS:
    case WM_MOUSEMOVE:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_VSCROLL:
        // ��� ��������� � ��������
        result = gConEmu.WndProc(hWnd, messg, wParam, lParam);
        return result;

	case WM_IME_NOTIFY:
		break;
    case WM_INPUTLANGCHANGE:
    case WM_INPUTLANGCHANGEREQUEST:
	{
		if (IsDebuggerPresent()) {
			WCHAR szMsg[128];
			wsprintf(szMsg, L"InChild %s(CP:%i, HKL:0x%08X)\n",
				(messg == WM_INPUTLANGCHANGE) ? L"WM_INPUTLANGCHANGE" : L"WM_INPUTLANGCHANGEREQUEST",
				wParam, lParam);
			OutputDebugString(szMsg);

		}
		result = DefWindowProc(hWnd, messg, wParam, lParam);
	} break;

#ifdef _DEBUG
		case WM_WINDOWPOSCHANGING:
			{
			WINDOWPOS* pwp = (WINDOWPOS*)lParam;
			result = DefWindowProc(hWnd, messg, wParam, lParam);
			}
			return result;
		case WM_WINDOWPOSCHANGED:
			{
			WINDOWPOS* pwp = (WINDOWPOS*)lParam;
			result = DefWindowProc(hWnd, messg, wParam, lParam);
			}
			return result;
#endif

    default:
		if (messg == mn_MsgTabChanged) {
			if (gSet.isTabs) {
				//���������� ����, �� ����� ����������
				#ifdef MSGLOGGER
					WCHAR szDbg[128]; swprintf(szDbg, L"Tabs:Notified(%i)\n", wParam);
					OutputDebugStringW(szDbg);
				#endif
				#pragma message("TODO: ����� ������ �� ������ OnTimer ������� �������� mn_TopProcessID")
				// ����� �� ����� ������� PID ���� ��� ����� ���� �� ��������...
				//gConEmu.OnTimer(0,0); �� ����������. ������ ������� �� �������, ��-�� ����� ������ ��������� ���� ��� � ������� 0
				TabBar.Retrieve();
			}
		}
        if (messg) result = DefWindowProc(hWnd, messg, wParam, lParam);
    }
    return result;
}

LRESULT CConEmuChild::OnPaint(WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	BOOL lbSkipDraw = FALSE;
    //if (gbInPaint)
	//    break;

	gSet.Performance(tPerfBlt, FALSE);

    if (gConEmu.isPictureView())
    {
		// ���� PictureView ���������� �� �� ��� ���� - ���������� ������� ����� �������!
		RECT rcPic, rcClient;
		GetClientRect(gConEmu.hPictureView, &rcPic);
		GetClientRect(ghWndDC, &rcClient);
		
		if (rcPic.right>=rcClient.right) {
			lbSkipDraw = TRUE;
	        result = DefWindowProc(ghWndDC, WM_PAINT, wParam, lParam);
        }
	}
	if (!lbSkipDraw)
	{
		gConEmu.PaintCon();
	}

	gSet.Performance(tPerfBlt, TRUE);

	// ���� ������� ���� �������� - �������� ��������� ���������� � ��������
	gConEmu.UpdateSizes();

    return result;
}

LRESULT CConEmuChild::OnSize(WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
    BOOL lbIsPicView = FALSE;

	RECT rcNewClient; GetClientRect(ghWndDC,&rcNewClient);
    
    if (gConEmu.isPictureView())
    {
        if (gConEmu.hPictureView) {
            lbIsPicView = TRUE;
            gConEmu.isPiewUpdate = true;
            RECT rcClient; GetClientRect(ghWndDC, &rcClient);
            //TODO: � ���� PictureView ����� � � QuickView ��������������...
            MoveWindow(gConEmu.hPictureView, 0,0,rcClient.right,rcClient.bottom, 1);
            //INVALIDATE(); //InvalidateRect(hWnd, NULL, FALSE);
			Invalidate();
            //SetFocus(hPictureView); -- ��� ����� �� ������ ������� ����� �������� ������...
        }
    }

	return result;
}

void CConEmuChild::Invalidate()
{
	if (ghWndDC)
		InvalidateRect(ghWndDC, NULL, FALSE);
}










CConEmuBack::CConEmuBack()
{
	mh_Wnd = NULL;
	mh_BackBrush = NULL;
	mn_LastColor = -1;
#ifdef _DEBUG
	mn_ColorIdx = 1;
#else
	mn_ColorIdx = 0;
#endif
}

CConEmuBack::~CConEmuBack()
{
}

HWND CConEmuBack::Create()
{
	mn_LastColor = gSet.Colors[mn_ColorIdx];
	mh_BackBrush = CreateSolidBrush(mn_LastColor);

	DWORD dwLastError = 0;
	WNDCLASS wc = {CS_OWNDC/*|CS_SAVEBITS*/, CConEmuBack::BackWndProc, 0, 0, 
			g_hInstance, NULL, LoadCursor(NULL, IDC_ARROW), 
			mh_BackBrush, 
			NULL, szClassNameBack};
	if (!RegisterClass(&wc)) {
		dwLastError = GetLastError();
		mh_Wnd = (HWND)-1; // ����� �������� �� �������
		MBoxA(_T("Can't register background window class!"));
		return NULL;
	}
	DWORD style = WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS ;
	//RECT rc = gConEmu.ConsoleOffsetRect();
	RECT rcClient; GetClientRect(ghWnd, &rcClient);
	RECT rc = gConEmu.CalcRect(CER_BACK, rcClient, CER_MAINCLIENT);

	mh_Wnd = CreateWindow(szClassNameBack, 0, style, 
		rc.left, rc.top,
		rcClient.right - rc.right - rc.left,
		rcClient.bottom - rc.bottom - rc.top,
		ghWnd, NULL, (HINSTANCE)g_hInstance, NULL);
	if (!mh_Wnd) {
		dwLastError = GetLastError();
		mh_Wnd = (HWND)-1; // ����� �������� �� �������
		MBoxA(_T("Can't create background window!"));
		return NULL; //
	}

	return mh_Wnd;
}

LRESULT CALLBACK CConEmuBack::BackWndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

	if (messg == WM_SYSCHAR)
		return TRUE;

	switch (messg) {
		case WM_CREATE:
			gConEmu.m_Back.mh_Wnd = hWnd;
			break;
		case WM_DESTROY:
			DeleteObject(gConEmu.m_Back.mh_BackBrush);
			break;
		//case WM_SETFOCUS:
		//
		//	if (messg == WM_SETFOCUS) {
		//		if (ghWndDC && IsWindow(ghWndDC))
		//			SetFocus(ghWndDC);
		//	}
		//	return 0;
	    case WM_VSCROLL:
	        POSTMESSAGE(ghConWnd, messg, wParam, lParam, FALSE);
	        break;
		case WM_PAINT:
			{
				PAINTSTRUCT ps; memset(&ps, 0, sizeof(ps));
				HDC hDc = BeginPaint(hWnd, &ps);
				if (!IsRectEmpty(&ps.rcPaint))
					FillRect(hDc, &ps.rcPaint, gConEmu.m_Back.mh_BackBrush);
				EndPaint(hWnd, &ps);
			}
			return 0;
#ifdef _DEBUG
		case WM_SIZE:
			{
			UINT nW = LOWORD(lParam), nH = HIWORD(lParam);
			result = DefWindowProc(hWnd, messg, wParam, lParam);
			}
			return result;
		case WM_WINDOWPOSCHANGING:
			{
			WINDOWPOS* pwp = (WINDOWPOS*)lParam;
			result = DefWindowProc(hWnd, messg, wParam, lParam);
			}
			return result;
		case WM_WINDOWPOSCHANGED:
			{
			WINDOWPOS* pwp = (WINDOWPOS*)lParam;
			result = DefWindowProc(hWnd, messg, wParam, lParam);
			}
			return result;
#endif
	}

    result = DefWindowProc(hWnd, messg, wParam, lParam);

	return result;
}

void CConEmuBack::Resize()
{
	if (!mh_Wnd || !IsWindow(mh_Wnd)) 
		return;

	//RECT rc = gConEmu.ConsoleOffsetRect();
	RECT rcClient; GetClientRect(ghWnd, &rcClient);
	RECT rc = gConEmu.CalcRect(CER_BACK, rcClient, CER_MAINCLIENT);

#ifdef _DEBUG
	GetClientRect(mh_Wnd, &rcClient);
#endif
	MoveWindow(mh_Wnd, 
		rc.left, rc.top,
		rc.right - rc.left,
		rc.bottom - rc.top,
		1);
#ifdef _DEBUG
	GetClientRect(mh_Wnd, &rcClient);
#endif
}

void CConEmuBack::Refresh()
{
	if (mn_LastColor == gSet.Colors[mn_ColorIdx])
		return;

	mn_LastColor = gSet.Colors[mn_ColorIdx];
	HBRUSH hNewBrush = CreateSolidBrush(mn_LastColor);

	SetClassLong(mh_Wnd, GCL_HBRBACKGROUND, (LONG)hNewBrush);
	DeleteObject(mh_BackBrush);
	mh_BackBrush = hNewBrush;

	//RECT rc; GetClientRect(mh_Wnd, &rc);
	//InvalidateRect(mh_Wnd, &rc, TRUE);
	Invalidate();
}

void CConEmuBack::Invalidate()
{
	if (this && mh_Wnd)
		InvalidateRect(mh_Wnd, NULL, FALSE);
}
