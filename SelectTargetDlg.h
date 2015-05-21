#pragma once


// CSelectTargetDlg dialog
class CSelectTargetDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectTargetDlg)

public:
	CSelectTargetDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectTargetDlg();

	int		m_ModeType;

// Dialog Data
	enum { IDD = IDD_DIALOG_SELECT_TARGET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
};
