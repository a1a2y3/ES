// PlaceTokenTrack.h: interface for the CPlaceTokenMatch class.
//
//////////////////////////////////////////////////////////////////////

#ifndef  _PTTRACKLIB_H_
#define  _PTTRACKLIB_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//*****************************************************************************
//**                   PlaceToken���ٷ���                                    **
//**        1��    ����һ��  PlaceTokenTrack ����                            **
//**        2��    �InitiateTrack()�����һ��ͼ����Ŀ��ĸ����͸��Ե�λ��    **
//**        3��    ����TrackOneImage()��ʼ���٣�������ǰͼ��ָ���Լ�         **
//**               ��������˶���ɵ�Ŀ����ͼ���ϵ��˶������Ŀ���ڵ�ǰ      **
//**               ͼ���е�λ��.                                             **                                 
//**                                                                         **
//**                                                                     :)  **
//*****************************************************************************


#include "DataDef.h"
class CTargetInfo  
{
public:
	CTargetInfo();
	virtual ~CTargetInfo();

	void    UpdateSelf(CFPt2D);
	CFPt2D  Forcast();

	CFPt2D    Rat;
	CFPt2D    Pos;
};

class CPlaceTokenMatch  
{
public:
	CPlaceTokenMatch();
	void        InitiateTrack(BYTE *pInImg, CFPt2D  TargetPos);
	bool        TrackOneImage(BYTE *pInImg, CFPt2D &TargetPos);
	void        GetToken(short  *pInMask, double *pToken);
	void        ResetCoeff0(BYTE *pInImg, int x0, int y0);

	void        DAT_copy(void* pSrc, void* pDst, int datalen );
	void        Get_CImage_Rect(short *pMask, BYTE *pImage, short x, short y, short Magnitude);

	CTargetInfo m_Target;
	float       MaxCor;
	float       CurCor;
	float       PreCor;
	int			Unstable;
	double      GetCoe(short *Mask0, short *Mask1, int Height, int Width);
	int         PT_IMGW;
	int         PT_IMGH;
private:	
	short       m_pGaborData[PT_MASKSIZE * PT_EQUNUM / 2];
	double      m_pAffineparm[PT_NDIM];
private:
	int  		m_UpdateNum;
	double      m_pToken1[PT_EQUNUM];
};
#endif // !defined(AFX_PLACETOKENTRACK_H__2EC15455_886D_4358_9146_265271FEF850__INCLUDED_)
