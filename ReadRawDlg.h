#pragma once


// CReadRawDlg 对话框

class CReadRawDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CReadRawDlg)

public:
	CReadRawDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CReadRawDlg();

// 对话框数据
	enum { IDD = IDD_READRAW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_RawWidth;
	int m_RawHeight;
	CString m_TextOut;
};
