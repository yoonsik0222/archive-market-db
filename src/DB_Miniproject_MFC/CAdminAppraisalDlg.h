#pragma once
#include "afxdialogex.h"


// CAdminAppraisalDlg 대화 상자

class CAdminAppraisalDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAdminAppraisalDlg)

public:
	CAdminAppraisalDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CAdminAppraisalDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADMIN_APPRAISAL_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
public:
	CString m_strTargetItemID;

	DECLARE_MESSAGE_MAP()
	CComboBox m_cbGradeCtrl;
	CString m_strGrade;
	CComboBox m_cbResultCtrl;
	CString m_strResult;
	CComboBox m_cbAppraiserCtrl;
	CString m_strComment;
	afx_msg void OnBnClickedOk();
};
