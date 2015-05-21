#pragma once
#include <cv.h>
#include <highgui.h>
class CPatchMatch
{
public:
	CPatchMatch(void);
	~CPatchMatch(void);
private:
	int m_patchR;
	int m_patchW;
	float m_smallRX;
	float m_smallRY;
	CvPoint m_StartPoint;

	short **m_pAngleLUT;	
	BYTE *m_pPatchMask;	
	IplImage *m_pLargeImg;
	IplImage *m_pSmallImg;
	IplImage *m_pOriginLargeImg;
	IplImage *m_pOriginSmallImg;

	cv::Mat m_hMat;
	cv::Mat m_pointMat;
	std::vector<CvPoint> m_samplepoints;
	std::vector<cv::Mat> m_simMaps;
	
	int  GetPatchSim(BYTE *pRef, BYTE *pReal);
	int  GetPatchSim_subtract_intensity(BYTE *pRef, BYTE *pReal);

public:
	void SetPara(CvPoint point, int pr=15);
	void MemPrepare();
	void InputImage(IplImage *pLargeImg, IplImage *pSmallImg);
	void ReleaseVector();
	//ntype: 0-Õ¨‘¥£¨1-“Ï‘¥
	void GetSamplePoints();
	void GetSimMap(int ntype);
	void SpaceSearchMatch();
	void ShowMatch();
	void GetMatchPara(CvPoint2D32f *fpoint, float *fs, float *fr);
};
