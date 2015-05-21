// ProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Es.h"
#include "ProgressDlg.h"
#include "resource.h"

// CProgressDlg dialog

IMPLEMENT_DYNAMIC(CProgressDlg, CDialog)

CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
{
	m_nCaptionID = IDD_PROCESSING_RATE;

	m_bCancel=FALSE;
	m_nLower=0;
	m_nUpper=100;
	m_nStep=10;
	//{{AFX_DATA_INIT(CProgressDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bParentDisabled = FALSE;
}

CProgressDlg::~CProgressDlg()
{
	if(m_hWnd!=NULL)
		DestroyWindow();
}
BOOL CProgressDlg::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	ReEnableParent();
	return CDialog::DestroyWindow();
}
void CProgressDlg::ReEnableParent()
{
	if(m_bParentDisabled && (m_pParentWnd!=NULL))
		m_pParentWnd->EnableWindow(TRUE);
	m_bParentDisabled=FALSE;
}
BOOL CProgressDlg::Create(CWnd *pParent)
{
	// Get the true parent of the dialog
	m_pParentWnd = CWnd::GetSafeOwner(pParent);

	// m_bParentDisabled is used to re-enable the parent window
	// when the dialog is destroyed. So we don't want to set
	// it to TRUE unless the parent was already enabled.

	if((m_pParentWnd!=NULL) && m_pParentWnd->IsWindowEnabled())
	{
		m_pParentWnd->EnableWindow(FALSE);
		m_bParentDisabled = TRUE;
	}

	if(!CDialog::Create(CProgressDlg::IDD,pParent))
	{
		ReEnableParent();
		return FALSE;
	}

	return TRUE;
}
void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
}
BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CProgressDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
void CProgressDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	m_bCancel=TRUE;
}
void CProgressDlg::SetStatus(LPCTSTR lpszMessage)
{
	ASSERT(m_hWnd); // Don't call this _before_ the dialog has
	// been created. Can be called from OnInitDialog
	CWnd *pWndStatus = GetDlgItem(IDD_PROCESSING_RATE);
	// Verify that the static text control exists
	ASSERT(pWndStatus!=NULL);
	pWndStatus->SetWindowText(lpszMessage);
}
void CProgressDlg::SetRange(int nLower,int nUpper)
{
	m_nLower = nLower;
	m_nUpper = nUpper;
	m_Progress.SetRange(nLower,nUpper);
}

int CProgressDlg::SetPos(int nPos)
{
	PumpMessages();
	int iResult = m_Progress.SetPos(nPos);
	UpdatePercent(nPos);
	return iResult;
}

int CProgressDlg::SetStep(int nStep)
{
	m_nStep = nStep; // Store for later use in calculating percentage
	return m_Progress.SetStep(nStep);
}

int CProgressDlg::OffsetPos(int nPos)
{
	PumpMessages();
	int iResult = m_Progress.OffsetPos(nPos);
	UpdatePercent(iResult+nPos);
	return iResult;
}

int CProgressDlg::StepIt()
{
	PumpMessages();
	int iResult = m_Progress.StepIt();
	UpdatePercent(iResult+m_nStep);
	return iResult;
}
void CProgressDlg::PumpMessages()
{
	// Must call Create() before using the dialog
//	ASSERT(m_hWnd!=NULL);

	MSG msg;
	// Handle dialog messages
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if(!IsDialogMessage(&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);  
		}
	}
}
BOOL CProgressDlg::CheckCancelButton()
{
	// Process all pending messages
	PumpMessages();

	// Reset m_bCancel to FALSE so that
	// CheckCancelButton returns FALSE until the user
	// clicks Cancel again. This will allow you to call
	// CheckCancelButton and still continue the operation.
	// If m_bCancel stayed TRUE, then the next call to
	// CheckCancelButton would always return TRUE

	BOOL bResult = m_bCancel;
	m_bCancel = FALSE;

	return bResult;
}

void CProgressDlg::UpdatePercent(int nNewPos)
{
	CWnd *pWndPercent = GetDlgItem(IDC_STATIC);
	int nPercent;

	int nDivisor = m_nUpper - m_nLower;
//	ASSERT(nDivisor>0);  // m_nLower should be smaller than m_nUpper

	int nDividend = (nNewPos - m_nLower);
//	ASSERT(nDividend>=0);   // Current position should be greater than m_nLower

	nPercent = nDividend * 100 / nDivisor;

	// Since the Progress Control wraps, we will wrap the percentage
	// along with it. However, don't reset 100% back to 0%
	if(nPercent!=100)
		nPercent %= 100;

	// Display the percentage
	CString strBuf;
	strBuf.Format(_T("%d%c"),nPercent,_T('%'));

	CString strCur; // get current percentage
	pWndPercent->GetWindowText(strCur);

	if (strCur != strBuf)
		pWndPercent->SetWindowText(strBuf);
}
BOOL CProgressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_Progress.SetRange(m_nLower,m_nUpper);
	m_Progress.SetStep(m_nStep);
	m_Progress.SetPos(m_nLower);

	//CString strCaption;
	//VERIFY(strCaption.LoadString(m_nCaptionID));
	//SetWindowText(strCaption);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}



