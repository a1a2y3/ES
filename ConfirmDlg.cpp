// ConfirmDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Es.h"
#include "ConfirmDlg.h"


// CConfirmDlg dialog

IMPLEMENT_DYNAMIC(CConfirmDlg, CDialog)

CConfirmDlg::CConfirmDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfirmDlg::IDD, pParent)
	, m_sText(_T(""))
{

}

CConfirmDlg::~CConfirmDlg()
{
}

void CConfirmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CONFIRM_STATIC, m_sText);
}


BEGIN_MESSAGE_MAP(CConfirmDlg, CDialog)
END_MESSAGE_MAP()

