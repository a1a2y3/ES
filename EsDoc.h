
// EsDoc.h : CEsDoc 类的接口
//


#pragma once
#include "opencv\cv.h"
#include "opencv\highgui.h"
class CEsView;

class CEsDoc : public CDocument
{
protected: // 仅从序列化创建
	CEsDoc();
	DECLARE_DYNCREATE(CEsDoc)

	// 属性
public:
	bool       m_isTrackRunning;
	CString    m_strFileName;
	IplImage *m_pImage,*bk,*bk2;
	int        m_RawImageWidth, m_RawImageHeight;
	std::vector<CvPoint2D32f> m_PointVector;         // txt文件中读入的遥测跟踪数据
	CEsView  *m_pView;
	int m_Rot;      // 旋转角度         0~359
	int m_Scale;    // 放缩百分比      初始为 100
	CvPoint2D32f m_targetPosition;
	CList< CPoint, CPoint >    m_cplistFeaturePoint;   
//	std::vector<CvPoint> m_cplistFeaturePoint; //  特征点链表
	// 操作
public:

	// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);

	// 实现
public:
	virtual ~CEsDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnImageFilter();
	afx_msg void OnEnhanceHist();
	afx_msg void OnActCancel();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnRotateImage();
	afx_msg void OnGetPreviousImage();
	afx_msg void OnGetNextImage();
	afx_msg void OnReloadImage();
	afx_msg void OnVflip();
	afx_msg void OnHflip();
	afx_msg void OnLeft90();
	afx_msg void OnRight90();
	afx_msg void OnGradientOrientationMap();
	afx_msg void OnGradientIntensityMap();
	afx_msg void OnPyramidGradientIntensityMap();
	afx_msg void OnGrayInverse();
	afx_msg void OnPtTrack();
	afx_msg void OnLoadTrackData();
	afx_msg void OnRegistrationTrack();
	afx_msg void OnRgstrack2();
};


