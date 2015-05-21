#pragma once
#include "afxcmn.h"


// CTargetNumDlg dialog

class CTargetNumDlg : public CDialog
{
	DECLARE_DYNAMIC(CTargetNumDlg)

public:
	CTargetNumDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTargetNumDlg();

// Dialog Data
	enum { IDD = IDD_SET_TARGETNUM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_iNum;
};
