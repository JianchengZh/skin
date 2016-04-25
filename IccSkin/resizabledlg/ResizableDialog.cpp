#include "stdafx.h"
#include "ResizableDialog.h"
#include "SkinUtil.h"
#include ".\resizabledialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if USE_SCINTILLAWND
extern bool Scintilla_Register(void *hInstance);
extern bool Scintilla_Release();
#endif


/////////////////////////////////////////////////////////////////////////////
// CResizableDialog

inline void CResizableDialog::Construct()
{
	m_bInitDone = FALSE;
	m_bResizing = FALSE;
	m_bIsMaxmize = false;
	m_bUseResizeTracker = false;
	m_bUseMoveTracker = false;
	m_bTracking = false;
	m_pGDXContext = NULL;
	m_bEnableSaveRestore = FALSE;
	m_bDraggable = TRUE;

}

CResizableDialog::CResizableDialog()
{
	Construct();

}


CResizableDialog::CResizableDialog(UINT nIDTemplate, CWnd* pParentWnd)
	: CDialog(nIDTemplate, pParentWnd)
{
	Construct();
}

CResizableDialog::CResizableDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
	: CDialog(lpszTemplateName, pParentWnd)
{
	Construct();
}

CResizableDialog::~CResizableDialog()
{
	FreeSkin();
}


BEGIN_MESSAGE_MAP(CResizableDialog, CDialog)
	//{{AFX_MSG_MAP(CResizableDialog)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CResizableDialog message handlers

BOOL CResizableDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if(m_pGDXContext == NULL){
		m_pGDXContext = new CGDXContext(this);
	}
	// gets the template size as the min track size
	CRect rc;
	GetWindowRect(&rc);
	//SetMinTrackSize(rc.Size());
#if USE_SCINTILLAWND
	Scintilla_Register(AfxGetInstanceHandle());
#endif

	m_bInitDone = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
}

//void CResizableDialog::SetShadow(HINSTANCE hInstance)
//{
//	m_shadow.Initialize(NULL);
//	m_shadow.Create(hInstance /*AfxGetInstanceHandle()*/, GetSafeHwnd());
//}

void CResizableDialog::OnDestroy() 
{
	if (m_bEnableSaveRestore)
		SaveWindowRect(m_sSection, m_bRectOnly);

	// remove child windows
	RemoveAllAnchors();

	if(m_pGDXContext != NULL){
		delete m_pGDXContext;
		m_pGDXContext = NULL;
	}

	FreeSkin();

	CDialog::OnDestroy();

#if USE_SCINTILLAWND
	Scintilla_Release();
#endif
}


void CResizableDialog::SetTracker_Resize(bool bUseTracker)
{
	m_bUseResizeTracker = bUseTracker;
}

void CResizableDialog::SetTracker_Move(bool bUseTracker)
{
	m_bUseMoveTracker = bUseTracker;
}


void CResizableDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if(GetResizableWnd()->GetSafeHwnd() && m_bInit){
		if(m_bSetMask)
			SetMask();
	}

	if (nType == SIZE_MAXHIDE || nType == SIZE_MAXSHOW)
		return;		// arrangement not needed

	if (m_bInitDone){
		if(m_nDLGStyle == DLGSTYLE_RESIZE){
			UpdateGripPos();
		}
		ArrangeLayout();
	}
	Invalidate(TRUE);
}

void CResizableDialog::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	if (!m_bInitDone)
		return;

	MinMaxInfo(lpMMI);
}

void CResizableDialog::EnableSaveRestore(LPCTSTR pszSection, BOOL bRectOnly)
{
	m_sSection = pszSection;

	m_bEnableSaveRestore = TRUE;
	m_bRectOnly = bRectOnly;

	// restore immediately
	LoadWindowRect(pszSection, bRectOnly);
}


BOOL CResizableDialog::OnEraseBkgnd(CDC* pDC) 
{

	if(m_nBGStyle == STYLE_IMAGE){
		ClipChildren(pDC);	
		SetBG(pDC);
	}else if(m_nBGStyle == STYLE_RGB){
		ClipChildren(pDC);	
		SetRGB(pDC);
	}else if(m_nBGStyle == STYLE_FIXED){
		ClipChildren(pDC);	
		SetFixed(pDC);
	}else{
		CDialog::OnEraseBkgnd(pDC);
	}

	return true;
}


