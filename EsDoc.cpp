
// EsDoc.cpp : CEsDoc 类的实现
//

#include "stdafx.h"
#include "Es.h"

#include "EsDoc.h"
#include "EsView.h"
#include "PTTrackLIB.h"
#include "IncludeFile.h"
#include "ReadRawDlg.h"

// learnmatch .h files  *3
#include "Detector.h"
#include "Rect.h"
#include "Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPlaceTokenMatch theMatchTrack;

// CEsDoc

IMPLEMENT_DYNCREATE(CEsDoc, CDocument)

BEGIN_MESSAGE_MAP(CEsDoc, CDocument)
	ON_COMMAND(ID_IMAGE_FILTER, &CEsDoc::OnImageFilter)
	ON_COMMAND(ID_ENHANCE_HIST, &CEsDoc::OnEnhanceHist)
	ON_COMMAND(ID_ACT_CANCEL, &CEsDoc::OnActCancel)
	ON_COMMAND(ID_ZOOM_IN, &CEsDoc::OnZoomIn)
	ON_COMMAND(ID_ZOOM_OUT, &CEsDoc::OnZoomOut)
	ON_COMMAND(ID_ROTATE_IMAGE, &CEsDoc::OnRotateImage)
	ON_COMMAND(ID_GET_PREVIOUS_IMAGE, &CEsDoc::OnGetPreviousImage)
	ON_COMMAND(ID_GET_NEXT_IMAGE, &CEsDoc::OnGetNextImage)
	ON_COMMAND(ID_RELOAD_IMAGE, &CEsDoc::OnReloadImage)
	ON_COMMAND(ID_VFLIP, &CEsDoc::OnVflip)
	ON_COMMAND(ID_HFLIP, &CEsDoc::OnHflip)
	ON_COMMAND(ID_LEFT90, &CEsDoc::OnLeft90)
	ON_COMMAND(ID_RIGHT90, &CEsDoc::OnRight90)
	ON_COMMAND(ID_GRADIENT_ORIENTATION_MAP, &CEsDoc::OnGradientOrientationMap)
	ON_COMMAND(ID_GRADIENT_INTENSITY_MAP, &CEsDoc::OnGradientIntensityMap)
	ON_COMMAND(ID_PYRAMID_GRADIENT_INTENSITY_MAP, &CEsDoc::OnPyramidGradientIntensityMap)
	ON_COMMAND(ID_GRAY_INVERSE, &CEsDoc::OnGrayInverse)
	ON_COMMAND(ID_PT_TRACK, &CEsDoc::OnPtTrack)
	ON_COMMAND(ID_LOAD_TRACK_DATA, &CEsDoc::OnLoadTrackData)
	ON_COMMAND(ID_REGISTRATION_TRACK, &CEsDoc::OnRegistrationTrack)
	ON_COMMAND(ID_RGSTRACK2, &CEsDoc::OnRgstrack2)
END_MESSAGE_MAP()


// CEsDoc 构造/析构

CEsDoc::CEsDoc()
{
	// TODO: 在此添加一次性构造代码
	m_isTrackRunning= FALSE;
	m_strFileName = "No image";
	m_pImage = NULL;
	bk       = NULL;
	bk2      = NULL;
	m_Rot= 0;
	m_Scale= 100;
	m_targetPosition= cvPoint2D32f(-1.0f,-1.0f);
}

CEsDoc::~CEsDoc()
{
}

BOOL CEsDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CEsDoc 序列化

void CEsDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CEsDoc 诊断

#ifdef _DEBUG
void CEsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CEsDoc 命令
BOOL CEsDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	m_strFileName= lpszPathName;
	// TODO: Add your specialized creation code here
	if (m_pImage!=NULL)
	{
		cvReleaseImage(&m_pImage);
	}
	if (bk!=NULL)
	{
		cvReleaseImage(&bk);
	}

	// 若为raw图像
	FILE *fid;
	CString str= m_strFileName.Right(3);
	if (!str.CompareNoCase("RAW"))
	{		
		// 查看图像大小，猜测可能长宽值
		fopen_s(&fid,lpszPathName, "rb");
		fseek(fid,0,SEEK_END);
		int len= ftell(fid);
		fclose(fid);
		if(len==m_RawImageHeight*m_RawImageWidth)
		{
			fopen_s(&fid,lpszPathName, "rb");
			m_pImage=cvCreateImage(cvSize(m_RawImageWidth,m_RawImageHeight),8,1);
			fread(m_pImage->imageData, 1, m_RawImageWidth*m_RawImageHeight, fid);
			fclose(fid);
		}
		else
		{
			CReadRawDlg dlg;
			if (len==81920)
			{
				dlg.m_RawHeight=256, dlg.m_RawWidth=320;  
			}
			else if(len==414720)
			{
				dlg.m_RawHeight=576, dlg.m_RawWidth=720;  
			}
			else if(len==786432)
			{
				dlg.m_RawHeight=768, dlg.m_RawWidth=1024;  
			}
			else if(abs(sqrt(double(len))*sqrt(double(len))-len)<0.000001)   //开平方为整数
			{
				dlg.m_RawHeight=int(sqrt(double(len))+0.5), dlg.m_RawWidth=int(sqrt(double(len))+0.5);  
			}
			else
			{
				dlg.m_RawHeight=256, dlg.m_RawWidth=320;  
			}

			dlg.m_TextOut.Format("图像文件大小为：%d",len);

			if(dlg.DoModal()== IDCANCEL)
				return FALSE;
			else if (len< (dlg.m_RawWidth*dlg.m_RawHeight))
			{
				AfxMessageBox("输入尺寸超过图像大小！");
				return FALSE;
			}
			else
			{
				m_RawImageHeight= dlg.m_RawHeight;
				m_RawImageWidth = dlg.m_RawWidth;
				fopen_s(&fid,lpszPathName, "rb");
				m_pImage=cvCreateImage(cvSize(m_RawImageWidth,m_RawImageHeight),8,1);
				fread(m_pImage->imageData, 1, m_RawImageWidth*m_RawImageHeight, fid);
				fclose(fid);
			}
		}
	}
	else
		m_pImage = cvLoadImage(lpszPathName,0);
	
	//
	bk = cvCloneImage(m_pImage);
	
	return TRUE;
}
void CEsDoc::OnReloadImage()
{
	// TODO: Add your command handler code here
	OnOpenDocument(LPCTSTR(m_strFileName));
	m_pView->Invalidate(TRUE);	
}
BOOL CEsDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// TODO: Add your specialized code here and/or call the base class
	CString str = lpszPathName;
	if( str.Right(3) == "bmp" || str.Right(3) == "BMP" || str.Right(3) == "Bmp" )
		return cvSaveImage(str,m_pImage);
	else if( str.Right(3) == "jpg" || str.Right(3) == "JPG" || str.Right(3) == "Jpg" )
		return cvSaveImage(str,m_pImage);
	else if( str.Right(3) == "png" || str.Right(3) == "PNG" || str.Right(3) == "Png" )
		return cvSaveImage(str,m_pImage);
	else if( str.Right(3) == "tif" || str.Right(3) == "TIF" || str.Right(3) == "Tif" )
		return cvSaveImage(str,m_pImage);
	else
	{
		str += ".bmp";
		return cvSaveImage(str,m_pImage);
	}
	return CDocument::OnSaveDocument(lpszPathName);
}

