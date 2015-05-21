#include "StdAfx.h"
#include "BlockHistMatch.h"
#include "ImageProcess.h"
#include "MatchFunc.h"
#include "ProgressDlg.h"
using namespace cv;
using namespace std;
CBlockHistMatch::CBlockHistMatch(void)
{	
	m_hMat=cv::Mat::eye(3,3,CV_32FC1);
	m_patchR      = 15;
	m_patchW      = 2* m_patchR+ 1;
	m_pLargeImg= NULL;
	m_pSmallImg= NULL;
	m_PYR_Large = (IplImage**) calloc(NSCALE,sizeof(IplImage*));
	m_PYR_Small = (IplImage**) calloc(NSCALE,sizeof(IplImage*));
	for (int i=0;i<NSCALE;i++)
	{
		m_PYR_Small[i]= NULL;
		m_PYR_Large[i]= NULL;
	}
	m_histLarge= NULL;
	m_histSmall= NULL;
}

CBlockHistMatch::~CBlockHistMatch(void)
{
	if (m_pLargeImg!= NULL)
		cvReleaseImage(&m_pLargeImg);
	if (m_pSmallImg!= NULL)
		cvReleaseImage(&m_pSmallImg);
	for (int i=0;i<NSCALE;i++)
	{
		if(m_PYR_Small[i]!=NULL)
			cvReleaseImage(&(m_PYR_Small[i]));
		if(m_PYR_Large[i]!=NULL)
			cvReleaseImage(&(m_PYR_Large[i]));
	}
	if (m_PYR_Large!= NULL)
		delete []m_PYR_Large;
	if (m_PYR_Small!= NULL)
		delete []m_PYR_Small;

	if (m_histLarge!=NULL)
		delete []m_histLarge;
	if (m_histSmall!=NULL)
		delete []m_histSmall;
		
}
void CBlockHistMatch::Image_Orientation_XY(IplImage *im)
{
	int avgsize=1; 
	uchar *pImage=(uchar*)im->imageData;
	int width= im->width, height= im->height, wstep= im->widthStep;
	int	i, j, x, y, index, Imagesize= wstep* height;
	double angle;
	int* pDx = new int[Imagesize];
	int* pDy = new int[Imagesize];
	uchar* pTemp= new uchar[Imagesize];
	for( x=1; x<width-1; x++ )
	{
		for( y=1; y<height-1; y++ )
		{
			index = y* wstep+ x;
			pDx[index] = pImage[index+1]- pImage[index-1];
			pDy[index] = pImage[index+wstep]- pImage[index-wstep];
		}
	}	
	memset( pTemp, 1, Imagesize );// 1 表示数据为空
	int arm = avgsize;
	int dx, dy, sumd, nG, vx, vy;
	for( x = arm+1; x < width - arm-1; x ++)
	{
		for( y = arm+1;  y < height - arm-1; y ++)
		{
			vx = vy = sumd = 0;
			for( i = -arm; i <= arm; i++ )
			{
				for( j = -arm; j <= arm; j++ )
				{
					index = (y+j)* wstep+ x+i;
					dx = pDx[index-wstep]+ 2*pDx[index]+ pDx[index+wstep];
					dy = pDy[index-1]+ 2*pDy[index]+ pDy[index+1];
					vx += 2* dx* dy;
					dx = dx* dx, dy = dy* dy;
					vy += dx- dy;
					sumd += dx+ dy;
				}
			}
			nG  = sumd/ (2*arm+1)/ (2*arm+1);
			angle = ( CV_PI- atan2( double(vy), vx ) )/*/ 2.0+PI/2*/;
//			pTemp[y*width+x] = nG < 169 ? 0 : (uchar)( int(angle/ CV_PI* 90)+ 74); //74~253
			pTemp[y*width+x] = (uchar)( int(angle/ CV_PI* 90)+ 74); //74~253
			if(pTemp[y*width+x]>252)
				pTemp[y*width+x]=252;
		}
	}
	memcpy( pImage, pTemp, Imagesize );
	delete []pDx;
	delete []pDy;
	delete []pTemp;
}