BOOL CResizableDialog::OnChooseCursor(CPoint point)
{
	if(m_bResizing) return FALSE;

	CRect rc, rcTemp;
	GetClientRect(&rc);
	rcTemp = rc;

	if(m_bIsMaxmize){
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		m_usCursor = RESIZENO;
		return FALSE;
	}

	if(m_nDLGStyle == DLGSTYLE_SKIN && m_bIsSetResize){

		rcTemp.bottom=rc.bottom;
		rcTemp.top =rc.bottom - 10;
		rcTemp.left=rc.right - 10;
		rcTemp.right=rc.right;
		if(rcTemp.PtInRect(point)){
			SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
			m_usCursor = RESIZENWSERB;
			return TRUE;
		}

		rcTemp.bottom =rc.top + 3;
		rcTemp.left=rc.left + 3;
		rcTemp.right=rc.right - 3;
		if(rcTemp.PtInRect(point)){
			SetCursor(LoadCursor(NULL, IDC_SIZENS));
			m_usCursor = RESIZENST;
			return TRUE;
		}

		rcTemp = rc;
		rcTemp.right =rc.left+3;
		rcTemp.bottom=rc.bottom-3;
		rcTemp.top =rc.top+3;
		if(rcTemp.PtInRect(point)){
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
			m_usCursor = RESIZEWEL;
			return TRUE;
		}

		rcTemp = rc;
		rcTemp.left =rc.right-3;
		rcTemp.bottom=rc.bottom-3;
		rcTemp.top =rc.top+3;
		if(rcTemp.PtInRect(point)){
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
			m_usCursor = RESIZEWER;
			return TRUE;
		}

		rcTemp = rc;
		rcTemp.top =rc.bottom-3;
		rcTemp.left=rc.left + 3;
		rcTemp.right=rc.right - 3;
		if(rcTemp.PtInRect(point)){
			SetCursor(LoadCursor(NULL, IDC_SIZENS));
			m_usCursor = RESIZENSB;
			return TRUE;
		}

		rcTemp.bottom=rc.top+3;
		rcTemp.top =rc.top;
		rcTemp.left=rc.left;
		rcTemp.right=rc.left + 3;
		if(rcTemp.PtInRect(point)){
			SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
			m_usCursor = RESIZENWSELT;
			return TRUE;
		}

		rcTemp.bottom=rc.top+3;
		rcTemp.top =rc.top;
		rcTemp.left=rc.right - 3;
		rcTemp.right=rc.right;
		if(rcTemp.PtInRect(point)){
			SetCursor(LoadCursor(NULL, IDC_SIZENESW));
			m_usCursor = RESIZENESWRT; 
			return TRUE;
		}

		rcTemp.bottom=rc.bottom;
		rcTemp.top =rc.bottom - 3;
		rcTemp.left=rc.left;
		rcTemp.right=rc.left + 3;
		if(rcTemp.PtInRect(point)){
			SetCursor(LoadCursor(NULL, IDC_SIZENESW));
			m_usCursor = RESIZENESWLB;
			return TRUE;
		}
	}

	SetCursor(LoadCursor(NULL, IDC_ARROW));
	m_usCursor = RESIZENO;
	return FALSE;
}