void CEsDoc::OnImageFilter()
{
	// TODO: Add your command handler code here
	if (bk!= NULL)
	{
		cvReleaseImage(&bk);
	}	
	bk = cvCloneImage(m_pImage);
	cvSmooth(m_pImage,m_pImage,CV_GAUSSIAN,5);
	m_pView->Invalidate(false);
}

void CEsDoc::OnEnhanceHist()
{
	// TODO: Add your command handler code here
	if (bk!= NULL)
	{
		cvReleaseImage(&bk);
	}
	bk = cvCloneImage(m_pImage);
	cvEqualizeHist(m_pImage,m_pImage);
	m_pView->Invalidate(false);
}

void CEsDoc::OnActCancel()
{
	// TODO: Add your command handler code here
	IplImage *tmp;
	if (bk== NULL)
	{
		return ;
	}
	tmp = cvCloneImage(m_pImage);
	cvReleaseImage(&m_pImage);
	m_pImage= cvCloneImage(bk);
	cvReleaseImage(&bk);
	bk= cvCloneImage(tmp);
	cvReleaseImage(&tmp);
	// 若取消
	m_targetPosition= cvPoint2D32f(-1.0f,-1.0f);
	m_pView->ResizeWindow();
	m_pView->Invalidate(TRUE);
}

void CEsDoc::OnZoomIn()
{
	// TODO: Add your command handler code here
	IplImage *tmp;
	cvReleaseImage(&bk);
	bk= cvCloneImage(m_pImage);
	tmp= cvCreateImage(cvSize(m_pImage->width*2, m_pImage->height*2), m_pImage->depth, m_pImage->nChannels);
	cvPyrUp(m_pImage,tmp);
	cvReleaseImage(&m_pImage);
	m_pImage= cvCloneImage(tmp);
	cvReleaseImage(&tmp);
	m_pView->ResizeWindow();
	m_pView->Invalidate(TRUE);
}

void CEsDoc::OnZoomOut()
{
	// TODO: Add your command handler code here
	if (bk!= NULL)
	{
		cvReleaseImage(&bk);
	}	
	bk= cvCloneImage(m_pImage);

	cvReleaseImage(&m_pImage);
	m_pImage= cvCreateImage(cvSize(bk->width/2, bk->height/2), bk->depth, bk->nChannels);
	cvPyrDown(bk,m_pImage);
	
	m_pView->ResizeWindow();
	m_pView->Invalidate(TRUE);
}

void CEsDoc::OnRotateImage()
{
	// TODO: Add your command handler code here	
	ManualAdjustDlg dlg;
	dlg.pDocImage= m_pImage;
	if(dlg.DoModal()== IDCANCEL)
	{
		return;
	}
	m_Rot  = dlg.m_Rot2;
	m_Scale= dlg.m_Scale2; 

	if (bk!= NULL)
	{
		cvReleaseImage(&bk);
	}	
	bk= cvCloneImage(m_pImage);

	if (m_pImage!= NULL)
	{
		cvReleaseImage(&m_pImage);
	}

	m_pImage= rotateScaleImage(bk, (float)m_Rot, m_Scale/100.0f);


	m_pView->ResizeWindow();
	m_pView->Invalidate(TRUE);
}

void CEsDoc::OnGetPreviousImage()
{
	// TODO: Add your command handler code here
	CString NextImageName = GetPreFileName_1(m_strFileName);
	if (!IsExistFile(NextImageName))
	{
		AfxMessageBox("未找到图像！");
		return;
	}
	m_strFileName = NextImageName;
	OnOpenDocument(LPCTSTR(m_strFileName));
	SetPathName(LPCTSTR(m_strFileName), false);

	if (m_pView->m_TargetCount>=1)
	{
		int i;
		for (i=0;i<m_pView->m_TargetCount;i++)
		{
			m_pView->m_pDialog[i]->SetImage(m_pImage);
		}
	}

	m_pView->Invalidate(FALSE);
}

void CEsDoc::OnGetNextImage()
{
	// TODO: Add your command handler code here
	CString NextImageName = GetNextFileName_1(m_strFileName);
	if (!IsExistFile(NextImageName))
	{
		AfxMessageBox("未找到图像！");
		return;
	}
	m_strFileName = NextImageName;
	OnOpenDocument(LPCTSTR(m_strFileName));
	SetPathName(LPCTSTR(m_strFileName), false);

	if (m_pView->m_TargetCount>=1)
	{
		int i;
		for (i=0;i<m_pView->m_TargetCount;i++)
		{
			m_pView->m_pDialog[i]->SetImage(m_pImage);
		}
	}
	
	m_pView->Invalidate(FALSE);	
}

void DrawMaskOnSaveImg(IplImage *im, CvPoint2D32f  Target)
{
	int   R = 20,w= im->width,h=im->height;
	int  x0 = int(Target.x + 0.5);
	int  y0 = int(Target.y + 0.5);
	CvScalar linecolor= cvScalar(0,0,255);
	// 若出视场，画箭头标志
	if (x0<0||x0>=w||y0<0||y0>=h)
	{
		float cRatio,d;
		d= sqrt((Target.x-w/2)*(Target.x-w/2)+(Target.y-h/2)*(Target.y-h/2));
		cRatio= (d-0.5*min(w,h)+10)/d;
		cvLine(im, cvPoint(w/2,h/2),cvPoint((x0-w/2)*cRatio+w/2,(y0-h/2)*cRatio+h/2),linecolor, 3);
		cvCircle (im, cvPoint((x0-w/2)*cRatio+w/2,(y0-h/2)*cRatio+h/2), 3, linecolor, 2);
	}
	else // 若在视场内，画方框十字标志
	{
		// 十字丝
		cvLine(im, cvPoint(x0, y0-R),cvPoint(x0,y0+R),linecolor, 1);
		cvLine(im, cvPoint(x0-R, y0),cvPoint(x0+R,y0), linecolor, 1);

		// 外框
		cvLine(im, cvPoint(x0-R-10    , y0 - R-5),cvPoint(x0+R+10    , y0 - R-5), linecolor, 3);
		cvLine(im, cvPoint(x0-R-10    , y0 - R-5),cvPoint(x0-R-10    , y0 - R+5), linecolor, 3);
		cvLine(im, cvPoint(x0+R+10    , y0 - R-5),cvPoint(x0+R+10    , y0 - R+5), linecolor, 3);

		cvLine(im, cvPoint(x0-R-10    , y0+ R+5),cvPoint(x0+R+10    , y0+ R+5), linecolor, 3);
		cvLine(im, cvPoint(x0-R-10    , y0+ R+5),cvPoint(x0-R-10    , y0+ R-5), linecolor, 3);
		cvLine(im, cvPoint(x0+R+10    , y0+ R+5),cvPoint(x0+R+10    , y0+ R-5), linecolor, 3);
	}
}

