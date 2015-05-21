#include "StdAfx.h"
#include "BlockMatchDirSub.h"
#include "ImageProcess.h"
#include "MatchFunc.h"
#include "ProgressDlg.h"
//#define  _MATCH_DEBUG
using namespace cv;
using namespace std;
CBlockMatchDirSub::CBlockMatchDirSub(void)
{	
	m_hMat=cv::Mat::eye(3,3,CV_32FC1);
	m_patchR      = 15;
	m_patchW      = 2* m_patchR+ 1;
	m_pointInLarge= cvPoint2D32f(-1.0f,-1.0f);
	m_pointInSmall= cvPoint2D32f(-1.0f,-1.0f);
	m_smallRX= -1;
	m_smallRY= -1;
	m_pAngleLUT   = NULL;
	m_pLargeImg   = NULL;
	m_pSmallImg   = NULL;
	m_pPatchMask  = NULL;
}

CBlockMatchDirSub::~CBlockMatchDirSub(void)
{
	if (m_pAngleLUT!=NULL)
	{
		int i;
		for( i=0; i<255; i++ )
		{
			delete[] m_pAngleLUT[i];
		}
		delete[] m_pAngleLUT;
	}
	if (m_pPatchMask!= NULL)
	{
		delete[] m_pPatchMask;
	}
	if (m_pLargeImg!= NULL)
	{
		cvReleaseImage(&m_pLargeImg);
	}
	if (m_pSmallImg!= NULL)
	{
		cvReleaseImage(&m_pSmallImg);
	}
	if (m_samplepoints.size()!=0)
	{
		m_samplepoints.clear();
	}	
	if (m_simMaps.size()!=0)
	{
		m_simMaps.clear();
	}	
}
void CBlockMatchDirSub::InputImage(IplImage *pLargeImg, IplImage *pSmallImg)
{
	if (m_pLargeImg!= NULL)
	{
		cvReleaseImage(&m_pLargeImg);
	}
	if (m_pSmallImg!= NULL)
	{
		cvReleaseImage(&m_pSmallImg);
	}
	m_pLargeImg= cvCloneImage(pLargeImg);
	m_pSmallImg= cvCloneImage(pSmallImg);
	m_pOriginLargeImg= pLargeImg;
	m_pOriginSmallImg= pSmallImg;
	m_smallRX= (pSmallImg->width-1)/2.0f;
	m_smallRY= (pSmallImg->height-1)/2.0f;
}
void CBlockMatchDirSub::SetPara(int pr)
{
	m_patchR= pr;
	m_patchW= 2*m_patchR+1;
}
void CBlockMatchDirSub::MemPrepare()
{
	// set LUT
	m_pPatchMask= new BYTE[m_patchW*m_patchW];
	// set patch radius mask
	int i,j,idx;
	for (i=0;i<m_patchW;i++)
	{
		for (j=0;j<m_patchW;j++)
		{
			idx=i*m_patchW+j;
			if ( (i-m_patchR)*(i-m_patchR)+(j-m_patchR)*(j-m_patchR) <= m_patchR*m_patchR )
			{
				m_pPatchMask[idx]=1;
			}
			else
			{
				m_pPatchMask[idx]=0;
			}
		}
	}
	m_pAngleLUT = SetAnlgeLUT();
}
int CBlockMatchDirSub::GetPatchSim_XY_window(BYTE *pRef, int refwstep, BYTE *pReal, int realwstep)
{
	int i,j,dref,dreal,sum=0;
	for (i=0;i<m_patchW;i++)
	{
		for (j=0;j<m_patchW;j++)
		{
			dref=*(pRef+i*refwstep+j);
			dreal=*(pReal+i*realwstep+j);
			sum = sum+ m_pAngleLUT[dreal][dref];//*m_pPatchMask[i];
		}
	}
	return sum;
}
int CBlockMatchDirSub::GetPatchSim_ORI_HIST(BYTE *pRef, int refwstep, BYTE *pReal, int realwstep)
{
	int i,j,dref,dreal,sim=0;
	int h1[16], h2[16], tmpsim=0;
	memset(h1,0,16*sizeof(int));
	memset(h2,0,16*sizeof(int));
	for (i=0;i<m_patchW;i++)
	{
		for (j=0;j<m_patchW;j++)
		{
			dref=*(pRef+i*refwstep+j);
			dreal=*(pReal+i*realwstep+j);
			if(dref>=74)
				h1[(dref-74)/15]++;
			if(dreal>=74)
				h2[(dreal-74)/15]++;
//			sum = sum+ m_pAngleLUT[dreal][dref];//*m_pPatchMask[i];
		}
	}
	for (i=0;i<12;i++)
	{
		tmpsim=0;
		for (j=0;j<12;j++)
		{
			tmpsim+= min(h1[j],h2[(j+i)%12]);
		}
		if (tmpsim>sim)
		{
			sim=tmpsim;
		}
	}
	return sim;
}
int CBlockMatchDirSub::GetPatchSim_GRAY_HIST(BYTE *pRef, int refwstep, BYTE *pReal, int realwstep)
{
	int i,j,dref,dreal,sim=0;
	int h1[32], h2[32];
	memset(h1,0,32*sizeof(int));
	memset(h2,0,32*sizeof(int));
	for (i=0;i<m_patchW;i++)
	{
		for (j=0;j<m_patchW;j++)
		{
			dref=*(pRef+i*refwstep+j);
			dreal=*(pReal+i*realwstep+j);
			h1[dref/32]++;
			h2[dreal/32]++;
		}
	}
	for (i=0;i<32;i++)
	{
		sim+= min(h1[i],h2[i]);
	}

	//sim= 200*m_patchW*m_patchW;
	//for (i=0;i<m_patchW;i++)
	//{
	//	for (j=0;j<m_patchW;j++)
	//	{
	//		dref=*(pRef+i*refwstep+j);
	//		dreal=*(pReal+i*realwstep+j);
	//		sim -= abs(dref-dreal);
	//	}
	//}
	return sim;
}
void CBlockMatchDirSub::GetSamplePoints()
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

