// SelectTargetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Es.h"
#include "SelectTargetDlg.h"


// CSelectTargetDlg dialog

IMPLEMENT_DYNAMIC(CSelectTargetDlg, CDialog)

CSelectTargetDlg::CSelectTargetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectTargetDlg::IDD, pParent)
{
	m_ModeType=1;
}

CSelectTargetDlg::~CSelectTargetDlg()
{
}

void CSelectTargetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSelectTargetDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIO1, &CSelectTargetDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CSelectTargetDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CSelectTargetDlg::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO4, &CSelectTargetDlg::OnBnClickedRadio4)
END_MESSAGE_MAP()


// CSelectTargetDlg message handlers

void CSelectTargetDlg::OnBnClickedRadio1()// 大图中心
{
	// TODO: Add your control notification handler code here
	m_ModeType=1;
}

void CSelectTargetDlg::OnBnClickedRadio2()// 小图中心
{
	// TODO: Add your control notification handler code here
	m_ModeType=3;
}

void CSelectTargetDlg::OnBnClickedRadio3()// 大图手选
{
	// TODO: Add your control notification handler code here
	m_ModeType=2;
}

void CSelectTargetDlg::OnBnClickedRadio4()// 小图手选
{
	// TODO: Add your control notification handler code here
	m_ModeType=4;
}