void CEsDoc::OnVflip()
{
	// TODO: Add your command handler code here
	if (bk!= NULL)
	{
		cvReleaseImage(&bk);
	}
	bk = cvCloneImage(m_pImage);
	cvFlip(m_pImage,NULL,0);
	m_pView->Invalidate(false);
}

void CEsDoc::OnHflip()
{
	// TODO: Add your command handler code here
	if (bk!= NULL)
	{
		cvReleaseImage(&bk);
	}
	bk = cvCloneImage(m_pImage);
	cvFlip(m_pImage,NULL,1);
	m_pView->Invalidate(false);
}

void CEsDoc::OnLeft90()
{
	// TODO: Add your command handler code here
	if (bk!= NULL)
	{
		cvReleaseImage(&bk);
	}	
	bk= cvCloneImage(m_pImage);

	if (m_pImage!= NULL)
	{
		cvReleaseImage(&m_pImage);
	}
	m_pImage=cvCreateImage(cvSize(bk->height,bk->width),8,1);
	cvTranspose(bk,m_pImage);
	cvFlip(m_pImage,NULL,0);

	m_pView->ResizeWindow();
	m_pView->Invalidate(TRUE);
}

void CEsDoc::OnRight90()
{
	// TODO: Add your command handler code here
	if (bk!= NULL)
	{
		cvReleaseImage(&bk);
	}	
	bk= cvCloneImage(m_pImage);

	if (m_pImage!= NULL)
	{
		cvReleaseImage(&m_pImage);
	}
	m_pImage=cvCreateImage(cvSize(bk->height,bk->width),8,1);
	cvTranspose(bk,m_pImage);
	cvFlip(m_pImage,NULL,1);

	m_pView->ResizeWindow();
	m_pView->Invalidate(TRUE);
}

void CEsDoc::OnGrayInverse()
{
	// TODO: 在此添加命令处理程序代码
	if (bk!= NULL)
	{
		cvReleaseImage(&bk);
	}	
	bk= cvCloneImage(m_pImage);
	int w= m_pImage->width;
	int h= m_pImage->height;
	int wstep= m_pImage->widthStep;
	uchar *pdata= (uchar*)m_pImage->imageData;
	int i,j;
	for (i=0;i<h;i++)
	{
		for (j=0;j<w;j++)
		{
			*(pdata+i*wstep+j)= 255- *(pdata+i*wstep+j);
		}
	}
	m_pView->Invalidate(TRUE);
}



void CEsDoc::OnGradientOrientationMap()
{
	// TODO: 在此添加命令处理程序代码
	if (bk!= NULL)
	{
		cvReleaseImage(&bk);
	}
	bk = cvCloneImage(m_pImage);
	cvSmooth(m_pImage,m_pImage,CV_GAUSSIAN,5);
	Image_Sobel_Direction_Fast( (BYTE*)m_pImage->imageData, m_pImage->width, m_pImage->height, 1 );
	m_pView->Invalidate(TRUE);
}

void CEsDoc::OnGradientIntensityMap()
{
	// TODO: 在此添加命令处理程序代码
	if (bk!= NULL)
	{
		cvReleaseImage(&bk);
	}
	bk = cvCloneImage(m_pImage);
	cvSmooth(m_pImage,m_pImage,CV_GAUSSIAN,5);
	GetGradientIntensity(m_pImage);
	m_pView->Invalidate(TRUE);
}