void CBlockMatchDirSub::GetSimMap_NCC_DFT()
{
	GetSamplePoints();
	int m;
	BYTE* pRealImg= (BYTE*)m_pSmallImg->imageData;
	cv::Mat realMat= Mat(m_pSmallImg);
	cv::Mat refMat = Mat(m_pLargeImg);
	//制作采样点
	int nPoints= m_samplepoints.size();
	CvPoint corner;		
	m_pointMat= cv::Mat::zeros(3,nPoints,CV_32FC1);

	int dw=m_pLargeImg->width-m_pSmallImg->width;
	int dh=m_pLargeImg->height-m_pSmallImg->height;

	bool isShowProgressDlg = false; 
	CProgressDlg * MsgHandler = new CProgressDlg;
	isShowProgressDlg = true;
	if(isShowProgressDlg)
	{
		MsgHandler->Create(NULL);
		MsgHandler->ShowWindow(SW_SHOW);
	}
	cv::Mat maskReal,maskRef;
	for (m=0;m<nPoints;m++)  // 对于每个采样点，匹配一次
	{
		if(isShowProgressDlg)
			MsgHandler->SetPos( m*100/nPoints );
		
		m_simMaps.push_back(cv::Mat::zeros(dh+1,dw+1,CV_32FC1));
		corner= m_samplepoints.at(m);
		maskReal= realMat(Rect(corner.x-m_patchR,corner.y-m_patchR,2*m_patchR+1,2*m_patchR+1));
		maskRef = refMat(Rect(corner.x-m_patchR,corner.y-m_patchR,2*m_patchR+1+dw,2*m_patchR+1+dh));
		matchTemplate(maskRef,maskReal,m_simMaps.at(m),CV_TM_CCOEFF_NORMED);
		// 采样点矩阵，后面将对采样点变换，求变换后的点坐标
		m_pointMat.at<float>(0,m)= (float)(corner.x-m_smallRX);
		m_pointMat.at<float>(1,m)= (float)(corner.y-m_smallRY);
		m_pointMat.at<float>(2,m)= 1.0f;
	}
	if(isShowProgressDlg)
	{
		MsgHandler->SetPos( 100);
		MsgHandler->DestroyWindow();
	}
}
void CBlockMatchDirSub::GetSimMap_XY()
{
	MemPrepare();
	GetSamplePoints();
	int i,j,m;
	//边缘图
	BYTE* pRefImg= (BYTE*)m_pLargeImg->imageData;
	int refw= m_pLargeImg->widthStep;
	int refh= m_pLargeImg->height; 
	int refsize   = refw* refh;
	BYTE* pRealImg= (BYTE*)m_pSmallImg->imageData;
	int realw= m_pSmallImg->widthStep;
	int realh= m_pSmallImg->height;
	int realsize  = realw* realh;

	cvSmooth(m_pLargeImg,m_pLargeImg,CV_GAUSSIAN,5);
	cvSmooth(m_pSmallImg,m_pSmallImg,CV_GAUSSIAN,5);
	Image_Sobel_Direction_Fast( pRefImg, refw, refh, 1 );
	Image_Sobel_Direction_Fast( pRealImg, realw, realh, 1 );

	//制作采样点

	int nPoints= m_samplepoints.size();

	CvPoint corner;		
	m_pointMat= cv::Mat::zeros(3,nPoints,CV_32FC1);
	float *pFloat;

	int srx=1+m_pLargeImg->width-m_pSmallImg->width;
	int sry=1+m_pLargeImg->height-m_pSmallImg->height;

	bool isShowProgressDlg = false; 
	CProgressDlg * MsgHandler = new CProgressDlg;
	isShowProgressDlg = true;
	if(isShowProgressDlg)
	{
		MsgHandler->Create(NULL);
		MsgHandler->ShowWindow(SW_SHOW);
	}

	float sum;
	for (m=0;m<nPoints;m++)  // 对于每个采样点，匹配一次
	{
		if(isShowProgressDlg)
			MsgHandler->SetPos( m*100/nPoints );

		m_simMaps.push_back(cv::Mat::zeros(sry,srx,CV_32FC1));
		pFloat= (m_simMaps.at(m)).ptr<float>(0);
		corner= m_samplepoints.at(m);
		// 采样点矩阵，后面将对采样点变换，求变换后的点坐标
		m_pointMat.at<float>(0,m)= (float)(corner.x-m_smallRX);
		m_pointMat.at<float>(1,m)= (float)(corner.y-m_smallRY);
		m_pointMat.at<float>(2,m)= 1.0f;
		for (i=0;i<sry;i++)
		{
			for (j=0;j<srx;j++)
			{
				sum = (float) (GetPatchSim_XY_window(pRefImg+(i+corner.y-m_patchR)* refw+j+corner.x-m_patchR, refw,
					pRealImg+(corner.y-m_patchR)* realw+corner.x-m_patchR, realw));
				pFloat[i*srx+j]= (float)sum;
			}
		}
//#ifdef _MATCH_DEBUG
//		{
//			IplImage *pSimMap=cvCreateImage(cvSize(srx,sry),8,1);	
//			uchar* pUChar= (uchar*)pSimMap->imageData;
//			char imagename[30],imagepath[50];
//			double amax,amin;
//			amax=0, amin= 99999999;
//			for (i=0;i<srx*sry;i++)
//			{
//				if (amin>pFloat[i])
//				{
//					amin= pFloat[i];
//				}
//				if (amax<pFloat[i])
//				{
//					amax= pFloat[i];
//				}
//			}
//			for(i= 0; i< pSimMap->height; i++)
//			{
//				for(j= 0; j< pSimMap->width; j++)
//				{
//					pUChar[i*pSimMap->widthStep+j]=(255.f*(pFloat[i*srx+j]-amin)/(amax-amin));
//				}
//			}
//			sprintf(imagename,"ES_simPatch%d.bmp",m);
//			cvShowImage(imagename,pSimMap);		
//			//sprintf(imagepath,"d:\\data\\ES_simPatch%d.bmp",m);
//			//cvSaveImage(imagepath,pSimMap);	
//			cvReleaseImage( &pSimMap );
//		}
//#endif
	}
	if(isShowProgressDlg)
	{
		MsgHandler->SetPos( 100);
		MsgHandler->DestroyWindow();
	}
}
void CBlockMatchDirSub::GetSimMap_ORI_HIST()
{
	MemPrepare();
	GetSamplePoints();
	int i,j,m;
	//边缘图
	BYTE* pRefImg= (BYTE*)m_pLargeImg->imageData;
	int refw= m_pLargeImg->widthStep;
	int refh= m_pLargeImg->height; 
	int refsize   = refw* refh;
	BYTE* pRealImg= (BYTE*)m_pSmallImg->imageData;
	int realw= m_pSmallImg->widthStep;
	int realh= m_pSmallImg->height;
	int realsize  = realw* realh;

	cvSmooth(m_pLargeImg,m_pLargeImg,CV_GAUSSIAN,5);
	cvSmooth(m_pSmallImg,m_pSmallImg,CV_GAUSSIAN,5);
	Image_Sobel_Direction_Fast( pRefImg, refw, refh, 1 );
	Image_Sobel_Direction_Fast( pRealImg, realw, realh, 1 );

	//制作采样点

	int nPoints= m_samplepoints.size();

	CvPoint corner;		
	m_pointMat= cv::Mat::zeros(3,nPoints,CV_32FC1);
	float *pFloat;

	int dw=m_pLargeImg->width-m_pSmallImg->width;
	int dh=m_pLargeImg->height-m_pSmallImg->height;

	bool isShowProgressDlg = false; 
	CProgressDlg * MsgHandler = new CProgressDlg;
	isShowProgressDlg = true;
	if(isShowProgressDlg)
	{
		MsgHandler->Create(NULL);
		MsgHandler->ShowWindow(SW_SHOW);
	}

	float sum;
	for (m=0;m<nPoints;m++)  // 对于每个采样点，匹配一次
	{
		if(isShowProgressDlg)
			MsgHandler->SetPos( m*100/nPoints );

		m_simMaps.push_back(cv::Mat::zeros(dh+1,dw+1,CV_32FC1));
		pFloat= (m_simMaps.at(m)).ptr<float>(0);
		corner= m_samplepoints.at(m);
		// 采样点矩阵，后面将对采样点变换，求变换后的点坐标
		m_pointMat.at<float>(0,m)= (float)(corner.x-m_smallRX);
		m_pointMat.at<float>(1,m)= (float)(corner.y-m_smallRY);
		m_pointMat.at<float>(2,m)= 1.0f;
		for (i=0;i<=dh;i++)
		{
			for (j=0;j<=dw;j++)
			{
				sum = (float) (GetPatchSim_ORI_HIST(pRefImg+(i+corner.y-m_patchR)* refw+j+corner.x-m_patchR, refw,
					pRealImg+(corner.y-m_patchR)* realw+corner.x-m_patchR, realw));
				pFloat[i*(dw+1)+j]= (float)sum;
			}
		}
	}
	if(isShowProgressDlg)
	{
		MsgHandler->SetPos( 100);
		MsgHandler->DestroyWindow();
	}
}
void CBlockMatchDirSub::GetSimMap_GRAY_HIST()
{
	MemPrepare();
	GetSamplePoints();
	int i,j,m;
	//边缘图
	BYTE* pRefImg= (BYTE*)m_pLargeImg->imageData;
	int refw= m_pLargeImg->widthStep;
	int refh= m_pLargeImg->height; 
	int refsize   = refw* refh;
	BYTE* pRealImg= (BYTE*)m_pSmallImg->imageData;
	int realw= m_pSmallImg->widthStep;
	int realh= m_pSmallImg->height;
	int realsize  = realw* realh;

	cvSmooth(m_pLargeImg,m_pLargeImg,CV_GAUSSIAN,5);
	cvSmooth(m_pSmallImg,m_pSmallImg,CV_GAUSSIAN,5);

	//制作采样点

	int nPoints= m_samplepoints.size();

	CvPoint corner;		
	m_pointMat= cv::Mat::zeros(3,nPoints,CV_32FC1);
	float *pFloat;

	int dw=m_pLargeImg->width-m_pSmallImg->width;
	int dh=m_pLargeImg->height-m_pSmallImg->height;

	bool isShowProgressDlg = false; 
	CProgressDlg * MsgHandler = new CProgressDlg;
	isShowProgressDlg = true;
	if(isShowProgressDlg)
	{
		MsgHandler->Create(NULL);
		MsgHandler->ShowWindow(SW_SHOW);
	}

	float sum;
	for (m=0;m<nPoints;m++)  // 对于每个采样点，匹配一次
	{
		if(isShowProgressDlg)
			MsgHandler->SetPos( m*100/nPoints );

		m_simMaps.push_back(cv::Mat::zeros(dh+1,dw+1,CV_32FC1));
		pFloat= (m_simMaps.at(m)).ptr<float>(0);
		corner= m_samplepoints.at(m);
		// 采样点矩阵，后面将对采样点变换，求变换后的点坐标
		m_pointMat.at<float>(0,m)= (float)(corner.x-m_smallRX);
		m_pointMat.at<float>(1,m)= (float)(corner.y-m_smallRY);
		m_pointMat.at<float>(2,m)= 1.0f;
		for (i=0;i<=dh;i++)
		{
			for (j=0;j<=dw;j++)
			{
				sum = (float) (GetPatchSim_GRAY_HIST(pRefImg+(i+corner.y-m_patchR)* refw+j+corner.x-m_patchR, refw,
					pRealImg+(corner.y-m_patchR)* realw+corner.x-m_patchR, realw));
				pFloat[i*(dw+1)+j]= (float)sum;
			}
		}
	}
	if(isShowProgressDlg)
	{
		MsgHandler->SetPos( 100);
		MsgHandler->DestroyWindow();
	}
}
void CBlockMatchDirSub::SpaceSearchMatch_rot360()
{
	int m,x,y,x0=0,y0=0,x1,y1,cx,cy;
	int dw=m_pLargeImg->width-m_pSmallImg->width;
	int dh=m_pLargeImg->height-m_pSmallImg->height;
	float s=1.0,s0=1.0,s1,r=0,r0=0,r1,sim,maxsim=0;
	int nPoints= m_samplepoints.size();
	cv::Mat pointTransformedMat(3,nPoints,CV_32FC1);
	cv::Mat tmphMat=cv::Mat::eye(3,3,CV_32FC1);

	bool isShowProgressDlg = false; 
	CProgressDlg * MsgHandler = new CProgressDlg;
	isShowProgressDlg = true;
	if(isShowProgressDlg)
	{
		MsgHandler->Create(NULL);
		MsgHandler->ShowWindow(SW_SHOW);
	}
	int cnt=0;
	// debug 调试变量
	CvPoint2D32f p0[10],p1[10];
	for (r=-CV_PI;r<=-CV_PI;r+=0.05f)
	{
		for (s=1.0f;s<=1.0f;s+=0.05f)
		{
			//if(isShowProgressDlg)
			//	MsgHandler->SetPos( cnt*100/1633 );
			cnt++;

			tmphMat.at<float>(0,0)= s*cos(r);
			tmphMat.at<float>(1,1)= s*cos(r);
			tmphMat.at<float>(0,1)= -s*sin(r);
			tmphMat.at<float>(1,0)= s*sin(r);
			tmphMat.at<float>(0,2)= 0;
			tmphMat.at<float>(1,2)= 0;
			pointTransformedMat= tmphMat* m_pointMat;
			// debug begin
			//for (m=0;m<9;m++)
			//{
			//	p0[m].x= m_pointMat.at<float>(0,m);
			//	p0[m].y= m_pointMat.at<float>(1,m);
			//	p1[m].x= pointTransformedMat.at<float>(0,m);
			//	p1[m].y= pointTransformedMat.at<float>(1,m);
			//}
			//sim= 0;	
			// debug over
			for (y=0;y<=dh;y+=1)
			{
				for (x=0;x<=dw;x+=1)
				{					
					sim= 0;					
					for (m=0;m<nPoints;m++)
					{
						cx= (int)(pointTransformedMat.at<float>(0,m)-m_pointMat.at<float>(0,m)+x+0.5);
						cy= (int)(pointTransformedMat.at<float>(1,m)-m_pointMat.at<float>(1,m)+y+0.5);
						if (cx<0||cx>dw||cy<0||cy>dh)
						{
							continue;
						}
						sim= sim+ (m_simMaps.at(m)).at<float>(cy,cx);
					}
					if (sim> maxsim)
					{
						maxsim= sim;
						x0=x;
						y0=y;
						s0=s;
						r0=r;
					}
				}
			}
		}
	}
	if(isShowProgressDlg)
	{
		MsgHandler->SetPos( 100);
		MsgHandler->DestroyWindow();
	}
	x1=x0,y1=y0,s1=s0,r1=r0;
	float fx,fy,I11,I12,I21,I22;
	for (r=r0-0.04f;r<=r0+0.04f;r+=0.001f)
	{
		for (s=s0-0.03f;s<=s0+0.03f;s+=0.005f)
		{
			tmphMat.at<float>(0,0)= s*cos(r);
			tmphMat.at<float>(1,1)= s*cos(r);
			tmphMat.at<float>(0,1)= -s*sin(r);
			tmphMat.at<float>(1,0)= s*sin(r);
			tmphMat.at<float>(0,2)= 0;
			tmphMat.at<float>(1,2)= 0;
			pointTransformedMat= tmphMat * m_pointMat;
			for (y=y0-3;y<=y0+3;y++)
			{
				for (x=x0-3;x<=x0+3;x++)
				{					
					sim= 0;
					for (m=0;m<nPoints;m++)
					{
						//亚像素，插值
						fx= (pointTransformedMat.at<float>(0,m)-m_pointMat.at<float>(0,m)+x);
						fy= (pointTransformedMat.at<float>(1,m)-m_pointMat.at<float>(1,m)+y);
						if (fx<0||fx>dw||fy<0||fy>dh)
						{
							continue;
						}
						cx= (int)fx;
						cy= (int)fy;
						fx= fx-cx;
						fy= fy-cy;						
						//simMap 间隔2 采样
						//						sim+= (m_simMaps.at(m)).at<float>(cy,cx);
						I11 = (m_simMaps.at(m)).at<float>(cy,cx);
						I12 = (m_simMaps.at(m)).at<float>(cy,cx+1);
						I21 = (m_simMaps.at(m)).at<float>(cy+1,cx);
						I22 = (m_simMaps.at(m)).at<float>(cy+1,cx+1);
						sim+= ( (I11 + (I12-I11)* fx)* (1.0f - fy) + (I21 + (I22-I21)* fx)* fy );
					}
					if (sim> maxsim)
					{
						maxsim= sim;
						x1=x;
						y1=y;
						s1=s;
						r1=r;
					}
				}
			}
		}
	}
	FILE *fp = fopen("d:\\feature\\BMDS.txt", "a" );
	fprintf(fp,"rot360, 粗匹配 x0:%3d, y0:%3d, s0:%4.3f, r0:%4.3f, cnt:%6d\n",x0,y0,s0,r0,cnt);
	fprintf(fp,"rot360, 精匹配 x1:%3d, y1:%3d, s1:%4.3f, r1:%4.3f, max:%6.1f\n\n",x1,y1,s1,r1,maxsim);
	fclose(fp);
	m_hMat.at<float>(0,0)= s1*cos(r1);
	m_hMat.at<float>(1,1)= s1*cos(r1);
	m_hMat.at<float>(0,1)= -s1*sin(r1);
	m_hMat.at<float>(1,0)= s1*sin(r1);
	m_hMat.at<float>(0,2)= x1+m_smallRX;
	m_hMat.at<float>(1,2)= y1+m_smallRY;
}

