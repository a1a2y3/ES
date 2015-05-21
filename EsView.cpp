
// EsView.cpp : CEsView 类的实现
//

#include "stdafx.h"
#include "Es.h"

#include "EsDoc.h"
#include "EsView.h"
#include "ChildFrm.h"
#include "MainFrm.h"
#include "ImageProcess.h"
#include "TargetRectDlg.h"
#include "TargetNumDlg.h"
#include "ConfirmDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEsView

IMPLEMENT_DYNCREATE(CEsView, CScrollView)

BEGIN_MESSAGE_MAP(CEsView, CScrollView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CEsView::OnFilePrintPreview)
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_SELECT_POINTS, &CEsView::OnSelectPoints)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_MANUAL_SELECT_TARGET, &CEsView::OnManualSelectTarget)
	ON_COMMAND(ID_CENTER_FROM_CORNERS, &CEsView::OnCenterFromCorners)
END_MESSAGE_MAP()

// CEsView 构造/析构

CEsView::CEsView()
{
	// TODO: 在此处添加构造代码
	m_pIsTargetSetted = NULL;
	m_pTarget       = NULL;
	m_pDialog       = NULL;
	m_bShowTargDlg  = TRUE;
	m_bDialogOK     = FALSE;
	m_bDrawLine     = FALSE;
	m_bDrawTarget   = FALSE;
	m_TargetCount   = 0;
	m_OldPt = CPoint(-1, -1);
}

CEsView::~CEsView()
{
	ReleaseMem();
}

int CEsView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	m_pDoc = GetDocument();
	m_pDoc->m_pView = this;
	return 0;
}

