#pragma once
#include "afxcmn.h"
#include "resource.h"

// CProgressDlg dialog

class CProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProgressDlg();
	BOOL Create(CWnd *pParent=NULL);

	// Progress Dialog manipulation
	void SetStatus(LPCTSTR lpszMessage);	
	void SetRange(int nLower,int nUpper);
	int  SetStep(int nStep);
	int  SetPos(int nPos);
	int  OffsetPos(int nPos);
	int  StepIt();
	BOOL CheckCancelButton();
	
// Dialog Data
	enum { IDD = IDD_PROCESSING_RATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_Progress;
	UINT m_nCaptionID;
	int m_nLower;
	int m_nUpper;
	int m_nStep;

	BOOL m_bCancel;
	BOOL m_bParentDisabled;
	void ReEnableParent();
	void UpdatePercent(int nCurrent);
	void PumpMessages();
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
protected:
	virtual void OnCancel();
};
