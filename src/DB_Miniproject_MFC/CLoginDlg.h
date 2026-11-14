#pragma once
#include "afxdialogex.h"


// CLoginDlg 대화 상자

class CLoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CLoginDlg();
public:
	int m_nUserID;		// 로그인 한 USER_ID (PK)
	int m_nUserRole;	// 로그인 한 USER의 권한 판정

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGIN_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CString m_strID;
	CString m_strPW;
	CString m_strLoginNickname;		// 닉네임 전달용 변수	
	afx_msg void OnBnClickedBtnLogin();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnRegister();
};
