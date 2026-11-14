#pragma once
#include "afxdialogex.h"


// CUserAppraisalDlg 대화 상자

class CUserAppraisalDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUserAppraisalDlg)

public:
	CUserAppraisalDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CUserAppraisalDlg();
public:
	int m_nUserID;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDC_USER_APPRAISAL_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listAppraisal;
};
