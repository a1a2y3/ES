
// EsDoc.h : CEsDoc ��Ľӿ�
//


#pragma once
#include "opencv\cv.h"
#include "opencv\highgui.h"
class CEsView;

class CEsDoc : public CDocument
{
protected: // �������л�����
	CEsDoc();
	DECLARE_DYNCREATE(CEsDoc)

	// ����
public:
	bool       m_isTrackRunning;
	CString    m_strFileName;
	IplImage *m_pImage,*bk,*bk2;
	int        m_RawImageWidth, m_RawImageHeight;
	std::vector<CvPoint2D32f> m_PointVector;         // txt�ļ��ж����ң���������
	CEsView  *m_pView;
	int m_Rot;      // ��ת�Ƕ�         0~359
	int m_Scale;    // �����ٷֱ�      ��ʼΪ 100
	CvPoint2D32f m_targetPosition;
	CList< CPoint, CPoint >    m_cplistFeaturePoint;   
//	std::vector<CvPoint> m_cplistFeaturePoint; //  ����������
	// ����
public:

	// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);

	// ʵ��
public:
	virtual ~CEsDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// ���ɵ���Ϣӳ�亯��
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