void CEsDoc::OnPyramidGradientIntensityMap()
{
	// TODO: 在此添加命令处理程序代码
	int nlevel= 4; // n 层金字塔
	int i,pw,ph;
	int w= m_pImage->width, h= m_pImage->height;
	IplImage *pIm[4], *pIntIm[4];
	// level 0
	pIm[0]= cvCloneImage(m_pImage);
	pIntIm[0]= cvCloneImage(pIm[0]);
	GetGradientIntensity(pIntIm[0]);
	cvShowImage("pIntIm[0]",pIntIm[0]);
	
	// level 1
	pw=w/2, ph=h/2;
	pIm[1]= cvCreateImage(cvSize(pw,ph),8,1);
	cvResize(pIm[0], pIm[1],CV_INTER_LINEAR);
	pIntIm[1]= cvCloneImage(pIm[1]);
	GetGradientIntensity(pIntIm[1]);
	cvShowImage("pIntIm[1]",pIntIm[1]);

	// level 2
	pw=w/4, ph=h/4;
	pIm[2]= cvCreateImage(cvSize(pw,ph),8,1);
	cvResize(pIm[1], pIm[2],CV_INTER_LINEAR);
	pIntIm[2]= cvCloneImage(pIm[2]);
	GetGradientIntensity(pIntIm[2]);
	cvShowImage("pIntIm[2]",pIntIm[2]);

	// level 3
	pw=w/8, ph=h/8;
	pIm[3]= cvCreateImage(cvSize(pw,ph),8,1);
	cvResize(pIm[2], pIm[3],CV_INTER_LINEAR);
	pIntIm[3]= cvCloneImage(pIm[3]);
	GetGradientIntensity(pIntIm[3]);
	cvShowImage("pIntIm[3]",pIntIm[3]);

	// release
	for (i=0;i<4;i++)
	{
		cvReleaseImage(&pIm[i]);
		cvReleaseImage(&pIntIm[i]);
	}
}
void CEsDoc::OnPtTrack()
{
	if (!assertBorder(m_targetPosition.x, 0.0f, m_pImage->width-1.0f)||
		!assertBorder(m_targetPosition.y, 0.0f, m_pImage->height-1.0f))
	{
		m_targetPosition= cvPoint2D32f(m_pImage->width/2,m_pImage->height/2);
		//		m_targetPosition= cvPoint2D32f((273+294)/2,(95+116)/2);   // 273,95,294,116
		m_pView->Invalidate(false);
		//		AfxMessageBox("未指定目标!");
		//		return;
	}
	IplImage *saveImg=cvCreateImage(cvGetSize(m_pImage),8,3);
	CString saveName;	
	CString NextImageName;
	CString filePath,fileName;
	GetFolderPathFromFilePath(m_strFileName,filePath);	
	FILE *fp = fopen(filePath+"\\仿射跟踪结果.txt", "w" );
	//跟踪结果输出
	GetFileNameFromFilePath(m_strFileName,fileName);
	fprintf(fp,fileName);
	fprintf(fp,"  %5.1f  %5.1f\n",m_targetPosition.x, m_targetPosition.y);
	fclose(fp);
	std::vector<CvPoint2D32f> trackPointVector;
	
	theMatchTrack.PT_IMGH= m_pImage->height;
	theMatchTrack.PT_IMGW= m_pImage->widthStep;
	CFPt2D ptpoint;
	ptpoint.x= m_targetPosition.x;
	ptpoint.y= m_targetPosition.y;
	trackPointVector.push_back(m_targetPosition);
	theMatchTrack.InitiateTrack((uchar*)(m_pImage->imageData), ptpoint);

	bool trackflag= TRUE;
	m_isTrackRunning= TRUE;
	while(trackflag && m_isTrackRunning)
	{
		// save result;
		cvCvtColor(m_pImage,saveImg,CV_GRAY2RGB);
		DrawMaskOnSaveImg(saveImg, m_targetPosition);

//		saveName= m_strFileName;
//		saveName.Insert(saveName.GetLength()-4,_T("_正向跟踪结果"));
//		cvSaveImage(saveName,saveImg);

		CString NextImageName = GetNextFileName_123(m_strFileName);
		if (!IsExistFile(NextImageName)) break;
		m_strFileName = NextImageName;
		OnOpenDocument(LPCTSTR(m_strFileName));
		SetPathName(LPCTSTR(m_strFileName), false);

		theMatchTrack.TrackOneImage((uchar*)(m_pImage->imageData),  ptpoint);
		m_targetPosition.x= ptpoint.x;
		m_targetPosition.y= ptpoint.y;
		trackPointVector.push_back(m_targetPosition);
		// 判断出视场
		trackflag= (assertBorder(int(m_targetPosition.x+0.5),15,m_pImage->width-15)&&
			assertBorder(int(m_targetPosition.y+0.5),15,m_pImage->height-15));
		if(!trackflag)
			AfxMessageBox("目标即将出视场，停止跟踪!");
		//跟踪结果输出	
		GetFileNameFromFilePath(m_strFileName,fileName);
		fp = fopen(filePath+"\\仿射跟踪结果.txt", "a" );
		fprintf(fp,fileName);
		fprintf(fp,"  %5.1f  %5.1f",m_targetPosition.x, m_targetPosition.y);
		fprintf(fp,"\n");	
		fclose(fp);
		m_pView->Invalidate(false);
		DelayMessage();
	}
	if (saveImg!=NULL) cvReleaseImage(&saveImg);
	// 画2D跟踪曲线
	IplImage* graph2D= draw2DFloatGraph(&trackPointVector[0], trackPointVector.size(), NULL,
		m_pImage->width, m_pImage->height, "2D tracklet", 0.5);
	cvSaveImage(filePath+"\\仿射跟踪--图像坐标下2维目标运动轨迹.jpg", graph2D);  
	cvShowImage("2D_graph", graph2D);  
	// 画跟踪结果曲线图
	std::vector<float> disVector;
	float disTop=0, disBot=m_pImage->imageSize;
	for (int i=0;i<trackPointVector.size();i++)
	{
		float dx= trackPointVector.at(i).x-m_pImage->width/2.0f;
		float dy= trackPointVector.at(i).y-m_pImage->height/2.0f;
		float dis= sqrt(dx*dx+dy*dy);
		disVector.push_back(dis);
		if (dis>disTop)
		{
			disTop= dis;
		}
		if (disBot>dis)
		{
			disBot= dis;
		}
	}
	int topBorder= cvCeil(disTop/10.0)*10;
	int botBorder= cvFloor(disBot/10.0)*10;
	IplImage *graphImg = drawFloatGraph(&disVector[0], disVector.size(), NULL,botBorder,topBorder, 400,200,  
		"t","log(d)","distance to image center",0.5 );//-25和25是数据最小值和最大值,400是窗口宽,180为高,"x angel"为曲线标签  
//	drawFloatGraph(&floatVec2[0], floatVec2.size(), graphImg, -25,25, 400,180);//在同样的图像上画曲线 
	cvShowImage("dis_graph", graphImg);  
//	cvWaitKey(0);
	cvSaveImage(filePath+"\\仿射跟踪--图像坐标下目标与视线中心距离变化曲线.jpg", graphImg);  
	cvReleaseImage(&graphImg);  
}


void CEsDoc::OnLoadTrackData()
{
	CFileDialog dlg(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
		NULL, NULL);                                        // 选项图片的约定
	dlg.m_ofn.lpstrTitle = _T("打开遥测跟踪数据");    // 打开文件对话框的标题名
	dlg.m_ofn.lpstrFilter=_T("*.txt\0*.TXT\0All Files (*.*) \0*.*");

	if( dlg.DoModal() != IDOK )                    // 判断是否获得文件
		return;
	CString mPath = dlg.GetPathName();            // 获取文件路径
	CStdioFile File;
	File.Open(mPath,CFile::modeRead);
	char TempStr0[50], TempStr1[50], TempStr2[50];
	memset(TempStr1,'\0',50);
	memset(TempStr2,'\0',50);
	int m_PointNum= 999999999;	
	CString FileData;
	
	// 无需文件头，直接读取到数据文件结束
	//File.ReadString(FileData);
	//sscanf(FileData,"%s",TempStr1);   // 读取坐标数组长度	
	//m_PointNum= atoi(TempStr1);

//	double lki,lkk;
	if (!m_PointVector.empty())
	{
		m_PointVector.clear();
	}
	int nField=0;
	int i;
	for (i=0;i<m_PointNum;i++)
	{
		if(FALSE== File.ReadString(FileData) || ""==FileData)
			break;
		nField= sscanf_s(FileData,"%s %s %s",TempStr0, _countof(TempStr0), TempStr1, _countof(TempStr1),TempStr2, _countof(TempStr2));   // 读取坐标
		if (nField==2)
		{
			m_PointVector.push_back(cvPoint2D32f(atof(TempStr0),atof(TempStr1)));
		}
		else if (nField==3)
		{
			m_PointVector.push_back(cvPoint2D32f(atof(TempStr1),atof(TempStr2)));
		}
		else if (nField==0)         // 空行，则结束
		{
			break;
		}
		else                        // 忽略无效格式行，继续读下一行
			continue;
//		lki= atof(TempStr1),lkk= atof(TempStr2);   // debug watch 
	}
	File.Close();
	CString strOut;
	strOut.Format("共载入 %d 组坐标",i);
	AfxMessageBox(strOut);
//	int i=10;	 // 调试 设断点用
}