void CBlockMatchDirSub::SpaceSearchMatch()
{
	int m,x,y,x0=0,y0=0,x1,y1,cx,cy;
	int dw=m_pLargeImg->width-m_pSmallImg->width;
	int dh=m_pLargeImg->height-m_pSmallImg->height;
	float s=1.0,s0=1.0,s1,r=0,r0=0,r1,sim,maxsim=0;
	int nPoints= m_samplepoints.size();
	cv::Mat pointTransformedMat(3,nPoints,CV_32FC1);
	cv::Mat tmphMat=cv::Mat::eye(3,3,CV_32FC1);
	
	bool isShowProgressDlg = false; 
	CProgressDlg * MsgHandler = new CProgressDlg;
	isShowProgressDlg = true;
	if(isShowProgressDlg)
	{
		MsgHandler->Create(NULL);
		MsgHandler->ShowWindow(SW_SHOW);
	}
	int cnt=0;
	for (r=-0.20f;r<=0.20f;r+=0.05f)
	{
		for (s=0.7f;s<=1.3f;s+=0.05f)
		{
			if(isShowProgressDlg)
				MsgHandler->SetPos( cnt*100/117 );
			cnt++;

			tmphMat.at<float>(0,0)= s*cos(r);
			tmphMat.at<float>(1,1)= s*cos(r);
			tmphMat.at<float>(0,1)= -s*sin(r);
			tmphMat.at<float>(1,0)= s*sin(r);
			tmphMat.at<float>(0,2)= 0;
			tmphMat.at<float>(1,2)= 0;
			pointTransformedMat= tmphMat* m_pointMat;
			for (y=0;y<=dh;y+=2)
			{
				for (x=0;x<=dw;x+=2)
				{					
					sim= 0;
					for (m=0;m<nPoints;m++)
					{
						cx= (int)(pointTransformedMat.at<float>(0,m)-m_pointMat.at<float>(0,m)+x+0.5);
						cy= (int)(pointTransformedMat.at<float>(1,m)-m_pointMat.at<float>(1,m)+y+0.5);
						if (cx<0||cx>dw||cy<0||cy>dh)
						{
							continue;
						}
						sim= sim+ (m_simMaps.at(m)).at<float>(cy,cx);
					}
					if (sim> maxsim)
					{
						maxsim= sim;
						x0=x;
						y0=y;
						s0=s;
						r0=r;
					}
				}
			}
		}
	}
	if(isShowProgressDlg)
	{
		MsgHandler->SetPos( 100);
		MsgHandler->DestroyWindow();
	}
	x1=x0,y1=y0,s1=s0,r1=r0;
	float fx,fy,I11,I12,I21,I22;
	for (r=r0-0.04f;r<=r0+0.04f;r+=0.001f)
	{
		for (s=s0-0.03f;s<=s0+0.03f;s+=0.005f)
		{
			tmphMat.at<float>(0,0)= s*cos(r);
			tmphMat.at<float>(1,1)= s*cos(r);
			tmphMat.at<float>(0,1)= -s*sin(r);
			tmphMat.at<float>(1,0)= s*sin(r);
			tmphMat.at<float>(0,2)= 0;
			tmphMat.at<float>(1,2)= 0;
			pointTransformedMat= tmphMat * m_pointMat;
			for (y=y0-3;y<=y0+3;y++)
			{
				for (x=x0-3;x<=x0+3;x++)
				{					
					sim= 0;
					for (m=0;m<nPoints;m++)
					{
						//亚像素，插值
						fx= (pointTransformedMat.at<float>(0,m)-m_pointMat.at<float>(0,m)+x);
						fy= (pointTransformedMat.at<float>(1,m)-m_pointMat.at<float>(1,m)+y);
						if (fx<0||fx>dw||fy<0||fy>dh)
						{
							continue;
						}
						cx= (int)fx;
						cy= (int)fy;
						fx= fx-cx;
						fy= fy-cy;						
						//simMap 间隔2 采样
//						sim+= (m_simMaps.at(m)).at<float>(cy,cx);
						I11 = (m_simMaps.at(m)).at<float>(cy,cx);
						I12 = (m_simMaps.at(m)).at<float>(cy,cx+1);
						I21 = (m_simMaps.at(m)).at<float>(cy+1,cx);
						I22 = (m_simMaps.at(m)).at<float>(cy+1,cx+1);
						sim+= ( (I11 + (I12-I11)* fx)* (1.0f - fy) + (I21 + (I22-I21)* fx)* fy );
					}
					if (sim> maxsim)
					{
						maxsim= sim;
						x1=x;
						y1=y;
						s1=s;
						r1=r;
					}
				}
			}
		}
	}
	//fp = fopen("d:\\feature\\BMDS.txt", "a" );
	//fprintf(fp,"精匹配 x1:%3d, y1:%3d, s1:%4.3f, r1:%4.3f, max:%6.1f\n\n",x1,y1,s1,r1,maxsim);
	//fclose(fp);
	m_hMat.at<float>(0,0)= s1*cos(r1);
	m_hMat.at<float>(1,1)= s1*cos(r1);
	m_hMat.at<float>(0,1)= -s1*sin(r1);
	m_hMat.at<float>(1,0)= s1*sin(r1);
	m_hMat.at<float>(0,2)= x1+m_smallRX;
	m_hMat.at<float>(1,2)= y1+m_smallRY;
}

