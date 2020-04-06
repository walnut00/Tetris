#include "stdafx.h"
#include "TetrisGame.h"

CTetrisGame::CTetrisGame(CWnd * pWorkWnd, unsigned short nGridSize, unsigned short nSpliterSize):
					m_pWorkWnd(pWorkWnd),
					m_pBlock(NULL),
					m_bPaused(false),
					m_bStarted(false),
					m_bDowning(false),
					m_pMainThread(NULL),
					m_nGameScore(0),
					m_nGameLevel(1),
					m_nPauseTime(1000),
					m_nFullRow(0),
					m_rectWorkarea(CRect(0,0,0,0)),
					m_ptCurrentLocation(CPoint(0,0))
{
	this->m_pBlock = NULL;
	this->m_pBlock = new CBlock(nGridSize, nSpliterSize);

	m_pMutex = NULL;
	m_pMutex = new CMutex();
}

CTetrisGame::~CTetrisGame(void)
{
	if(this->m_pWorkWnd) m_pWorkWnd = NULL;

	if(NULL != m_pBlock) 
	{
		delete m_pBlock;
		m_pBlock = NULL;
	}

	if(NULL != m_pMutex)
	{
		delete m_pMutex;
		m_pMutex = NULL;
	}
}

bool CTetrisGame::GameStart(void)
{
	if(NULL != this->m_pMainThread) 
	{
		if(!this->GamePause()) return false;
		if(IDNO == AfxMessageBox("Game is running, sure you want to start a new game?", MB_YESNO | MB_ICONQUESTION)) 
		{
			if(!this->GameRestore()) return true;//failed to restore the game, start a new one
			else return false;
		}
		if(!this->GameRestore()) return true;//failed to restore the game, start a new one
		
		if(0 == ::TerminateThread(this->m_pMainThread->m_hThread, 0xFF)) return false;
		Sleep(100);

		DWORD nExitCode;
		if(0 == GetExitCodeThread(this->m_pMainThread->m_hThread, &nExitCode)) return false;
		if(0xFF != nExitCode) return false;
	}

	this->GameInitialize();
	this->m_pMainThread = AfxBeginThread(ThreadMain, (LPVOID)this);
	if(NULL == this->m_pMainThread) 
	{
		AfxMessageBox("Failed to open block downing thread !");
		return false;
	}
	
	m_bStarted = true;
	return true;
}

bool CTetrisGame::GamePause(void)
{
	if(NULL == this->m_pMainThread) return false;
	if(this->m_bPaused) return true;
	if(0xFFFFFFFF == this->m_pMainThread->SuspendThread()) return false;
	this->m_bPaused = true;
	return true;
}

bool CTetrisGame::GameRestore(void)
{
	if(NULL == this->m_pMainThread) return false;
	if(!this->m_bPaused) return true;
	if(0xFFFFFFFF == this->m_pMainThread->ResumeThread()) return false;
	this->m_bPaused = false;
	return true;
}

bool CTetrisGame::GameSetup(void)
{
	bool bRet = true;
	CDC * pdcContext = this->m_pWorkWnd->GetDC();
	CRect rectWnd;
	CDC dcMemory;
	CBitmap bmMemory;
	CBitmap *pbmOld = NULL;

	if(!GetWindowRect(this->m_pWorkWnd->m_hWnd, &rectWnd)) return false;

	dcMemory.CreateCompatibleDC(pdcContext);
	bmMemory.CreateCompatibleBitmap(pdcContext, rectWnd.Width(), this->m_rectWorkarea.top + this->m_rectWorkarea.Height());

	pbmOld = dcMemory.SelectObject(&bmMemory); //select the bitmap to memory DC
	dcMemory.FillSolidRect(0 , 0, rectWnd.Width(), this->m_rectWorkarea.top + this->m_rectWorkarea.Height(), GetSysColor(COLOR_3DFACE));//erase the bitmap with background color 

	bRet = (this->DrawWorkarea(&dcMemory) & this->DrawAllShape(&dcMemory));
	//bRet = (this->DrawWorkarea(pdcContext) & this->DrawAllShape(pdcContext));

	if(!pdcContext->BitBlt(0, 0, rectWnd.Width(), this->m_rectWorkarea.top + this->m_rectWorkarea.Height(), &dcMemory, 0,0,SRCCOPY)) bRet = false; //copy memory bitmap to screen
	if(NULL == dcMemory.SelectObject(pbmOld)) bRet = false;
	if(!bmMemory.DeleteObject()) bRet = false;
	if(!dcMemory.DeleteDC()) bRet = false;

	return bRet;
}

