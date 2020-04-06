#include "stdafx.h"
#include "Block.h"

CBlock::CBlock(unsigned short nGridSize, unsigned short nSpliterSize):
			m_nType(TYPE_I),
			m_nAngle(ANGLE_0),
			m_nColor(RGB(0,0,255)),
			m_nGridSize(nGridSize),
			m_nSpliterSize(nSpliterSize)
{
	for(int nRect = 0; nRect < 4; nRect ++)
	{
		this->m_rectRect[nRect].SetRectEmpty();
	}
}

CBlock::~CBlock(void)
{
}

bool CBlock::Draw(CDC * pDC, const CPoint & point)
{
	if(NULL == pDC) return false;
	if(!this->CalculateFourRect(this->m_nType, this->m_nAngle, point, this->m_rectRect)) return false;	

	//CClientDC dc(pWorkWnd);
	CBrush NewBrush(this->m_nColor);
	CBrush * pOldBrush = NULL;

	pOldBrush = pDC->SelectObject(&NewBrush);
	if(NULL == pOldBrush) return false;
	
	for(int nRect = 0; nRect < 4; nRect ++)
	{
		if(0 == pDC->Rectangle(this->m_rectRect[nRect])) return false;
	}

	if(NULL == pDC->SelectObject(pOldBrush)) return false;

	return true;
}

void	CBlock::Randomizer(void)	//generate a shape randomly
{
	srand( (unsigned)time( NULL));

	this->m_nType = (BLOCK_TYPE)(rand() % 7);//0 to 6
	this->m_nAngle = (BLOCK_ANGLE)(rand() % 4);//0 to 3

	switch(this->m_nType)
	{
	case TYPE_I:
		this->m_nColor = RGB(0,0,255);//blue
		break;
	case TYPE_L:
		this->m_nColor = RGB(255,0,255);//red+blue
		break;
	case TYPE_J:
		this->m_nColor = RGB(0,0,0);
		break;
	case TYPE_T:
		this->m_nColor = RGB(0,255,255);//green+blue
		break;
	case TYPE_Z:
		this->m_nColor = RGB(255,0,0);
		break;
	case TYPE_S:
		this->m_nColor = RGB(0,255,0);
		break;
	case TYPE_O:
		this->m_nColor = RGB(255,255,0);
		break;
	default:
		this->m_nColor = RGB(255,255,255);//default is balck
		break;
	}
}

