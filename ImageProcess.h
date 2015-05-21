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
//��ӡ��� cv::Mat
//void SaveFloatMat(CString filepath, CString matName, cv::Mat &aMat);
//��ӡ��� cv::Mat
void SaveFloatCvMat(CString filepath, CString matName, CvMat *pMat);
// Ӧ��˫���Բ�ֵ�����������ػҶ�ֵ
BYTE GetGrayV(IplImage *img, double x, double y, int type=0);
//  �ݶȷ���ͼ
void Image_Sobel_Direction_Fast(BYTE* pImage, int width, int height, int avgsize );
//�����ǶȲ��ұ�
short** SetAnlgeLUT();
// Խ����
bool assertBorder(int x, int left, int right);
bool assertBorder(float x, float left, float right);
bool CheckValid(int x,int y,int w,int h);
bool CheckValid(double x,double y,double w,double h);
// �ϲ�����ͼ�� 
IplImage* Merge2Images(IplImage *pLargeImage,IplImage *pSmallImage);
// ���Ʊ任
// ��ת m_Rot     0 ~359   ��
// ���� m_Scale   30~300   %
// ������Ӱ�任����
IplImage* rotateScaleImage(IplImage* src, float fRot, float fScale, CvPoint *tarPt=NULL);

void  FillBitmapInfo( BITMAPINFO* bmi, int width, int height, int bpp, int origin );

// ��ȷ����㷨����ʱ�����(��������)
class CTimeCount
{
private:	
	double			UseTime;				// �㷨����ʱ��(��λ:��)
	LARGE_INTEGER	Time, Frequency, old;	// ����ֵ

public:	
	void Start() // ��ʱ��ʼ
	{
		QueryPerformanceFrequency( &Frequency );
		QueryPerformanceCounter  ( &old );
		UseTime = 0.0;
	}
	void End() // ��ʱ����
	{
		QueryPerformanceCounter( &Time );
		UseTime = (double) ( Time.QuadPart - old.QuadPart) / (double)Frequency.QuadPart;
		TRACE("Use time = %20.10f(ms)\n", UseTime );
	}
	double GetUseTime() // ����㷨����ʱ��(��λ:��)
	{		
		return UseTime;
	}
};