void CTetrisGame::BlockMoveleft(void)
{
	if(this->m_bPaused) return;
	if(!this->CanMoveleft()) return;
	this->m_ptCurrentLocation.x -= (this->m_pBlock->m_nGridSize + this->m_pBlock->m_nSpliterSize);
	//this->m_pWorkWnd->Invalidate();
	InvalidateRect(this->m_pWorkWnd->m_hWnd, this->m_rectWorkarea, FALSE);
	//this->m_pWorkWnd->UpdateWindow();
}

void CTetrisGame::BlockMoveright(void)
{
	if(this->m_bPaused) return;
	if(!this->CanMoveright()) return;
	this->m_ptCurrentLocation.x += (this->m_pBlock->m_nGridSize + this->m_pBlock->m_nSpliterSize);
	//this->m_pWorkWnd->Invalidate();
	InvalidateRect(this->m_pWorkWnd->m_hWnd, this->m_rectWorkarea, FALSE);
	//this->m_pWorkWnd->UpdateWindow();
}

void CTetrisGame::BlockMovedown(void)
{
	if(this->m_bPaused) return;
	if(NULL == this->m_pMainThread) return;
	AfxBeginThread(this->ThreadDown, this);
}

void CTetrisGame::BlockRotate(void)
{	
	if(!this->CanRotate()) return;
	this->m_pBlock->m_nAngle = (BLOCK_ANGLE)((this->m_pBlock->m_nAngle + 1) % 4);
	//this->m_pWorkWnd->Invalidate();
	InvalidateRect(this->m_pWorkWnd->m_hWnd, this->m_rectWorkarea, FALSE);
	//this->m_pWorkWnd->UpdateWindow();
}

bool CTetrisGame::DrawWorkarea(CDC * pDC)
{
	if(NULL == pDC) return false;

	m_rectWorkarea.left = 10;
	m_rectWorkarea.right = m_rectWorkarea.left + gride_number_horizontal * m_pBlock->m_nGridSize + (gride_number_horizontal +1) * m_pBlock->m_nSpliterSize;
	m_rectWorkarea.top = 10;
	m_rectWorkarea.bottom = m_rectWorkarea.top + gride_number_vertical * m_pBlock->m_nGridSize + (gride_number_vertical + 1) * m_pBlock->m_nSpliterSize;
	
	/*CBrush newbrush(RGB(0,0,0));
	CBrush * poldbrush = pDC->SelectObject(&newbrush);
	if(NULL == poldbrush) return false;
	if(0 == pDC->Rectangle(m_rectWorkarea)) return false;
	if(NULL == pDC->SelectObject(poldbrush)) return false;*/

	CPen newpen(PS_SOLID, this->m_pBlock->m_nSpliterSize, RGB(255,255,255));
	CPen * poldpen = pDC->SelectObject(&newpen);
	if(NULL == poldpen) return false;

	for(int nx = this->m_rectWorkarea.left; 
		nx <= this->m_rectWorkarea.right; 
		nx += (this->m_pBlock->m_nGridSize + this->m_pBlock->m_nSpliterSize))
	{
		for(int ny =this->m_rectWorkarea.top; 
			ny <= this->m_rectWorkarea.bottom; 
			ny += (this->m_pBlock->m_nGridSize + this->m_pBlock->m_nSpliterSize))	
		{
			pDC->MoveTo(this->m_rectWorkarea.left, ny);
			if(0 == pDC->LineTo(this->m_rectWorkarea.right, ny)) return false;

			pDC->MoveTo(nx, this->m_rectWorkarea.top);
			if(0 == pDC->LineTo(nx, this->m_rectWorkarea.bottom)) return false;
		}
	}

	return NULL != pDC->SelectObject(poldpen);
}

