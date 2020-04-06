// TetrisDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Tetris.h"
#include "TetrisDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTetrisDlg dialog

CTetrisDlg::CTetrisDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTetrisDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	pTetrisGame = NULL;
}

CTetrisDlg::~CTetrisDlg(void)
{
	if(NULL != this->pTetrisGame)
	{
		delete pTetrisGame;
		pTetrisGame = NULL;
	}
}

void CTetrisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTetrisDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_MENU_START, &CTetrisDlg::OnMenuStart)
	ON_COMMAND(ID_MENU_PAUSE, &CTetrisDlg::OnMenuPause)
	ON_COMMAND(ID_GAME_RESTORE, &CTetrisDlg::OnGameRestore)
END_MESSAGE_MAP()


// CTetrisDlg message handlers

BOOL CTetrisDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	this->pTetrisGame = new CTetrisGame(this, 16, 1);
	AfxBeginThread(this->ThreadRefresh, this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTetrisDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}

	if(!pTetrisGame->GameSetup()) AfxMessageBox("Failed to setup game!");
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTetrisDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CTetrisDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(WM_KEYUP == pMsg->message)
	{
		switch(pMsg->wParam)
		{
		case VK_LEFT:
			this->pTetrisGame->BlockMoveleft();
			break;
		case VK_RIGHT:
			this->pTetrisGame->BlockMoveright();
			break;
		case VK_UP:
			this->pTetrisGame->BlockRotate();
			break;
		case VK_DOWN:
			this->pTetrisGame->BlockMovedown();
			break;
		default:
			break;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CTetrisDlg::OnMenuStart()
{
	// TODO: Add your command handler code here
	pTetrisGame->GameStart();
}

void CTetrisDlg::OnMenuPause()
{
	// TODO: Add your command handler code here
	this->pTetrisGame->GamePause();
}

void CTetrisDlg::OnGameRestore()
{
	// TODO: Add your command handler code here
	this->pTetrisGame->GameRestore();
}

UINT CTetrisDlg::Refresh(void)
{
	CString strMsg;
	while(true)
	{
		strMsg.Format("Level %d", this->pTetrisGame->m_nGameLevel);
		this->SetDlgItemTextA(IDC_STATIC_LEVEL, strMsg);
		strMsg.Format("Score %d",  this->pTetrisGame->m_nGameScore);
		this->SetDlgItemTextA(IDC_STATIC_SCORE, strMsg);
		Sleep(1000);
	}

	return 0;
}

UINT CTetrisDlg::ThreadRefresh(LPVOID pParam)
{
	CTetrisDlg * pThis = (CTetrisDlg *)pParam;
	return pThis->Refresh();
}
