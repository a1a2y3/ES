
// MainFrm.cpp : CMainFrame 类的实现

#include "stdafx.h"
#include "Es.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "EsDoc.h"
#include "EsView.h"
#include "IncludeFile.h"
#include "BlockMatchDirSub.h"
#include "BlockHistMatch.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_MULTI_MATCH, &CMainFrame::OnMultiMatch)
	ON_COMMAND(ID_MANUAL_POINT_ALIGN, &CMainFrame::OnManualPointAlign)
	ON_COMMAND(ID_AUTO_MATCH, &CMainFrame::OnAutoMatch)
	ON_COMMAND(ID_SELECT_TARGET, &CMainFrame::OnSelectTarget)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2005);
	m_targetMode= 0;// 小图中心
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;
	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// prevent the menu bar from taking the focus on activation
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_TOOLBAR1))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // fail to create
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// Allow user-defined toolbars operations:
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these five lines if you don't want the toolbar and menubar to be dockable
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Enable enhanced windows management dialog
	EnableWindowsDialog(ID_WINDOW_MANAGER, IDS_WINDOWS_MANAGER, TRUE);

	// Enable toolbar and docking window menu replacement
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// enable quick (Alt+drag) toolbar customization
	//CMFCToolBar::EnableQuickCustomization();

	//if (CMFCToolBar::GetUserImages() == NULL)
	//{
	//	// load user-defined toolbar images
	//	if (m_UserImages.Load(_T(".\\UserImages.bmp")))
	//	{
	//		m_UserImages.SetImageSize(CSize(16, 15), FALSE);
	//		CMFCToolBar::SetUserImages(&m_UserImages);
	//	}
	//}

	// enable menu personalization (most-recently used commands)
	// TODO: define your own basic commands, ensuring that each pulldown menu has at least one basic command.
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);

	//CMFCToolBar::SetBasicCommands(lstBasicCommands);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 基类将执行真正的工作

	if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// enable customization button for all user toolbars
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}