bool CTetrisGame::DrawAllShape(CDC * pDC)
{
	if(this->m_vectorRect.size() != this->m_vectorColor.size()) return false;
	if(this->m_bStarted && !this->m_pBlock->Draw(pDC, this->m_ptCurrentLocation)) return false;
	if(NULL == pDC) return false;

	for(size_t n = 0; n < m_vectorRect.size(); n++)
	{
		CBrush newbrush(this->m_vectorColor[n]);
		CBrush * poldbrush = pDC->SelectObject(&newbrush);
		if(NULL == poldbrush) return false;

		if(0 == pDC->Rectangle(this->m_vectorRect[n])) return false;
		if(NULL == pDC->SelectObject(poldbrush)) return false;
	}

	return true;
}

UINT CTetrisGame::GameMain(void)
{
	this->GameResetlocation();
	this->m_pBlock->Randomizer();
	
	while(!this->GameOver())
	{		
		//this->m_pWorkWnd->Invalidate();
		InvalidateRect(this->m_pWorkWnd->m_hWnd, this->m_rectWorkarea, FALSE);
		//this->m_pWorkWnd->UpdateWindow();
		Sleep(m_nPauseTime);

		if(!this->m_pMutex->Lock()) break;
		if(this->CanMovedown())
		{
			m_ptCurrentLocation.y += (this->m_pBlock->m_nGridSize + this->m_pBlock->m_nSpliterSize);
		}
		else
		{
			for(int nRect = 0; nRect < 4; nRect ++) //save the 4 rectangle of tetris shape and its color to vector
			{
				this->m_vectorRect.push_back(this->m_pBlock->m_rectRect[nRect]);
				this->m_vectorColor.push_back(this->m_pBlock->m_nColor);
			}

			this->GameResetlocation();
			this->m_pBlock->Randomizer();

			this->RefreshGame();
		}
		if(!this->m_pMutex->Unlock()) break;
	}
	return 0;
}

UINT CTetrisGame::ThreadMain(LPVOID pParam)
{
	CTetrisGame * pThis = (CTetrisGame *)pParam;
	return pThis->GameMain();
}

UINT CTetrisGame::GameDown(void)
{
	if(this->m_bDowning) return 1; //has created a downing thread, can't create again
	
	this->m_pMainThread->SuspendThread();
	while(this->CanMovedown())
	{
		this->m_bDowning = true;

		if(!this->m_pMutex->Lock()) break;
		this->m_ptCurrentLocation.y += (this->m_pBlock->m_nGridSize + this->m_pBlock->m_nSpliterSize);
		if(!this->m_pMutex->Unlock()) break;

		//this->m_pWorkWnd->Invalidate();
		InvalidateRect(this->m_pWorkWnd->m_hWnd, this->m_rectWorkarea, FALSE);
		//this->m_pWorkWnd->UpdateWindow();
		Sleep(10);
	}
	this->m_pMainThread->ResumeThread();

	this->m_bDowning = false;
	return 0;
}

UINT CTetrisGame::ThreadDown(LPVOID pParam)
{
	CTetrisGame * pThis = (CTetrisGame *)pParam;
	return pThis->GameDown();
}

