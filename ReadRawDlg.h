#pragma once


// CReadRawDlg �Ի���

class CReadRawDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CReadRawDlg)

public:
	CReadRawDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CReadRawDlg();

// �Ի�������
	enum { IDD = IDD_READRAW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_RawWidth;
	int m_RawHeight;
	CString m_TextOut;
};
