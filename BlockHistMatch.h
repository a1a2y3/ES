#pragma once
#include <opencv\cv.h>
#include <opencv\highgui.h>
class CBlockHistMatch
{
public:
	CBlockHistMatch(void);
	~CBlockHistMatch(void);
private:
	cv::Mat m_pointMat;
	std::vector<CvPoint> m_samplepoints;
	cv::Mat m_hMat;
	int m_patchR;
	int m_patchW;
	float m_smallRX;
	float m_smallRY;
	CvPoint2D32f m_pointInLarge,m_pointInSmall;
	const static int NSCALE= 4;
	IplImage **m_PYR_Small;
	IplImage **m_PYR_Large;
	IplImage *m_pLargeImg;
	IplImage *m_pSmallImg;
	IplImage *m_pOriginLargeImg;
	IplImage *m_pOriginSmallImg;
	short *m_histLarge;
	short *m_histSmall;
public:
	void Image_Orientation_XY(IplImage *im);
	void GetDirHist(IplImage *pImg, int x0, int y0, int r, short *pHist);
	void InputImageHist(IplImage *pLargeImg, IplImage *pSmallImg);
	void GetSamplePoints();

	void SimpleMatch2D();
	int  Mul_Scale_Circu_Dis(IplImage **PYRa, IplImage **PYRb, int x, int y);

	void GetTransformedPoint(CvPoint2D32f OldPoint, CvPoint2D32f* pNewPoint);
	void ManualPointMatch(CvPoint2D32f* plargepoint, CvPoint2D32f* psmallpoint);
	void ShowMatch(CString filepath="0");
	void GetMatchResult(int targetMode,CvPoint2D32f *pointLarge,CvPoint2D32f *pointSmall);
};
