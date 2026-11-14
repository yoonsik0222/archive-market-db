#pragma once
#include "afxdialogex.h"


// CUserInsertDlg 대화 상자

class CUserInsertDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUserInsertDlg)

public:
	CUserInsertDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CUserInsertDlg();
public:
	int m_nUserID;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_USER_INSERT_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CString m_strPostTitle;
	CString m_strModelNames;
	CString m_strItemSize;
	CString m_strConditions;
	CComboBox m_cbConditionsCtrl;
	int m_nPrice;
	CString m_strBrandName;
	CComboBox m_cbBrandCtrl;
	CString m_strSeason;
	CString m_strDesigner;
	CString m_strOriginalModel;
	CComboBox m_cbOriginalModelCtrl;
	afx_msg void OnBnClickedOk();
};