void CEsDoc::OnRegistrationTrack()
{
	// TODO: 在此添加命令处理程序代码
	if (!assertBorder(m_targetPosition.x, 0.0f, m_pImage->width-1.0f)||
		!assertBorder(m_targetPosition.y, 0.0f, m_pImage->height-1.0f))
	{
		m_targetPosition= cvPoint2D32f(m_pImage->width/2,m_pImage->height/2);
		//		m_targetPosition= cvPoint2D32f((273+294)/2,(95+116)/2);   // 273,95,294,116
		m_pView->Invalidate(false);
		//		AfxMessageBox("未指定目标!");
		//		return;
	}
	IplImage *saveImg=cvCreateImage(cvGetSize(m_pImage),8,3);
	CString saveName;	
	CString NextImageName;
	CString filePath,fileName;
	GetFolderPathFromFilePath(m_strFileName,filePath);	
	FILE *fp = fopen(filePath+"\\仿射跟踪结果.txt", "w" );
	FILE *fp1 = fopen(filePath+"\\learnmatch.txt", "w" );
	//跟踪结果输出
	fclose(fp);
	std::vector<CvPoint2D32f> trackPointVector;
// 初始化
// pt 跟踪器初始化
	theMatchTrack.PT_IMGH= m_pImage->height;
	theMatchTrack.PT_IMGW= m_pImage->widthStep;
	CFPt2D ptpoint;
	ptpoint.x= m_targetPosition.x;
	ptpoint.y= m_targetPosition.y;
	trackPointVector.push_back(m_targetPosition);
	theMatchTrack.InitiateTrack((uchar*)(m_pImage->imageData), ptpoint);
// learnmatch 跟踪器初始化
	srand(0);        // init rand seed
	Config conf= Config("config.txt");
	Detector detector(conf); // init detector
	// read first frame to get size
	std::vector<float> sigVec;
	float sigMul=1.0f,sigma=1.0f;  // 尺度累积
	Mat frame, frameGrey;
	bool doFlip = false;
	static const char* kWindowName = "learnmatch";
	if (!conf.quietMode)
	{
		namedWindow(kWindowName);
		cvMoveWindow(kWindowName, 640, 20);
	}
	frameGrey= Mat(m_pImage,1);
	if (!frameGrey.empty())
	{
		cvtColor(frameGrey, frame, CV_GRAY2RGB);
	}
	IntRect rect(frameGrey.cols/3, frameGrey.rows/3, frameGrey.cols/3, frameGrey.rows/3);	
	detector.SetModel(frameGrey, rect);
	//imshow("1",Mat(m_pImage));
	//waitKey(0);
	sigVec.push_back(sigMul*sigma);
// 设置跟踪控制标志
	bool trackflag= TRUE;
	m_isTrackRunning= TRUE;
// 序列跟踪
	int cnt1=0,cnt0=0;
	char saveNameChar[100];
	while(trackflag && m_isTrackRunning)
	{
		// save result;
		cvCvtColor(m_pImage,saveImg,CV_GRAY2RGB);
		DrawMaskOnSaveImg(saveImg, m_targetPosition);

		saveName= m_strFileName;
		saveName.Insert(saveName.GetLength()-4,_T("_正向跟踪结果"));
		//		cvSaveImage(saveName,saveImg);

		CString NextImageName = GetNextFileName_123(m_strFileName);
		if (!IsExistFile(NextImageName)) break;
		m_strFileName = NextImageName;
		OnOpenDocument(LPCTSTR(m_strFileName));
		SetPathName(LPCTSTR(m_strFileName), false);
		// pt track
		theMatchTrack.TrackOneImage((uchar*)(m_pImage->imageData),  ptpoint);
		m_targetPosition.x= ptpoint.x;
		m_targetPosition.y= ptpoint.y;
		trackPointVector.push_back(m_targetPosition);

		// learnmatch 
		if (detector.HasModel())
		{
			frameGrey= Mat(m_pImage,1);
			if (!frameGrey.empty())
			{
				cvtColor(frameGrey, frame, CV_GRAY2RGB);
			}
			detector.Detect(frameGrey, conf.enableLearning);
			if (detector.HasDetection())
			{
				cnt1++;
				DrawHomography(detector.GetH(), frame, rect.Width(), rect.Height(), detector.HasDetection() ? CV_RGB(0, 0, 255) : CV_RGB(255, 0, 0));
				Mat h_mat= detector.GetH();
				//				cout<< h_mat;
				int ctx= int(rect.XCentre()+0.5);
				int cty= int(rect.YCentre()+0.5);
				line(frame,Point2i(ctx-10,cty),Point2i(ctx+10,cty),CV_RGB(0, 255, 0),2);
				line(frame,Point2i(ctx,cty-10),Point2i(ctx,cty+10),CV_RGB(0, 255, 0),2);
				double a[4];
				a[0]= h_mat.at<double>(0,0);
				a[1]= h_mat.at<double>(0,1);
				a[2]= h_mat.at<double>(1,0);
				a[3]= h_mat.at<double>(1,1);	

				sigma= sqrt((a[0]*a[0]+a[1]*a[1]+a[2]*a[2]+a[3]*a[3])/2);
				float areaRatio= detector.validAreaRatio(frameGrey,rect.Width(), rect.Height());	
				// write debug info to file
				fprintf(fp1,"cnt1: %3d, a0~3: (%5.3f, %5.3f, %5.3f, %5.3f), s: %5.3f, a: %3.2f\n",cnt1,a[0],a[1],a[2],a[3],sigma,areaRatio);

				if(theMatchTrack.MaxCor>0.6)  // 判断相关性，避免不稳定帧更新
				{
					if (sigma>1.5 || sigma<1.0/1.7)  // 如果 H 满足 更新条件
						// 条件1   尺度 sigma> 1.732					
					{
						detector.SetModel(frameGrey, rect);  // 更新 跟踪 区域
						sigMul= sigMul*sigma;
						sigma= 1.0f;
					}				
					else if (areaRatio<0.4)  // 如果 H 满足 更新条件
						// 条件2   矩形在图像内面积与矩形总面积比值 小于 0.7 
					{
						detector.SetModel(frameGrey, rect);  // 更新 跟踪 区域
						sigMul= sigMul*sigma;
						sigma= 1.0f;
					}
				}				
			}
		}
		sigVec.push_back(sigMul*sigma);
		// write debug info to file
		// write debug info to file
		cnt0++;
		fprintf(fp1,"cnt0: %4d                                          sigma: %6.4f\n",cnt0,sigMul*sigma);		
		
		// display
		if (!conf.quietMode)
		{
			imshow(kWindowName, detector.HasModel() ? detector.GetDebugImage() : frame);
			sprintf(saveNameChar, "d:\\feature\\%04d.jpg", cnt0);
			imwrite(saveNameChar,detector.GetDebugImage());

			//int ctx= int(m_targetPosition.x+0.5);
			//int cty= int(m_targetPosition.y+0.5);
			//line(frame,Point2i(ctx-20,cty),Point2i(ctx+20,cty),CV_RGB(255, 0, 0),2);
			//line(frame,Point2i(ctx,cty-20),Point2i(ctx,cty+20),CV_RGB(255, 0, 0),2);
			//sprintf(saveNameChar, "d:\\feature\\%04d.jpg", cnt0);
			//imwrite(saveNameChar,frame);
		}
		// 判断出视场
		trackflag= (assertBorder(int(m_targetPosition.x+0.5),15,m_pImage->width-15)&&
			assertBorder(int(m_targetPosition.y+0.5),15,m_pImage->height-15));
		if(!trackflag)
			AfxMessageBox("目标即将出视场，停止跟踪!");
		//跟踪结果输出	
		GetFileNameFromFilePath(m_strFileName,fileName);
		fp = fopen(filePath+"\\配准跟踪结果.txt", "a" );
		fprintf(fp,fileName);
		fprintf(fp,"  %5.1f  %5.1f",m_targetPosition.x, m_targetPosition.y);
		fprintf(fp,"\n");	
		fclose(fp);
		m_pView->Invalidate(false);
		DelayMessage();
	}
	//fp = fopen(filePath+"\\debugLog.txt", "w" );
	//fprintf(fp,"cnt1                  :     %d\n",cnt1);
	//fprintf(fp,"sigVec.size           :     %d\n",sigVec.size());
	//fprintf(fp,"trackPointVector.size :     %d\n",trackPointVector.size());
	//for (int i=0;i<sigVec.size();i++)
	//{
	//	fprintf(fp,"%6.4f\n",sigVec.at(i));
	//}
	fclose(fp);
	fclose(fp1);
	if (saveImg!=NULL) cvReleaseImage(&saveImg);
	// 画2D跟踪曲线
	IplImage* graph2D= draw2DFloatGraph(&trackPointVector[0], trackPointVector.size(), NULL,
		m_pImage->width, m_pImage->height, "2D tracklet", 0.5);
	cvShowImage("2D_graph", graph2D);  
	cvSaveImage(filePath+"\\配准跟踪目标在图像中坐标2D变化曲线sigma.jpg", graph2D);  
	cvReleaseImage(&graph2D);
	// 画跟踪结果曲线图
	std::vector<float> disVector;
	float disTop=0, disBot=m_pImage->imageSize;
	for (int i=0;i<trackPointVector.size();i++)
	{
		float fSig= i<sigVec.size()? sigVec.at(sigVec.size()-1)/sigVec.at(i):1.0f;  // 归一化至末帧
		float dx= (trackPointVector.at(i).x-m_pImage->width/2.0f)*fSig;
		float dy= (trackPointVector.at(i).y-m_pImage->height/2.0f)*fSig;
		//float dis= 20*fSig;    //  输出sigma变化曲线，debug
		float dis= sqrt(dx*dx+dy*dy);
		disVector.push_back(dis);
		if (dis>disTop)
		{
			disTop= dis;
		}
		if (disBot>dis)
		{
			disBot= dis;
		}
	}
	int topBorder= cvCeil(disTop/10.0)*10;
	int botBorder= cvFloor(disBot/10.0)*10;
	IplImage *graphImg = drawFloatGraph(&disVector[0], disVector.size(), NULL,botBorder,topBorder, 600,400,  
		"frame","d","normalized distance to image center",0.5 );//-25和25是数据最小值和最大值,400是窗口宽,180为高,"x angel"为曲线标签  
	//	drawFloatGraph(&floatVec2[0], floatVec2.size(), graphImg, -25,25, 400,180);//在同样的图像上画曲线 
	cvShowImage("dis_graph", graphImg);  
//	cvWaitKey(0);
	cvSaveImage(filePath+"\\配准跟踪目标距离中心点空间距离曲线sigma.jpg", graphImg);  
	cvReleaseImage(&graphImg);  
}