void CBlockHistMatch::GetDirHist(IplImage *pImg, int x0, int y0, int r, short *pHist)
{
	int i,j,g;
	uchar *pdata= (uchar*)pImg->imageData;
	int wstep= pImg->widthStep;
	for (i=-r;i<=r;i++)
	{
		for (j=-r;j<=r;j++)
		{
			g= *(pdata+(i+y0)*wstep+j+x0);
			// 考虑到方向反转情况 74-252 ------> 74-163
			if(g>=74)
			{
				if (g>163)
					pHist[(g-163)/10]++;
				else
					pHist[(g-74)/10]++;
			}
		}
	}
}
void CBlockHistMatch::InputImageHist(IplImage *pLargeImg, IplImage *pSmallImg)
{
	int i,j;
	if (m_pLargeImg!= NULL)
		cvReleaseImage(&m_pLargeImg);
	if (m_pSmallImg!= NULL)
		cvReleaseImage(&m_pSmallImg);
	m_pLargeImg= cvCloneImage(pLargeImg);
	m_pSmallImg= cvCloneImage(pSmallImg);
	m_pOriginLargeImg= pLargeImg;
	m_pOriginSmallImg= pSmallImg;
	int wLarge,hLarge, wSmall, hSmall;
	wLarge= pLargeImg->width;
	hLarge= pLargeImg->height;
	wSmall= pSmallImg->width;
	hSmall= pSmallImg->height;
	if (m_histLarge!=NULL)
		delete []m_histLarge;
	m_histLarge= new short[wLarge*hLarge*9];
	if (m_histSmall!=NULL)
		delete []m_histSmall;
	m_histSmall= new short[wSmall*hSmall*9];
	int histR= m_patchR;
	for (i=histR;i<hLarge-histR;i++)
	{
		for (j=histR;j<wLarge-histR;j++)
		{
			GetDirHist(m_pLargeImg, j, i, histR, m_histLarge+9*(i*wLarge+j));
		}
	}
	for (i=histR;i<hSmall-histR;i++)
	{
		for (j=histR;j<wSmall-histR;j++)
		{
			GetDirHist(m_pSmallImg, j, i, histR, m_histSmall+9*(i*wSmall+j));
		}
	}
	GetSamplePoints();
	m_smallRX= (pSmallImg->width-1)/2.0f;
	m_smallRY= (pSmallImg->height-1)/2.0f;
}
void CBlockHistMatch::GetSamplePoints()
{
	int i,j,m,n;
	int w= m_pSmallImg->width;
	int h= m_pSmallImg->height;
	int sd= 25;	
	for (i=m_patchR;i<h;i+=sd)
	{
		for (j=m_patchR;j<w;j+=sd)
		{
			m=i;
			n=j;
			if (i>h-m_patchR-1)
			{
				m= h-m_patchR-1;
			}
			if (j>m_pSmallImg->width-m_patchR-1)
			{
				n= m_pSmallImg->width-m_patchR-1;
			}
			m_samplepoints.push_back(cvPoint(n,m));
		}
	}
}