void CResizableDialog::OnMouseMove(UINT nFlags, CPoint point)
{
	if((m_nDLGStyle == DLGSTYLE_SKIN) && m_bIsSetResize)
	{
		OnChooseCursor(point);

		if(m_bResizing && nFlags&MK_LBUTTON){
			CRect rcWnd;
			CPoint pt;
			GetWindowRect(&rcWnd);
#if _DEBUG
			TRACE(_T("\nm_ptResizeFrom.x => %d         \n"), m_ptResizeFrom.x);
			TRACE(_T("\nm_ptResizeFrom.y => %d         \n"), m_ptResizeFrom.y);
			TRACE(_T("\npoint.x => %d         \n"), point.x);
			TRACE(_T("\npoint.y => %d         \n"), point.y);
#endif

			if(!(((point.x - m_ptResizeFrom.x) == 0) && ((point.y - m_ptResizeFrom.y) == 0))){
				if(m_usCursor == RESIZENST){
					pt.y = point.y - m_ptResizeFrom.y;
					rcWnd.top += pt.y;
					MoveWindow(&rcWnd, true);
					UpdateWindow();
				}else if(m_usCursor == RESIZENSB){
					pt.y = point.y - m_ptResizeFrom.y;
					rcWnd.bottom += pt.y;
					m_ptResizeFrom.y += pt.y;
					MoveWindow(&rcWnd, true);
					UpdateWindow();
				}else if(m_usCursor == RESIZEWEL){
					pt.x = point.x - m_ptResizeFrom.x;
					rcWnd.left += pt.x;
					MoveWindow(&rcWnd, true);
					UpdateWindow();
				}else if(m_usCursor == RESIZEWER){
					pt.x = point.x - m_ptResizeFrom.x;
					rcWnd.right += pt.x;
					m_ptResizeFrom.x += pt.x;
					MoveWindow(&rcWnd, true);
					UpdateWindow();
				}else if(m_usCursor == RESIZENWSELT){
					pt.x = point.x - m_ptResizeFrom.x;
					pt.y = point.y - m_ptResizeFrom.y;
					rcWnd.left += pt.x;
					rcWnd.top += pt.y;
					MoveWindow(&rcWnd, true);
					UpdateWindow();
				}else if(m_usCursor == RESIZENESWRT){
					pt.x = point.x - m_ptResizeFrom.x;
					pt.y = point.y - m_ptResizeFrom.y;
					rcWnd.right += pt.x;
					rcWnd.top += pt.y;
					m_ptResizeFrom.x += pt.x;
					MoveWindow(&rcWnd, true);
					UpdateWindow();
				}else if(m_usCursor == RESIZENESWLB){
					pt.x = point.x - m_ptResizeFrom.x;
					pt.y = point.y - m_ptResizeFrom.y;
					rcWnd.left += pt.x;
					rcWnd.bottom += pt.y;
					m_ptResizeFrom.y += pt.y;
					MoveWindow(&rcWnd, true);
					UpdateWindow();
				}else if(m_usCursor == RESIZENWSERB){
					pt.x = point.x - m_ptResizeFrom.x;
					pt.y = point.y - m_ptResizeFrom.y;
					rcWnd.right += pt.x;
					rcWnd.bottom += pt.y;
					m_ptResizeFrom.x += pt.x;
					m_ptResizeFrom.y += pt.y;
					MoveWindow(&rcWnd, true);
					UpdateWindow();
				}
			}
		}
	}

	__super::OnMouseMove(nFlags, point);
}

void CResizableDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(!m_bDraggable)
		return __super::OnLButtonDown(nFlags, point);

	if(m_nDLGStyle == DLGSTYLE_SKIN)
	{
		if(m_bUseResizeTracker && OnChooseCursor(point) && m_bIsSetResize){
			if(m_pGDXContext == NULL){
				m_pGDXContext = new CGDXContext(this);
			}

			ClientToScreen(&point);
			switch(m_usCursor){
			case RESIZENWSELT:
				{
					m_bTracking  = true;
					m_pGDXContext->StartResize(HTTOPLEFT, point);
					break;
				}
			case RESIZENWSERB:
				{
					m_bTracking  = true;
					m_pGDXContext->StartResize(HTBOTTOMRIGHT, point);
					break;
				}
			case RESIZENESWRT:
				{
					m_bTracking  = true;
					m_pGDXContext->StartResize(HTTOPRIGHT, point);
					break;
				}
			case RESIZENESWLB:
				{
					m_bTracking  = true;
					m_pGDXContext->StartResize(HTBOTTOMLEFT, point);
					break;
				}
			case RESIZEWEL:
				{
					m_bTracking  = true;
					m_pGDXContext->StartResize(HTLEFT, point);
					break;
				}
			case RESIZEWER:
				{
					m_bTracking  = true;
					m_pGDXContext->StartResize(HTRIGHT, point);
					break;
				}
			case RESIZENST:
				{
					m_bTracking  = true;
					m_pGDXContext->StartResize(HTTOP, point);
					break;
				}
			case RESIZENSB:
				{
					m_bTracking  = true;
					m_pGDXContext->StartResize(HTBOTTOM, point);
					break;
				}

			default:
				{
					m_bTracking  = false;
					break;
				}
			}

		}else{
			if(OnChooseCursor(point) && m_bIsSetResize){
				SetCapture();
				m_bResizing = TRUE;
				m_ptResizeFrom = point;
			}else{
				if(!m_bIsMaxmize){
					if(m_bUseMoveTracker){
						if(m_pGDXContext == NULL){
							m_pGDXContext = new CGDXContext(this);
						}
						ClientToScreen(&point);
						m_bTracking  = true;
						m_pGDXContext->StartDrag(point);
					}else{
						SendMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y)); 
					}
				}
			}
		}
	}

	__super::OnLButtonDown(nFlags, point);
}

void CResizableDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_nSize.cx = 0;
	m_nSize.cy = 0;

	if(m_nDLGStyle == DLGSTYLE_SKIN){
		if(m_bResizing){
			ReleaseCapture();
			m_bResizing = FALSE;
		}

		if(m_bTracking){
			m_bTracking = false;
			CRect newRect;
			newRect = m_pGDXContext->GetDragRect();

			RECT orgrc;
			GetWindowRect(&orgrc);

			m_nSize.cx = newRect.Width() - (orgrc.right - orgrc.left);
			m_nSize.cy = newRect.Height() - (orgrc.bottom - orgrc.top);

			MoveWindow(&newRect, true);
			UpdateWindow();
		}
	}

	__super::OnLButtonUp(nFlags, point);
}

bool CResizableDialog::ReLoadSkins()
{
	CDC *pDC = GetDC();
	bool bResult = false;
	bResult = _ReLoadSkins(pDC);
	ReleaseDC(pDC);

	return bResult;
}

void CResizableDialog::SetContextSize(int cx, int cy)
{
	m_pGDXContext->SetDefSize(cx, cy);
}


bool CResizableDialog::SetSkin(CString strSkinFile,CBitmap *bmBaseImage)
{
	if(InitSkin(strSkinFile)){
		CDC *pDC = GetDC();
		if(!LoadSkins(pDC, this, bmBaseImage))
			return false;
		ReleaseDC(pDC);
	}else{
		return false;
	}

	if(m_bDefSize && m_pGDXContext!= NULL){
		m_pGDXContext->SetDefSize(m_rDefRect.Width(), m_rDefRect.Height());
	}

	SetStyle();

	return true;
}
bool CResizableDialog::SetSkin(CString strSkinFile)
{
	if(InitSkin(strSkinFile)){
		CDC *pDC = GetDC();
		if(!LoadSkins(pDC, this))
			return false;
		ReleaseDC(pDC);
	}else{
		return false;
	}
	bool ret = SetStyle();
	return ret;
}

