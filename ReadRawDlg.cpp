// ReadRawDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Es.h"
#include "ReadRawDlg.h"
#include "afxdialogex.h"


// CReadRawDlg 对话框

IMPLEMENT_DYNAMIC(CReadRawDlg, CDialogEx)

CReadRawDlg::CReadRawDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CReadRawDlg::IDD, pParent)
	, m_RawWidth(320)
	, m_RawHeight(256)
	, m_TextOut(_T(""))
{

}

CReadRawDlg::~CReadRawDlg()
{
}

void CReadRawDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_RawWidth);
	DDV_MinMaxInt(pDX, m_RawWidth, 1, 100000000);
	DDX_Text(pDX, IDC_EDIT3, m_RawHeight);
	DDV_MinMaxInt(pDX, m_RawHeight, 1, 10000000);
	DDX_Text(pDX, IDC_CONFIRM_STATIC, m_TextOut);
}


BEGIN_MESSAGE_MAP(CReadRawDlg, CDialogEx)
END_MESSAGE_MAP()


// CReadRawDlg 消息处理程序
