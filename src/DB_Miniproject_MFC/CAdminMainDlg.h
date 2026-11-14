#pragma once
#include "afxdialogex.h"


// CAdminMainDlg 대화 상자

class CAdminMainDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAdminMainDlg)

public:
	CAdminMainDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CAdminMainDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADMIN_MAIN_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	virtual void RefreshWaitList();

	DECLARE_MESSAGE_MAP()
private:
	CFont m_fontForAdmin1;
public:
	CString m_strBrandName;
	CString m_strFoundationYear;
	CString m_strModelName;
	CString m_strBrandListName;
	CComboBox m_cbBrandListCtrl;
	CString m_strSeason;
	CString m_strDesigner;
	CListCtrl m_listTransaction;
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton10();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnQuery();
};