BOOL CEsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CScrollView::PreCreateWindow(cs);
}
void CEsView::DrawCrossOnTarget(CvPoint2D32f  Target, int TargetNo)
{
	CClientDC  dc(this);
	OnPrepareDC( &dc );
	int   R = 30;
	CPen *NewPen;
	NewPen = new CPen(PS_SOLID, 1, RGB(0, 255, 0));
	CPen *OldPen = (CPen*) dc.SelectObject(NewPen);
	int  x0 = int(Target.x + 0.5);
	int  y0 = int(Target.y + 0.5);
	dc.MoveTo(x0    , y0 - R);
	dc.LineTo(x0    , y0 + R);
	dc.MoveTo(x0 -1 , y0 - R);
	dc.LineTo(x0 -1 , y0 + R);
	dc.MoveTo(x0 +1 , y0 - R);
	dc.LineTo(x0 +1 , y0 + R);

	dc.MoveTo(x0 - R, y0    );
	dc.LineTo(x0 + R, y0    );
	dc.MoveTo(x0 - R, y0 - 1);
	dc.LineTo(x0 + R, y0 - 1);
	dc.MoveTo(x0 - R, y0 + 1);
	dc.LineTo(x0 + R, y0 + 1);

	if (TargetNo!=-1)
	{
		CString Name;
		Name.Format("T%d", TargetNo);
		dc.SetTextColor(RGB(255, 255, 0));
		dc.SetBkMode(TRANSPARENT);
		dc.TextOut(x0 + R+3, y0 - 2*R-3, Name);
		dc.SelectObject(OldPen);
	}

	DelayMessage();
	delete NewPen;
}
void CEsView::DrawMaskOnTarget(CvPoint2D32f  Target, int TargetNo)
{
	CClientDC  dc(this);
	OnPrepareDC( &dc );
	int   R = 20;
	CPen *NewPen;
	NewPen = new CPen(PS_SOLID, 1, RGB(255, 0, 0));
	CPen *OldPen = (CPen*) dc.SelectObject(NewPen);
	int  x0 = int(Target.x + 0.5);
	int  y0 = int(Target.y + 0.5);
	// 十字丝
	dc.MoveTo(x0    , y0 - R);
	dc.LineTo(x0    , y0 + R);
	//dc.MoveTo(x0 -1 , y0 - R);
	//dc.LineTo(x0 -1 , y0 + R);
	//dc.MoveTo(x0 +1 , y0 - R);
	//dc.LineTo(x0 +1 , y0 + R);

	dc.MoveTo(x0 - R, y0    );
	dc.LineTo(x0 + R, y0    );
	//dc.MoveTo(x0 - R, y0 - 1);
	//dc.LineTo(x0 + R, y0 - 1);
	//dc.MoveTo(x0 - R, y0 + 1);
	//dc.LineTo(x0 + R, y0 + 1);

	// 外框
	dc.MoveTo(x0-R-10    , y0 - R-5);
	dc.LineTo(x0+R+10    , y0 - R-5);
	dc.MoveTo(x0-R-10    , y0 - R-4);
	dc.LineTo(x0+R+10    , y0 - R-4);
	dc.MoveTo(x0-R-10    , y0 - R-6);
	dc.LineTo(x0+R+10    , y0 - R-6);
	dc.MoveTo(x0-R-10    , y0 - R-5);
	dc.LineTo(x0-R-10    , y0 - R+5);
	dc.MoveTo(x0-R-9    , y0 - R-5);
	dc.LineTo(x0-R-9    , y0 - R+5);
	dc.MoveTo(x0-R-11    , y0 - R-5);
	dc.LineTo(x0-R-11    , y0 - R+5);
	dc.MoveTo(x0+R+10    , y0 - R-5);
	dc.LineTo(x0+R+10    , y0 - R+5);
	dc.MoveTo(x0+R+9    , y0 - R-5);
	dc.LineTo(x0+R+9    , y0 - R+5);
	dc.MoveTo(x0+R+11    , y0 - R-5);
	dc.LineTo(x0+R+11    , y0 - R+5);

	dc.MoveTo(x0-R-10    , y0+ R+5);
	dc.LineTo(x0+R+10    , y0+ R+5);
	dc.MoveTo(x0-R-10    , y0+ R+4);
	dc.LineTo(x0+R+10    , y0+ R+4);
	dc.MoveTo(x0-R-10    , y0+ R+6);
	dc.LineTo(x0+R+10    , y0+ R+6);
	dc.MoveTo(x0-R-10    , y0+ R-5);
	dc.LineTo(x0-R-10    , y0+ R+5);
	dc.MoveTo(x0-R-9    , y0+ R-5);
	dc.LineTo(x0-R-9    , y0+ R+5);
	dc.MoveTo(x0-R-11    , y0+ R-5);
	dc.LineTo(x0-R-11    , y0+ R+5);
	dc.MoveTo(x0+R+10    , y0+ R-5);
	dc.LineTo(x0+R+10    , y0+ R+5);
	dc.MoveTo(x0+R+9    , y0+ R-5);
	dc.LineTo(x0+R+9    , y0+ R+5);
	dc.MoveTo(x0+R+11    , y0+ R-5);
	dc.LineTo(x0+R+11    , y0+ R+5);

	if (TargetNo!=-1)
	{
		CString Name;
		Name.Format("T%d", TargetNo);
		dc.SetTextColor(RGB(255, 255, 0));
		dc.SetBkMode(TRANSPARENT);
		dc.TextOut(x0 + R+3, y0 - 2*R-3, Name);
		dc.SelectObject(OldPen);
	}

	DelayMessage();
	delete NewPen;
}
void CEsView::DrawRectOnTarget(CvPoint2D32f  Target, int TargetNo)
{
	CClientDC  dc(this);
	OnPrepareDC( &dc );
	int   R = 10;
	CPen *NewPen;
	NewPen = new CPen(PS_SOLID, 1, RGB(255, 255, 0));
	CPen *OldPen = (CPen*) dc.SelectObject(NewPen);
	int  x0 = int(Target.x + 0.5);
	int  y0 = int(Target.y + 0.5);
	dc.MoveTo(x0 - R, y0 - R);
	dc.LineTo(x0 - R, y0 + R);
	dc.LineTo(x0 + R, y0 + R);
	dc.LineTo(x0 + R, y0 - R);
	dc.LineTo(x0 - R, y0 - R);
	R -= 1;
	dc.MoveTo(x0 - R, y0 - R);
	dc.LineTo(x0 - R, y0 + R);
	dc.LineTo(x0 + R, y0 + R);
	dc.LineTo(x0 + R, y0 - R);
	dc.LineTo(x0 - R, y0 - R);
	R -= 1;
	dc.MoveTo(x0 - R, y0 - R);
	dc.LineTo(x0 - R, y0 + R);
	dc.LineTo(x0 + R, y0 + R);
	dc.LineTo(x0 + R, y0 - R);
	dc.LineTo(x0 - R, y0 - R);

	CString Name;
	Name.Format("T%d", TargetNo);
	dc.SetTextColor(RGB(255, 255, 0));
	dc.SetBkMode(TRANSPARENT);
	dc.TextOut(x0 + R+3, y0 - 2*R-3, Name);
	dc.SelectObject(OldPen);
	DelayMessage();
	delete NewPen;
	m_pDialog[TargetNo]->SetTargetPos(Target);
}
void CEsView::OnDraw(CDC* pDC)
{
	CEsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	if (!pDoc->m_pImage)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	HDC hdcthis = pDC->GetSafeHdc();
	int x=0;
	int y=0;
	int w=70000;//3500;//1024;   
	int h=70000;//2400;//768;
	if( pDoc->m_pImage != NULL && pDoc->m_pImage->imageData != NULL && pDoc->m_pImage->depth == IPL_DEPTH_8U )
	{
		uchar buffer[sizeof(BITMAPINFOHEADER) + 1024];
		BITMAPINFO* bmi = (BITMAPINFO*)buffer;
		int bmp_w = pDoc->m_pImage->width, bmp_h = pDoc->m_pImage->height;

		int bpp = pDoc->m_pImage ? (pDoc->m_pImage->depth & 255)*pDoc->m_pImage->nChannels : 0;
		//代替cvvimage的Bpp()函数

		FillBitmapInfo( bmi, bmp_w, bmp_h, bpp, pDoc->m_pImage->origin );

		int from_x = MIN( MAX( 0, 0 ), bmp_w - 1 );
		int from_y = MIN( MAX( 0, 0 ), bmp_h - 1 );

		int sw = MAX( MIN( bmp_w - from_x, w ), 0 );
		int sh = MAX( MIN( bmp_h - from_y, h ), 0 );

		SetDIBitsToDevice(
			hdcthis, x, y, sw, sh, from_x, from_y, from_y, sh,
			pDoc->m_pImage->imageData + from_y*pDoc->m_pImage->widthStep,
			bmi, DIB_RGB_COLORS );
	}
	//显示目标
	if (assertBorder(m_pDoc->m_targetPosition.x, 0.0f, m_pDoc->m_pImage->width-1.0f)&&
		assertBorder(m_pDoc->m_targetPosition.y, 0.0f, m_pDoc->m_pImage->height-1.0f))
	{
		DrawMaskOnTarget(m_pDoc->m_targetPosition);
	}
	// 放大显示	
	GetClientRect(&m_DialogRect);
	ClientToScreen(m_DialogRect);	
	CRect tmpRect = CRect(m_DialogRect.left, m_DialogRect.bottom, m_DialogRect.left + 200, m_DialogRect.bottom+200);
	if(!m_bDialogOK)
		return;
	CPoint point1;
	CRect mainRect;
	CMainFrame* pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	pMain->GetClientRect(&mainRect);
	pMain->ClientToScreen(mainRect);	
	for(int i= 0; i< m_TargetCount; i++)
	{
		m_pDialog[i]->ShowWindow(tmpRect.left-mainRect.left, tmpRect.top-mainRect.top); 
		tmpRect.OffsetRect(205, 0);
		if(tmpRect.right> m_DialogRect.left+830)
		{
			tmpRect = CRect(m_DialogRect.left, m_DialogRect.bottom, m_DialogRect.left + 200, m_DialogRect.bottom+200);
			tmpRect.OffsetRect(0, 205);
		}
		if (m_pIsTargetSetted[i])
		{
			DrawRectOnTarget(m_pTarget[i], i);
		}		
	}
}
//支持Scroll
void CEsView::OnInitialUpdate()
{
	CEsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->m_pView = this;
	CSize ScrollSize;
	if(pDoc->m_pImage == NULL)
	{
		ScrollSize = CSize(100, 100);
	}
	else 
	{	
		int cxDIB = (int)pDoc->m_pImage->width;//+10;   
		int cyDIB = (int)pDoc->m_pImage->height;//+10;  

		ScrollSize = CSize(cxDIB, cyDIB); 
	}
	SetScrollSizes(MM_TEXT, ScrollSize);
	ResizeParentToFit(); 
//	CScrollView::OnInitialUpdate();
}

