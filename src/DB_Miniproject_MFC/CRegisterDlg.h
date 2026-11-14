#pragma once
#include "afxdialogex.h"


// CRegisterDlg 대화 상자

class CRegisterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRegisterDlg)

public:
	CRegisterDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CRegisterDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REGISTER_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strEmail;
	CString m_strNickname;
	CString m_strPassword;
	CString m_strCheckedEmail;
	CString m_strCheckedNickname;
	afx_msg void OnBnClickedOk3();
	afx_msg void OnBnClickedOk4();
	afx_msg void OnBnClickedOk();
};
