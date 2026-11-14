#pragma once
#include "afxdialogex.h"


// CCustomQueryDlg 대화 상자

class CCustomQueryDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCustomQueryDlg)

public:
	CCustomQueryDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CCustomQueryDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CUSTOM_QUERY_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strQuery;
	CListCtrl m_listResult;
	afx_msg void OnBnClickedBtnQuery();
};