// CEsView 打印


void CEsView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CEsView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CEsView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CEsView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CEsView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CEsView::OnContextMenu(CWnd* pWnd, CPoint point)
{
//	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CEsView 诊断

#ifdef _DEBUG
void CEsView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CEsView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CEsDoc* CEsView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEsDoc)));
	return (CEsDoc*)m_pDocument;
}
#endif //_DEBUG

void CEsView::ResizeWindow()
{
	CScrollView::OnInitialUpdate();
	m_pDoc = GetDocument();
	ASSERT_VALID(m_pDoc);
	int w = int( m_pDoc->m_pImage->width );
	int h = int( m_pDoc->m_pImage->height );

	if( w <= 0 )
		SetScrollSizes( MM_TEXT, CSize(100, 100) );
	else
		SetScrollSizes( MM_TEXT, CSize(w, h) );

	CChildFrame* pFrame = (CChildFrame*)GetParentFrame();
	pFrame->ShowWindow( SW_SHOWNORMAL );
	if (w>0&&w<=1000&&h>0&&h<=800)
	{
		ResizeParentToFit(false); 
	}
	else
	{
		ResizeParentToFit(true); 
	}
	
}

void CEsView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CClientDC dc( this );
	OnPrepareDC( &dc );
	dc.DPtoLP( &point );	

	CEsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CMFCStatusBar *pSB;
	if(pDoc->m_pImage)
	{
		if(pDoc->m_pImage->imageData!=NULL && CheckValid(point.x,point.y,pDoc->m_pImage->width,pDoc->m_pImage->height))
		{
			CString str;
			int temp;
			temp = *((uchar*)pDoc->m_pImage->imageData+ point.y*pDoc->m_pImage->widthStep+ point.x);
			str.Format("鼠标位置:(%4d, %4d), 灰度图像 灰度值= %4d, 图像宽度%5d, 图像高度%5d\n", 
					point.x, point.y, temp,pDoc->m_pImage->width,pDoc->m_pImage->height );
			pSB = ( CMFCStatusBar*)AfxGetApp()->m_pMainWnd-> GetDescendantWindow(AFX_IDW_STATUS_BAR);
			pSB->SetPaneText(0, str);
		}
	}
	// 选点	
	m_MousePt = point;
	if(m_pDoc->m_pImage == NULL)
		return;	
	int nDrawMode = dc.SetROP2(R2_WHITE | R2_NOT) ;	
	int j;
	if(m_bDrawLine||m_bDrawTarget)
	{
		for(j= 0; j< m_TargetCount; j++)
		{
			if(m_pIsTargetSetted[j])
				continue;
			else
				break;
		}
		if(j< m_TargetCount)
		{	
			CPen m_DashPen(PS_DASH, 1, RGB(255, 0, 0));
			CPen * pOldPen = (CPen*)dc.SelectObject(&m_DashPen);
			int nHeight = m_pDoc->m_pImage->height;
			int nWidth  = m_pDoc->m_pImage->width;
			if(m_OldPt.x >= 0)
			{
				dc.MoveTo (m_OldPt.x, 0);
				dc.LineTo (m_OldPt.x, m_OldPt.y-16);
				dc.MoveTo (m_OldPt.x, m_OldPt.y+16);
				dc.LineTo (m_OldPt.x, nHeight);
				dc.MoveTo (0, m_OldPt.y);
				dc.LineTo (m_OldPt.x-16, m_OldPt.y);
				dc.MoveTo (m_OldPt.x+16, m_OldPt.y);
				dc.LineTo (nWidth, m_OldPt.y);
			}
			m_OldPt = point;
			dc.MoveTo (m_OldPt.x, 0);
			dc.LineTo (m_OldPt.x, m_OldPt.y-16);
			dc.MoveTo (m_OldPt.x, m_OldPt.y+16);
			dc.LineTo (m_OldPt.x, nHeight);
			dc.MoveTo (0, m_OldPt.y);
			dc.LineTo (m_OldPt.x-16, m_OldPt.y);
			dc.MoveTo (m_OldPt.x+16, m_OldPt.y);
			dc.LineTo (nWidth, m_OldPt.y);
			dc.SelectObject(pOldPen);	
			m_pDialog[j]->SetTargetPos(cvPoint2D32f(point.x, point.y));
		}
	}
	dc.SetROP2(nDrawMode);
	CScrollView::OnMouseMove(nFlags, point);
}