BOOL CMainFrame::GetDocFromPOSITION(void** ppLarge, void** ppSmall)
{
	CEsDoc  *pFirstDoc , *pSecondDoc;
	POSITION pos = theApp.pDocTemplate->GetFirstDocPosition();
	if (pos== NULL)
	{
		AfxMessageBox("请打开两幅图像!");
		return 0; 
	}
	pFirstDoc = (CEsDoc*)theApp.pDocTemplate->GetNextDoc(pos); 
	if (pos== NULL)
	{
		AfxMessageBox("请打开两幅图像!");
		return 0; 
	}
	pSecondDoc = (CEsDoc*)theApp.pDocTemplate->GetNextDoc(pos); 
	if (pos!= NULL)
	{
		AfxMessageBox("请勿打开多于两幅图像!");
		return 0; 
	}

	if( (pFirstDoc->m_pImage->width - pSecondDoc->m_pImage->width) * 
		(pFirstDoc->m_pImage->height - pSecondDoc->m_pImage->height) <= 0 )
	{
		AfxMessageBox("图像尺寸有误，A图长宽都必须小于B图长宽!");
		return 0;
	}
	CEsDoc  *pLargeDoc, *pSmallDoc;
	if (pFirstDoc->m_pImage->width < pSecondDoc->m_pImage->width)
	{
		pLargeDoc= pSecondDoc;
		pSmallDoc= pFirstDoc;
	} 
	else
	{
		pLargeDoc= pFirstDoc;
		pSmallDoc= pSecondDoc;		
	}	
	*ppLarge= (void*)pLargeDoc;
	*ppSmall= (void*)pSmallDoc;
	return 1;
}
// 异源图匹配2D平移
void CMainFrame::OnMultiMatch()
{
	// TODO: Add your command handler code here
	CEsDoc  *pLargeDoc, *pSmallDoc;
	if( GetDocFromPOSITION((void**)&pLargeDoc, (void**)&pSmallDoc)!=1)
		return;
	if (m_targetMode==0)     // 小图中心
	{
		m_targetMode=3;
		pSmallDoc->m_targetPosition= cvPoint2D32f(pSmallDoc->m_pImage->width/2,pSmallDoc->m_pImage->height/2);
		pLargeDoc->m_targetPosition= cvPoint2D32f(-1.0f,-1.0f);
	}
	// 备份图像
	if (pLargeDoc->bk!= NULL)
	{
		cvReleaseImage(&pLargeDoc->bk);
	}
	pLargeDoc->bk = cvCloneImage(pLargeDoc->m_pImage);
	if (pSmallDoc->bk!= NULL)
	{
		cvReleaseImage(&pSmallDoc->bk);
	}
	pSmallDoc->bk = cvCloneImage(pSmallDoc->m_pImage);
	//  开始匹配评估
	cvDestroyAllWindows();
	CBlockMatchDirSub bmds;
	bmds.InputImage(pLargeDoc->m_pImage,pSmallDoc->m_pImage);	
	bmds.SimpleMatch2D();
	bmds.GetMatchResult(m_targetMode,&(pLargeDoc->m_targetPosition),&(pSmallDoc->m_targetPosition));

	//结果输出	
	CString filePath,largefileName,smallfilename;
	GetFolderPathFromFilePath(pLargeDoc->m_strFileName,filePath);
	GetFileNameFromFilePath(pLargeDoc->m_strFileName,largefileName);
	GetFileNameFromFilePath(pSmallDoc->m_strFileName,smallfilename);

	bmds.ShowMatch(filePath,"匹配结果_2D.jpg");

	FILE *fp = fopen(filePath+"\\匹配结果_2D.txt", "w" );
	if (m_targetMode<=2)
	{
		fprintf(fp,"大基准图模式：\n");
		pSmallDoc->m_pView->SetFocus();
	}
	else
	{
		fprintf(fp,"小基准图模式：\n");
		pLargeDoc->m_pView->SetFocus();
	}

	fprintf(fp,"大图目标位置：");
	fprintf(fp,"(%5.1f, %5.1f)    ",pLargeDoc->m_targetPosition.x,pLargeDoc->m_targetPosition.y);
	fprintf(fp,largefileName);
	fprintf(fp,"\n");	

	fprintf(fp,"小图目标位置：");
	fprintf(fp,"(%5.1f, %5.1f)    ",pSmallDoc->m_targetPosition.x,pSmallDoc->m_targetPosition.y);
	fprintf(fp,smallfilename);
	fprintf(fp,"\n");
	fclose(fp);

	pSmallDoc->m_pView->Invalidate(false);
	pLargeDoc->m_pView->Invalidate(false);
	AfxMessageBox("结果保存在\n"+filePath);
}
// 手选点对计算变形参数
void CMainFrame::OnManualPointAlign()
{
	// TODO: Add your command handler code here
	CEsDoc  *pLargeDoc, *pSmallDoc;
	if( GetDocFromPOSITION((void**)&pLargeDoc, (void**)&pSmallDoc)!=1)
		return;
	if (m_targetMode==0)     // 小图中心
	{
		m_targetMode=3;
		pSmallDoc->m_targetPosition= cvPoint2D32f(pSmallDoc->m_pImage->width/2,pSmallDoc->m_pImage->height/2);
		pLargeDoc->m_targetPosition= cvPoint2D32f(-1.0f,-1.0f);
	}
	//if (pLargeDoc->m_pView->m_TargetCount!=3 || pSmallDoc->m_pView->m_TargetCount!=3)
	//{
	//	AfxMessageBox("请分别在两图上选取3组对应点!");
	//	return;
	//}
	// 备份图像
	if (pLargeDoc->bk!= NULL)
	{
		cvReleaseImage(&pLargeDoc->bk);
	}
	pLargeDoc->bk = cvCloneImage(pLargeDoc->m_pImage);
	if (pSmallDoc->bk!= NULL)
	{
		cvReleaseImage(&pSmallDoc->bk);
	}
	pSmallDoc->bk = cvCloneImage(pSmallDoc->m_pImage);
	//  开始匹配评估
	cvDestroyAllWindows();
	CBlockMatchDirSub bmds;
	bmds.InputImage(pLargeDoc->m_pImage,pSmallDoc->m_pImage);	
//	bmds.ManualPointMatch(pLargeDoc->m_pView->m_pTarget,pSmallDoc->m_pView->m_pTarget); bmds.saveHmat("~~~~~~~~~临时文件-hmat.txt");
	bmds.loadHmat("~~~~~~~~~临时文件-hmat.txt");
	//结果输出	
	CString filePath,largefileName,smallfilename;
	GetFolderPathFromFilePath(pLargeDoc->m_strFileName,filePath);
	GetFileNameFromFilePath(pLargeDoc->m_strFileName,largefileName);
	GetFileNameFromFilePath(pSmallDoc->m_strFileName,smallfilename);
	FILE *fp = fopen(filePath+"\\匹配结果_6D.txt", "w" );
	if (m_targetMode<=2)
	{
		bmds.GetMatchResult(m_targetMode,&(pLargeDoc->m_targetPosition),&(pSmallDoc->m_targetPosition));
		bmds.LocateTargetForLargeRef(&(pLargeDoc->m_targetPosition),&(pSmallDoc->m_targetPosition));
		bmds.ShowMatch(filePath,"匹配结果_6D.jpg");
		fprintf(fp,"大基准图模式：\n");
		pSmallDoc->m_pView->SetFocus();
	}
	else
	{
		bmds.GetMatchResult(m_targetMode,&(pLargeDoc->m_targetPosition),&(pSmallDoc->m_targetPosition));
		bmds.LocateTargetForSmallRef(&(pLargeDoc->m_targetPosition),&(pSmallDoc->m_targetPosition));
		bmds.ShowMatch(filePath,"匹配结果_6D.jpg");
		fprintf(fp,"小基准图模式：\n");
		pLargeDoc->m_pView->SetFocus();
	}

	fprintf(fp,"大图目标位置：");
	fprintf(fp,"(%5.1f, %5.1f)    ",pLargeDoc->m_targetPosition.x,pLargeDoc->m_targetPosition.y);
	fprintf(fp,largefileName);
	fprintf(fp,"\n");	

	fprintf(fp,"小图目标位置：");
	fprintf(fp,"(%5.1f, %5.1f)    ",pSmallDoc->m_targetPosition.x,pSmallDoc->m_targetPosition.y);
	fprintf(fp,smallfilename);
	fprintf(fp,"\n");
	fclose(fp);

	pSmallDoc->m_pView->Invalidate(false);
	pLargeDoc->m_pView->Invalidate(false);

	AfxMessageBox("结果保存在\n"+filePath);
}


