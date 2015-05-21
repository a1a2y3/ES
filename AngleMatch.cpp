#include "StdAfx.h"
#include "PatchMatch.h"
#include "ImageProcess.h"
#include "MatchFunc.h"
CPatchMatch::CPatchMatch(void)
{	
	m_patchR      = 15;
	m_patchW      = 2* m_patchR+ 1;
	m_StartPoint.x= -1;
	m_StartPoint.y= -1;
	m_smallRX= -1;
	m_smallRY= -1;
	m_pAngleLUT   = NULL;
	m_pLargeImg   = NULL;
	m_pSmallImg   = NULL;
	m_pPatchMask  = NULL;
}

CPatchMatch::~CPatchMatch(void)
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
}
void CPatchMatch::InputImage(IplImage *pLargeImg, IplImage *pSmallImg)
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
	m_smallRX= (pSmallImg->width-1)/2.0;
	m_smallRY= (pSmallImg->height-1)/2.0;
}
void CPatchMatch::SetPara(CvPoint point, int pr)
{
	m_StartPoint.x= point.x;
	m_StartPoint.y= point.y;
	m_patchR= pr;
	m_patchW= 2*m_patchR+1;
}
void CPatchMatch::MemPrepare()
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
void CPatchMatch::ReleaseVector()
{
	m_samplepoints.clear();
	m_simMaps.clear();
}

int CPatchMatch::GetPatchSim(BYTE *pRef, BYTE *pReal)
{
	int i, dref, dreal,imagesize= m_patchW*m_patchW;
	int sum = 0;
	for( i=0; i<imagesize; i++ )
	{
		dref = pRef[i];
		dreal = pReal[i];
		sum = sum+ m_pAngleLUT[dreal][dref]*m_pPatchMask[i];
	}
	return sum;
}
//用于跟踪，区域灰度0为无效点，不参与计算
int CPatchMatch::GetPatchSim_subtract_intensity(BYTE *pRef, BYTE *pReal)
{
	int i, dref, dreal,imagesize= m_patchW*m_patchW;
	int sum = 0;
	for( i=0; i<imagesize; i++ )
	{
		dref = pRef[i];
		dreal = pReal[i];
		//sum= sum+ abs(dreal-dref)*m_pPatchMask[i];
		sum = dreal>0? sum+ abs(dreal-dref)*m_pPatchMask[i]: sum;
	}
	return 255*imagesize-sum;
}

void CPatchMatch::GetSamplePoints()
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

