// ManualAdjustDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Es.h"
#include "EsDoc.h"
#include "EsView.h"
#include "ManualAdjustDlg.h"
#include "ImageProcess.h"


// ManualAdjustDlg dialog

IMPLEMENT_DYNAMIC(ManualAdjustDlg, CDialog)

ManualAdjustDlg::ManualAdjustDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ManualAdjustDlg::IDD, pParent)
	, m_Rot2(0)
	, m_Scale2(100)
{
	if( pParent )
		m_pFrameWnd = (CMDIFrameWnd*)pParent;
	else
		m_pFrameWnd = NULL;

	m_OldRect.SetRectEmpty();
	m_IsImageOpen = false;
	m_RedPen.CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
	m_pSmallImage= NULL;
	m_pbkImage   = NULL;
}

ManualAdjustDlg::~ManualAdjustDlg()
{
	if (m_pSmallImage!= NULL)
	{
		cvReleaseImage(&m_pSmallImage);
	}
	if (m_pbkImage!= NULL)
	{
		cvReleaseImage(&m_pbkImage);
	}
}

void ManualAdjustDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ROT, m_Rot2);
	DDV_MinMaxInt(pDX, m_Rot2, -360, 360);
	DDX_Text(pDX, IDC_EDIT_SCALE, m_Scale2);
	DDV_MinMaxInt(pDX, m_Scale2, 5, 1000);
}


BEGIN_MESSAGE_MAP(ManualAdjustDlg, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_SHOW_RESULT, &ManualAdjustDlg::OnBnClickedShowResult)

	ON_EN_CHANGE(IDC_EDIT_ROT, &ManualAdjustDlg::OnEnChangeEditRot)
END_MESSAGE_MAP()

void ManualAdjustDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	HDC hdcthis = dc.GetSafeHdc();

	int x=m_ImageRect.left;
	int y=m_ImageRect.top;
	if( m_pSmallImage != NULL && m_pSmallImage->imageData != NULL && m_pSmallImage->depth == IPL_DEPTH_8U )
	{
		uchar buffer[sizeof(BITMAPINFOHEADER) + 1024];
		BITMAPINFO* bmi = (BITMAPINFO*)buffer;
		int bmp_w = m_pSmallImage->width, bmp_h = m_pSmallImage->height;

		int bpp = m_pSmallImage ? (m_pSmallImage->depth & 255)*m_pSmallImage->nChannels : 0;
		//代替cvvimage的Bpp()函数

		FillBitmapInfo( bmi, bmp_w, bmp_h, bpp, m_pSmallImage->origin );

		SetDIBitsToDevice(	hdcthis, x, y, bmp_w, bmp_h, 0, 0, 0, bmp_h,m_pSmallImage->imageData,
			bmi, DIB_RGB_COLORS );
	}
}


BOOL ManualAdjustDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_IsImageOpen = true;
	m_OldRect.SetRectEmpty();

	m_ImgW = pDocImage->width;
	m_ImgH = pDocImage->height;

	CStatic* pic = (CStatic*)GetDlgItem(IDC_OVERVIEW_IMAGE);
	pic->GetWindowRect( &m_PicRect );
	ScreenToClient( &m_PicRect );
	m_ImageRect = m_PicRect;

	if (m_pSmallImage!= NULL)
	{
		cvReleaseImage(&m_pSmallImage);
	}
	m_pSmallImage= cvCreateImage(
		cvSize(MIN(m_ImageRect.Width(),pDocImage->width) ,
			   MIN(m_ImageRect.Height(),pDocImage->height)),
				pDocImage->depth, pDocImage->nChannels);
//	cvResize(pDocImage,m_pSmallImage);
	int xstart=0,ystart=0;
	if (m_pSmallImage->width< pDocImage->width)
	{
		xstart= (pDocImage->width- m_pSmallImage->width)/2;
	}
	if (m_pSmallImage->height< pDocImage->height)
	{
		ystart= (pDocImage->height- m_pSmallImage->height)/2;
	}
	cvSetImageROI(pDocImage, cvRect(xstart, ystart, m_pSmallImage->width,m_pSmallImage->height));
	cvCopy(pDocImage,m_pSmallImage, NULL);
	cvResetImageROI(pDocImage);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void ManualAdjustDlg::OnBnClickedShowResult()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (m_pbkImage!= NULL)
	{
		cvReleaseImage(&m_pbkImage);
	}
	
	m_pbkImage= rotateScaleImage(pDocImage, (float)m_Rot2, m_Scale2/100.0f);

	if (m_pSmallImage!= NULL)
	{
		cvReleaseImage(&m_pSmallImage);
	}
	m_pSmallImage= cvCreateImage(
		cvSize(MIN(m_ImageRect.Width(),m_pbkImage->width) ,
		MIN(m_ImageRect.Height(),m_pbkImage->height)),
		m_pbkImage->depth, m_pbkImage->nChannels);
	//	cvResize(pDocImage,m_pSmallImage);
	int xstart=0,ystart=0;
	if (m_pSmallImage->width< m_pbkImage->width)
	{
		xstart= (m_pbkImage->width- m_pSmallImage->width)/2;
	}
	if (m_pSmallImage->height< m_pbkImage->height)
	{
		ystart= (m_pbkImage->height- m_pSmallImage->height)/2;
	}
	cvSetImageROI(m_pbkImage, cvRect(xstart, ystart, m_pSmallImage->width,m_pSmallImage->height));
	cvCopy(m_pbkImage,m_pSmallImage, NULL);
	cvResetImageROI(m_pbkImage);

	Invalidate( TRUE );
}


void ManualAdjustDlg::OnEnChangeEditRot()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}
