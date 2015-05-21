// SetTarPos.cpp : implementation file
//

#include "stdafx.h"
#include "Es.h"
#include "SetTarPos.h"


// CSetTarPos dialog

IMPLEMENT_DYNAMIC(CSetTarPos, CDialog)

CSetTarPos::CSetTarPos(CWnd* pParent /*=NULL*/)
	: CDialog(CSetTarPos::IDD, pParent)
	, m_x(0)
	, m_y(0)
{

}

CSetTarPos::~CSetTarPos()
{
}

void CSetTarPos::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_x);
	DDV_MinMaxFloat(pDX, m_x, 0, 99999999);
	DDX_Text(pDX, IDC_EDIT2, m_y);
	DDV_MinMaxFloat(pDX, m_y, 0, 99999999);
}


BEGIN_MESSAGE_MAP(CSetTarPos, CDialog)
END_MESSAGE_MAP()


// CSetTarPos message handlers
