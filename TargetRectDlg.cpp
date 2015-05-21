// TargetRectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Es.h"
#include "TargetRectDlg.h"
#include "ImageProcess.h"


// CTargetRectDlg dialog

IMPLEMENT_DYNAMIC(CTargetRectDlg, CDialog)

CTargetRectDlg::CTargetRectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTargetRectDlg::IDD, pParent)
{
	m_pImage= NULL;
	m_pScaledImage= NULL;
}
CTargetRectDlg::CTargetRectDlg(CEsView  *pView, IplImage *pInputImg, int WindowW, int WindowH, int TargetNo)
{
	m_WindowW = WindowW;
	m_WindowH = WindowH;
	m_pView   = pView;
	m_pImage   = pInputImg;
	m_TargetNo = TargetNo;
	Create(IDD_RIGHTBAR, NULL);
	m_MagnifyCoeff = 4.0f;
	m_TargetPos = cvPoint2D32f(m_pImage->width / 2, m_pImage->height / 2);
	m_pScaledImage= cvCreateImage(cvSize(m_WindowW,m_WindowH),8,1);	
}
CTargetRectDlg::~CTargetRectDlg()
{
	if (m_pScaledImage!= NULL)
	{
		cvReleaseImage(&m_pScaledImage);
	}
}

void CTargetRectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTargetRectDlg, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CTargetRectDlg message handlers
void CTargetRectDlg::getScaledImage(IplImage* srcImg, IplImage* dstImg, float targetX, float targetY, float mag)
{
	int i,j,wstep=dstImg->widthStep,dw=dstImg->width,dh=dstImg->height;
	float si,sj;
	BYTE* dstData= (BYTE*)dstImg->imageData;
	BYTE* srcData= (BYTE*)srcImg->imageData;
	for (i=0;i<dh;i++)
	{
		for (j=0;j<dw;j++)
		{
			si= ( targetY+(i-dh/2)/mag );    
			sj= ( targetX+(j-dw/2)/mag );
			*(dstData+i*wstep+j)= (BYTE) GetGrayV(srcImg,sj,si);  
		}
	}
}
void CTargetRectDlg::SetImage(IplImage *pImg)
{
	m_pImage= pImg;
}
void CTargetRectDlg::ShowWindow(int x0, int y0)
{
	m_ShowPos = CPoint(x0, y0);
	SetWindowPos(NULL, m_ShowPos.x, m_ShowPos.y, m_WindowW, m_WindowH, SWP_SHOWWINDOW);
}
void CTargetRectDlg::MoveWindow(int x0, int y0)
{
	m_ShowPos = CPoint(x0, y0);
	CDialog::MoveWindow(m_ShowPos.x, m_ShowPos.y, m_WindowW, m_WindowH);
}
void CTargetRectDlg::SetTargetPos(CvPoint2D32f TargetPos)
{
	m_TargetPos = TargetPos;
	Invalidate(NULL);
	DelayMessage();
}
void CTargetRectDlg::EnableShow(BOOL Flag)
{
	if(Flag)
		SetWindowPos(NULL, m_ShowPos.x, m_ShowPos.y, m_WindowW, m_WindowH, SWP_SHOWWINDOW);
	else
		SetWindowPos(NULL, m_ShowPos.x, m_ShowPos.y, m_WindowW, m_WindowH, SWP_HIDEWINDOW);
}

void CTargetRectDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	if(m_pImage == NULL)
		return;
	CRect rect;
	GetClientRect(&rect);
	getScaledImage(m_pImage, m_pScaledImage, m_TargetPos.x, m_TargetPos.y, m_MagnifyCoeff);
	// show m_pScaledImage
	HDC hdcthis = dc.GetSafeHdc();
	if( m_pScaledImage != NULL && m_pScaledImage->imageData != NULL && m_pScaledImage->depth == IPL_DEPTH_8U )
	{
		uchar buffer[sizeof(BITMAPINFOHEADER) + 1024];
		BITMAPINFO* bmi = (BITMAPINFO*)buffer;
		int bmp_w = m_pScaledImage->width, bmp_h = m_pScaledImage->height;

		int bpp = m_pScaledImage ? (m_pScaledImage->depth & 255)*m_pScaledImage->nChannels : 0;
		//´úÌæcvvimageµÄBpp()º¯Êý

		FillBitmapInfo( bmi, bmp_w, bmp_h, bpp, m_pScaledImage->origin );

		SetDIBitsToDevice(	hdcthis, 0, 0, bmp_w, bmp_h, 0, 0, 0, bmp_h,m_pScaledImage->imageData,
			bmi, DIB_RGB_COLORS );
	}
	//////////////////////////////////////////////////////////////////////////
	CPen *NewPen;
	NewPen = new CPen(PS_SOLID, 1, RGB(255, 0, 0));
	CPen *OldPen = (CPen*) dc.SelectObject(NewPen);
	int CX = (rect.left + rect.right) / 2;
	int CY = (rect.top  + rect.bottom) / 2;
	int width = 15;
	dc.MoveTo(CX - width, CY - width);
	dc.LineTo(CX + width, CY - width);
	dc.LineTo(CX + width, CY + width);
	dc.LineTo(CX - width, CY + width);
	dc.LineTo(CX - width, CY - width);
	dc.MoveTo(CX + width, CY - width);
	dc.LineTo(CX + 2 * width, CY - 2 * width);
	CString str;
	str.Format("T%d", m_TargetNo);
	dc.SetTextColor(RGB(255, 0, 0));
	dc.SetBkMode(TRANSPARENT);

	dc.TextOut(CX + 2 * width, CY - 2 * width - 7, str);
	dc.SelectObject(OldPen);
	delete NewPen;
}