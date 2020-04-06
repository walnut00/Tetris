#ifndef		_TETRISGAME_H
#define	_TETRISGAME_H

const short gride_number_horizontal = 16;
const short gride_number_vertical = 22;
const short upgrade_number = 50;

#include "Block.h"
#include <vector>
#include <algorithm>
#include <afxmt.h>

class CTetrisGame
{
public:
	CTetrisGame(CWnd * pWorkWnd, unsigned short nGridSize, unsigned short nSpliterSize);
	~CTetrisGame();

	bool GameStart(void);
	bool GamePause(void);
	bool GameRestore(void);
	bool GameSetup(void);

	void BlockMoveleft(void);
	void BlockMoveright(void);
	void BlockMovedown(void);
	void BlockRotate(void);

private:
	bool DrawWorkarea(CDC * pDC);
	bool DrawAllShape(CDC * pDC);
	
	UINT GameMain(void);
	static UINT ThreadMain(LPVOID pParam);
	UINT GameDown(void);
	static UINT ThreadDown(LPVOID pParam);
	
	bool CanMoveleft(void) const; //check whether can move left
	bool CanMoveright(void) const;
	bool CanMovedown(void) const;
	bool CanRotate(void);

	int RefreshBolckLayout(void); //check if current block layout has full filled row, if true return the full filled row number
	bool RemoveFullRow(int nRowIndex); //remove the full filled row from the vector, and adjust others rectangle's location
	void RefreshGame(void);

	bool GameOver(void);
	void GameInitialize(void);
	void GameResetlocation(void);

public:
	unsigned long m_nGameScore;
	unsigned short m_nGameLevel;

private:
	CWnd * m_pWorkWnd;
	CBlock * m_pBlock;;

	bool m_bPaused;
	bool m_bStarted;
	bool m_bDowning;

	CRect m_rectWorkarea;
	CPoint m_ptCurrentLocation;

	CWinThread * m_pMainThread;

	std::vector<CRect> m_vectorRect;
	std::vector<COLORREF> m_vectorColor;

	unsigned short m_nPauseTime;
	unsigned short m_nFullRow;

	CMutex * m_pMutex;
};

#endif