void CBlockMatchDirSub::SpaceSearchMatch_6d()
{
	int i,m,x,y,x0=0,y0=0,x1,y1,cx,cy;
	int dw=m_pLargeImg->width-m_pSmallImg->width;
	int dh=m_pLargeImg->height-m_pSmallImg->height;
	//   a0,a1
	//   a2,a3
	float s=1.0,s0=1.0,r=0,r0=0,r1,sim,maxsim=0;
	int nPoints= m_samplepoints.size();
	cv::Mat pointTransformedMat(3,nPoints,CV_32FC1);
	cv::Mat tmphMat=cv::Mat::eye(3,3,CV_32FC1);
	m_hMat=cv::Mat::eye(3,3,CV_32FC1);

	bool isShowProgressDlg = false; 
	CProgressDlg * MsgHandler = new CProgressDlg;
	isShowProgressDlg = true;
	if(isShowProgressDlg)
	{
		MsgHandler->Create(NULL);
		MsgHandler->ShowWindow(SW_SHOW);
	}
	int cnt=0;
	for (r=-0.20;r<=0.20;r+=0.05)
	{
		for (s=0.7;s<=1.3;s+=0.05)
		{
			if(isShowProgressDlg)
				MsgHandler->SetPos( cnt*100/117 );
			cnt++;
			tmphMat.at<float>(0,0)= s*cos(r);
			tmphMat.at<float>(1,1)= s*cos(r);
			tmphMat.at<float>(0,1)= -s*sin(r);
			tmphMat.at<float>(1,0)= s*sin(r);
			tmphMat.at<float>(0,2)= 0;
			tmphMat.at<float>(1,2)= 0;
			pointTransformedMat= tmphMat* m_pointMat;
			for (y=0;y<=dh;y+=2)
			{
				for (x=0;x<=dw;x+=2)
				{					
					//tmphMat.at<float>(0,2)= x;
					//tmphMat.at<float>(1,2)= y;
					//pointTransformedMat= tmphMat* m_pointMat;
					sim= 0;
					for (m=0;m<nPoints;m++)
					{
						cx= (int)(pointTransformedMat.at<float>(0,m)-m_pointMat.at<float>(0,m)+x+0.5);
						cy= (int)(pointTransformedMat.at<float>(1,m)-m_pointMat.at<float>(1,m)+y+0.5);
						if (cx<0||cx>dw||cy<0||cy>dh)
						{
							continue;
						}
						//simMap 间隔2 采样
						sim= sim+ (m_simMaps.at(m)).at<float>(cy,cx);
					}
					if (sim> maxsim)
					{
						maxsim= sim;
						x0=x;
						y0=y;
						s0=s;
						r0=r;
					}
				}
			}
		}
	}
	if(isShowProgressDlg)
	{
		MsgHandler->SetPos( 100);
		MsgHandler->DestroyWindow();
	}
	float a0,a1,a2,a3,a0_0,a1_0,a2_0,a3_0,a0_1,a1_1,a2_1,a3_1;
	a0_0= s0*cos(r0); 
	a3_0= a0_0;
	a1_0= -s0*sin(r0);
	a2_0= -a1_0;
	a0_1= a0_0; a1_1= a1_0; a2_1= a2_0; a3_1= a3_0;
	for (a0=a0_0-0.1;a0<=a0_0+0.1;a0+=0.05)
	{
		for (a3=a3_0-0.1;a3<=a3_0+0.1;a3+=0.05)
		{
			for (a1=a1_0-0.1;a1<=a1_0+0.1;a1+=0.05)
			{
				for (a2=a2_0-0.1;a2<=a2_0+0.1;a2+=0.05)
				{
					tmphMat.at<float>(0,0)= a0;
					tmphMat.at<float>(1,1)= a3;
					tmphMat.at<float>(0,1)= a1;
					tmphMat.at<float>(1,0)= a2;
					tmphMat.at<float>(0,2)= 0;
					tmphMat.at<float>(1,2)= 0;
					pointTransformedMat= tmphMat * m_pointMat;
					for (y=y0-3;y<=y0+3;y++)
					{
						for (x=x0-3;x<=x0+3;x++)
						{					
							sim= 0;
							for (m=0;m<nPoints;m++)
							{
								cx= (int)(pointTransformedMat.at<float>(0,m)-m_pointMat.at<float>(0,m)+x+0.5);
								cy= (int)(pointTransformedMat.at<float>(1,m)-m_pointMat.at<float>(1,m)+y+0.5);
								if (cx<0||cx>dw||cy<0||cy>dh)
								{
									continue;
								}
								sim+= (m_simMaps.at(m)).at<float>(cy,cx);
							}
							if (sim> maxsim)
							{
								maxsim= sim;
								x1=x;
								y1=y;
								a0_1= a0;
								a1_1= a1;
								a2_1= a2;
								a3_1= a3;
							}
						}
					}
				}
			}
		}
	}
	//fp = fopen("f:\\feature\\BMDS.txt", "a" );
	//fprintf(fp,"精匹配 x1:%3d, y1:%3d, s1:%4.3f, r1:%4.3f, max:%6.1f\n\n",x1,y1,s1,r1,maxsim);
	//fclose(fp);
	m_hMat.at<float>(0,0)= a0_1;
	m_hMat.at<float>(1,1)= a3_1;
	m_hMat.at<float>(0,1)= a1_1;
	m_hMat.at<float>(1,0)= a2_1;
	m_hMat.at<float>(0,2)= x1+m_smallRX;
	m_hMat.at<float>(1,2)= y1+m_smallRY;
}

