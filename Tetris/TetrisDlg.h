// TetrisDlg.h : header file
//

#pragma once
#include "TetrisGame.h"

// CTetrisDlg dialog
class CTetrisDlg : public CDialog
{
// Construction
public:
	CTetrisDlg(CWnd* pParent = NULL);	// standard constructor
	~CTetrisDlg(void);
// Dialog Data
	enum { IDD = IDD_TETRIS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMenuStart();
	afx_msg void OnMenuPause();
	afx_msg void OnGameRestore();

	UINT Refresh(void);
	static UINT ThreadRefresh(LPVOID pParam);

public:
	CTetrisGame * pTetrisGame;
};