void CEsDoc::OnRgstrack2()
{
	// function test debug

	// TODO: 在此添加命令处理程序代码
	if (!assertBorder(m_targetPosition.x, 0.0f, m_pImage->width-1.0f)||
		!assertBorder(m_targetPosition.y, 0.0f, m_pImage->height-1.0f))
	{
		m_targetPosition= cvPoint2D32f(m_pImage->width/2,m_pImage->height/2);
		//		m_targetPosition= cvPoint2D32f((273+294)/2,(95+116)/2);   // 273,95,294,116
		m_pView->Invalidate(false);
		//		AfxMessageBox("未指定目标!");
		//		return;
	}
	IplImage *saveImg=cvCreateImage(cvGetSize(m_pImage),8,3);
	CString saveName;	
	CString NextImageName;
	CString filePath,fileName;
	GetFolderPathFromFilePath(m_strFileName,filePath);	
	FILE *fp = fopen(filePath+"\\配准跟踪结果.txt", "w" );
	FILE *fp1 = fopen(filePath+"\\配准跟踪--中间结果.txt", "w" );
	//跟踪结果输出
	GetFileNameFromFilePath(m_strFileName,fileName);
	fprintf(fp,fileName);
	fprintf(fp,"  %5.1f  %5.1f\n",m_targetPosition.x, m_targetPosition.y);
	fclose(fp);
	std::vector<CvPoint2D32f> trackPointVector;
	// 初始化
	// pt 跟踪器初始化
	theMatchTrack.PT_IMGH= m_pImage->height;
	theMatchTrack.PT_IMGW= m_pImage->widthStep;
	CFPt2D ptpoint;
	ptpoint.x= m_targetPosition.x;
	ptpoint.y= m_targetPosition.y;
	Point2f tp0(m_targetPosition.x,m_targetPosition.y);
	trackPointVector.push_back(m_targetPosition);
	theMatchTrack.InitiateTrack((uchar*)(m_pImage->imageData), ptpoint);
	// learnmatch 跟踪器初始化
	srand(0);        // init rand seed
	Config conf= Config("learnmatch\\config.txt");
	Detector detector(conf); // init detector
	// read first frame to get size
//	std::vector<float> sigVec;
	
	float sigMul=1.0f,sigma=1.0f;  // 尺度累积
	Mat frame, frameGrey;
	bool doFlip = false;
	static const char* kWindowName = "learnmatch";
	if (!conf.quietMode)
	{
		namedWindow(kWindowName);
		cvMoveWindow(kWindowName, 640, 20);
	}
	frameGrey= Mat(m_pImage,1);
	if (!frameGrey.empty())
	{
		cvtColor(frameGrey, frame, CV_GRAY2RGB);
	}
	int xshift= frameGrey.cols/3;
	int yshift= frameGrey.rows/3;
	IntRect rect(frameGrey.cols/3, frameGrey.rows/3, frameGrey.cols/3, frameGrey.rows/3);	
	detector.SetModel(frameGrey, rect);
//	vector<Mat> hmatPartVec;
	vector<Mat> hmatMulVec;
	vector<bool> validFlagVec;
//	vector<Mat> hmatUpdateVec;
//	vector<int> prjFlag;
//	Point2f pntShift(xshift,yshift);
	Mat hShift= (Mat_<double>(3,3) << 0, 0, xshift, 0, 0, yshift, 0, 0, 0);
	Mat hmatMul=Mat::eye(3,3,CV_64F);
	Mat hmat=Mat::eye(3,3,CV_64F);
	//imshow("1",Mat(m_pImage));
	//waitKey(0);
	hmatMulVec.push_back(hmat*hmatMul);
	validFlagVec.push_back(TRUE);
//	hmatPartVec.push_back(hmat);
//	hmatUpdateVec.push_back(hmat);
//	prjFlag.push_back(0);
	// 设置跟踪控制标志
	bool trackflag= TRUE;
	m_isTrackRunning= TRUE;
	// 序列跟踪
	int cnt1=0,cnt0=0;//,cntUpdate=0;
	char saveNameChar[100];
	bool learnUpdateFlag= FALSE, isValid;
	while(trackflag && m_isTrackRunning)
	{
		isValid= FALSE;
		learnUpdateFlag= FALSE;		
		// save result;
		//cvCvtColor(m_pImage,saveImg,CV_GRAY2RGB);
		//DrawMaskOnSaveImg(saveImg, m_targetPosition);

		//saveName= m_strFileName;
		//saveName.Insert(saveName.GetLength()-4,_T("_正向跟踪结果"));
		//cvSaveImage(saveName,saveImg);

		CString NextImageName = GetNextFileName_123(m_strFileName);
		if (!IsExistFile(NextImageName)) break;
		m_strFileName = NextImageName;
		OnOpenDocument(LPCTSTR(m_strFileName));
		SetPathName(LPCTSTR(m_strFileName), false);
		// pt track
		theMatchTrack.TrackOneImage((uchar*)(m_pImage->imageData),  ptpoint);
		m_targetPosition.x= ptpoint.x;
		m_targetPosition.y= ptpoint.y;
		trackPointVector.push_back(m_targetPosition);
		// learnmatch 
//		hmat=cv::Mat::eye(3,3,CV_64F);
		if (detector.HasModel())
		{
			frameGrey= Mat(m_pImage,1);
			if (!frameGrey.empty())
			{
				cvtColor(frameGrey, frame, CV_GRAY2RGB);
			}
			detector.Detect(frameGrey, conf.enableLearning);
			if (detector.HasDetection())
			{
				cnt1++;
				isValid= TRUE;
				DrawHomography(detector.GetH(), frame, rect.Width(), rect.Height(), detector.HasDetection() ? CV_RGB(0, 0, 255) : CV_RGB(255, 0, 0));
				Mat h_mat= detector.GetH();
				hmat= h_mat-h_mat*hShift;
				//				cout<< h_mat;
				int ctx= int(rect.XCentre()+0.5);
				int cty= int(rect.YCentre()+0.5);
				line(frame,Point2i(ctx-10,cty),Point2i(ctx+10,cty),CV_RGB(0, 255, 0));
				line(frame,Point2i(ctx,cty-10),Point2i(ctx,cty+10),CV_RGB(0, 255, 0));
				double a[9];
				a[0]= hmat.at<double>(0,0);
				a[1]= hmat.at<double>(0,1);
				a[2]= hmat.at<double>(1,0);
				a[3]= hmat.at<double>(1,1);
				a[4]= hmat.at<double>(0,2);
				a[5]= hmat.at<double>(1,2);
				a[6]= hmat.at<double>(2,0);
				a[7]= hmat.at<double>(2,1);
				a[8]= hmat.at<double>(2,2);
				
				sigma= sqrt((a[0]*a[0]+a[1]*a[1]+a[2]*a[2]+a[3]*a[3])/2);
				float areaRatio= detector.validAreaRatio(frameGrey,rect.Width(), rect.Height());	
				// write debug info to file
				fprintf(fp1,"cnt1: %3d,   | %5.3f, %5.3f, %5.3f | \n",cnt1,a[0],a[1],a[4]);
				fprintf(fp1,"             | %5.3f, %5.3f, %5.3f |    s: %5.3f, a: %3.2f\n",a[2],a[3],a[5],sigma,areaRatio);
				fprintf(fp1,"             | %5.3f, %5.3f, %5.3f |\n",a[6],a[7],a[8]);
				//////////////////////////////////////////////////////////////////////////
				//  测试每帧更新
				//detector.SetModel(frameGrey, rect);  
				//hmatMul= hmat*hmatMul;
				//hmat=Mat::eye(3,3,CV_64F);
				//sigMul= sigMul*sigma;
				//sigma= 1.0f;
				//if (cnt0%2==1)
				//{
				//	learnUpdateFlag= TRUE;
				//}				
				if(theMatchTrack.MaxCor>0.6)  // 判断相关性，避免不稳定帧更新
				{
					if (sigma>1.5 || sigma<1.0/1.5)  // 如果 H 满足 更新条件
						// 条件1   尺度 sigma> 1.732					
					{
						learnUpdateFlag= TRUE;
					}				
					else if (areaRatio<0.5)  // 如果 H 满足 更新条件
						// 条件2   矩形在图像内面积与矩形总面积比值 小于 0.7 
					{
						learnUpdateFlag= TRUE;
					}
				}
				
			}
		}
//		sigVec.push_back(sigMul*sigma);
//		prjFlag.push_back(cntUpdate);
//		hmatPartVec.push_back(hmat);

		validFlagVec.push_back(isValid);
		hmatMulVec.push_back(hmat*hmatMul);

		if (learnUpdateFlag)
		{
//			cntUpdate++;
//			hmatUpdateVec.push_back(hmat);
			detector.SetModel(frameGrey, rect);  // 更新 跟踪 区域
			hmatMul= hmat*hmatMul;
			hmat=Mat::eye(3,3,CV_64F);
			sigMul= sigMul*sigma;
			sigma= 1.0f;					
		}
		// write debug info to file
		cnt0++;
		double b[4];
		Mat b_mat= hmat*hmatMul;
		b[0]= b_mat.at<double>(0,0);
		b[1]= b_mat.at<double>(0,1);
		b[2]= b_mat.at<double>(1,0);
		b[3]= b_mat.at<double>(1,1);
		float b_sigma= sqrt((b[0]*b[0]+b[1]*b[1]+b[2]*b[2]+b[3]*b[3])/2);
		// write debug info to file
//		fprintf(fp1,"cnt0: %3d, b0~3: (%5.3f, %5.3f, %5.3f, %5.3f),bs: %5.3f\n",cnt0,b[0],b[1],b[2],b[3],b_sigma);
		//		fprintf(fp1,"                                                       sigma: %6.4f\n",sigma);

		if (!conf.quietMode)
		{
			imshow(kWindowName, detector.HasModel() ? detector.GetDebugImage() : frame);
			sprintf_s(saveNameChar, "d:\\feature\\%04d.jpg", cnt0);
//			imwrite(saveNameChar,detector.GetDebugImage());
		}

		
		// 判断出视场
		trackflag= (assertBorder(int(m_targetPosition.x+0.5),15,m_pImage->width-15)&&
			assertBorder(int(m_targetPosition.y+0.5),15,m_pImage->height-15));
		if(!trackflag)
			AfxMessageBox("目标即将出视场，停止跟踪!");
		//跟踪结果输出	
		GetFileNameFromFilePath(m_strFileName,fileName);
		fp = fopen(filePath+"\\配准跟踪结果.txt", "a" );
		fprintf(fp,fileName);
		fprintf(fp,"  %5.1f  %5.1f",m_targetPosition.x, m_targetPosition.y);
		fprintf(fp,"\n");	
		fclose(fp);
		m_pView->Invalidate(false);
		DelayMessage();
	}
	fprintf(fp1,"\n start plotting!!\n");
//	if (saveImg!=NULL) cvReleaseImage(&saveImg);
	// 画非归一化图像坐标2D跟踪曲线
	//IplImage* graph2D= draw2DFloatGraph(&trackPointVector[0], trackPointVector.size(), NULL,
	//	m_pImage->width, m_pImage->height, "2D tracklet", 0.5);
	//cvShowImage("2D_graph", graph2D);  
	//cvSaveImage(filePath+"\\目标在图像中坐标2D变化曲线hmat.jpg", graph2D);  
	//cvReleaseImage(&graph2D);
	//fprintf(fp1,"图像中坐标2D变化曲线 complete !!\n");
	// 画跟踪结果曲线图
	std::vector<CvPoint2D32f> CPVector;
	std::vector<float> disVector;
	float disTop=0, disBot=m_pImage->imageSize;
	Point2f tp, cp0, cp=Point2f(m_pImage->width/2.0f,m_pImage->height/2.0f);
	float xmax=0,ymax=0;
	Mat hmatupdate;
	int videoLength=0;
	for (int i=validFlagVec.size();i>0;i--)
	{
		if (validFlagVec.at(i-1))
		{
			videoLength= i;
			break;
		}
	}
	fprintf(fp1,"videoLength= %4d\n", videoLength);
	for (int i=0;i<videoLength;i++)
	{
		hmat= hmatMulVec.at(i);
		cp0= detector.projectPoint(cp,hmat.inv());
		CPVector.push_back(cp0-tp0);
		xmax= max(xmax,abs(tp0.x-cp0.x));
		ymax= max(ymax,abs(tp0.y-cp0.y));
		//////////////////////////////////////////////////////////////////////////
		// 1d distance
		hmat= hmatMulVec.at(i);
		tp= Point2f(trackPointVector.at(i));
		//tp= Point2f(m_pImage->width/2.0f-20,m_pImage->height/2.0f);         // 输出尺度变化结果
//		float dis= detector.homoDis(tp,cp,hmat.inv());              // 末帧hmat不稳定，转换到首帧尺度	
		float dis= sqrt( (cp0.x-tp0.x)*(cp0.x-tp0.x)+(cp0.y-tp0.y)*(cp0.y-tp0.y) );
		disVector.push_back(dis);
		disTop= max(dis,disTop);
		disBot= min(dis,disBot);
	}
	fprintf(fp1,"CPVector & disVector complete!\n");
	int topBorder= cvCeil(disTop/10.0)*10;
	int botBorder= cvFloor(disBot/10.0)*10;
	IplImage *graphImg = drawFloatGraph(&disVector[0], disVector.size(), NULL,botBorder,topBorder, 600,400,  
		"frame","d","ND from center to object",0.5 );//-25和25是数据最小值和最大值,400是窗口宽,180为高,"x angel"为曲线标签  
	//	drawFloatGraph(&floatVec2[0], floatVec2.size(), graphImg, -25,25, 400,180);//在同样的图像上画曲线 
	cvShowImage("dis_graph", graphImg);  
	//	cvWaitKey(0);
	cvSaveImage(filePath+"\\配准跟踪--视线中心与参考目标空间距离曲线.jpg", graphImg);  
	cvReleaseImage(&graphImg);  
	fprintf(fp1,"配准跟踪--视线中心与参考目标空间距离曲线 complete !!\n");
	// 画归一化2D跟踪曲线
//	saveArray(&CPVector[0], CPVector.size(),"d:\\feature\\cpvector.txt");
	IplImage* normGraph2D= drawNorm2DFloatGraph(&CPVector[0], CPVector.size(), NULL,"center-object tracklet",max(xmax,ymax));
	cvShowImage("normGraph2D", normGraph2D);  
	cvSaveImage(filePath+"\\配准跟踪--视线中心与参考目标2D空间关系.jpg", normGraph2D);  
	cvReleaseImage(&normGraph2D);
	fprintf(fp1,"配准跟踪--视线中心与参考目标2D空间关系 complete !!\n");

	//////////////////////////////////////////////////////////////////////////
	// 有实验跟踪坐标时，画实验跟踪坐标与目标真实坐标关系图
	if (!m_PointVector.empty())
	{
		xmax=0,ymax=0;
		videoLength= min(videoLength,m_PointVector.size());
		fprintf(fp1,"2nd videoLength= %4d\n", videoLength);
		if (!CPVector.empty())
		{
			CPVector.clear();
		}
		if (!disVector.empty())
		{
			disVector.clear();
		}
		for (int i=0;i<videoLength;i++)
		{
			cp=Point2f(m_PointVector.at(i));
			hmat= hmatMulVec.at(i);
			cp0= detector.projectPoint(cp,hmat.inv());
			CPVector.push_back(cp0-tp0);
			xmax= max(xmax,abs(tp0.x-cp0.x));
			ymax= max(ymax,abs(tp0.y-cp0.y));
			//////////////////////////////////////////////////////////////////////////
			// 1d distance
			hmat= hmatMulVec.at(i);
			tp= Point2f(trackPointVector.at(i));
			//tp= Point2f(m_pImage->width/2.0f-20,m_pImage->height/2.0f);         // 输出尺度变化结果
			//		float dis= detector.homoDis(tp,cp,hmat.inv());              // 末帧hmat不稳定，转换到首帧尺度	
			float dis= sqrt( (cp0.x-tp0.x)*(cp0.x-tp0.x)+(cp0.y-tp0.y)*(cp0.y-tp0.y) );
			disVector.push_back(dis);
			disTop= max(dis,disTop);
			disBot= min(dis,disBot);
		}
		fprintf(fp1,"2nd   CPVector & disVector complete!\n");
		topBorder= cvCeil(disTop/10.0)*10;
		botBorder= cvFloor(disBot/10.0)*10;
		graphImg = drawFloatGraph(&disVector[0], disVector.size(), NULL,botBorder,topBorder, 600,400,  
			"frame","d","ND from flying data to object",0.5 );//-25和25是数据最小值和最大值,400是窗口宽,180为高,"x angel"为曲线标签  
		//	drawFloatGraph(&floatVec2[0], floatVec2.size(), graphImg, -25,25, 400,180);//在同样的图像上画曲线 
		cvShowImage("flying data dis_graph", graphImg);  
		//	cvWaitKey(0);
		cvSaveImage(filePath+"\\配准跟踪--飞试结果与参考目标空间距离曲线.jpg", graphImg);  
		cvReleaseImage(&graphImg);  
		fprintf(fp1,"配准跟踪--飞试结果与参考目标空间距离曲线 complete !!\n");
		// 画归一化2D跟踪曲线
		saveArray(&CPVector[0], CPVector.size(),filePath+"\\cpvector.txt");
		normGraph2D= drawNorm2DFloatGraph(&CPVector[0], CPVector.size(), NULL,"flying data-object tracklet",max(xmax,ymax));
		cvShowImage("flying data normGraph2D", normGraph2D);  
		cvSaveImage(filePath+"\\配准跟踪--飞试结果与参考目标2D空间关系.jpg", normGraph2D);  
		cvReleaseImage(&normGraph2D);
		fprintf(fp1,"配准跟踪--飞试结果与参考目标2D空间关系hmat complete !!\n");
	}
	
	fclose(fp);
	fclose(fp1);
}
