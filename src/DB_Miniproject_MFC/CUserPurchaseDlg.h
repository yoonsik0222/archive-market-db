#pragma once
#include "afxdialogex.h"


// CUserPurchaseDlg 대화 상자

class CUserPurchaseDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUserPurchaseDlg)

public:
	CUserPurchaseDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CUserPurchaseDlg();
public:
	int m_nUserID;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_USER_PURCHASE_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CString m_strTargetItemID;
	CString m_strItemTitle;
	CString m_strItemPrice;
	CString m_strAddress;
	CString m_strPaymentMethod;
	CComboBox m_cbPaymentCtrl;
	afx_msg void OnBnClickedOk();
};