void CPatchMatch::GetSimMap(int ntype)
{
	int i,j,m,n,idx;
	BYTE *pRef = new BYTE[m_patchW*m_patchW];
	BYTE *pReal = new BYTE[m_patchW*m_patchW];
	//边缘图
	BYTE* pRefImg= (BYTE*)m_pLargeImg->imageData;
	int refw= m_pLargeImg->widthStep;
	int refh= m_pLargeImg->height; 
	int refsize   = refw* refh;
	BYTE* pRealImg= (BYTE*)m_pSmallImg->imageData;
	int realw= m_pSmallImg->widthStep;
	int realh= m_pSmallImg->height;
	int realsize  = realw* realh;
	if (ntype==1)
	{
		cvSmooth(m_pLargeImg,m_pLargeImg,CV_GAUSSIAN,5);
		cvSmooth(m_pSmallImg,m_pSmallImg,CV_GAUSSIAN,5);
		Image_Sobel_Direction_Fast( pRefImg, refw, refh, 1 );
		Image_Sobel_Direction_Fast( pRealImg, realw, realh, 1 );
	}
	//制作采样点

	int nPoints= m_samplepoints.size();

	//  小图边缘图 补边	
	//IplImage *pEsmallimg= cvCreateImage(cvSize(m_pSmallImg->width+2*m_patchR,realh+2*m_patchR),
	//	m_pSmallImg->depth,m_pSmallImg->nChannels);
	//cvFillImage(pEsmallimg,0);
	//cvSetImageROI(pEsmallimg,cvRect(m_patchR,m_patchR,m_pSmallImg->width,realh));
	//cvCopy(m_pSmallImg,pEsmallimg);
	//cvResetImageROI(pEsmallimg);
	//realw= pEsmallimg->widthStep;
	//pRealImg= (BYTE*)pEsmallimg->imageData;

	CvPoint corner;	
	cv::Mat *pMat;
	m_pointMat= cv::Mat::zeros(3,nPoints,CV_32FC1);
	float *pFloat;

	int dw=m_pLargeImg->width-m_pSmallImg->width;
	int dh=m_pLargeImg->height-m_pSmallImg->height;

	float sum;
	for (m=0;m<nPoints;m++)  // 对于每个采样点，匹配一次
	{
		m_simMaps.push_back(cv::Mat::zeros(dh+1,dw+1,CV_32FC1));
		pFloat= (m_simMaps.at(m)).ptr<float>(0);
		corner= m_samplepoints.at(m);
		// 采样点矩阵，后面将对采样点变换，求变换后的点坐标
		m_pointMat.at<float>(0,m)= (float)(corner.x-m_smallRX);
		m_pointMat.at<float>(1,m)= (float)(corner.y-m_smallRY);
		m_pointMat.at<float>(2,m)= 1.0f;
		for( n=0; n<m_patchW; n++ )
		{
			// +pr 是因为补边宽度pr， -pr是由中心移到左上角
//			idx = (corner.y+m_patchR-m_patchR+n)* realw+ corner.x+m_patchR-m_patchR;
			// -pr是由中心移到左上角
			idx = (corner.y-m_patchR+n)* realw+ corner.x- m_patchR;
			memcpy( pReal+ n* m_patchW, pRealImg+ idx, m_patchW );
		}
		for (i=0;i<=dh;i++)
		{
			for (j=0;j<=dw;j++)
			{
				sum = 0;
				for( n=0; n<m_patchW; n++ )
				{
					idx = (i+corner.y-m_patchR+n)* refw+ j+corner.x-m_patchR;
					memcpy( pRef+ n* m_patchW, pRefImg+ idx, m_patchW );
				}
				if (ntype==0)
				{
					sum = (float) (GetPatchSim_subtract_intensity(pRef,pReal));
				}
				else
				{
					sum = (float) (GetPatchSim(pRef,pReal));
				}
				pFloat[i*(dw+1)+j]= (float)sum;
			}
		}
	}
	delete[] pRef;
	delete[] pReal;
//	cvReleaseImage(&pEsmallimg);
	// 显示 std::vector<CvMat> simMaps， 调试用
	CString str;
	std::string str1;
	for (m=0;m<nPoints;m++) 
	{
		pMat= (&(m_simMaps.at(m)));
		cv::normalize(*pMat,*pMat,1,0,CV_MINMAX);
		str.Format("sim%3d",m);
		str1= str;
		cv::imshow(str1,*pMat);
	}
}
void CPatchMatch::SpaceSearchMatch()
{
	int i,m,x,y,x0=0,y0=0,x1,y1;
	int dw=m_pLargeImg->width-m_pSmallImg->width;
	int dh=m_pLargeImg->height-m_pSmallImg->height;
	float s=1.0,s0=1.0,s1,r=0,r0=0,r1,sim,maxsim=0;
	int nPoints= m_samplepoints.size();
	cv::Mat pointTransformedMat(3,nPoints,CV_32FC1);
	cv::Mat tmphMat=cv::Mat::eye(3,3,CV_32FC1);
	m_hMat=cv::Mat::eye(3,3,CV_32FC1);
	CvPoint corner;
	for (r=-0.17;r<=0.17;r+=0.05)
	{
		for (s=0.7;s<=1.3;s+=0.05)
		{
			for (y=0;y<=dh;y+=2)
			{
				for (x=0;x<=dw;x+=2)
				{
					tmphMat.at<float>(0,0)= s*cos(r);
					tmphMat.at<float>(1,1)= s*cos(r);
					tmphMat.at<float>(0,1)= -s*sin(r);
					tmphMat.at<float>(1,0)= s*sin(r);
					tmphMat.at<float>(0,2)= x;
					tmphMat.at<float>(1,2)= y;
					//  矩阵乘法
					pointTransformedMat= tmphMat* m_pointMat;
					sim= 0;
					for (m=0;m<nPoints;m++)
					{
						corner.x= (int)(pointTransformedMat.at<float>(0,m)-m_pointMat.at<float>(0,m)+0.5);
						corner.y= (int)(pointTransformedMat.at<float>(1,m)-m_pointMat.at<float>(1,m)+0.5);
						if (!(assertBorder(corner.x,0,dw)&&assertBorder(corner.y,0,dh)))
						{
							continue;
						}
						//simMap 间隔2 采样
						sim+= (m_simMaps.at(m)).at<float>(corner.y,corner.x);
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
	// 精搜索
	x1=x0,y1=y0,s1=s0,r1=r0;
	float fx,fy,I11,I12,I21,I22;
	for (r=r0-0.04;r<=r0+0.04;r+=0.001)
	{
		for (s=s0-0.03;s<=s0+0.03;s+=0.005)
		{
			for (y=y0-3;y<=y0+3;y++)
			{
				for (x=x0-3;x<=x0+3;x++)
				{
					tmphMat.at<float>(0,0)= s*cos(r);
					tmphMat.at<float>(1,1)= s*cos(r);
					tmphMat.at<float>(0,1)= -s*sin(r);
					tmphMat.at<float>(1,0)= s*sin(r);
					tmphMat.at<float>(0,2)= x;
					tmphMat.at<float>(1,2)= y;
					//  矩阵乘法
					pointTransformedMat= tmphMat * m_pointMat;
					sim= 0;
					for (m=0;m<nPoints;m++)
					{
						//亚像素，插值
						fx= (pointTransformedMat.at<float>(0,m)-m_pointMat.at<float>(0,m)+0.5);
						fy= (pointTransformedMat.at<float>(1,m)-m_pointMat.at<float>(1,m)+0.5);
						corner.x= (int)fx;
						corner.y= (int)fy;
						fx= fx-corner.x;
						fy= fy-corner.y;
						if (!(assertBorder(fx,0.0001,dw-0.0001)&&assertBorder(fy,0.0001,dh-0.0001)))
						{
							continue;
						}
						//simMap 间隔2 采样
						//sim+= (m_simMaps.at(m)).at<float>(corner.y,corner.x);
						//
						I11 = (m_simMaps.at(m)).at<float>(corner.y,corner.x);
						I12 = (m_simMaps.at(m)).at<float>(corner.y,corner.x+1);
						I21 = (m_simMaps.at(m)).at<float>(corner.y+1,corner.x);
						I22 = (m_simMaps.at(m)).at<float>(corner.y+1,corner.x+1);
						sim+= ( (I11 + (I12-I11)* fx)* (1.0 - fy) + (I21 + (I22-I21)* fx)* fy );
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
	m_hMat.at<float>(0,0)= s1*cos(r1);
	m_hMat.at<float>(1,1)= s1*cos(r1);
	m_hMat.at<float>(0,1)= -s1*sin(r1);
	m_hMat.at<float>(1,0)= s1*sin(r1);
	m_hMat.at<float>(0,2)= x1+(m_pSmallImg->width-1)/2.0;
	m_hMat.at<float>(1,2)= y1+(m_pSmallImg->height-1)/2.0;
}
void CPatchMatch::GetMatchPara(CvPoint2D32f *fpoint, float *fs, float *fr)
{
	fpoint->x= m_hMat.at<float>(0,2);
	fpoint->y= m_hMat.at<float>(1,2);
	float a0=m_hMat.at<float>(0,0);
	float a1=m_hMat.at<float>(0,1);
	float s0=sqrt(a0*a0+a1*a1);
	float r0=acos(a0/s0)*180/CV_PI;
	*fs= s0;
	*fr= r0;
}
void CPatchMatch::ShowMatch()
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
	//  校正大图
	int i,j;
	
	CvPoint ishowPoint=cvPoint(-2,-2);

	cv::Mat pointTransformedMat(3,5,CV_32FC1);
	cv::Mat smallcenterpoint(3,5,CV_32FC1);
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
	//center
	smallcenterpoint.at<float>(0,4)= 0;
	smallcenterpoint.at<float>(1,4)= 0;
	smallcenterpoint.at<float>(2,4)= 1.0f;

	pointTransformedMat= m_hMat* smallcenterpoint;
	ishowPoint.x= (int)(pointTransformedMat.at<float>(0,4)+0.5);
	ishowPoint.y= (int)(pointTransformedMat.at<float>(1,4)+0.5);

	//SaveFloatMat("f:\\feature\\smallcenterpoint.txt", smallcenterpoint);
	//SaveFloatMat("f:\\feature\\pointTransformedMat.txt", pointTransformedMat);
	//SaveFloatMat("f:\\feature\\m_hMat.txt", m_hMat);

	int linewidth=1;
	int lineR= 20;
	cvLine(m_pLargeImg, 
		cvPoint(ishowPoint.x-lineR, ishowPoint.y), 
		cvPoint(ishowPoint.x+lineR, ishowPoint.y), 
		cvScalar(255,0,0), linewidth);
	cvLine(m_pLargeImg, 
		cvPoint(ishowPoint.x, ishowPoint.y-lineR), 
		cvPoint(ishowPoint.x, ishowPoint.y+lineR), 
		cvScalar(255,0,0), linewidth);

	linewidth=1;
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
	
	
	//小图画十字丝
	CvPoint smTarget;
	smTarget= cvPoint((int)(m_smallRX+0.5), (int)(m_smallRY+0.5));
	linewidth=1;
	cvLine(m_pSmallImg, cvPoint(smTarget.x-lineR, smTarget.y), cvPoint(smTarget.x+lineR, smTarget.y), cvScalar(255,0,0), linewidth);
	cvLine(m_pSmallImg, cvPoint(smTarget.x, smTarget.y-lineR), cvPoint(smTarget.x, smTarget.y+lineR), cvScalar(255,0,0), linewidth);		
	
	float a0=m_hMat.at<float>(0,0);
	float a1=m_hMat.at<float>(0,1);
	float s0=sqrt(a0*a0+a1*a1);
	float r0=acos(a0/s0)*180/CV_PI;

	CString str;
	str.Format("x%3dy%3ds%4.3fr%4.3f",ishowPoint.x,ishowPoint.y,s0,r0);
	show2Images(m_pLargeImg,m_pSmallImg,"warped"+str);
}