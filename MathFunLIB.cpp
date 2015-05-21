#include "stdafx.h"
#include "MathFunLIB.h"
#include "stdio.h"
#define  MAGNIFY    10000
#define  D2R		0.017453292519943295769236907684886f
// 矩阵转置，用二维数组时，如果行数不等于列数，则不能直接输出到输入矩阵
void M_Transpose(double **MatI, int nRow, int nCol, double **MatO)
{
    int				i, j;

	for (j = 0; j < nRow; j++) 
	{
		for (i = 0; i < nCol; i++)  
		{
			MatO[i][j] = MatI[j][i];
		}
	}
}
// 矩阵相乘
void M_Multiply(double  **MatIa, double  **MatIb, double **MatO, int nRow, int nDim, int nCol)
{
	int		i, j, l;
    for (i = 0; i < nRow; i++)
	{
		for (j = 0; j < nCol; j++)
		{ 
			for (MatO[i][j] = 0.0, l = 0; l < nDim; l++)
				MatO[i][j] += MatIa[i][l] * MatIb[l][j];
		}
	}
}





double GetCoe(short* Mask0, short* Mask1, int datanum)
{
	int fm, gm;
	fm = gm = 0;
	int j;
	for(j = 0; j< datanum; j++)
	{
		fm += *(Mask0 + j);
		gm += *(Mask1 + j);
	}
	
	fm /= datanum;
	gm /= datanum;
	int Sum0, Sum1, Sum2;
	Sum0 = Sum1 = Sum2 = 0;
	for(j = 0; j< datanum; j++)
	{	
		Sum0 += (*(Mask0 + j) - fm) * (*(Mask1 + j) - gm);
		Sum1 += (*(Mask0 + j) - fm) * (*(Mask0 + j) - fm);
		Sum2 += (*(Mask1 + j) - gm) * (*(Mask1 + j) - gm);
	}
	double x = Sum0 / sqrt(Sum1 * (double)Sum2);
	return x;
}


int E_SolveGauss1(float	tt[PT_NDIM][PT_NDIM],	// 输入的系数方阵
				  float	AffineP[PT_NDIM])	// 未知数个数
{

	int		l = 1, k, i, j, is, js[6];
	float   d, t;
	float   aa[PT_NDIM][PT_NDIM];
	for (k = 0; k < PT_NDIM; k++)
		memcpy(aa[k], tt[k], PT_NDIM * sizeof(float));
	for (k = 0; k < PT_NDIM - 1; k++)
	{
		d = 0.0;
		for (i = k; i < PT_NDIM; i++)
			for (j = k; j < PT_NDIM; j++)
			{
				t = fabs(aa[i][j]);
				if (t > d) 
				{ 
					d = t; 
					js[k] = j; 
					is = i;
				}
			}
			if (d == 0.0) 
			{
				l = 0;
			}
			else
			{
				if (js[k] != k)
				{
					for (i = 0; i < PT_NDIM; i++)
					{
						t			 = aa[i][k]; 
						aa[i][k]	 = aa[i][js[k]];
						aa[i][js[k]] = t;
					}
				}
				if (is != k)
				{
					for (j = k; j< PT_NDIM; j++)
					{
						t		  = aa[k][j];
						aa[k][j]  = aa[is][j];
						aa[is][j] = t;
					} 
					t	  = AffineP[k]; 
					AffineP[k]  = AffineP[is]; 
					AffineP[is] = t;
				}
			}
			if (l == 0)
				return(0);
			for (d = aa[k][k], j = k+1; j < PT_NDIM; j++)
			{ 
				aa[k][j] /= d;
			}
			AffineP[k] /= d;
			for (i = k+ 1; i < PT_NDIM; i++)
			{
				for (j = k + 1; j < PT_NDIM; j++)
				{
					aa[i][j] -= aa[i][k] * aa[k][j];
				}
				AffineP[i] -= aa[i][k] * AffineP[k];
			}
	} 
	d = aa[PT_NDIM - 1][PT_NDIM - 1];
	if (fabs(d) < 1E-10)
		return(0);
	AffineP[PT_NDIM - 1] /= d;
	for (i = PT_NDIM - 2; i >= 0; i--)
	{		
		for (t = 0.0, j = i + 1; j < PT_NDIM; j++)
			t += aa[i][j] * AffineP[j];
		AffineP[i] -= t;
	} 
	js[PT_NDIM-1] = PT_NDIM - 1;
	for (k = PT_NDIM - 1; k >= 0; k--)
	{
		if (js[k] != k)
		{ 
			t		 = AffineP[k]; 
			AffineP[k]	 = AffineP[js[k]];
			AffineP[js[k]] = t;
		}
	}
	return(1);
}



