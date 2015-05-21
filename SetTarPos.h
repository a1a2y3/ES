#pragma once


// CSetTarPos dialog

class CSetTarPos : public CDialog
{
	DECLARE_DYNAMIC(CSetTarPos)

public:
	CSetTarPos(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetTarPos();

// Dialog Data
	enum { IDD = IDD_DIALOG_SET_TARGET_POSITION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float m_x;
	float m_y;
};