bool CTetrisGame::CanMoveleft(void) const
{
	CRect rect;
	bool bNeedcheck[4] = {false, false, false, false}; //indicate whether the 4 rectangle of the block need to be check
	int nLeftestRect = -1;

	switch(this->m_pBlock->m_nType)
	{
	case TYPE_I:
		switch(this->m_pBlock->m_nAngle)
		{
		case ANGLE_0:
		case ANGLE_180:	
			nLeftestRect = 0;
			bNeedcheck[0] = bNeedcheck[1] = bNeedcheck[2] = bNeedcheck[3] = true;
			break;
		case ANGLE_90:
		case ANGLE_270:
			nLeftestRect = 0;
			bNeedcheck[0] = true;
			break;
		default:
			return false;
			break;
		}
		break;
	case TYPE_L:
		switch(this->m_pBlock->m_nAngle)
		{
		case ANGLE_0:
			nLeftestRect = 0;
			bNeedcheck[0] = bNeedcheck[1] = bNeedcheck[2] = true;
			break;
		case ANGLE_90:
			nLeftestRect = 0;
			bNeedcheck[0] = bNeedcheck[3] = true;
			break;
		case ANGLE_180:		
			nLeftestRect = 0;
			bNeedcheck[0] = bNeedcheck[2] = bNeedcheck[3] = true;
			break;
		case ANGLE_270:
			nLeftestRect = 0;
			bNeedcheck[0] = bNeedcheck[3] = true;
			break;
		default:
			return false;
			break;
		}
		break;
	case TYPE_J:
		switch(this->m_pBlock->m_nAngle)
		{
		case ANGLE_0:
			nLeftestRect = 3;
			bNeedcheck[0] = bNeedcheck[1] = bNeedcheck[3] = true;
			break;
		case ANGLE_90:
			nLeftestRect = 0;
			bNeedcheck[0] =  bNeedcheck[3] = true;
			break;
		case ANGLE_180:
			nLeftestRect = 0;
			bNeedcheck[0] = bNeedcheck[1] = bNeedcheck[2] = true;
			break;
		case ANGLE_270:
			nLeftestRect = 0;
			bNeedcheck[0] = bNeedcheck[1] = true;
			break;
		default:
			return false;
			break;
		}
		break;
	case TYPE_T:
		switch(this->m_pBlock->m_nAngle)
		{
		case ANGLE_0:
			nLeftestRect = 0;
			bNeedcheck[0] = bNeedcheck[3] = true;
			break;
		case ANGLE_90:
			nLeftestRect = 0;
			bNeedcheck[0] = bNeedcheck[1] = bNeedcheck[2] = true;
			break;
		case ANGLE_180:
			nLeftestRect = 0;
			bNeedcheck[0] = bNeedcheck[3] = true;
			break;
		case ANGLE_270:
			nLeftestRect = 3;
			bNeedcheck[0] = bNeedcheck[2] = bNeedcheck[3] = true;
			break;
		default:
			return false;
			break;
		}
		break;
	case TYPE_Z:
		switch(this->m_pBlock->m_nAngle)
		{
		case ANGLE_0:
		case ANGLE_180:
			nLeftestRect = 0;
			bNeedcheck[0] = bNeedcheck[2] = true;
			break;		
		case ANGLE_90:
		case ANGLE_270:
			nLeftestRect = 2;
			bNeedcheck[0] = bNeedcheck[2] = bNeedcheck[3] = true;
			break;	
		default:
			return false;
			break;
		}
		break;
	case TYPE_S:
		switch(this->m_pBlock->m_nAngle)
		{
		case ANGLE_0:
		case ANGLE_180:
			nLeftestRect = 0;
			bNeedcheck[0] = bNeedcheck[2] = true;
			break;		
		case ANGLE_90:
		case ANGLE_270:
			nLeftestRect = 0;
			bNeedcheck[0] = bNeedcheck[1] = bNeedcheck[3] = true;
			break;
		default:
			return false;
			break;
		}
		break;
	case TYPE_O:
		switch(this->m_pBlock->m_nAngle)
		{
		case ANGLE_0:
		case ANGLE_90:
		case ANGLE_180:
		case ANGLE_270:
			nLeftestRect = 0;
			bNeedcheck[0] = bNeedcheck[2] = true;
			break;
		default:
			return false;
			break;
		}
		break;
	default:
		return false;
		break;
	}

	if(this->m_pBlock->m_rectRect[nLeftestRect].left <= (this->m_rectWorkarea.left + this->m_pBlock->m_nSpliterSize))
	{
		return false;
	}

	for(int nRect = 0; nRect < 4; nRect ++)
	{
		if(!bNeedcheck[nRect]) continue;

		rect.left = this->m_pBlock->m_rectRect[nRect].left - this->m_pBlock->m_nGridSize - this->m_pBlock->m_nSpliterSize;
		rect.top =  this->m_pBlock->m_rectRect[nRect].top;
		rect.right = this->m_pBlock->m_rectRect[nRect].left - this->m_pBlock->m_nSpliterSize;
		rect.bottom = this->m_pBlock->m_rectRect[nRect].bottom;

		if(this->m_vectorRect.end() != std::find(this->m_vectorRect.begin(), this->m_vectorRect.end(), rect)) //find, indicate left has a block
		{
			return false;
		}
	}

	return true;
}

