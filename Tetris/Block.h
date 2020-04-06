#ifndef _SHAPE_H
#define _SHAPE_H

enum BLOCK_TYPE
{
	TYPE_I,
	TYPE_L,
	TYPE_J,
	TYPE_T,
	TYPE_Z,
	TYPE_S,
	TYPE_O
};

enum BLOCK_ANGLE
{
	ANGLE_0,
	ANGLE_90,
	ANGLE_180,
	ANGLE_270
};

class CBlock
{
public:
	CBlock(unsigned short nGridSize, unsigned short nSpliterSize);
	~CBlock(void);

	void Randomizer(void);	//generate a shape randomly
	bool Draw(CDC * pDC, const CPoint & point);
	bool CalculateFourRect(BLOCK_TYPE nType, BLOCK_ANGLE nAngle, const CPoint & point, CRect * pRect) const;//calculate the 4 rectangle of the block

public:
	BLOCK_TYPE m_nType;
	BLOCK_ANGLE m_nAngle;
	COLORREF m_nColor;
	unsigned short m_nGridSize; //最小像元正方形之边长
	unsigned short m_nSpliterSize; //像元之间的分割线宽
	CRect m_rectRect[4]; //store the 4 rectangle of the block shape
};

#endif