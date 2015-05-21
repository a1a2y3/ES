
// EsView.h : CEsView ��Ľӿ�
#pragma once
#include "opencv\cv.h"
class CTargetRectDlg;
class CEsDoc;
class CEsView : public CScrollView
{
protected: // �������л�����
	CEsView();
	DECLARE_DYNCREATE(CEsView)

	// ����
public:
	CEsDoc* GetDocument() const;

	// ����
public:
	CEsDoc             *m_pDoc;
	BOOL                m_bDialogOK;
	CTargetRectDlg*    *m_pDialog;
	BOOL               *m_pIsTargetSetted;
	CvPoint2D32f       *m_pTarget;
	int                 m_TargetCount;
	CRect               m_DialogRect;
	BOOL                m_bShowTargDlg;
	CPoint              m_OldPt;
	CPoint              m_MousePt;
	BOOL                m_bDrawLine;
	BOOL                m_bDrawTarget;
	BOOL				m_bTargetSetCanceld;

	// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ������һ�ε���
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// ʵ��
public:
	void ReleaseMem();
	void DrawCrossOnTarget(CvPoint2D32f  Target, int TargetNo=-1);
	void DrawMaskOnTarget(CvPoint2D32f  Target, int TargetNo=-1);
	void DrawRectOnTarget(CvPoint2D32f  Target, int TargetNo=-1);
	void InitiateTarget(int TargetCount);
	void ResizeWindow();
	void DrawCursor(CPoint point);
	virtual ~CEsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSelectPoints();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnManualSelectTarget();
	afx_msg void OnCenterFromCorners();
};

#ifndef _DEBUG  // EsView.cpp �еĵ��԰汾
inline CEsDoc* CEsView::GetDocument() const
{ return reinterpret_cast<CEsDoc*>(m_pDocument); }
#endif

