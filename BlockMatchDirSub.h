#pragma once
#include <opencv\cv.h>
#include <opencv\highgui.h>
class CBlockMatchDirSub
{
public:
	CBlockMatchDirSub(void);
	~CBlockMatchDirSub(void);
private:
	int m_patchR;
	int m_patchW;
	float m_smallRX;
	float m_smallRY;
	CvPoint2D32f m_pointInLarge,m_pointInSmall;

	short **m_pAngleLUT;	
	BYTE *m_pPatchMask;	
	IplImage *m_pLargeImg;
	IplImage *m_pSmallImg;
	IplImage *m_pOriginLargeImg;
	IplImage *m_pOriginSmallImg;

	
	cv::Mat m_pointMat;
	std::vector<CvPoint> m_samplepoints;
	std::vector<cv::Mat> m_simMaps;
	int  GetPatchSim_GRAY_HIST(BYTE *pRef, int refwstep, BYTE *pReal, int realwstep);
	int  GetPatchSim_ORI_HIST(BYTE *pRef, int refwstep, BYTE *pReal, int realwstep);
	int  GetPatchSim_XY_window(BYTE *pRef, int refwstep, BYTE *pReal, int realwstep);
	int  GetPatchSim_intensity(BYTE *pRef, BYTE *pReal);
	void MemPrepare();
	void GetSamplePoints();
public:
	cv::Mat m_hMat;
	void GetTransformedPoint(CvPoint2D32f OldPoint, CvPoint2D32f* pNewPoint);
	void SetPara(int pr=15);
	void InputImage(IplImage *pLargeImg, IplImage *pSmallImg);
	void GetSimMap_XY();
	void GetSimMap_NCC_DFT();
	void GetSimMap_ORI_HIST();
	void GetSimMap_GRAY_HIST();
	void SimpleMatch2D();
	void SimpleMatch2D_HIST();
	void SpaceSearchMatch();
	void SpaceSearchMatch_rot360();
	void SpaceSearchMatch_6d();
	void ManualPointMatch(CvPoint2D32f* plargepoint, CvPoint2D32f* psmallpoint);
	void ShowMatch(CString filepath,CString fname);
	void GetMatchResult(int targetMode,CvPoint2D32f *pointLarge,CvPoint2D32f *pointSmall);
	void LocateTargetForSmallRef(CvPoint2D32f *pointLarge,CvPoint2D32f *pointSmall);
	void LocateTargetForLargeRef(CvPoint2D32f *pointLarge,CvPoint2D32f *pointSmall);
	void saveHmat(CString filename);
	void loadHmat(CString filename);
	//≤‚ ‘ ‰≥ˆ
//	void TestPatchSim();
};
