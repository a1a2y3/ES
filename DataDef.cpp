#include "stdafx.h"
#include "DataDef.h"

BYTE    pBTEMPMASK[PT_MASKSIZE ];
short    pCurMASK[PT_MASKSIZE];
short    pTMask1 [PT_MASKSIZE];
short    pTMASK[3][PT_MASKSIZE];
short   pSTEMPMASK[PT_MASKSIZE];
short   pSTEMPMASK_2[PT_MASKSIZE];
double  Boeff[3][PT_NDIM][PT_EQUNUM];
double  pCoeff0[PT_EQUNUM][PT_NDIM];
double  pCoeff1[PT_EQUNUM][PT_NDIM];
double  pCoeff2[PT_EQUNUM][PT_NDIM];
double  pCoeffT0[PT_NDIM][PT_EQUNUM];
double  pCoeffT1[PT_NDIM][PT_EQUNUM];
double  pCoeffT2[PT_NDIM][PT_EQUNUM];
double  pTempA[PT_NDIM][PT_NDIM];