bool CResizableDialog::SetStyle()
{
	if(m_bDefSize && m_pGDXContext!= NULL){
		m_pGDXContext->SetDefSize(m_rDefRect.Width(), m_rDefRect.Height());
	}

	if(m_nDLGStyle == DLGSTYLE_RESIZE){
		if (!InitGrip()) return false;

		//CRect rect;
		//GetClientRect(&rect);

		// set resizable style
		//ModifyStyle(DS_MODALFRAME, WS_POPUP | WS_THICKFRAME |WS_CAPTION| WS_VISIBLE);
		// adjust size to reflect new style
		//::AdjustWindowRectEx(&rect, GetStyle(),
		//	::IsMenu(GetMenu()->GetSafeHmenu()), GetExStyle());
		/*
		if(m_bDefSize){
		int cx = GetSystemMetrics(SM_CXFULLSCREEN);
		int cy = GetSystemMetrics(SM_CYFULLSCREEN);
		int nXDef= (cx-m_rDefRect.Width())/2;
		int nYDef = (cy-m_rDefRect.Height())/2;

		SetWindowPos(NULL, nXDef, nYDef, m_rDefRect.Width(), m_rDefRect.Height(), SWP_FRAMECHANGED|
		SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOREPOSITION);
		}
		*/
		UpdateGripPos();
	}else if(m_nDLGStyle == DLGSTYLE_SKIN){
		//ModifyStyle(0, WS_SYSMENU|WS_MINIMIZEBOX,0);

	}
	if(m_strCaption.IsEmpty())
		m_strCaption = "SkinDlg";
	SetWindowText(m_strCaption);

	if(m_bDefSize)
	{
		int cx = GetSystemMetrics(SM_CXFULLSCREEN);
		int cy = GetSystemMetrics(SM_CYFULLSCREEN);
		int nXDef= (cx-m_rDefRect.Width())/2;
		int nYDef = (cy-m_rDefRect.Height())/2;

		//ShowWindow Flags
		//SW_HIDE 윈도우를 숨기고 다른 윈도우를 활성 상태로 만든다.
		//SW_MAXIMIZE 윈도우를 최대화 한다.
		//SW_MINIMIZE 윈도우를 최소화하고 다른 윈도우를 활성 상태로 만든다.
		//SW_RESTORE 최대/최소화를 원래 상태로 복원한다.
		//SW_SHOW 윈도우를 나타내고 활성 상태로 만든다.
		//SW_SHOWNA 윈도우를 나타내고 활성 상태로 하지 않는다.
		//SW_SHOWDEFAULT 윈도우를 처음 프로그램 시작할 때 지정된 값으로 변경한다.
		//SW_SHOWNORMAL 윈도우를 나타내고 활성상태로 만든다. 최대화/최소화된 경우에는 원래대로 복원한다.
		//SW_SHOWACTIVE 윈도우를 활성 상태로 하지 않는다는 것을 제외하면 SW_SHOWNORMAL과 같다.

		//SetWindowPos Flags
		//SWP_HIDEWINDOW 윈도우를 숨긴다.
		//SWP_NOACTIVATE 윈도우를 활성화하지 않는다.
		//SWP_NOMOVE 윈도우를 이동하지 않는다.
		//SWP_MOOWNERZORDER 소유 윈도우의 Z순서를 변경하지 않는다.
		//SWP_NOREDRAW 윈도우를 다시 그리지 않는다.
		//SWP_NOSIZE 윈도우의 크기를 변경하지 않는다.
		//SWP_NOZORDER 윈도우의 Z 순서를 변경하지 않는다.
		//SWP_SHOWINDOW 윈도우를 나타낸다.
		//- SWP_NOMOVE     : SetWindowPos함수에서 두번째와 세번째 인자가 무시.
		//- SWP_NOZORDER  : SetWindowPos함수에서 첫번째 인자가 무시.
		//- SWP_NOSIZE       : SetWindowPos함수에서 네번째와 다섯번째 인자가 무시.

		SetWindowPos(NULL, nXDef, nYDef, m_rDefRect.Width(), m_rDefRect.Height(),SWP_HIDEWINDOW);//여기서 SHOW하지말고, 상속받은 다이얼로그에서 처리
			//SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREPOSITION);
	}
	
	return true;
}


void CResizableDialog::GetImageFromSkin(CBitmap &bmImage, CRect rect)
{
	CBitmap *bmpSkin;
	bmpSkin = GetSkinImage();
	CDC* pDC = GetDC();
	CSkinUtil skinUtil;
	if(bmImage.m_hObject){
		bmImage.DeleteObject();
	}

	skinUtil.CopyBitmap(pDC, bmImage, *bmpSkin, rect);
	ReleaseDC(pDC);	
}

void CResizableDialog::OnClose()
{
	FreeSkin();
	__super::OnClose();
}

BOOL CResizableDialog::PreTranslateMessage(MSG* pMsg)
{
	if(m_bBlockExit){
		if(pMsg->message == WM_KEYDOWN){
			if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			{
				return FALSE;
			}
		}
	}
	return __super::PreTranslateMessage(pMsg);
}

void CResizableDialog::UpdateShadow()
{
	if(m_bUseShadow) {
		m_shadow.Initialize(NULL);
		if(!m_shadow.Create(AfxGetInstanceHandle(), GetSafeHwnd()))
			return;
		m_shadow.SetSize(m_shadowSize);
		m_shadow.SetSharpness(m_shadowSharp);
		m_shadow.SetDarkness(m_shadowDark);
		m_shadow.SetPosition(m_shadowPosX, m_shadowPosY);
		m_shadow.SetColor(m_shadowColor);
		
	}
}