void CEsView::ReleaseMem()
{
	if(m_pIsTargetSetted!= NULL)
	{
		delete []m_pIsTargetSetted;
		m_pIsTargetSetted = NULL;
	}
	if(m_pTarget!= NULL)
	{
		delete []m_pTarget;
		m_pTarget = NULL;
	}
	if(m_pDialog!= NULL)
	{
		for(int i= 0; i< m_TargetCount; i++)
		{
			m_pDialog[i]->DestroyWindow();
			delete []m_pDialog[i];
		}
		delete []m_pDialog;
		m_pDialog = NULL;
	}
	m_TargetCount = 0;
}
void CEsView::InitiateTarget(int TargetCount)
{
	m_bDialogOK = FALSE;
	ReleaseMem();
	m_TargetCount = TargetCount;
	m_pIsTargetSetted = new BOOL[m_TargetCount];
	m_pTarget       = new CvPoint2D32f[m_TargetCount];
	m_pDialog       = new CTargetRectDlg*[m_TargetCount];

	//CRect  tmpRect	= CRect(m_DialogRect.left, m_DialogRect.bottom - 200, m_DialogRect.left + 200, m_DialogRect.bottom);
	//if(tmpRect.bottom> 740)
	//	tmpRect = CRect(m_DialogRect.left, 540, m_DialogRect.left + 200, 740);

	for(int i= 0; i< m_TargetCount; i++)
	{
		m_pIsTargetSetted[i] = FALSE;
		m_pDialog[i] = new CTargetRectDlg(this, m_pDoc->m_pImage, 201, 201, i);
		//m_pDialog[i]->ShowWindow(tmpRect.left, tmpRect.bottom-5);
		//m_pDialog[i]->EnableShow(m_bShowTargDlg);
		DelayMessage();
		//tmpRect.OffsetRect(205, 0);
		//if(tmpRect.right> m_DialogRect.left+620)
		//{
		//	tmpRect	= CRect(m_DialogRect.left, m_DialogRect.bottom - 200, m_DialogRect.left + 200, m_DialogRect.bottom);
		//	tmpRect.OffsetRect(0, 205);
		//}
	}
	m_bDialogOK = TRUE;	
}
void CEsView::OnManualSelectTarget()  // 选目标中心
{
	// TODO: Add your command handler code here
	InitiateTarget(1);
	Invalidate();
	DelayMessage();
	m_OldPt = CPoint(-1,-1);
	int  i;
	m_bDrawTarget = TRUE;
	CRect  Rect(0, 0, m_pDoc->m_pImage->width, m_pDoc->m_pImage->height);
	ClientToScreen(Rect);
	ClipCursor(Rect);
	SetCursorPos(Rect.left, Rect.top);
	m_bTargetSetCanceld = FALSE;
	MSG msg;
	do
	{
		PeekMessage(&msg, m_hWnd, 0 , 0, PM_REMOVE);
		::SendMessage(m_hWnd, msg.message, msg.wParam, msg.lParam);
		msg.message = 0;
		if(m_bTargetSetCanceld)
			break;
		for(i= 0; i< m_TargetCount; i++)
		{
			if(m_pIsTargetSetted[i])
				continue;
			else
				break;
		}
		SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR1));
	}while(i< m_TargetCount);
	m_bDrawTarget = FALSE;
	ClipCursor(NULL);	
	ReleaseMem();

	//CClientDC dc( this );
	//OnPrepareDC( &dc );
	//int nDrawMode = dc.SetROP2(R2_WHITE | R2_NOT) ;	
	//int nHeight = m_pDoc->m_pImage->height;
	//int nWidth  = m_pDoc->m_pImage->width;
	//m_OldPt = m_MousePt;
	//dc.MoveTo (m_OldPt.x, 0);
	//dc.LineTo (m_OldPt.x, m_OldPt.y - 16);
	//dc.MoveTo (m_OldPt.x, m_OldPt.y + 16);
	//dc.LineTo (m_OldPt.x, nHeight);
	//dc.MoveTo (0, m_OldPt.y);
	//dc.LineTo (m_OldPt.x - 16, m_OldPt.y);
	//dc.MoveTo (m_OldPt.x + 16, m_OldPt.y);
	//dc.LineTo (nWidth, m_OldPt.y);
	//dc.SetROP2(nDrawMode);
}
void CEsView::OnCenterFromCorners()   // 选4点，用于找目标中心
{
	// TODO: Add your command handler code here
	CConfirmDlg dlg;
	dlg.m_sText="对选点满意？";
	while (1)
	{
		InitiateTarget(4);
		Invalidate();
		DelayMessage();
		m_OldPt = CPoint(-1,-1);
		int  i;
		m_bDrawLine = TRUE;
		CRect  Rect(0, 0, m_pDoc->m_pImage->width, m_pDoc->m_pImage->height);
		ClientToScreen(Rect);
		ClipCursor(Rect);
		SetCursorPos(Rect.left, Rect.top);
		m_bTargetSetCanceld = FALSE;
		MSG msg;
		do
		{
			PeekMessage(&msg, m_hWnd, 0 , 0, PM_REMOVE);
			::SendMessage(m_hWnd, msg.message, msg.wParam, msg.lParam);
			msg.message = 0;
			if(m_bTargetSetCanceld)
				break;
			for(i= 0; i< m_TargetCount; i++)
			{
				if(m_pIsTargetSetted[i])
					continue;
				else
					break;
			}
			SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR1));
		}while(i< m_TargetCount);
		m_bDrawLine = FALSE;
		float cx=0,cy=0;
		for (i=0;i<4;i++)
		{
			cx+= m_pTarget[i].x;
			cy+= m_pTarget[i].y;
		}
		m_pDoc->m_targetPosition.x = cx/4;
		m_pDoc->m_targetPosition.y = cy/4;
		Invalidate();
		ClipCursor(NULL);
		if( dlg.DoModal() == IDOK )
		{
			for (i=0;i<4;i++)
			{
				m_pIsTargetSetted[i]=FALSE;
			}					
			ReleaseMem();
			break;
		}
		else
		{
			m_pDoc->m_targetPosition=cvPoint2D32f(-1,-1);
			ReleaseMem();
		}
}
	
	
	//CClientDC dc( this );
	//OnPrepareDC( &dc );
	//int nDrawMode = dc.SetROP2(R2_WHITE | R2_NOT) ;	
	//int nHeight = m_pDoc->m_pImage->height;
	//int nWidth  = m_pDoc->m_pImage->width;
	//m_OldPt = m_MousePt;
	//dc.MoveTo (m_OldPt.x, 0);
	//dc.LineTo (m_OldPt.x, m_OldPt.y - 16);
	//dc.MoveTo (m_OldPt.x, m_OldPt.y + 16);
	//dc.LineTo (m_OldPt.x, nHeight);
	//dc.MoveTo (0, m_OldPt.y);
	//dc.LineTo (m_OldPt.x - 16, m_OldPt.y);
	//dc.MoveTo (m_OldPt.x + 16, m_OldPt.y);
	//dc.LineTo (nWidth, m_OldPt.y);
	//dc.SetROP2(nDrawMode);
}
void CEsView::OnSelectPoints() // 选3点，用于手工匹配
{
	// TODO: Add your command handler code here
	InitiateTarget(3);
	Invalidate();
	DelayMessage();
	m_OldPt = CPoint(-1,-1);
	int  i;
	m_bDrawLine = TRUE;
	CRect  Rect(0, 0, m_pDoc->m_pImage->width, m_pDoc->m_pImage->height);
	ClientToScreen(Rect);
	ClipCursor(Rect);
	SetCursorPos(Rect.left, Rect.top);
	m_bTargetSetCanceld = FALSE;
	MSG msg;
	do
	{
		PeekMessage(&msg, m_hWnd, 0 , 0, PM_REMOVE);
		::SendMessage(m_hWnd, msg.message, msg.wParam, msg.lParam);
		msg.message = 0;
		if(m_bTargetSetCanceld)
			break;
		for(i= 0; i< m_TargetCount; i++)
		{
			if(m_pIsTargetSetted[i])
				continue;
			else
				break;
		}
		SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR1));
	}while(i< m_TargetCount);
	m_bDrawLine = FALSE;
	ClipCursor(NULL);		

	//CClientDC dc( this );
	//OnPrepareDC( &dc );
	//int nDrawMode = dc.SetROP2(R2_WHITE | R2_NOT) ;	
	//int nHeight = m_pDoc->m_pImage->height;
	//int nWidth  = m_pDoc->m_pImage->width;
	//m_OldPt = m_MousePt;
	//dc.MoveTo (m_OldPt.x, 0);
	//dc.LineTo (m_OldPt.x, m_OldPt.y - 16);
	//dc.MoveTo (m_OldPt.x, m_OldPt.y + 16);
	//dc.LineTo (m_OldPt.x, nHeight);
	//dc.MoveTo (0, m_OldPt.y);
	//dc.LineTo (m_OldPt.x - 16, m_OldPt.y);
	//dc.MoveTo (m_OldPt.x + 16, m_OldPt.y);
	//dc.LineTo (nWidth, m_OldPt.y);
	//dc.SetROP2(nDrawMode);
}
void CEsView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CClientDC dc( this );
	OnPrepareDC( &dc );
	dc.DPtoLP( &point );
	int j;
	if(m_bDrawLine) // 选辅助点
	{
		for(j= 0; j< m_TargetCount; j++)
		{
			if(m_pIsTargetSetted[j])
				continue;
			else
				break;
		}
		if(j== m_TargetCount)
			return;
		m_pTarget[j] = cvPoint2D32f(point.x, point.y);
		m_pIsTargetSetted[j] = true;
		DrawRectOnTarget(m_pTarget[j], j);
	}
	if(m_bDrawTarget) // 选目标点
	{
		for(j= 0; j< m_TargetCount; j++)
		{
			if(m_pIsTargetSetted[j])
				continue;
			else
				break;
		}
		if(j== m_TargetCount)
			return;
		m_pTarget[j] = cvPoint2D32f(point.x, point.y);
		m_pIsTargetSetted[j] = true;
		m_pDoc->m_targetPosition = cvPoint2D32f(point.x, point.y);
		Invalidate();
	}
	CScrollView::OnLButtonDown(nFlags, point);
}

void CEsView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CClientDC dc( this );
	OnPrepareDC( &dc );
	dc.DPtoLP( &point );	
	ReleaseCapture();
	CScrollView::OnLButtonUp(nFlags, point);
}

void CEsView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CPoint tmpPt; 
	GetCursorPos(&tmpPt);
	switch(nChar)
	{
	case VK_ESCAPE:
		m_bTargetSetCanceld = TRUE;
		m_pDoc->m_isTrackRunning= FALSE;
		break;
	case VK_UP:
		tmpPt.y= tmpPt.y - 1;
		SetCursorPos(tmpPt.x, tmpPt.y);
		break;
	case VK_DOWN:
		tmpPt.y= tmpPt.y + 1;
		SetCursorPos(tmpPt.x, tmpPt.y);
		break;
	case VK_LEFT:
		tmpPt.x= tmpPt.x - 1;
		SetCursorPos(tmpPt.x, tmpPt.y );
		break;
	case VK_RIGHT:
		tmpPt.x= tmpPt.x + 1;
		SetCursorPos(tmpPt.x, tmpPt.y);
		break;
	default:
		break;
	}
	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}


