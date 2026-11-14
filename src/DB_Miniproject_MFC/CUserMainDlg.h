#pragma once
#include "afxdialogex.h"


// CUserMainDlg 대화 상자

class CUserMainDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUserMainDlg)

public:
	CUserMainDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CUserMainDlg();
public:
	int m_nUserID;
	CString m_strLoginNickname;		// 닉네임 전달용 변수


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_USER_MAIN_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	CFont m_fontForUserName;
	
public:
	afx_msg void OnBnClickedButton1();
	CListCtrl m_listItems;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnNMCustomdrawList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton6();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClose();
};
