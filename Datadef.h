#ifndef _COMDEF_
#define _COMDEF_

#include "string.h"
#include "math.h"

typedef unsigned char BYTE;
// 二维点的坐标类型定义
class CFPt2D
{
public:
	double	x;		// x坐标
	double	y;		// y坐标

	CFPt2D( ) { x= y= 0; };
	CFPt2D( double Ix, double Iy ) { x= Ix; y= Iy; };
};


#define PT_NDIM              6
#define PT_EQUNUM            48
#define PT_MASKD             96	      // 72
#define PT_MASKSIZE          9216//9216	    // 5184
#define ITERNUM  	         3

extern  BYTE      pBTEMPMASK[PT_MASKSIZE];//[THREEMASK];
extern  short     pTMASK[3][PT_MASKSIZE];
extern  short     pCurMASK[PT_MASKSIZE];
extern  short     pTMask1 [PT_MASKSIZE];

extern  short     pSTEMPMASK[PT_MASKSIZE];
extern  short     pSTEMPMASK_2[PT_MASKSIZE];

extern  double    Boeff[3][PT_NDIM][PT_EQUNUM];
extern  double    pCoeff0[PT_EQUNUM][PT_NDIM];
extern  double    pCoeff1[PT_EQUNUM][PT_NDIM];
extern  double    pCoeff2[PT_EQUNUM][PT_NDIM];
extern  double    pCoeffT0[PT_NDIM][PT_EQUNUM];
extern  double    pCoeffT1[PT_NDIM][PT_EQUNUM];
extern  double    pCoeffT2[PT_NDIM][PT_EQUNUM];
extern  double    pTempA[PT_NDIM][PT_NDIM];


#endif