// a: 小图,    b: 大图
int CBlockHistMatch::Mul_Scale_Circu_Dis(IplImage **PYRa, IplImage **PYRb, int x0, int y0)
{
	int i,j,s,smax,x,y,r=10;
	int dref,dreal;
	int h1[36], h2[36];
	memset(h1,0,36*sizeof(int));
	memset(h2,0,36*sizeof(int));
	smax= 4;
	for (s=0;s<4;s++)
	{
		if(min(PYRa[s]->width, PYRa[s]->height)<(2*r+1))
		{
			smax= s;
		}
	}
	// 建立直方图
	int wa,wb,ha,hb,wstepa,wstepb;
	uchar *pa,*pb;
	x=x0, y=y0;
	for (s=0;s<smax;s++)
	{
		wa= PYRa[s]->width;
		wb= PYRb[s]->width;
		ha= PYRa[s]->height;
		hb= PYRb[s]->height;
		wstepa= PYRa[s]->widthStep;
		wstepb= PYRb[s]->widthStep;
		pa= (uchar*)(PYRa[s]->imageData);
		pb= (uchar*)(PYRb[s]->imageData);
		for (i=-r;i<=r;i++)
		{
			for (j=-r;j<=r;j++)
			{
				dref=*(pb+(y+ha/2+i)*wstepb+x+wa/2+j);
				dreal=*(pa+(ha/2+i)*wstepa+wa/2+j);
				// 考虑到方向反转情况 74-252 ------> 74-163
				if(dref>=74)
				{
					if (dref>163)
						h1[(dref-163)/10+s*9]++;
					else
						h1[(dref-74)/10+s*9]++;
				}
				if(dreal>=74)
				{
					if (dreal>163)
						h2[(dreal-163)/10+s*9]++;
					else
						h2[(dreal-74)/10+s*9]++;
				}
				// 不考虑方向反转情况
				//h1[(dref-74)/20+s*9]++;
				//h2[(dreal-74)/20+s*9]++;
			}
		}
		x/=2, y/=2;
	}
	// 计算直方图距离
	int sim=0, totalsim=0, scalesim=0;
//	for (i=0;i<9;i++)	// 9种循环
	for (i=0;i<1;i++)	// 不循环
	{
		totalsim=1;
		for (s=0;s<smax;s++)  // 多个尺度
		{		
			scalesim= 0;
			for (j=0;j<9;j++)  // 9 bins
			{
				scalesim+= min(h1[j+s*9],h2[(j+i)%9+s*9]);
			}
			totalsim += scalesim;   // 多尺度直方图相似性求和
		}
		if (totalsim>sim)
		{
			sim=totalsim;
		}
	}
	sim++;
	return sim;
}
void CBlockHistMatch::SimpleMatch2D()
{
	// 加匹配代码
	CvPoint pointInLarge = cvPoint(-1, -1);
	int i,j,dh,dw;
	int sim=0,maxsim=0,minsim= 999999999;
	dh= m_PYR_Large[0]->height- m_PYR_Small[0]->height;
	dw= m_PYR_Large[0]->width - m_PYR_Small[0]->width;
	IplImage *pRstMap= cvCreateImage(cvSize(dw+1,dh+1),8,1);
	int *pRst= new int[(dw+1)*(dh+1)];
	for (i=0;i<=dh;i++)
	{
		for (j=0;j<=dw;j++)
		{		
			sim = Mul_Scale_Circu_Dis(m_PYR_Small, m_PYR_Large, j, i);	
			if(sim>maxsim)
			{
				maxsim=sim;
				pointInLarge.x= j;
				pointInLarge.y= i;
			}
			if (sim<minsim) 
				minsim= sim;
			*(pRst+i*(dw+1)+j)= sim;
		}
	}
	for (i=0;i<=dh;i++)
	{
		for (j=0;j<=dw;j++)
		{	
			*(pRstMap->imageData+i*pRstMap->widthStep+j)= (*(pRst+i*(dw+1)+j)-minsim)*255/(maxsim-minsim);
		}
	}
	cvShowImage("pRstMap",pRstMap);
	cvSaveImage("d:\\feature\\pRstMap.bmp",pRstMap);
	cvReleaseImage(&pRstMap);
	delete []pRst;
	m_hMat.at<float>(0,0)= 1;
	m_hMat.at<float>(1,1)= 1;
	m_hMat.at<float>(0,1)= 0;
	m_hMat.at<float>(1,0)= 0;
	m_hMat.at<float>(0,2)= pointInLarge.x+m_smallRX;
	m_hMat.at<float>(1,2)= pointInLarge.y+m_smallRY;
}