void M_Inversion(double MatIO[PT_NDIM][PT_NDIM])
{ 
	int		i, j, k, u, v;
	int		is[PT_NDIM];
	int		js[PT_NDIM];
	double	d, p;

	for (k = 0; k < PT_NDIM; k++)//zai  MatIO[PT_NDIM][PT_NDIM] zhong zhao dao cong  MatIO[k][k] kai shi de zui da yuan 
	{
		d = MatIO[k][k];
		is[k]	= k;
		js[k]	= k;
        for (i = k; i < PT_NDIM; i++)
		{        
			for (j = k; j < PT_NDIM; j++)
			{
				p = fabs(MatIO[i][j]);
				if (p > d)
				{
					d		= p;
					is[k]	= i;
					js[k]	= j;
				}
			}
		}
//         if (fabs(d) < 1E-15) 
// 			break;
        if (is[k] != k)
		{
			for (j = 0; j < PT_NDIM; j++)
			{
				v		= is[k];
				p		= MatIO[k][j];
				MatIO[k][j]	= MatIO[v][j];
				MatIO[v][j]	= p;
			}
		}
       if (js[k] != k)
		{
			for (i = 0; i < PT_NDIM; i++)
			{
				u		= js[k];
				p		= MatIO[i][k];
				MatIO[i][k]	= MatIO[i][u];// jiang zui da yuan huan dao dui jiao xian shang
				MatIO[i][u] = p;
			}
		}
     
        MatIO[k][k] = 1.0 / MatIO[k][k];

        for (j = 0; j < PT_NDIM; j++)	
			if (j != k) 
				MatIO[k][j] *= MatIO[k][k];

		for (i = 0; i < PT_NDIM; i++)
		{
			if (i != k) 
			{
				for (j = 0; j < PT_NDIM; j++) 
					if (j != k) 
						MatIO[i][j] -= MatIO[i][k] * MatIO[k][j];
			}
		}
        for (i = 0; i < PT_NDIM; i++)	
			if (i != k) 
				MatIO[i][k] *= -MatIO[k][k];
	}

	for (k = PT_NDIM - 1; k >= 0; k--)
	{
		if (js[k] != k)
		{
			for (j = 0; j < PT_NDIM; j++)
			{
				v		= js[k];
				p		= MatIO[k][j];
				MatIO[k][j]	= MatIO[v][j];
				MatIO[v][j]	= p;
			}
		}
		if (is[k] != k)
		{
			for (i = 0; i < PT_NDIM; i++)
			{
				u		= is[k];
				p		= MatIO[i][k];
				MatIO[i][k]	= MatIO[i][u];
				MatIO[i][u]	= p;
			}
		}
	}
}

int	V_InnerProduct(short *vecIa, short *vecIb)
{
	int     fTemp = 0;
	int     i = 0;
   	//#pragma MUST_ITERATE(PT_MASKSIZE, PT_MASKSIZE);    
   	for (; i < PT_MASKSIZE; i++)	
   	{
   		fTemp += vecIa[i] * vecIb[i];
   	}   	
	return fTemp;
}

void M_Multiply1(double matIa[PT_NDIM][PT_EQUNUM], double matIb[PT_EQUNUM][PT_NDIM], double matO[PT_NDIM][PT_NDIM])
{ 
	int		i, j, l;
	for (i = 0; i < PT_NDIM; i++)
	{
		for (j = 0; j < PT_NDIM; j++)
		{ 
			for (matO[i][j] = 0.0, l = 0; l < PT_EQUNUM; l++)
				matO[i][j] += matIa[i][l] * matIb[l][j];
		}
	}
}
void M_Multiply2(double matIa[PT_NDIM][PT_NDIM], double matIb[PT_NDIM][PT_EQUNUM], double matO[PT_NDIM][PT_EQUNUM])
{ 
	int		i, j, l;
	for (i = 0; i < PT_NDIM; i++)
	{
		for (j = 0; j < PT_EQUNUM; j++)
		{ 
			for (matO[i][j] = 0.0, l = 0; l < PT_NDIM; l++)
				matO[i][j] += matIa[i][l] * matIb[l][j];
		}
	}
}

void M_Multiply3(double matIa[PT_NDIM][PT_EQUNUM], double matIb[PT_EQUNUM], double matO[PT_NDIM])
{ 
	int		i, j;
	for (i = 0; i < PT_NDIM; i++)
	{
		for (j = 0, matO[i]= 0; j < PT_EQUNUM; j++)
			matO[i] += matIa[i][j] * matIb[j];
	}
}
