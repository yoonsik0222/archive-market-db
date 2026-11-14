#pragma once
#include "afxdialogex.h"


// CUserSearchDlg 대화 상자

class CUserSearchDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUserSearchDlg)

public:
	CUserSearchDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CUserSearchDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_USER_SEARCH_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CString m_strKeyword;
	CString m_strBrand;
	CString m_strCondition;
	int m_nMinPrice;
	int m_nMaxPrice;
	CComboBox m_cbBrandCtrl;
	CComboBox m_cbConditionCtrl;
	afx_msg void OnBnClickedOk();
	BOOL m_bIsVintageOnly;
};