void CMainFrame::OnAutoMatch()
{
	// TODO: Add your command handler code here
	CEsDoc  *pLargeDoc, *pSmallDoc;
	if( GetDocFromPOSITION((void**)&pLargeDoc, (void**)&pSmallDoc)!=1)
		return;
	if (m_targetMode==0)     // 小图中心
	{
		m_targetMode=3;
		pSmallDoc->m_targetPosition= cvPoint2D32f(pSmallDoc->m_pImage->width/2,pSmallDoc->m_pImage->height/2);
		pLargeDoc->m_targetPosition= cvPoint2D32f(pLargeDoc->m_pImage->width/2,pLargeDoc->m_pImage->height/2);
	}
	if (pSmallDoc->m_targetPosition.x<0 || pSmallDoc->m_targetPosition.y<0 )
		pSmallDoc->m_targetPosition= cvPoint2D32f(pSmallDoc->m_pImage->width/2,pSmallDoc->m_pImage->height/2);
	if (pLargeDoc->m_targetPosition.x<0 || pLargeDoc->m_targetPosition.y<0 )
		pLargeDoc->m_targetPosition= cvPoint2D32f(pLargeDoc->m_pImage->width/2,pLargeDoc->m_pImage->height/2);
	// 备份图像
	if (pLargeDoc->bk!= NULL)
	{
		cvReleaseImage(&pLargeDoc->bk);
	}
	pLargeDoc->bk = cvCloneImage(pLargeDoc->m_pImage);
	if (pSmallDoc->bk!= NULL)
	{
		cvReleaseImage(&pSmallDoc->bk);
	}
	pSmallDoc->bk = cvCloneImage(pSmallDoc->m_pImage);
	//结果输出路径	
	CString filePath,largefileName,smallfilename;
	GetFolderPathFromFilePath(pLargeDoc->m_strFileName,filePath);
	GetFileNameFromFilePath(pLargeDoc->m_strFileName,largefileName);
	GetFileNameFromFilePath(pSmallDoc->m_strFileName,smallfilename);
	//  开始匹配评估
	cvDestroyAllWindows();
	CBlockMatchDirSub bmds;

	bmds.InputImage(pLargeDoc->m_pImage,pSmallDoc->m_pImage);	
	bmds.GetSimMap_XY();	bmds.SpaceSearchMatch_6d(); 	bmds.saveHmat("~~~~~~~~~临时文件-hmat.txt");
//	bmds.loadHmat("~~~~~~~~~临时文件-hmat.txt");
	//bmds.SimpleMatch2D();
	

	FILE *fp = fopen(filePath+"\\匹配结果_6D.txt", "w" );
	fprintf(fp,"mode：%3d\n", m_targetMode);
	if (m_targetMode<=2)
	{
		bmds.GetMatchResult(m_targetMode,&(pLargeDoc->m_targetPosition),&(pSmallDoc->m_targetPosition));
		bmds.LocateTargetForLargeRef(&(pLargeDoc->m_targetPosition),&(pSmallDoc->m_targetPosition));
		bmds.ShowMatch(filePath,"匹配结果_6D.jpg");
		fprintf(fp,"大基准图模式：\n");
		pSmallDoc->m_pView->SetFocus();
	}
	else
	{
		bmds.GetMatchResult(m_targetMode,&(pLargeDoc->m_targetPosition),&(pSmallDoc->m_targetPosition));
		bmds.LocateTargetForSmallRef(&(pLargeDoc->m_targetPosition),&(pSmallDoc->m_targetPosition));
		bmds.ShowMatch(filePath,"匹配结果_6D.jpg");
		fprintf(fp,"小基准图模式：\n");
		pLargeDoc->m_pView->SetFocus();
	}
	
	fprintf(fp,"大图目标位置：");
	fprintf(fp,"(%5.1f, %5.1f)    ",pLargeDoc->m_targetPosition.x,pLargeDoc->m_targetPosition.y);
	fprintf(fp,largefileName);
	fprintf(fp,"\n");	
	
	fprintf(fp,"小图目标位置：");
	fprintf(fp,"(%5.1f, %5.1f)    ",pSmallDoc->m_targetPosition.x,pSmallDoc->m_targetPosition.y);
	fprintf(fp,smallfilename);
	fprintf(fp,"\n");
	fclose(fp);

	pSmallDoc->m_pView->Invalidate(false);
	pLargeDoc->m_pView->Invalidate(false);
	AfxMessageBox("结果保存在\n"+filePath);
}

