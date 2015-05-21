#include "stdafx.h"
#include "MatchFunc.h"
#include "ImageProcess.h"

int GetDirectionSimilarity( BYTE* pRef, BYTE* pReal, int imagesize, short** pLUT )
{
	int i, dref, dreal;
	int sum = 0;
	for( i=0; i<imagesize; i++ )
	{
		dref = pRef[i];
		dreal = pReal[i];
		sum = sum+ pLUT[dreal][dref];
	}
	return sum;
}
double lz_MatchDirect( IplImage *larimg, IplImage *smallimg, CvPoint *pMatchLoc,int step,CvRect searchRect)
{
	BYTE* pRefImg= (BYTE*)larimg->imageData;
	int refw= larimg->widthStep;
	int refh= larimg->height; 
	int refsize   = refw* refh;
	BYTE* pRealImg= (BYTE*)smallimg->imageData;
	int realw= smallimg->widthStep;
	int realh= smallimg->height;
	int realsize  = realw* realh;

	cvSmooth(larimg,larimg,CV_GAUSSIAN,5);
	cvSmooth(smallimg,smallimg,CV_GAUSSIAN,5);

	Image_Sobel_Direction_Fast( (BYTE*)larimg->imageData, refw, refh, 1 );
	Image_Sobel_Direction_Fast( (BYTE*)smallimg->imageData, realw, realh, 1 );

	// find matching place
	// set LUT
	short** pAngleLUT= SetAnlgeLUT();

	int i,j,n, indexref;
	int sum, maxsum, minsum;

	BYTE *pRef = new BYTE[realsize];

	minsum = realsize;
	maxsum = 0;
	int io,jo;
	int wstart,wend, hstart,hend;
	wstart= searchRect.x;
	wend= wstart+ searchRect.width- 1;
	hstart= searchRect.y;
	hend= hstart+ searchRect.height-1;
	for(i= hstart; i<= hend; i+=step)
	{
		for(j= wstart; j<= wend; j+=step)
		{
			sum = 0;
			for( n=0; n<realh; n++ )
			{
				indexref = (i+n)* refw+ j;
				memcpy( pRef+ n* realw, pRefImg+ indexref, realw );
			}
			sum = GetDirectionSimilarity( pRef, pRealImg, realsize, pAngleLUT );
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
	pMatchLoc->x= jo;
	pMatchLoc->y= io;
	// 清理内存
	delete[] pRef;
	for( i=0; i<255; i++ )
	{
		delete[] pAngleLUT[i];
	}
	delete[] pAngleLUT;

	return (double)maxsum;
}
// 异源图像匹配
double lz_Multi_Sensor_MatchTemplate(IplImage *pLargeImg, IplImage *pSmallImg, CvPoint* pointInLarge,
								   int step, CvPoint smTarget, CvRect targetRect)
{
	if (step<1 || step>8)
	{
		step= 1;
	}	

	if (targetRect.width==0 || targetRect.height==0)
	{
		targetRect.x = 0;
		targetRect.y = 0;
		targetRect.width  = pLargeImg->width;
		targetRect.height  = pLargeImg->height;
	}
	CvRect searchRect;
	searchRect.x= targetRect.x- smTarget.x;
	searchRect.y= targetRect.y- smTarget.y;
	searchRect.width  = targetRect.width;
	searchRect.height = targetRect.height;

	if (searchRect.x+ searchRect.width+ pSmallImg->width > pLargeImg->width) // x 右越界  ?
	{
		searchRect.width= pLargeImg->width- pSmallImg->width+ 1- searchRect.x;
	}
	if (searchRect.x< 0)													      // x 左越界  
	{
		searchRect.width= searchRect.width+ searchRect.x;
		searchRect.x= 0;
	}
	if (searchRect.y+ searchRect.height+ pSmallImg->height > pLargeImg->height) // y 右越界  ?
	{
		searchRect.height= pLargeImg->height- pSmallImg->height+ 1- searchRect.y;
	}
	if (searchRect.y< 0)														  // y 左越界  
	{
		searchRect.height= searchRect.height+ searchRect.y;
		searchRect.y= 0;
	}
	double similarity;
	IplImage *larImg,*smaImg;
	larImg= cvCloneImage(pLargeImg);
	smaImg= cvCloneImage(pSmallImg);

	similarity= lz_MatchDirect( larImg, smaImg, pointInLarge,step,searchRect);
	
	// Show me what you got
	cvReleaseImage(&larImg);
	cvReleaseImage(&smaImg);
	return similarity;
}
