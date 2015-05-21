#pragma once

#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <stdio.h>
void saveArray(CvPoint2D32f *arraySrc, int nArrayLength,CString title);
IplImage* drawNorm2DFloatGraph(const CvPoint2D32f *arraySrc, int nArrayLength, IplImage *imageDst,char *title, float maxR);
IplImage* draw2DFloatGraph(const CvPoint2D32f *arraySrc, int nArrayLength, IplImage *imageDst,
	int width, int height, char *title=0, float drawScale=1.0f);
IplImage* drawFloatGraph(const float *arraySrc, int nArrayLength, IplImage *imageDst=0, 
	float minV=0.0, float maxV=0.0, int width=0, int height=0, 
	char *xlabel=0, char *ylabel=0, char *title=0, bool showScale=false);
void GetGradientIntensity(IplImage *pim);
//打印输出 cv::Mat
//void SaveFloatMat(CString filepath, CString matName, cv::Mat &aMat);
//打印输出 cv::Mat
void SaveFloatCvMat(CString filepath, CString matName, CvMat *pMat);
// 应用双线性插值，计算亚像素灰度值
BYTE GetGrayV(IplImage *img, double x, double y, int type=0);
//  梯度方向图
void Image_Sobel_Direction_Fast(BYTE* pImage, int width, int height, int avgsize );
//建立角度查找表
short** SetAnlgeLUT();
// 越界检测
bool assertBorder(int x, int left, int right);
bool assertBorder(float x, float left, float right);
bool CheckValid(int x,int y,int w,int h);
bool CheckValid(double x,double y,double w,double h);
// 合并两幅图像 
IplImage* Merge2Images(IplImage *pLargeImage,IplImage *pSmallImage);
// 相似变换
// 旋转 m_Rot     0 ~359   度
// 放缩 m_Scale   30~300   %
// 利用射影变换函数
IplImage* rotateScaleImage(IplImage* src, float fRot, float fScale, CvPoint *tarPt=NULL);

void  FillBitmapInfo( BITMAPINFO* bmi, int width, int height, int bpp, int origin );

// 精确获得算法处理时间的类(毫秒量级)
class CTimeCount
{
private:	
	double			UseTime;				// 算法处理时间(单位:秒)
	LARGE_INTEGER	Time, Frequency, old;	// 计数值

public:	
	void Start() // 计时开始
	{
		QueryPerformanceFrequency( &Frequency );
		QueryPerformanceCounter  ( &old );
		UseTime = 0.0;
	}
	void End() // 计时结束
	{
		QueryPerformanceCounter( &Time );
		UseTime = (double) ( Time.QuadPart - old.QuadPart) / (double)Frequency.QuadPart;
		TRACE("Use time = %20.10f(ms)\n", UseTime );
	}
	double GetUseTime() // 获得算法处理时间(单位:秒)
	{		
		return UseTime;
	}
};