bool CBlock::CalculateFourRect(BLOCK_TYPE nType, BLOCK_ANGLE nAngle, const CPoint & point, CRect * pRect) const
{
	CPoint ptLeftTop, ptRightBottom;
	switch(nType)
	{
	case TYPE_I:
		switch(nAngle)
		{
		case ANGLE_0:
		case ANGLE_180:
			for(int nRect = 0; nRect < 4; nRect ++)
			{
				ptLeftTop.SetPoint(point.x, point.y + nRect * (m_nGridSize + m_nSpliterSize));
				ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
				pRect[nRect].SetRect(ptLeftTop,  ptRightBottom);
			}
			break;
		case ANGLE_90:
		case ANGLE_270:
			for(int nRect = 0; nRect < 4; nRect ++)
			{
				ptLeftTop.SetPoint(point.x + nRect * (m_nGridSize + m_nSpliterSize), point.y);
				ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
				pRect[nRect].SetRect(ptLeftTop,  ptRightBottom);
			}
			break;
		default:
			return false;
			break;
		}
		break;
	case TYPE_L:
		switch(nAngle)
		{
		case ANGLE_0:
			for(int nRect = 0; nRect < 3; nRect ++)
			{
				ptLeftTop.SetPoint(point.x, point.y + nRect * (m_nGridSize + m_nSpliterSize));
				ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
				pRect[nRect].SetRect(ptLeftTop,  ptRightBottom);
			}

			ptLeftTop.SetPoint(pRect[2].right + m_nSpliterSize, pRect[2].top);
			ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
			pRect[3].SetRect(ptLeftTop,  ptRightBottom);
			break;
		case ANGLE_90:
			for(int nRect = 0; nRect < 3; nRect ++)
			{
				ptLeftTop.SetPoint(point.x + nRect * (m_nGridSize + m_nSpliterSize), point.y);
				ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
				pRect[nRect].SetRect(ptLeftTop,  ptRightBottom);
			}

			ptLeftTop.SetPoint(pRect[2].left, pRect[2].top - m_nSpliterSize - m_nGridSize);
			ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
			pRect[3].SetRect(ptLeftTop,  ptRightBottom);
			break;
		case ANGLE_180:
			for(int nRect = 0; nRect < 2; nRect ++)
			{
				ptLeftTop.SetPoint(point.x + nRect * (m_nGridSize + m_nSpliterSize), point.y);
				ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
				pRect[nRect].SetRect(ptLeftTop,  ptRightBottom);
			}

			for(int nRect = 2; nRect < 4; nRect ++)
			{
				ptLeftTop.SetPoint(pRect[1].left, pRect[nRect - 1].bottom + m_nSpliterSize);
				ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
				pRect[nRect].SetRect(ptLeftTop,  ptRightBottom);
			}
			break;
		case ANGLE_270:
			for(int nRect = 0; nRect < 3; nRect ++)
			{
				ptLeftTop.SetPoint(point.x + nRect * (m_nGridSize + m_nSpliterSize), point.y);
				ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
				pRect[nRect].SetRect(ptLeftTop,  ptRightBottom);
			}
			
			ptLeftTop.SetPoint(pRect[0].left, pRect[0].bottom + m_nSpliterSize);
			ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
			pRect[3].SetRect(ptLeftTop,  ptRightBottom);
			break;
		default:
			return false;
			break;
		}
		break;
	case TYPE_J:
		switch(nAngle)
		{
		case ANGLE_0:
			for(int nRect = 0; nRect < 3; nRect ++)
			{
				ptLeftTop.SetPoint(point.x, point.y + nRect * (m_nGridSize + m_nSpliterSize));
				ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
				pRect[nRect].SetRect(ptLeftTop,  ptRightBottom);
			}

			ptLeftTop.SetPoint(pRect[2].left - m_nSpliterSize - m_nGridSize, pRect[2].top);
			ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
			pRect[3].SetRect(ptLeftTop,  ptRightBottom);
			break;
		case ANGLE_90:
			for(int nRect = 0; nRect < 3; nRect ++)
			{
				ptLeftTop.SetPoint(point.x + nRect * (m_nGridSize + m_nSpliterSize), point.y);
				ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
				pRect[nRect].SetRect(ptLeftTop,  ptRightBottom);
			}

			ptLeftTop.SetPoint(pRect[2].left, pRect[2].bottom + this->m_nSpliterSize);
			ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
			pRect[3].SetRect(ptLeftTop,  ptRightBottom);
			break;
		case ANGLE_180:
			for(int nRect = 0; nRect < 3; nRect ++)
			{
				ptLeftTop.SetPoint(point.x, point.y + nRect * (m_nGridSize + m_nSpliterSize));
				ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
				pRect[nRect].SetRect(ptLeftTop,  ptRightBottom);
			}

			ptLeftTop.SetPoint(pRect[0].right + m_nSpliterSize, pRect[0].top);
			ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
			pRect[3].SetRect(ptLeftTop,  ptRightBottom);
			break;
		case ANGLE_270:
			for(int nRect = 0; nRect < 2; nRect ++)
			{
				ptLeftTop.SetPoint(point.x, point.y + nRect * (m_nGridSize + m_nSpliterSize));
				ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
				pRect[nRect].SetRect(ptLeftTop,  ptRightBottom);
			}

			ptLeftTop.SetPoint(pRect[1].right + this->m_nSpliterSize, pRect[1].top);
			ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
			pRect[2].SetRect(ptLeftTop,  ptRightBottom);

			ptLeftTop.SetPoint(pRect[2].right + this->m_nSpliterSize, pRect[2].top);
			ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
			pRect[3].SetRect(ptLeftTop,  ptRightBottom);
			break;
		default:
			return false;
		}
		break;
	case TYPE_T:
		switch(nAngle)
		{
		case ANGLE_0:
			for(int nRect = 0; nRect < 3; nRect ++)
			{
				ptLeftTop.SetPoint(point.x + nRect * (m_nGridSize + m_nSpliterSize), point.y);
				ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
				pRect[nRect].SetRect(ptLeftTop,  ptRightBottom);
			}

			ptLeftTop.SetPoint(pRect[1].left, pRect[1].bottom + m_nSpliterSize);
			ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
			pRect[3].SetRect(ptLeftTop,  ptRightBottom);
			break;
		case ANGLE_90:
			for(int nRect = 0; nRect < 3; nRect ++)
			{
				ptLeftTop.SetPoint(point.x, point.y + nRect * (m_nGridSize + m_nSpliterSize));
				ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
				pRect[nRect].SetRect(ptLeftTop,  ptRightBottom);
			}

			ptLeftTop.SetPoint(pRect[1].right + m_nSpliterSize, pRect[1].top);
			ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
			pRect[3].SetRect(ptLeftTop,  ptRightBottom);
			break;
		case ANGLE_180:
			for(int nRect = 0; nRect < 3; nRect ++)
			{
				ptLeftTop.SetPoint(point.x + nRect * (m_nGridSize + m_nSpliterSize), point.y);
				ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
				pRect[nRect].SetRect(ptLeftTop,  ptRightBottom);
			}

			ptLeftTop.SetPoint(pRect[1].left, pRect[1].top - m_nSpliterSize - m_nGridSize);
			ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
			pRect[3].SetRect(ptLeftTop,  ptRightBottom);
			break;
		case ANGLE_270:
			for(int nRect = 0; nRect < 3; nRect ++)
			{
				ptLeftTop.SetPoint(point.x, point.y + nRect * (m_nGridSize + m_nSpliterSize));
				ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
				pRect[nRect].SetRect(ptLeftTop,  ptRightBottom);
			}

			ptLeftTop.SetPoint(pRect[1].left - m_nSpliterSize - m_nGridSize, pRect[1].top);
			ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
			pRect[3].SetRect(ptLeftTop,  ptRightBottom);
			break;
		default:
			return false;
			break;
		}
		break;
	case TYPE_Z:
		switch(nAngle)
		{
		case ANGLE_0:
		case ANGLE_180:
			for(int nRect = 0; nRect < 2; nRect ++)
			{
				ptLeftTop.SetPoint(point.x + nRect * (m_nGridSize + m_nSpliterSize), point.y);
				ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
				pRect[nRect].SetRect(ptLeftTop,  ptRightBottom);
			}

			ptLeftTop.SetPoint(pRect[1].left, pRect[1].bottom + m_nSpliterSize);
			ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
			pRect[2].SetRect(ptLeftTop,  ptRightBottom);
			
			ptLeftTop.SetPoint(pRect[2].right + m_nSpliterSize, pRect[2].top);
			ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
			pRect[3].SetRect(ptLeftTop,  ptRightBottom);
			break;
		case ANGLE_90:
		case ANGLE_270:
			for(int nRect = 0; nRect < 2; nRect ++)
			{
				ptLeftTop.SetPoint(point.x, point.y + nRect * (m_nGridSize + m_nSpliterSize));
				ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
				pRect[nRect].SetRect(ptLeftTop,  ptRightBottom);
			}

			ptLeftTop.SetPoint(pRect[1].left - m_nSpliterSize - m_nGridSize, pRect[1].top);
			ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
			pRect[2].SetRect(ptLeftTop,  ptRightBottom);
			
			ptLeftTop.SetPoint(pRect[2].left, pRect[2].bottom + m_nSpliterSize);
			ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
			pRect[3].SetRect(ptLeftTop,  ptRightBottom);
			break;
		default:
			return false;
			break;
		}
		break;
	case TYPE_S:
		switch(nAngle)
		{
		case ANGLE_0:
		case ANGLE_180:
			for(int nRect = 0; nRect < 2; nRect ++)
			{
				ptLeftTop.SetPoint(point.x + nRect * (m_nGridSize + m_nSpliterSize), point.y);
				ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
				pRect[nRect].SetRect(ptLeftTop,  ptRightBottom);
			}

			ptLeftTop.SetPoint(pRect[1].left, pRect[1].top - m_nSpliterSize - m_nGridSize);
			ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
			pRect[2].SetRect(ptLeftTop,  ptRightBottom);
			
			ptLeftTop.SetPoint(pRect[2].right + m_nSpliterSize, pRect[2].top);
			ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
			pRect[3].SetRect(ptLeftTop,  ptRightBottom);
			break;
		case ANGLE_90:
		case ANGLE_270:
			for(int nRect = 0; nRect < 2; nRect ++)
			{
				ptLeftTop.SetPoint(point.x, point.y + nRect * (m_nGridSize + m_nSpliterSize));
				ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
				pRect[nRect].SetRect(ptLeftTop,  ptRightBottom);
			}

			ptLeftTop.SetPoint(pRect[1].right + m_nSpliterSize, pRect[1].top);
			ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
			pRect[2].SetRect(ptLeftTop,  ptRightBottom);
			
			ptLeftTop.SetPoint(pRect[2].left, pRect[2].bottom + m_nSpliterSize);
			ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
			pRect[3].SetRect(ptLeftTop,  ptRightBottom);
			break;
		default:
			return false;
			break;
		}
		break;
	case TYPE_O:
		switch(nAngle)
		{
		case ANGLE_0:		
		case ANGLE_90:
		case ANGLE_180:		
		case ANGLE_270:
			for(int nRect = 0; nRect < 2; nRect ++)
			{
				ptLeftTop.SetPoint(point.x + nRect * (m_nGridSize + m_nSpliterSize), point.y);
				ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
				pRect[nRect].SetRect(ptLeftTop,  ptRightBottom);
			}

			ptLeftTop.SetPoint(pRect[0].left, pRect[0].bottom + m_nSpliterSize); 
			ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
			pRect[2].SetRect(ptLeftTop,  ptRightBottom);

			ptLeftTop.SetPoint(pRect[1].left, pRect[1].bottom + m_nSpliterSize); 
			ptRightBottom.SetPoint(ptLeftTop.x + m_nGridSize, ptLeftTop.y + m_nGridSize);
			pRect[3].SetRect(ptLeftTop,  ptRightBottom);
		}
		break;
	default:
		return false;
		break;
	}
	
	return true;
}