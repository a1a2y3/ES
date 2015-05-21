#pragma once
#include "EsView.h"	// Added by ClassView
#include "opencv\cv.h"


// CTargetRectDlg dialog

class CTargetRectDlg : public CDialog
{
	DECLARE_DYNAMIC(CTargetRectDlg)

public:
	CTargetRectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTargetRectDlg();

	void SetImage(IplImage *pImg);
	void EnableShow(BOOL Flag);
	void SetTargetPos(CvPoint2D32f TargetPos);
	void ShowWindow(int x0, int y0);
	void MoveWindow(int x0, int y0);
	void getScaledImage(IplImage* srcImg, IplImage* dstImg, float targetX, float targetY, float mag);
	CEsView *m_pView;
	CTargetRectDlg(CEsView  *pView, IplImage *pInputImg, int WindowW, int WindowH, int TargetNo = 0);

private:
	IplImage 	*m_pImage;
	int			 m_TargetNo;
	float		 m_MagnifyCoeff;
	int          m_WindowW;
	int          m_WindowH;
	CvPoint2D32f m_TargetPos;
	CPoint       m_ShowPos;
	IplImage    *m_pScaledImage;

// Dialog Data
	enum { IDD = IDD_RIGHTBAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};
