#include "stdafx.h"
#include "math.h"
#include "ImageProcess.h"
void saveArray(CvPoint2D32f *arraySrc, int nArrayLength,CString title)
{
	FILE *fp = fopen(title, "w" );
	for (int i=0;i<nArrayLength;i++)
	{
		fprintf(fp,"%4d:   ( %5.2f, %5.2f )\n",i, arraySrc[i].x, arraySrc[i].y);
	}
	fclose(fp);
}
// 画归一化2d目标坐标曲线图
IplImage* drawNorm2DFloatGraph(const CvPoint2D32f *arraySrc, int nArrayLength, IplImage *imageDst,char *title, float maxR)
{
	const CvScalar BLACK = CV_RGB(0,0,0);
	const CvScalar WHITE = CV_RGB(255,255,255);
	const CvScalar GREY = CV_RGB(150,150,150);
	int graphR= 200;
	int iR= 10*ceil(maxR/10.0f);
	float drawScale= float(graphR)/iR;
	int b = 30;		// border around graph within the image
	int w = graphR*2+2*b;
	int h = graphR*2+2*b;

	IplImage *imageGraph;	// output image

	// Get the desired image to draw into.
	if (!imageDst) {
		// Create an RGB image for graphing the data
		imageGraph = cvCreateImage(cvSize(w,h), 8, 3);
		// Clear the image
		cvSet(imageGraph, WHITE);
	}
	else {
		// Draw onto the given image.
		imageGraph = imageDst;
	}
	if (!imageGraph) {
		std::cerr << "ERROR in drawFloatGraph(): Couldn't create image of " << w << " x " << h << std::endl;
		exit(1);
	}
	CvScalar colorGraph = CV_RGB(60,60,255);	// light-blue

	// Draw Rectangle Border
	cvRectangle(imageGraph,cvPoint(b,b),cvPoint(w-b,h-b), BLACK,2);
	// Draw Rectangle Center
	cvLine(imageGraph, cvPoint(w/2,b),cvPoint(w/2,h-b), BLACK, 1, CV_AA);	
	cvLine(imageGraph, cvPoint(b,h/2),cvPoint(w-b,h/2), BLACK, 1, CV_AA);	
	// Write 4 corners of image plane
	CvFont font;
	cvInitFont(&font,CV_FONT_HERSHEY_PLAIN,1,1, 0,1,CV_AA);	// For OpenCV 1.1
	
	char text[16];
	sprintf_s(text, sizeof(text)-1, "(%d,%d)", -iR,-iR);
	cvPutText(imageGraph, text, cvPoint(b-15, b-15), &font, BLACK);
	sprintf_s(text, sizeof(text)-1, "(%d,%d)", -iR,iR);
	cvPutText(imageGraph, text, cvPoint(b-15, h-b+20), &font, BLACK);
	sprintf_s(text, sizeof(text)-1, "(%d,%d)", iR,-iR);
	cvPutText(imageGraph, text, cvPoint(w-b-60, b-15), &font, BLACK);
	sprintf_s(text, sizeof(text)-1, "(%d,%d)", iR,iR);
	cvPutText(imageGraph, text, cvPoint(w-b-60, h-b+20), &font, BLACK);

	// Draw the values
	// Start the lines at the 1st point.
	int y = cvRound(arraySrc[0].y*drawScale);	// Get the values at a bigger scale
	int x = cvRound(arraySrc[0].x*drawScale);
	CvPoint ptPrev = cvPoint(b+graphR+x, b+graphR+y);
	// 起始点，绿色矩形标记
	cvRectangle(imageGraph,cvPoint(b+graphR+x-3,b+graphR+y-3),cvPoint(b+graphR+x+3,b+graphR+y+3), CV_RGB(0,200,0),2);

	for (int i=1; i<nArrayLength; i++) {
		y = cvRound(arraySrc[i].y*drawScale);	// Get the values at a bigger scale
		x = cvRound(arraySrc[i].x*drawScale);
		CvPoint ptNew = cvPoint(b+graphR+x, b+graphR+y);
		cvLine(imageGraph, ptPrev, ptNew, colorGraph, 1, CV_AA);	// Draw a line from the previous point to the new point
		ptPrev = ptNew;
	}
	// 终止点，红色矩形标记
	cvRectangle(imageGraph,cvPoint(b+graphR+x-3,b+graphR+y-3),cvPoint(b+graphR+x+3,b+graphR+y+3), CV_RGB(200,0,0),2);
	// Write the label, if desired
	if (title != NULL && strlen(title) > 0) {
		//cvInitFont(&font,CV_FONT_HERSHEY_PLAIN, 0.5,0.7, 0,1,CV_AA);
		cvPutText(imageGraph, title, cvPoint(120, b-15), &font,BLACK);	// black text
	}
	cvRectangle(imageGraph,cvPoint(113-3,h-b+15-3),cvPoint(113+3,h-b+15+3), CV_RGB(0,200,0),2);
	cvRectangle(imageGraph,cvPoint(200-3,h-b+15-3),cvPoint(200+3,h-b+15+3), CV_RGB(200,0,0),2);
	cvPutText(imageGraph, ":start     :end", cvPoint(120, h-b+20), &font,BLACK);
	return imageGraph;
}
// 画2d目标坐标曲线图
IplImage* draw2DFloatGraph(const CvPoint2D32f *arraySrc, int nArrayLength, IplImage *imageDst,
	int width, int height, char *title, float drawScale)
{
	const CvScalar BLACK = CV_RGB(0,0,0);
	const CvScalar WHITE = CV_RGB(255,255,255);
	const CvScalar GREY = CV_RGB(150,150,150);
	
	int b = 30;		// border around graph within the image
	if (drawScale<0)
	{
		drawScale= 400.f/cv::min(width,height);
	}
	if (cv::min(width,height)<400.f/drawScale)
	{
		drawScale= 400.f/cv::min(width,height);
	}
	if (cv::max(width,height)>800.f/drawScale)
	{
		drawScale= 1200.f/cv::max(width,height);
	}
	int w = cvRound(width*drawScale)+2*b;
	int h = cvRound(height*drawScale)+2*b;

	IplImage *imageGraph;	// output image

	// Get the desired image to draw into.
	if (!imageDst) {
		// Create an RGB image for graphing the data
		imageGraph = cvCreateImage(cvSize(w,h), 8, 3);

		// Clear the image
		cvSet(imageGraph, WHITE);
	}
	else {
		// Draw onto the given image.
		imageGraph = imageDst;
	}
	if (!imageGraph) {
		std::cerr << "ERROR in drawFloatGraph(): Couldn't create image of " << w << " x " << h << std::endl;
		exit(1);
	}
	CvScalar colorGraph = CV_RGB(60,60,255);	// light-blue

	// Draw Rectangle Border
	cvRectangle(imageGraph,cvPoint(b,b),cvPoint(w-b,h-b), BLACK,2);
	// Draw Rectangle Center
	cvLine(imageGraph, cvPoint(w/2,b),cvPoint(w/2,h-b), BLACK, 1, CV_AA);	
	cvLine(imageGraph, cvPoint(b,h/2),cvPoint(w-b,h/2), BLACK, 1, CV_AA);	
	// Write 4 corners of image plane
	CvFont font;
	cvInitFont(&font,CV_FONT_HERSHEY_PLAIN,1,1, 0,1,CV_AA);	// For OpenCV 1.1
	cvPutText(imageGraph, "(0,0)", cvPoint(b-15, b-15), &font, BLACK);
	char text[16];
	sprintf_s(text, sizeof(text)-1, "(0,%d)", height-1);
	cvPutText(imageGraph, text, cvPoint(b-15, h-b+20), &font, BLACK);
	sprintf_s(text, sizeof(text)-1, "(%d,0)", width-1);
	cvPutText(imageGraph, text, cvPoint(w-b-60, b-15), &font, BLACK);
	sprintf_s(text, sizeof(text)-1, "(%d,%d)", width-1,height-1);
	cvPutText(imageGraph, text, cvPoint(w-b-60, h-b+20), &font, BLACK);

	// Draw the values
	// Start the lines at the 1st point.
	int y = cvRound(arraySrc[0].y*drawScale);	// Get the values at a bigger scale
	int x = cvRound(arraySrc[0].x*drawScale);
	CvPoint ptPrev = cvPoint(b+x, b+y);
	// 起始点，绿色矩形标记
	cvRectangle(imageGraph,cvPoint(b+x-3,b+y-3),cvPoint(b+x+3,b+y+3), CV_RGB(0,200,0),2);

	for (int i=1; i<nArrayLength; i++) {
		y = cvRound(arraySrc[i].y*drawScale);	// Get the values at a bigger scale
		x = cvRound(arraySrc[i].x*drawScale);
		CvPoint ptNew = cvPoint(b+x, b+y);
		cvLine(imageGraph, ptPrev, ptNew, colorGraph, 1, CV_AA);	// Draw a line from the previous point to the new point
		ptPrev = ptNew;
	}
	// 起始点，红色矩形标记
	cvRectangle(imageGraph,cvPoint(b+x-3,b+y-3),cvPoint(b+x+3,b+y+3), CV_RGB(200,0,0),2);
	// Write the label, if desired
	if (title != NULL && strlen(title) > 0) {
		//cvInitFont(&font,CV_FONT_HERSHEY_PLAIN, 0.5,0.7, 0,1,CV_AA);
		cvPutText(imageGraph, title, cvPoint(120, b-15), &font,BLACK);	// black text
	}
	cvRectangle(imageGraph,cvPoint(113-3,h-b+15-3),cvPoint(113+3,h-b+15+3), CV_RGB(0,200,0),2);
	cvRectangle(imageGraph,cvPoint(200-3,h-b+15-3),cvPoint(200+3,h-b+15+3), CV_RGB(200,0,0),2);
	cvPutText(imageGraph, ":start     :end", cvPoint(120, h-b+20), &font,BLACK);
	return imageGraph;
}
// 画坐标曲线图
IplImage* drawFloatGraph(const float *arraySrc, int nArrayLength, IplImage *imageDst,
	float minV, float maxV, int width, int height, 
	char *xlabel, char *ylabel, char *title, bool showScale)
{
	const CvScalar BLACK = CV_RGB(0,0,0);
	const CvScalar WHITE = CV_RGB(255,255,255);
	const CvScalar GREY = CV_RGB(150,150,150);
	int w = width;
	int h = height;
	int b = 30;		// border around graph within the image
	if (w <= 20)
		w = nArrayLength + b*2;	// width of the image
	if (h <= 20)
		h = 220;

	int s = h - b*2;// size of graph height
	float xscale = 1.0;
	if (nArrayLength > 1)
		xscale = (w - b*3) / (float)(nArrayLength-1);	// horizontal scale
	IplImage *imageGraph;	// output image

	// Get the desired image to draw into.
	if (!imageDst) {
		// Create an RGB image for graphing the data
		imageGraph = cvCreateImage(cvSize(w,h), 8, 3);

		// Clear the image
		cvSet(imageGraph, WHITE);
	}
	else {
		// Draw onto the given image.
		imageGraph = imageDst;
	}
	if (!imageGraph) {
		std::cerr << "ERROR in drawFloatGraph(): Couldn't create image of " << w << " x " << h << std::endl;
		exit(1);
	}
	CvScalar colorGraph = CV_RGB(60,60,255);	// light-blue

	// If the user didnt supply min & mav values, find them from the data, so we can draw it at full scale.
	if (fabs(minV) < 0.0000001f && fabs(maxV) < 0.0000001f) {
		for (int i=0; i<nArrayLength; i++) {
			float v = (float)arraySrc[i];
			if (v < minV)
				minV = v;
			if (v > maxV)
				maxV = v;
		}
	}
	float diffV = maxV - minV;
	if (diffV == 0)
		diffV = 0.00000001f;	// Stop a divide-by-zero error
	float fscale = (float)s / diffV;

	// Draw the horizontal & vertical axis
	int y0 = cvRound(minV*fscale);
	cvLine(imageGraph, cvPoint(b*2,h-(b-0)), cvPoint(w-b, h-(b-0)), BLACK,2);   // axis X
	cvLine(imageGraph, cvPoint(b*2,h-(b-0)), cvPoint(b*2, h-(b+s)), BLACK,2);         // axis Y

	// Write the scale of the y axis
	CvFont font;
	cvInitFont(&font,CV_FONT_HERSHEY_PLAIN,1,1, 0,1,CV_AA);	// For OpenCV 1.1
	if (showScale) {
		//cvInitFont(&font,CV_FONT_HERSHEY_PLAIN,0.5,0.6, 0,1, CV_AA);	// For OpenCV 2.0
		CvScalar clr = BLACK;
		cvPutText(imageGraph, "0", cvPoint(2*b, h-b+15), &font, clr);
		char text[16];
		sprintf_s(text, sizeof(text)-1, "%d", cvRound(minV));
		cvPutText(imageGraph, text, cvPoint(2*b-10*strlen(text)-5, h-b), &font, clr);
		sprintf_s(text, sizeof(text)-1, "%d", cvRound(maxV));
		cvPutText(imageGraph, text, cvPoint(2*b-10*strlen(text)-5, b+5), &font, clr);
		sprintf_s(text, sizeof(text)-1, "%d", (nArrayLength-1) );
		cvPutText(imageGraph, text, cvPoint(w-b-5*strlen(text), h-b+15), &font, clr);
	}

	// Draw the values
	// Start the lines at the 1st point.
	int y = cvRound((arraySrc[0] - minV) * fscale);	// Get the values at a bigger scale
	int x = cvRound(0 * xscale);
	CvPoint ptPrev = cvPoint(b*2+x, h-(b+y));
	for (int i=1; i<nArrayLength; i++) {
		int y = cvRound((arraySrc[i] - minV) * fscale);	// Get the values at a bigger scale
		int x = cvRound(i * xscale);
		CvPoint ptNew = cvPoint(b*2+x, h-(b+y));
		cvLine(imageGraph, ptPrev, ptNew, colorGraph, 1, CV_AA);	// Draw a line from the previous point to the new point
		ptPrev = ptNew;
	}

	// Write the label, if desired
	if (xlabel != NULL && strlen(xlabel) > 0) {
		//cvInitFont(&font,CV_FONT_HERSHEY_PLAIN, 0.5,0.7, 0,1,CV_AA);
		cvPutText(imageGraph, ylabel, cvPoint(5, h/2), &font,BLACK);	// black text
	}
	if (ylabel != NULL && strlen(ylabel) > 0) {
		//cvInitFont(&font,CV_FONT_HERSHEY_PLAIN, 0.5,0.7, 0,1,CV_AA);
		cvPutText(imageGraph, xlabel, cvPoint(w/2, h-b+15), &font,BLACK);	// black text
	}
	if (title != NULL && strlen(title) > 0) {
		//cvInitFont(&font,CV_FONT_HERSHEY_PLAIN, 0.5,0.7, 0,1,CV_AA);
		cvPutText(imageGraph, title, cvPoint(100, b-5), &font,BLACK);	// black text
	}

	return imageGraph;
}
void GetGradientIntensity(IplImage *pim)
{
	int w= pim->width, h= pim->height;
	int dx,dy;
	int i,j,idx, s;
	IplImage *pTmpImg= cvCreateImage(cvSize(w,h),8,1);
	memset(pTmpImg->imageData,0,pTmpImg->imageSize);
	for (i=1;i<h-1;i++)
	{
		for(j=1;j<w-1;j++)
		{
			idx= i*w+j;
			dx = pim->imageData[idx+1]-pim->imageData[idx-1];
			dy = pim->imageData[idx+w]-pim->imageData[idx-w];
			s= abs(dx)+abs(dy);
			pTmpImg->imageData[idx]= s>255? 255:s;
		}
	}
	cvCopyImage(pTmpImg,pim);
}
void SaveFloatCvMat(CString filepath, CString matName, CvMat *pMat)
{
	FILE *fp = fopen(filepath, "a" );	
	int i,j,w,h;
	h= pMat->rows;
	w= pMat->cols;
	fprintf(fp,matName);
	fprintf(fp,":\n");
	for(i= 0; i<h; i++)
	{
		for(j= 0; j< w; j++)
		{
			fprintf(fp,"%7.4f  ",cvmGet(pMat,i,j));
		}
		fprintf(fp,"\n");
	}
	fclose(fp);
}
//void SaveFloatMat(CString filepath, CString matName, cv::Mat &aMat)
//{
//	FILE *fp = fopen(filepath, "a" );	
//	int i,j,w,h;
//	h= aMat.rows;
//	w= aMat.cols;
//	fprintf(fp,matName);
//	fprintf(fp,":\n");
//	for(i= 0; i<h; i++)
//	{
//		for(j= 0; j< w; j++)
//		{
//			fprintf(fp,"%7.4f  ",aMat.at<float>(i,j));
//		}
//		fprintf(fp,"\n");
//	}
//	fclose(fp);
//}