void CBlockMatchDirSub::SimpleMatch2D()
{
	// 加匹配代码
	CvPoint pointInLarge = cvPoint(-1, -1);
	CvPoint smTarget;
	smTarget= cvPoint(m_pSmallImg->width/2, m_pSmallImg->height/2);
	CTimeCount tt;
	tt.Start();
	lz_Multi_Sensor_MatchTemplate(m_pLargeImg, m_pSmallImg, &pointInLarge,2);
	lz_Multi_Sensor_MatchTemplate(m_pLargeImg, m_pSmallImg, &pointInLarge,1,cvPoint(0,0),
		cvRect(pointInLarge.x-2,pointInLarge.y-2,5,5));
	m_hMat.at<float>(0,0)= 1;
	m_hMat.at<float>(1,1)= 1;
	m_hMat.at<float>(0,1)= 0;
	m_hMat.at<float>(1,0)= 0;
	m_hMat.at<float>(0,2)= pointInLarge.x+m_smallRX;
	m_hMat.at<float>(1,2)= pointInLarge.y+m_smallRY;
}
int totalSim_ORI_HIST(BYTE *pRef, int refwstep, BYTE *pReal, int realwstep, int realw, int realh)
{
	int i,j,dref,dreal,sim=0;
	int h1[16], h2[16], tmpsim=0;
	memset(h1,0,16*sizeof(int));
	memset(h2,0,16*sizeof(int));
	for (i=0;i<realh;i++)
	{
		for (j=0;j<realw;j++)
		{
			dref=*(pRef+i*refwstep+j);
			dreal=*(pReal+i*realwstep+j);
			if(dref>=74)
				h1[(dref-74)/15]++;
			if(dreal>=74)
				h2[(dreal-74)/15]++;
		}
	}
	for (i=0;i<12;i++)
	{
		tmpsim=0;
		for (j=0;j<12;j++)
		{
			tmpsim+= min(h1[j],h2[(j+i)%12]);
		}
		if (tmpsim>sim)
		{
			sim=tmpsim;
		}
	}
	return sim;
}
void CBlockMatchDirSub::SimpleMatch2D_HIST()
{
	// 加匹配代码
	CvPoint pointInLarge = cvPoint(-1, -1);
	CTimeCount tt;
	tt.Start();
	BYTE* pRefImg= (BYTE*)m_pLargeImg->imageData;
	int refw= m_pLargeImg->widthStep;
	int refh= m_pLargeImg->height; 
	int refsize   = refw* refh;
	BYTE* pRealImg= (BYTE*)m_pSmallImg->imageData;
	int realw= m_pSmallImg->widthStep;
	int realh= m_pSmallImg->height;
	int realsize  = realw* realh;
	int i,j,dh,dw;
	int sim=0,maxsim=0,minsim= 999999999;
	dh= m_pLargeImg->height-m_pSmallImg->height;
	dw= m_pLargeImg->width-m_pSmallImg->width;
	IplImage *pRstMap= cvCreateImage(cvSize(dw+1,dh+1),8,1);
	int *pRst= new int[(dw+1)*(dh+1)];
	cvSmooth(m_pLargeImg,m_pLargeImg,CV_GAUSSIAN,5);
	cvSmooth(m_pSmallImg,m_pSmallImg,CV_GAUSSIAN,5);
	Image_Sobel_Direction_Fast( pRefImg, refw, refh, 1 );
	Image_Sobel_Direction_Fast( pRealImg, realw, realh, 1 );
	for (i=0;i<=dh;i++)
	{
		for (j=0;j<=dw;j++)
		{			
			sim = totalSim_ORI_HIST(pRefImg+i* refw+j, refw,pRealImg, realw, m_pSmallImg->width, realh);	
			if(sim>maxsim)
			{
				maxsim=sim;
				pointInLarge.x= j;
				pointInLarge.y= i;
			}
			if (sim<minsim) minsim= sim;
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
	cvReleaseImage(&pRstMap);
	delete []pRst;
	//lz_Multi_Sensor_MatchTemplate(m_pLargeImg, m_pSmallImg, &pointInLarge,2);
	//lz_Multi_Sensor_MatchTemplate(m_pLargeImg, m_pSmallImg, &pointInLarge,1,cvPoint(0,0),
	//	cvRect(pointInLarge.x-2,pointInLarge.y-2,5,5));
	m_hMat.at<float>(0,0)= 1;
	m_hMat.at<float>(1,1)= 1;
	m_hMat.at<float>(0,1)= 0;
	m_hMat.at<float>(1,0)= 0;
	m_hMat.at<float>(0,2)= pointInLarge.x+m_smallRX;
	m_hMat.at<float>(1,2)= pointInLarge.y+m_smallRY;
}
void CBlockMatchDirSub::ManualPointMatch(CvPoint2D32f* plargepoint, CvPoint2D32f* psmallpoint)
{
	CvMat *AMat;	
	AMat= cvCreateMat(2,3,CV_32FC1);
	CvPoint2D32f *psmallpointShift= new CvPoint2D32f[3];
	for (int i=0;i<3;i++)
	{
		psmallpointShift[i].x= psmallpoint[i].x-m_smallRX;
		psmallpointShift[i].y= psmallpoint[i].y-m_smallRY;
	}
	cvGetAffineTransform(psmallpointShift,plargepoint,AMat);

	m_hMat.at<float>(0,0)= cvmGet(AMat,0,0);
	m_hMat.at<float>(1,1)= cvmGet(AMat,1,1);
	m_hMat.at<float>(0,1)= cvmGet(AMat,0,1);
	m_hMat.at<float>(1,0)= cvmGet(AMat,1,0);
	m_hMat.at<float>(0,2)= cvmGet(AMat,0,2);
	m_hMat.at<float>(1,2)= cvmGet(AMat,1,2);

	cvReleaseMat(&AMat);
}
void CBlockMatchDirSub::GetTransformedPoint(CvPoint2D32f OldPoint, CvPoint2D32f* pNewPoint)
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
void CBlockMatchDirSub::saveHmat(CString filename)
{
	FILE *fp = fopen(filename, "w" );
	for (int i=0;i<3;i++)
	{
		for (int j=0;j<3;j++)
		{
			fprintf(fp,"%f\n", m_hMat.at<float>(i,j));
		}
	}
	fclose(fp);
}
void CBlockMatchDirSub::loadHmat(CString filename)
{
	CStdioFile File;
	if(!File.Open(filename,CFile::modeRead))
	{
		AfxMessageBox("load hMat failed!");
		return;
	}
	CString FileData;
	char TempStr0[50];
	for (int i=0;i<3;i++)
	{
		for (int j=0;j<3;j++)
		{
			if(FALSE== File.ReadString(FileData) || ""==FileData)
			{
				AfxMessageBox("load hMat failed!");
				File.Close();
				return;
			}
			//sscanf_s(FileData,"%s",TempStr0, _countof(TempStr0));
			//m_hMat.at<float>(i,j)=atof(TempStr0);
			sscanf_s(FileData,"%f",&(m_hMat.at<float>(i,j)));
		}
	}
	File.Close();
}
void CBlockMatchDirSub::LocateTargetForSmallRef(CvPoint2D32f *pointLarge,CvPoint2D32f *pointSmall)
{
	// 变换计算目标点接近边界，不宜进行精匹配
	if (pointLarge->x<30 || pointLarge->x> m_pOriginLargeImg->width-31)
	{
		return;
	}
	if (pointLarge->y<30 || pointLarge->y> m_pOriginLargeImg->height-31)
	{
		return;
	}
	cv::Mat matLarge=cv::Mat(m_pOriginLargeImg).clone(), matSmall=cv::Mat(m_pOriginSmallImg).clone();
	cv::Mat matWarp;
	cv::Mat matT= (Mat_<float>(3,3) << 0, 0, m_hMat.at<float>(0,0)*m_smallRX+m_hMat.at<float>(0,1)*m_smallRY,
		0, 0, m_hMat.at<float>(1,0)*m_smallRX+m_hMat.at<float>(1,1)*m_smallRY, 0, 0, 0);
//	cv::blur(matLarge,matLarge,cv::Size(3,3));
//	cv::blur(matSmall,matSmall,cv::Size(3,3));
	// 原图亮度+1，以区别于无效区域
	cv::warpPerspective(matSmall+1,matWarp,m_hMat-matT,cv::Size(matLarge.cols,matLarge.rows));
	cv::Mat matFlag;
	cv::threshold(matWarp,matFlag,0,255,THRESH_BINARY);
	cv::erode(matFlag,matFlag,cv::Mat::ones(5,5,CV_8U));
	cv::Mat matLarge0= matLarge.clone();
	cv::Mat matWarp0= matWarp.clone();
	Image_Sobel_Direction_Fast( matLarge.data, matLarge.step, matLarge.rows, 1 );
	Image_Sobel_Direction_Fast( matWarp.data, matWarp.step, matWarp.rows, 1 );
	matWarp= matWarp.mul(matFlag/255);	
	int patchR= (int)(min(matSmall.cols,matSmall.rows)*0.33);
	int tx,ty;
	// 应该从小图中心点×hMat计算变换后坐标点
	cv::Mat newPoint(3,1,CV_32FC1);
	cv::Mat oldPoint(3,1,CV_32FC1);
	oldPoint.at<float>(0,0)= pointSmall->x;
	oldPoint.at<float>(1,0)= pointSmall->y;
	oldPoint.at<float>(2,0)= 1.0f;
	newPoint= (m_hMat-matT)* oldPoint;
	tx= (int)(newPoint.at<float>(0,0)+0.5);
	ty= (int)(newPoint.at<float>(1,0)+0.5);
	// 在矩阵变换无误情况，该点等同于大图中预测目标点，故这里用大图预测目标点
//	tx= (int)(pointLarge->x+0.5),    ty= (int)(pointLarge->y+0.5);
	cv::Mat matTmplt= matWarp(cv::Range(ty-patchR,ty+patchR), cv::Range(tx-patchR, tx+patchR)).clone();
	int srchR= 40;
	int i,j;
	int sum, maxsum, minsum;

	minsum = 99999999;
	maxsum = 0;
	int io,jo;
	int wstart,wend, hstart,hend;
	wstart= std::max(0,tx-patchR-srchR);
	wend= std::min(tx+patchR+srchR,m_pOriginLargeImg->width-1)-(2*patchR+1);
	hstart= std::max(0,ty-patchR-srchR);
	hend= std::min(ty+patchR+srchR,m_pOriginLargeImg->height-1)-(2*patchR+1);
	if (NULL==m_pAngleLUT)
	{
		m_pAngleLUT = SetAnlgeLUT();
	}
	//cv::rectangle(matWarp0,Rect(wstart+patchR,hstart+patchR,wend-wstart+1,hend-hstart+1),Scalar(255,255,0));
	cv::rectangle(matLarge0,Rect(wstart+patchR,hstart+patchR,wend-wstart+1,hend-hstart+1),Scalar(255,255,0));
	//cv::line(matWarp0,Point(tx-10,ty),Point(tx+10,ty),Scalar(255,255,0));
	//cv::line(matWarp0,Point(tx,ty-10),Point(tx,ty+10),Scalar(255,255,0));
	cv::line(matLarge0,Point(tx-10,ty),Point(tx+10,ty),Scalar(255,255,0));
	cv::line(matLarge0,Point(tx,ty-10),Point(tx,ty+10),Scalar(255,255,0));
	cv::Mat matTmplt0= matWarp0(cv::Range(ty-patchR,ty+patchR), cv::Range(tx-patchR, tx+patchR)).clone();
	imshow("warp",matWarp0);
	imshow("tmplt",matTmplt);
	imshow("large",matLarge0);
	imwrite("~warp.jpg",matWarp0);
	imwrite("~tmplt.jpg",matTmplt0);
	imwrite("~large.jpg",matLarge0);
	waitKey(0);
	for(i= hstart; i<= hend; i++)
	{
		for(j= wstart; j<= wend; j++)
		{
			sum = 0;
			sum = (float) (GetPatchSim_XY_window(matLarge.data+i* matLarge.cols+j, matLarge.cols,
				matTmplt.data, matTmplt.cols));
			if( sum < minsum )
				minsum = sum;
			if( sum > maxsum )
			{
				maxsum = sum;
				io = i;
				jo = j;
			}
		}
	}
	pointLarge->x= jo+patchR;
	pointLarge->y= io+patchR;

	m_pointInSmall= *pointSmall;
	m_pointInLarge= *pointLarge;
}
void CBlockMatchDirSub::LocateTargetForLargeRef(CvPoint2D32f *pointLarge,CvPoint2D32f *pointSmall)
{
	// 变换计算目标点接近边界，不宜进行精匹配
	if (pointSmall->x<20 || pointSmall->x> m_pOriginSmallImg->width-21)
	{
		return;
	}
	if (pointSmall->y<20 || pointSmall->y> m_pOriginSmallImg->height-21)
	{
		return;
	}

	int xMargin= min(pointSmall->x, m_pOriginSmallImg->width-pointSmall->x+1);
	int yMargin= min(pointSmall->y, m_pOriginSmallImg->height-pointSmall->y+1);

	cv::Mat matLarge=cv::Mat(m_pOriginLargeImg).clone(), matSmall=cv::Mat(m_pOriginSmallImg).clone();
	cv::Mat matWarp;
	// matT 将小图中心原点的H矩阵 转换到 小图左上角原点的H矩阵
	cv::Mat matT= (Mat_<float>(3,3) << 0, 0, m_hMat.at<float>(0,0)*m_smallRX+m_hMat.at<float>(0,1)*m_smallRY,
		0, 0, m_hMat.at<float>(1,0)*m_smallRX+m_hMat.at<float>(1,1)*m_smallRY, 0, 0, 0);
	//	cv::blur(matLarge,matLarge,cv::Size(3,3));
	//	cv::blur(matSmall,matSmall,cv::Size(3,3));
	// 原图亮度+1，以区别于无效区域
	cv::warpPerspective(matLarge,matWarp,(m_hMat-matT).inv(),cv::Size(matLarge.cols,matLarge.rows));
	cv::Mat matSmall0= matSmall.clone();
	cv::Mat matWarp0= matWarp.clone();
	Image_Sobel_Direction_Fast( matSmall.data, matSmall.step, matSmall.rows, 1 );
	Image_Sobel_Direction_Fast( matWarp.data, matWarp.step, matWarp.rows, 1 );
	int patchR= (int)(min(xMargin,yMargin));
	int tx,ty;
	// 应该从大图中心点×hMat计算变换后坐标点
	cv::Mat newPoint(3,1,CV_32FC1);
	cv::Mat oldPoint(3,1,CV_32FC1);
	oldPoint.at<float>(0,0)= pointLarge->x;
	oldPoint.at<float>(1,0)= pointLarge->y;
	oldPoint.at<float>(2,0)= 1.0f;
	newPoint= (m_hMat-matT).inv()* oldPoint;
	tx= (int)(newPoint.at<float>(0,0)+0.5);
	ty= (int)(newPoint.at<float>(1,0)+0.5);
	// 在矩阵变换无误情况，该点等同于小图中预测目标点，故这里用小图预测目标点
//	tx= (int)(pointSmall->x+0.5),    ty= (int)(pointSmall->y+0.5);
	cv::Mat matTmplt= matWarp(cv::Range(ty-patchR,ty+patchR), cv::Range(tx-patchR, tx+patchR)).clone();
	int srchR= 40;
	int i,j;
	int sum, maxsum, minsum;

	minsum = 99999999;
	maxsum = 0;
	int io,jo;
	int wstart,wend, hstart,hend;
	wstart= std::max(0,tx-patchR-srchR);
	wend= std::min(tx+patchR+srchR,m_pOriginLargeImg->width-1)-(2*patchR+1);
	hstart= std::max(0,ty-patchR-srchR);
	hend= std::min(ty+patchR+srchR,m_pOriginLargeImg->height-1)-(2*patchR+1);
	if (NULL==m_pAngleLUT)
	{
		m_pAngleLUT = SetAnlgeLUT();
	}
//	cv::rectangle(matWarp0,Rect(wstart+patchR,hstart+patchR,wend-wstart+1,hend-hstart+1),Scalar(255,255,0));
	cv::rectangle(matSmall0,Rect(wstart+patchR,hstart+patchR,wend-wstart+1,hend-hstart+1),Scalar(255,255,0));
//	cv::line(matWarp0,Point(tx-10,ty),Point(tx+10,ty),Scalar(255,255,0));
//	cv::line(matWarp0,Point(tx,ty-10),Point(tx,ty+10),Scalar(255,255,0));
	cv::line(matSmall0,Point(tx-10,ty),Point(tx+10,ty),Scalar(255,255,0));
	cv::line(matSmall0,Point(tx,ty-10),Point(tx,ty+10),Scalar(255,255,0));
	cv::Mat matTmplt0= matWarp0(cv::Range(ty-patchR,ty+patchR), cv::Range(tx-patchR, tx+patchR)).clone();
	imshow("warp",matWarp0);
	imshow("tmplt",matTmplt);
	imshow("small",matSmall0);
	imwrite("~warp.jpg",matWarp0);
	imwrite("~tmplt.jpg",matTmplt0);
	imwrite("~small.jpg",matSmall0);
	waitKey(0);
	for(i= hstart; i<= hend; i++)
	{
		for(j= wstart; j<= wend; j++)
		{
			sum = 0;
			sum = (float) (GetPatchSim_XY_window(matSmall.data+i* matSmall.cols+j, matSmall.cols,
				matTmplt.data, matTmplt.cols));
			if( sum < minsum )
				minsum = sum;
			if( sum > maxsum )
			{
				maxsum = sum;
				io = i;
				jo = j;
			}
		}
	}
	pointSmall->x= jo+patchR;
	pointSmall->y= io+patchR;

	m_pointInSmall= *pointSmall;
	m_pointInLarge= *pointLarge;
}
void CBlockMatchDirSub::GetMatchResult(int targetMode,CvPoint2D32f *pointLarge,CvPoint2D32f *pointSmall)
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
void CBlockMatchDirSub::ShowMatch(CString filepath,CString fname)
{
	//  灰度图替换边缘图，用于匹配，显示
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
		str="d:\\feature\\"+fname;
	}
	else
	{
		str=filepath+fname;
	}
	IplImage* pMergeImage= Merge2Images(m_pLargeImg,m_pSmallImg);
	cvShowImage("图像变形",pMergeImage);
	cvSaveImage(str,pMergeImage);
	cvReleaseImage(&pMergeImage);
}
