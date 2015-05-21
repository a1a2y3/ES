#pragma once

// CConfirmDlg dialog

class CConfirmDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfirmDlg)

public:
	CConfirmDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfirmDlg();

// Dialog Data
	enum { IDD = IDD_CONFIRM_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_sText;
};