void CBlockHistMatch::GetTransformedPoint(CvPoint2D32f OldPoint, CvPoint2D32f* pNewPoint)
{
	cv::Mat newPoint(3,1,CV_32FC1);
	cv::Mat oldPoint(3,1,CV_32FC1);
	oldPoint.at<float>(0,0)= OldPoint.x-m_smallRX;
	oldPoint.at<float>(1,0)= OldPoint.y-m_smallRY;
	oldPoint.at<float>(2,0)= 1.0f;
	newPoint= m_hMat* oldPoint;
	pNewPoint->x= newPoint.at<float>(0,0);
	pNewPoint->y= newPoint.at<float>(1,0);
}
void CBlockHistMatch::GetMatchResult(int targetMode,CvPoint2D32f *pointLarge,CvPoint2D32f *pointSmall)
{
	cv::Mat outputTargetPoint(3,1,CV_32FC1);
	cv::Mat inputTargetPoint(3,1,CV_32FC1);
	if (targetMode==1||targetMode==2)      // 大图变换到小图
	{
		cv::Mat hMatinv(3,3,CV_32FC1);
		invert(m_hMat,hMatinv);
		inputTargetPoint.at<float>(0,0)= pointLarge->x;
		inputTargetPoint.at<float>(1,0)= pointLarge->y;
		inputTargetPoint.at<float>(2,0)= 1.0f;
		outputTargetPoint= hMatinv* inputTargetPoint;
		pointSmall->x= outputTargetPoint.at<float>(0,0)+m_smallRX;
		pointSmall->y= outputTargetPoint.at<float>(1,0)+m_smallRY;
	}
	else if (targetMode==3||targetMode==4)  // 小图变换到大图
	{
		inputTargetPoint.at<float>(0,0)= pointSmall->x-m_smallRX;
		inputTargetPoint.at<float>(1,0)= pointSmall->y-m_smallRY;
		inputTargetPoint.at<float>(2,0)= 1.0f;
		outputTargetPoint= m_hMat* inputTargetPoint;
		pointLarge->x= outputTargetPoint.at<float>(0,0);
		pointLarge->y= outputTargetPoint.at<float>(1,0);
	}
	else
	{
		return;
	}
	m_pointInSmall= *pointSmall;
	m_pointInLarge= *pointLarge;
}
void CBlockHistMatch::ShowMatch(CString filepath)
{
	if (m_pLargeImg!= NULL)
	{
		cvReleaseImage(&m_pLargeImg);
	}
	if (m_pSmallImg!= NULL)
	{
		cvReleaseImage(&m_pSmallImg);
	}
	m_pLargeImg= cvCloneImage(m_pOriginLargeImg);
	m_pSmallImg= cvCloneImage(m_pOriginSmallImg);
	//  灰度图替换边缘图，用于匹配，显示
	cv::Mat pointTransformedMat(3,4,CV_32FC1);
	cv::Mat smallcenterpoint(3,4,CV_32FC1);
	//  1:lt, 2:rt, 3:rb, 4:lb
	//lt
	smallcenterpoint.at<float>(0,0)= (float)(0-m_smallRX);
	smallcenterpoint.at<float>(1,0)= (float)(0-m_smallRY);
	smallcenterpoint.at<float>(2,0)= 1.0f;
	//rt
	smallcenterpoint.at<float>(0,1)= (float)(m_smallRX);
	smallcenterpoint.at<float>(1,1)= (float)(0-m_smallRY);
	smallcenterpoint.at<float>(2,1)= 1.0f;
	//rb
	smallcenterpoint.at<float>(0,2)= (float)(m_smallRX);
	smallcenterpoint.at<float>(1,2)= (float)(m_smallRY);
	smallcenterpoint.at<float>(2,2)= 1.0f;
	//lb
	smallcenterpoint.at<float>(0,3)= (float)(0-m_smallRX);
	smallcenterpoint.at<float>(1,3)= (float)(m_smallRY);
	smallcenterpoint.at<float>(2,3)= 1.0f;

	pointTransformedMat= m_hMat* smallcenterpoint;

	int linewidth=2;
	int lineR= 20;
	// 画图
	cvCircle (m_pLargeImg,
		cvPoint((int)(pointTransformedMat.at<float>(0,0)+0.5), (int)(pointTransformedMat.at<float>(1,0)+0.5)),
		3, cvScalar(255,0,0), 2);
	//大图画小图对应矩形区域
	cvLine(m_pLargeImg, 
		cvPoint((int)(pointTransformedMat.at<float>(0,0)+0.5), (int)(pointTransformedMat.at<float>(1,0)+0.5)), 
		cvPoint((int)(pointTransformedMat.at<float>(0,1)+0.5), (int)(pointTransformedMat.at<float>(1,1)+0.5)), 
		cvScalar(255,0,0), linewidth);
	cvLine(m_pLargeImg, 
		cvPoint((int)(pointTransformedMat.at<float>(0,2)+0.5), (int)(pointTransformedMat.at<float>(1,2)+0.5)), 
		cvPoint((int)(pointTransformedMat.at<float>(0,1)+0.5), (int)(pointTransformedMat.at<float>(1,1)+0.5)), 
		cvScalar(255,0,0), linewidth);
	cvLine(m_pLargeImg, 
		cvPoint((int)(pointTransformedMat.at<float>(0,0)+0.5), (int)(pointTransformedMat.at<float>(1,0)+0.5)), 
		cvPoint((int)(pointTransformedMat.at<float>(0,3)+0.5), (int)(pointTransformedMat.at<float>(1,3)+0.5)), 
		cvScalar(255,0,0), linewidth);
	cvLine(m_pLargeImg, 
		cvPoint((int)(pointTransformedMat.at<float>(0,2)+0.5), (int)(pointTransformedMat.at<float>(1,2)+0.5)), 
		cvPoint((int)(pointTransformedMat.at<float>(0,3)+0.5), (int)(pointTransformedMat.at<float>(1,3)+0.5)), 
		cvScalar(255,0,0), linewidth);
	//大图画十字丝
	CvPoint tp;
	tp= cvPoint((int)(m_pointInLarge.x+0.5), (int)(m_pointInLarge.y+0.5));
	cvLine(m_pLargeImg, cvPoint(tp.x-lineR, tp.y), cvPoint(tp.x+lineR, tp.y), cvScalar(255,0,0), linewidth);
	cvLine(m_pLargeImg, cvPoint(tp.x, tp.y-lineR), cvPoint(tp.x, tp.y+lineR), cvScalar(255,0,0), linewidth);		
	//小图画十字丝
	tp= cvPoint((int)(m_pointInSmall.x+0.5), (int)(m_pointInSmall.y+0.5));
	cvLine(m_pSmallImg, cvPoint(tp.x-lineR, tp.y), cvPoint(tp.x+lineR, tp.y), cvScalar(255,0,0), linewidth);
	cvLine(m_pSmallImg, cvPoint(tp.x, tp.y-lineR), cvPoint(tp.x, tp.y+lineR), cvScalar(255,0,0), linewidth);		
	
	float a0=m_hMat.at<float>(0,0);
	float a1=m_hMat.at<float>(0,1);
	float s0=sqrt(a0*a0+a1*a1);
	float r0=acos(a0/s0)*180/(float)(CV_PI);
	CString str;
	if (filepath=="0")
	{
		str="d:\\feature\\匹配结果.bmp";
	}
	else
	{
		str=filepath+"匹配结果.bmp";
	}
	IplImage* pMergeImage= Merge2Images(m_pLargeImg,m_pSmallImg);
	cvShowImage("图像变形",pMergeImage);
	cvSaveImage(str,pMergeImage);
	cvReleaseImage(&pMergeImage);
}
