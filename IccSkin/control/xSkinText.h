#ifndef XSKINTEXT_H
#define XSKINTEXT_H

#if _MSC_VER > 1000
#pragma once
#endif
// CxSkinText


#define TOOLTIP_ID		1

#ifdef _AFXEXT
class AFX_EXT_CLASS CxSkinText : public CStatic
#else
class CxSkinText : public CStatic
#endif
{
	DECLARE_DYNAMIC(CxSkinText)

public:
	CxSkinText();
	virtual ~CxSkinText();

protected:
	DECLARE_MESSAGE_MAP()

	int		m_nId;
	CString		m_strName;
	CString		m_strText;
	CFont		m_Font;

	COLORREF	m_clrText;
	CString		m_strFontName;
	int			m_nFontSize;
	BOOL		m_bFontBold;
	BOOL		m_bUnderline;

	CBitmap		m_bmBG;

	CToolTipCtrl		m_ToolTip;
	COLORREF		m_clrTTFont;
	COLORREF		m_clrTTBckGnd;
	BOOL			m_bToolTipCreated;
	CString			m_strTT;
	BOOL			m_bShowToolTip;

public:
	void Init(void);
	void SetCaption(CString val){SetWindowText(val);};
	void SetId(int id){m_nId = id;};
	UINT GetId(void){return m_nId;};
	CString GetName(void){return m_strName;};
	void SetText(CString strText);
	CString GetText(void);

	void SetTextColor(COLORREF clrText){m_clrText = clrText;};
	void SetFontName(CString strFontName){m_strFontName = strFontName;};
	CString GetFontName(void){return m_strFontName;};
	void SetFontSize(int nFontSize){m_nFontSize = nFontSize;};
	int GetFontSize(void){return m_nFontSize;};
	void SetFontBold(BOOL bBold){m_bFontBold = bBold;};
	BOOL GetFontBold(void){return m_bFontBold;};
	void SetFontUnderline(BOOL bUnderline){m_bUnderline = bUnderline;};
	void SetFontType(CString strFontName, int nFontSize, BOOL bBold, BOOL bUnderline, COLORREF clrText);

	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnSetText(WPARAM,LPARAM);

	BOOL SetToolTipFGColor(COLORREF crTTFgColor);
	BOOL SetToolTipBGColor(COLORREF crTTBgColor);
	BOOL SetToolTipText(CString strText);
	void ShowToolTip(BOOL bShow) {m_bShowToolTip = bShow;};

	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#endif