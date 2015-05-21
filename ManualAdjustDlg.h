#pragma once

#include "opencv\cv.h"
// ManualAdjustDlg dialog

class ManualAdjustDlg : public CDialog
{
	DECLARE_DYNAMIC(ManualAdjustDlg)
public:
	CMDIFrameWnd*   m_pFrameWnd;

	int     m_ImgW, m_ImgH;
	CPen    m_RedPen,  *m_pOldPen;
	BOOL	m_IsImageOpen;
	CRect   m_ImageRect, m_RedRect, m_PicRect, m_OldRect;
	CSize   m_ImageOffset;
	IplImage *m_pSmallImage,*pDocImage, *m_pbkImage;

public:
	ManualAdjustDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ManualAdjustDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_MANUAL_ADJUST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedShowResult();
	int m_Rot2;
	int m_Scale2;
	afx_msg void OnEnChangeEditRot();
};
#pragma once

#ifdef _WIN32_WCE
#error "CDHtmlDialog is not supported for Windows CE."
#endif 