bool CTetrisGame::CanMoveright(void) const
{
	CRect rect;
	bool bNeedcheck[4] = {false, false, false, false}; //indicate whether the 4 rectangle of the block need to be check
	int nRightestRect = -1;

	switch(this->m_pBlock->m_nType)
	{
	case TYPE_I:
		switch(this->m_pBlock->m_nAngle)
		{
		case ANGLE_0:
		case ANGLE_180:
			nRightestRect = 0;
			bNeedcheck[0] = bNeedcheck[1] = bNeedcheck[2] = bNeedcheck[3] = true;
			break;
		case ANGLE_90:	
		case ANGLE_270:
			nRightestRect = 3;
			bNeedcheck[3] = true;
			break;
		default:
			return false;
			break;
		}
		break;
	case TYPE_L:
		switch(this->m_pBlock->m_nAngle)
		{
		case ANGLE_0:
			nRightestRect = 3;
			bNeedcheck[0] = bNeedcheck[1] = bNeedcheck[3] = true;
			break;
		case ANGLE_90:
			nRightestRect = 3;
			bNeedcheck[2] = bNeedcheck[3] = true;
			break;
		case ANGLE_180:
			nRightestRect = 3;
			bNeedcheck[1] = bNeedcheck[2] = bNeedcheck[3] = true;
			break;
		case ANGLE_270:
			nRightestRect = 2;
			bNeedcheck[2] = bNeedcheck[3] = true;
			break;
		default:
			return false;
			break;
		}
		break;
	case TYPE_J:
		switch(this->m_pBlock->m_nAngle)
		{
		case ANGLE_0:
			nRightestRect = 0;
			bNeedcheck[0] = bNeedcheck[1] = bNeedcheck[2] = true;
			break;
		case ANGLE_90:
			nRightestRect = 3;
			bNeedcheck[2] = bNeedcheck[3] = true;
			break;
		case ANGLE_180:
			nRightestRect = 3;
			bNeedcheck[1] = bNeedcheck[2] = bNeedcheck[3] = true;
			break;
		case ANGLE_270:
			nRightestRect = 3;
			bNeedcheck[0] = bNeedcheck[3] = true;
			break;
		default:
			return false;
			break;
		}
		break;
	case TYPE_T:
		switch(this->m_pBlock->m_nAngle)
		{
		case ANGLE_0:
			nRightestRect = 2;
			bNeedcheck[2] = bNeedcheck[3] = true;
			break;
		case ANGLE_90:
			nRightestRect = 3;
			bNeedcheck[0] = bNeedcheck[2] = bNeedcheck[3] = true;
			break;
		case ANGLE_180:
			nRightestRect = 2;
			bNeedcheck[2] = bNeedcheck[3] = true;
			break;
		case ANGLE_270:
			nRightestRect = 0;
			bNeedcheck[0] = bNeedcheck[1] = bNeedcheck[2] = true;
			break;
		default:
			return false;
			break;
		}
		break;
	case TYPE_Z:
		switch(this->m_pBlock->m_nAngle)
		{
		case ANGLE_0:
		case ANGLE_180:
			nRightestRect = 3;
			bNeedcheck[1] = bNeedcheck[3] = true;
			break;	
		case ANGLE_90:
		case ANGLE_270:
			nRightestRect = 0;
			bNeedcheck[0] = bNeedcheck[1] = bNeedcheck[3] = true;
			break;
		default:
			return false;
			break;
		}
		break;
	case TYPE_S:
		switch(this->m_pBlock->m_nAngle)
		{
		case ANGLE_0:
		case ANGLE_180:
			nRightestRect = 3;
			bNeedcheck[1] = bNeedcheck[3] = true;
			break;	
		case ANGLE_90:
		case ANGLE_270:
			nRightestRect = 2;
			bNeedcheck[0] = bNeedcheck[2] = bNeedcheck[3] = true;
			break;
		default:
			return false;
			break;
		}
		break;
	case TYPE_O:
		switch(this->m_pBlock->m_nAngle)
		{
		case ANGLE_0:
		case ANGLE_90:
		case ANGLE_180:
		case ANGLE_270:
			nRightestRect = 3;
			bNeedcheck[1] = bNeedcheck[3] = true;
			break;
		default:
			return false;
			break;
		}
		break;
	default:
		return false;
		break;
	}

	if(this->m_pBlock->m_rectRect[nRightestRect].right >= (this->m_rectWorkarea.right - this->m_pBlock->m_nSpliterSize))
	{
		return false;
	}

	for(int nRect = 0; nRect < 4; nRect ++)
	{
		if(!bNeedcheck[nRect]) continue;

		rect.left = this->m_pBlock->m_rectRect[nRect].right + this->m_pBlock->m_nSpliterSize;
		rect.top =  this->m_pBlock->m_rectRect[nRect].top;
		rect.right =this->m_pBlock->m_rectRect[nRect].right + this->m_pBlock->m_nSpliterSize + this->m_pBlock->m_nGridSize;
		rect.bottom = this->m_pBlock->m_rectRect[nRect].bottom;

		if(this->m_vectorRect.end() != std::find(this->m_vectorRect.begin(), this->m_vectorRect.end(), rect)) //find, indicate right has a block
		{
			return false;
		}
	}

	return true;
}

