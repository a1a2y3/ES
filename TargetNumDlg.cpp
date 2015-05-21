// TargetNumDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Es.h"
#include "TargetNumDlg.h"


// CTargetNumDlg dialog

IMPLEMENT_DYNAMIC(CTargetNumDlg, CDialog)

CTargetNumDlg::CTargetNumDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTargetNumDlg::IDD, pParent)
	, m_iNum(3)
{
}

CTargetNumDlg::~CTargetNumDlg()
{
}

void CTargetNumDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_iNum);
	DDV_MinMaxInt(pDX, m_iNum, 1, 6);
}


BEGIN_MESSAGE_MAP(CTargetNumDlg, CDialog)
END_MESSAGE_MAP()


// CTargetNumDlg message handlers
