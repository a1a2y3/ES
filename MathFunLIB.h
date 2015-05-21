#ifndef  _MATHFUN_H
#define  _MATHFUN_H
#include "math.h"
#include "DataDef.h"
void    InitBeforCalc();

// nDim维向量vecIa与nDim维向量vecIb的内积
double GetCoe(short* Mask0, short* Mask1, int datanum);
void   MaskMinus(short* Mask0, short* Mask1, int datanum);
int    V_InnerProduct   (short *vecIa, short   *vecIb);
//void    Get_Gabor_7Mask(CM_Short  Mask[7], CGaborParm Parm, int FunMode);

// 一般实方阵的逆矩阵(全选主元高斯—约当法)
void M_Inversion(double MatIO[PT_NDIM][PT_NDIM]);
void M_Multiply1(double matIa[PT_NDIM][PT_EQUNUM], double matIb[PT_EQUNUM][PT_NDIM], double matO[PT_NDIM][PT_NDIM]);
void M_Multiply2(double matIa[PT_NDIM][PT_NDIM],   double matIb[PT_NDIM][PT_EQUNUM], double matO[PT_NDIM][PT_EQUNUM]);
void M_Multiply3(double matIa[PT_NDIM][PT_EQUNUM], double matIb[PT_EQUNUM],          double matO[PT_NDIM]);

#endif