bool CTetrisGame::CanMovedown(void) const
{
	CRect rect;
	bool bNeedcheck[4] = {false, false, false, false}; //indicate whether the 4 rectangle of the block need to be check
	int nDownestRect = -1;

	switch(this->m_pBlock->m_nType)
	{
	case TYPE_I:
		switch(this->m_pBlock->m_nAngle)
		{
		case ANGLE_0:
		case ANGLE_180:
			nDownestRect = 3;
			bNeedcheck[3] = true;
			break;
		case ANGLE_90:
		case ANGLE_270:
			nDownestRect = 3;
			bNeedcheck[0] = bNeedcheck[1] = bNeedcheck[2] = bNeedcheck[3] = true;
			break;
		default:
			return false;
			break;
		}
		break;
	case TYPE_L:
		switch(this->m_pBlock->m_nAngle)
		{
		case ANGLE_0:
			nDownestRect = 3;
			bNeedcheck[2] = bNeedcheck[3] = true;
			break;
		case ANGLE_90:
			nDownestRect = 0;
			bNeedcheck[0] = bNeedcheck[1] = bNeedcheck[2] = true;
			break;
		case ANGLE_180:
			nDownestRect = 3;
			bNeedcheck[0] = bNeedcheck[3] = true;
			break;
		case ANGLE_270:
			nDownestRect = 3;
			bNeedcheck[1] = bNeedcheck[2] = bNeedcheck[3] = true;
			break;
		default:
			return false;
			break;
		}
		break;
	case TYPE_J:
		switch(this->m_pBlock->m_nAngle)
		{
		case ANGLE_0:
			nDownestRect = 3;
			bNeedcheck[2] = bNeedcheck[3] = true;
			break;
		case ANGLE_90:
			nDownestRect = 3;
			bNeedcheck[0] = bNeedcheck[1] = bNeedcheck[3] = true;
			break;
		case ANGLE_180:
			nDownestRect = 2;
			bNeedcheck[2] = bNeedcheck[3] = true;
			break;
		case ANGLE_270:
			nDownestRect = 3;
			bNeedcheck[1] = bNeedcheck[2] = bNeedcheck[3] = true;
			break;
		default:
			return false;
			break;
		}
		break;
	case TYPE_T:
		switch(this->m_pBlock->m_nAngle)
		{
		case ANGLE_0:
			nDownestRect = 3;
			bNeedcheck[0] = bNeedcheck[2] = bNeedcheck[3] = true;
			break;
		case ANGLE_90:
			nDownestRect = 2;
			bNeedcheck[2] = bNeedcheck[3] = true;
			break;
		case ANGLE_180:
			nDownestRect = 0;
			bNeedcheck[0] = bNeedcheck[1] = bNeedcheck[2] = true;
			break;
		case ANGLE_270:
			nDownestRect = 2;
			bNeedcheck[2] = bNeedcheck[3] = true;
			break;
		default:
			return false;
			break;
		}
		break;
	case TYPE_Z:
		switch(this->m_pBlock->m_nAngle)
		{
		case ANGLE_0:
		case ANGLE_180:
			nDownestRect = 3;
			bNeedcheck[0] = bNeedcheck[2] = bNeedcheck[3] = true;
			break;	
		case ANGLE_90:
		case ANGLE_270:
			nDownestRect = 3;
			bNeedcheck[1] = bNeedcheck[3] = true;
			break;
		default:
			return false;
			break;
		}
		break;
	case TYPE_S:
		switch(this->m_pBlock->m_nAngle)
		{
		case ANGLE_0:
		case ANGLE_180:
			nDownestRect = 0;
			bNeedcheck[0] = bNeedcheck[1] = bNeedcheck[3] = true;
			break;	
		case ANGLE_90:
		case ANGLE_270:
			nDownestRect = 3;
			bNeedcheck[1] = bNeedcheck[3] = true;
			break;
		default:
			return false;
			break;
		}
		break;
	case TYPE_O:
		switch(this->m_pBlock->m_nAngle)
		{
		case ANGLE_0:
		case ANGLE_180:
		case ANGLE_90:
		case ANGLE_270:
			nDownestRect = 3;
			bNeedcheck[2] = bNeedcheck[3] = true;
			break;
		default:
			return false;
			break;
		}
		break;
	default:
		return false;
		break;
	}

	if(!this->m_pMutex->Lock()) return false;
	if(this->m_pBlock->m_rectRect[nDownestRect].bottom >= (this->m_rectWorkarea.bottom - this->m_pBlock->m_nSpliterSize))
	{
		this->m_pMutex->Unlock();
		return false;
	}

	for(int nRect = 0; nRect < 4; nRect ++)
	{
		if(!bNeedcheck[nRect]) continue;

		rect.left = this->m_pBlock->m_rectRect[nRect].left;
		rect.top =  this->m_pBlock->m_rectRect[nRect].bottom + this->m_pBlock->m_nSpliterSize;
		rect.right = this->m_pBlock->m_rectRect[nRect].right;
		rect.bottom = this->m_pBlock->m_rectRect[nRect].bottom + this->m_pBlock->m_nGridSize + this->m_pBlock->m_nSpliterSize;

		if(this->m_vectorRect.end() != std::find(this->m_vectorRect.begin(), this->m_vectorRect.end(), rect)) //find, indicate bottom has a block
		{
			this->m_pMutex->Unlock();
			return false;
		}
	}
	if(!this->m_pMutex->Unlock()) return false;

	return true;
}