void CMainFrame::OnSelectTarget()
{
	// TODO: Add your command handler code here
	CEsDoc  *pLargeDoc, *pSmallDoc;
	if( GetDocFromPOSITION((void**)&pLargeDoc, (void**)&pSmallDoc)!=1)
		return;
	CSelectTargetDlg stdlg;
	if (stdlg.DoModal()==IDOK)
	{
		m_targetMode= stdlg.m_ModeType;
	}
	else
	{
		m_targetMode= 0;
		pLargeDoc->m_targetPosition= cvPoint2D32f(-1.0f,-1.0f);
		pSmallDoc->m_targetPosition= cvPoint2D32f(-1.0f,-1.0f);
	}
	if(m_targetMode==1)           // 大图中心
	{
		pLargeDoc->m_targetPosition= cvPoint2D32f(pLargeDoc->m_pImage->width/2,pLargeDoc->m_pImage->height/2);
		pSmallDoc->m_targetPosition= cvPoint2D32f(-1,-1);
	}
	else if (m_targetMode==2)     // 大图手选
	{
		pLargeDoc->m_pView->SetFocus();
		CSetTarPos stpdlg;
		if(stpdlg.DoModal()==IDOK)
		{
			pLargeDoc->m_targetPosition= cvPoint2D32f(stpdlg.m_x,stpdlg.m_y);
			pSmallDoc->m_targetPosition= cvPoint2D32f(-1.0f,-1.0f);
		}
		else
		{
			pLargeDoc->m_pView->OnManualSelectTarget();
			pSmallDoc->m_targetPosition= cvPoint2D32f(-1.0f,-1.0f);
		}
	}
	else if (m_targetMode==3)     // 小图中心
	{
		pSmallDoc->m_targetPosition= cvPoint2D32f(pSmallDoc->m_pImage->width/2,pSmallDoc->m_pImage->height/2);
		pLargeDoc->m_targetPosition= cvPoint2D32f(-1.0f,-1.0f);
	}
	else if (m_targetMode==4)     // 小图手选
	{
		pSmallDoc->m_pView->SetFocus();
		CSetTarPos stpdlg;
		if(stpdlg.DoModal()==IDOK)
		{
			pSmallDoc->m_targetPosition= cvPoint2D32f(stpdlg.m_x,stpdlg.m_y);
			pLargeDoc->m_targetPosition= cvPoint2D32f(-1.0f,-1.0f);
		}
		else
		{
			pSmallDoc->m_pView->OnManualSelectTarget();
			pLargeDoc->m_targetPosition= cvPoint2D32f(-1.0f,-1.0f);
		}
	}
	pSmallDoc->m_pView->Invalidate(false);
	pLargeDoc->m_pView->Invalidate(false);
}