// 应用双线性插值，计算亚像素灰度值
//type: 1-最近, 0-插值
BYTE GetGrayV(IplImage *img, double x, double y, int type)
{
	if ( !(assertBorder((float)x,0.0f,(float)(img->width-1.0))&&
		assertBorder((float)y,0.0f,(float)(img->height-1.0))) )
	{
		return 0;
	}
	int		ix, iy;
	double  dx, dy;	
	BYTE g,*pData=(BYTE*)img->imageData;
	if (type == 1) //最近邻
	{
		ix = int(x+0.5), iy = int(y+0.5);
		g= *(pData+iy*img->widthStep+ix);
	}
	else  //双线性插值
	{		
		ix = int(x), iy = int(y);
		dx = x - ix, dy = y - iy;
		double I11 = *(pData+iy*img->widthStep+ix);
		double I12 = *(pData+iy*img->widthStep+ix+1); 
		double I21 = *(pData+(iy+1)*img->widthStep+ix); 
		double I22 = *(pData+(iy+1)*img->widthStep+ix+1); 
		g= (BYTE)( (I11 + (I12-I11)* dx)* (1.0 - dy) + (I21 + (I22-I21)* dx)* dy +0.5 );
	}
	return	g;
}
// 梯度方向相关
//  梯度方向图
void Image_Sobel_Direction_Fast(BYTE* pImage, int width, int height, int avgsize )
{
	int	i, j, x, y, index, Imagesize= width* height;
	double angle;
	int* pDx = new int[Imagesize];
	int* pDy = new int[Imagesize];
	BYTE* pTemp= new BYTE[Imagesize];
	for( x=1; x<width-1; x++ )
	{
		for( y=1; y<height-1; y++ )
		{
			index = y* width+ x;
			pDx[index] = pImage[index+1]- pImage[index-1];
			pDy[index] = pImage[index+width]- pImage[index-width];
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
					index = (y+j)* width+ x+i;
					dx = pDx[index-width]+ 2*pDx[index]+ pDx[index+width];
					dy = pDy[index-1]+ 2*pDy[index]+ pDy[index+1];
					vx += 2* dx* dy;
					dx = dx* dx, dy = dy* dy;
					vy += dx- dy;
					sumd += dx+ dy;
				}
			}
			nG  = sumd/ (2*arm+1)/ (2*arm+1);
			angle = ( CV_PI- atan2( double(vy), vx ) )/*/ 2.0+PI/2*/;
			pTemp[y*width+x] = nG < 169 ? 0 : (unsigned char)( int(angle/ CV_PI* 90)+ 74); //74~253
		}
	}
	memcpy( pImage, pTemp, Imagesize );
	delete []pDx;
	delete []pDy;
	delete []pTemp;
}
short** SetAnlgeLUT()
{
	// set LUT
	short** pAngleLUT = new short*[255];
	int i, j, temp;
	for( i=0; i<255; i++ )
	{
		pAngleLUT[i] = new short[255];
	}
	for( i=0; i<255; i++ )
	{
		for( j=0; j<255; j++ )
		{
			pAngleLUT[j][i] = 0;
			if( i > 1 && j > 1 )
			{
				temp = abs( i- j );
				if( temp > 90 )
					temp = 180- temp;
				if( temp < 45 )
				{
//					pAngleLUT[j][i] = (45-temp)* (45-temp);
					pAngleLUT[j][i] = (45-temp)>0? (45-temp): (temp-45);
				}
			}
			else
				pAngleLUT[j][i] = 0;
		}
	}
	return pAngleLUT;
}
bool assertBorder(int x, int left, int right)
{
	return (x<left||x>right)? FALSE: TRUE;
	//if (x<left || x>right)
	//{
	//	return FALSE;
	//}
	//else
	//{
	//	return TRUE;
	//}
}
bool assertBorder(float x, float left, float right)
{
	//	return (x<left||x>right)? FALSE: TRUE;
	if (x<left || x>right)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
bool CheckValid(int x,int y,int w,int h)
{
	if (x >= 0 && x < w && y >= 0 && y < h)
		return true;
	else
		return false;
}
bool CheckValid(double x,double y,double w,double h)
{
	if (x >= 0 && x <= w-1.0 && y >= 0 && y <= h-1.0)
		return true;
	else
		return false;
}
IplImage* Merge2Images(IplImage *pLargeImage,IplImage *pSmallImage)
{
	int w,h,dis=15;
	w= pLargeImage->width+pSmallImage->width+dis;
	h= MAX(pLargeImage->height,pSmallImage->height);
	IplImage *pMergeImage;
	pMergeImage= cvCreateImage(cvSize(w,h),pLargeImage->depth,pLargeImage->nChannels);
	memset(pMergeImage->imageData,0,pMergeImage->widthStep*pMergeImage->height);
//	cvFillImage(pMergeImage,0);
	cvSetImageROI(pMergeImage,cvRect(0,0,pLargeImage->width,pLargeImage->height));
	cvCopy(pLargeImage,pMergeImage);
	cvSetImageROI(pMergeImage,cvRect(pLargeImage->width+dis,0,pSmallImage->width,pSmallImage->height));
	cvCopy(pSmallImage,pMergeImage);
	cvResetImageROI(pMergeImage);
	return pMergeImage;
}

// 相似变换
// 旋转 fRot    
// 放缩 fScale   
// 利用射影变换函数
IplImage* rotateScaleImage(IplImage* src, float fRot, float fScale, CvPoint *tarPt)
{
	int i,w=src->width,h=src->height;
	double angleRad= fRot*CV_PI/180;
	int newwidth= (int)( fabs(fScale* h* sin(angleRad))+fabs(fScale* w* cos(angleRad))+ 1 );
	int newheight=(int)( fabs(fScale* h* cos(angleRad))+fabs(fScale* w* sin(angleRad))+ 1 );
	IplImage *warpImage= cvCreateImage(cvSize(newwidth,newheight),src->depth,src->nChannels);

	CvPoint2D32f srcPts[4],dstPts[4];
	srcPts[0]= cvPoint2D32f(0,0);
	srcPts[1]= cvPoint2D32f(0,h);
	srcPts[2]= cvPoint2D32f(w,h);
	srcPts[3]= cvPoint2D32f(w,0);

	dstPts[0]= cvPoint2D32f(0,0);
	dstPts[1]= cvPoint2D32f(fScale*h*sin(angleRad),fScale*h*cos(angleRad));
	dstPts[2]= cvPoint2D32f(fScale*h*sin(angleRad)+fScale*w*cos(angleRad),
		fScale*h*cos(angleRad)-fScale*w*sin(angleRad));
	dstPts[3]= cvPoint2D32f(fScale*w*cos(angleRad),-fScale*w*sin(angleRad));

	float xoffset,yoffset;
	xoffset= MIN( MIN(dstPts[0].x,dstPts[1].x),MIN(dstPts[2].x,dstPts[3].x));
	yoffset= MIN( MIN(dstPts[0].y,dstPts[1].y),MIN(dstPts[2].y,dstPts[3].y));
	for(i=0;i<4;i++)
	{		
		dstPts[i].x -= xoffset;		
		dstPts[i].y -= yoffset;
	}

	CvMat *Amat=cvCreateMat(3,3,CV_32FC1);

	cvGetPerspectiveTransform(srcPts,dstPts,Amat);
	cvWarpPerspective(src,warpImage,Amat);

	if (tarPt!=NULL)
	{
		CvMat *Bmat=cvCreateMat(3,1,CV_32FC1);
		cvmSet(Bmat,0,0,(float)tarPt->x);
		cvmSet(Bmat,1,0,(float)tarPt->y);
		cvmSet(Bmat,2,0,1.0f);
		cvMatMul(Amat,Bmat,Bmat);
		tarPt->x= (int)(cvmGet(Bmat,0,0)/cvmGet(Bmat,2,0 )+0.5);
		tarPt->y= (int)(cvmGet(Bmat,1,0)/cvmGet(Bmat,2,0)+0.5);
		cvReleaseMat(&Bmat);
	}

	cvReleaseMat(&Amat);
	
	return warpImage;
}

void  FillBitmapInfo( BITMAPINFO* bmi, int width, int height, int bpp, int origin )
{
	assert( bmi && width >= 0 && height >= 0 && (bpp == 8 || bpp == 24 || bpp == 32));

	BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);

	memset( bmih, 0, sizeof(*bmih));
	bmih->biSize = sizeof(BITMAPINFOHEADER);
	bmih->biWidth = width;
	bmih->biHeight = origin ? abs(height) : -abs(height);
	bmih->biPlanes = 1;
	bmih->biBitCount = (unsigned short)bpp;
	bmih->biCompression = BI_RGB;

	if( bpp == 8 )
	{
		RGBQUAD* palette = bmi->bmiColors;
		int i;
		for( i = 0; i < 256; i++ )
		{
			palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
			palette[i].rgbReserved = 0;
		}
	}
}

//void drawCrossOnImage(IplImage* pImg, CvPoint pt)
//{
//	cvLine(pFirstImg, cvPoint(smTarget.x-lineR, smTarget.y), cvPoint(smTarget.x+lineR, smTarget.y), cvScalar(255,0,0), linewidth);
//	cvLine(pFirstImg, cvPoint(smTarget.x, smTarget.y-lineR), cvPoint(smTarget.x, smTarget.y+lineR), cvScalar(255,0,0), linewidth);		
//}