bool CTetrisGame::CanRotate(void)
{
	CRect rect[4];
	BLOCK_ANGLE nAngle = (BLOCK_ANGLE)((this->m_pBlock->m_nAngle + 1) % 4);
	if(!this->m_pBlock->CalculateFourRect(this->m_pBlock->m_nType, nAngle, this->m_ptCurrentLocation, rect)) return false;

	for(int iRect = 0; iRect < 4; iRect ++)
	{
		if(this->m_vectorRect.end() != std::find(this->m_vectorRect.begin(), this->m_vectorRect.end(), rect[iRect])) 
		{
			return false;
		}
		else if(rect[iRect].left <= this->m_rectWorkarea.left 
					|| rect[iRect].top <= this->m_rectWorkarea.top
					|| rect[iRect].right >= this->m_rectWorkarea.right
					|| rect[iRect].bottom >= this->m_rectWorkarea.bottom)
		{
			return false;
		}
	}

	return true;
}

int CTetrisGame::RefreshBolckLayout(void)
{
	CRect rect;		
	int nCount = 0;
	int nFullRow = 0;
	int nx, ny;

	for(ny = 0; ny < gride_number_vertical; ny ++)
	{
		for(nx = 0; nx < gride_number_horizontal; nx ++)
		{
			rect.top = this->m_rectWorkarea.top + this->m_pBlock->m_nSpliterSize + ny * (this->m_pBlock->m_nGridSize + this->m_pBlock->m_nSpliterSize);
			rect.left = this->m_rectWorkarea.left + this->m_pBlock->m_nSpliterSize + nx * (this->m_pBlock->m_nGridSize + this->m_pBlock->m_nSpliterSize);
			rect.right = rect.left + this->m_pBlock->m_nGridSize;
			rect.bottom = rect.top + this->m_pBlock->m_nGridSize;
			
			if(this->m_vectorRect.end() != std::find(this->m_vectorRect.begin(), this->m_vectorRect.end(), rect))
				nCount ++;
			else
				break;
		}

		if(gride_number_horizontal == nCount) //all grid of a row are filled
		{
			nFullRow ++;
			this->RemoveFullRow(ny);
		}
		nCount = 0;
	}

	return nFullRow;
}

bool CTetrisGame::RemoveFullRow(int nRowIndex)
{
	CRect rect;
	std::vector<CRect>::iterator iterLocation;
	__w64 int nLocation = 0;

	rect.top = this->m_rectWorkarea.top + this->m_pBlock->m_nSpliterSize + nRowIndex * (this->m_pBlock->m_nGridSize + this->m_pBlock->m_nSpliterSize);
	rect.bottom = rect.top + this->m_pBlock->m_nGridSize;
	
	for(int nx = 0; nx < gride_number_horizontal; nx ++)
	{
		rect.left = this->m_rectWorkarea.left + this->m_pBlock->m_nSpliterSize + nx * (this->m_pBlock->m_nGridSize + this->m_pBlock->m_nSpliterSize);
		rect.right = rect.left + this->m_pBlock->m_nGridSize;

		iterLocation = std::find(this->m_vectorRect.begin(), this->m_vectorRect.end(), rect);
		nLocation = iterLocation - this->m_vectorRect.begin();

		if(this->m_vectorRect.end() == iterLocation)
			return false;
		else
		{
			this->m_vectorRect.erase(iterLocation);
			this->m_vectorColor.erase(this->m_vectorColor.begin() + nLocation);
		}
	}

	if(this->m_vectorColor.size() != this->m_vectorRect.size()) return false;

	for(UINT nIndex = 0; nIndex < this->m_vectorRect.size(); nIndex ++)
	{
		if(this->m_vectorRect[nIndex].top > rect.bottom) continue;
		this->m_vectorRect[nIndex].top += (this->m_pBlock->m_nGridSize + this->m_pBlock->m_nSpliterSize);
		this->m_vectorRect[nIndex].bottom += (this->m_pBlock->m_nGridSize + this->m_pBlock->m_nSpliterSize);
	}
	return true;
}

void CTetrisGame::RefreshGame(void)
{
	int nRow = this->RefreshBolckLayout();
	if(0 != nRow)
	{
		this->m_nFullRow += nRow;
		this->m_nGameScore += (100 * (this->m_nGameLevel * 2 - 1) * (nRow * 2 - 1));
		if(upgrade_number == this->m_nFullRow) 
		{
			this->m_nGameLevel ++;
			this->m_nPauseTime -= 150;
		}	
	}
}

bool CTetrisGame::GameOver(void)
{
	CRect rect;
	rect.top = this->m_rectWorkarea.top + this->m_pBlock->m_nSpliterSize;
	rect.left = this->m_rectWorkarea.left + this->m_pBlock->m_nSpliterSize + gride_number_horizontal / 2 * (this->m_pBlock->m_nGridSize + this->m_pBlock->m_nSpliterSize); //center
	rect.right = rect.left + this->m_pBlock->m_nGridSize;
	rect.bottom = rect.top +  this->m_pBlock->m_nGridSize;

	if(this->m_vectorRect.end() != std::find(this->m_vectorRect.begin(), this->m_vectorRect.end(), rect)) 
	{
		this->m_pMainThread = NULL;
		AfxMessageBox("I'm sorry to tell you game is over!", MB_OK | MB_ICONINFORMATION);
		m_bStarted = false;
		return true;
	}

	return false;
}

void CTetrisGame::GameInitialize(void)
{
	m_pMainThread = NULL;

	this->m_nGameLevel = 1;
	this->m_nGameScore = 0;
	this->m_nPauseTime = 1000;
	this->m_nFullRow = 0;
	this->m_bPaused = false;
	this->m_bStarted = false;
	this->m_bDowning = false;

	this->m_vectorColor.clear();
	this->m_vectorRect.clear();
}

void CTetrisGame::GameResetlocation(void)
{
	this->m_ptCurrentLocation.x = this->m_rectWorkarea.left + this->m_pBlock->m_nSpliterSize + gride_number_horizontal / 2 * (this->m_pBlock->m_nGridSize + this->m_pBlock->m_nSpliterSize); //center
	this->m_ptCurrentLocation.y = this->m_rectWorkarea.top + this->m_pBlock->m_nSpliterSize;
}