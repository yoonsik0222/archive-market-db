// CUserPurchaseDlg.cpp: 구현 파일
//

#include "pch.h"
#include "DB_Miniproject_MFC.h"
#include "afxdialogex.h"
#include "CUserPurchaseDlg.h"


// CUserPurchaseDlg 대화 상자

IMPLEMENT_DYNAMIC(CUserPurchaseDlg, CDialogEx)

CUserPurchaseDlg::CUserPurchaseDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_USER_PURCHASE_DLG, pParent)
	, m_strAddress(_T(""))
	, m_strPaymentMethod(_T(""))
{

}

CUserPurchaseDlg::~CUserPurchaseDlg()
{
}

void CUserPurchaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strAddress);
	DDX_CBString(pDX, IDC_COMBO1, m_strPaymentMethod);
	DDX_Control(pDX, IDC_COMBO1, m_cbPaymentCtrl);
}


BEGIN_MESSAGE_MAP(CUserPurchaseDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CUserPurchaseDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CUserPurchaseDlg 메시지 처리기

BOOL CUserPurchaseDlg::OnInitDialog() {

	CDialogEx::OnInitDialog();

	SetDlgItemText(IDC_PURCHASE_TITLE, m_strItemTitle);

	CString strPriceFormatted;
	strPriceFormatted.Format(_T("%s 원"), m_strItemPrice);
	SetDlgItemText(IDC_PURCHASE_PRICE, strPriceFormatted);

	m_cbPaymentCtrl.AddString(_T("신용카드"));
	m_cbPaymentCtrl.AddString(_T("계좌이체"));
	m_cbPaymentCtrl.AddString(_T("카카오페이"));
	m_cbPaymentCtrl.AddString(_T("토스페이"));
	m_cbPaymentCtrl.SetCurSel(0);

	return TRUE;
}
void CUserPurchaseDlg::OnBnClickedOk()
{
    UpdateData(TRUE);

    if (m_strAddress.IsEmpty() || m_strPaymentMethod.IsEmpty()) {
        AfxMessageBox(_T("배송지 주소와 결제 수단을 모두 입력해 주세요."));
        return;
    }

    if (AfxMessageBox(_T("결제를 진행하시겠습니까?\n(상품이 '감정 대기' 상태로 전환됩니다.)"), MB_YESNO | MB_ICONQUESTION) == IDYES)
    {
        SQLHSTMT hStmt;
        SQLHDBC hDbc = theApp.m_dbManager.GetConnectionHandle();

        if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS)
        {
            SQLCHAR queryCheck[500];
            sprintf_s((char*)queryCheck, 500, "SELECT OWN_USER FROM ITEM WHERE ITEM_ID = %s", m_strTargetItemID);

            if (SQLExecDirect(hStmt, queryCheck, SQL_NTS) == SQL_SUCCESS)
            {
                SQLINTEGER nRealOwnerID = 0;
                SQLLEN cbOwner;
                SQLBindCol(hStmt, 1, SQL_C_LONG, &nRealOwnerID, 0, &cbOwner);

                if (SQLFetch(hStmt) == SQL_SUCCESS)
                {
                    SQLCloseCursor(hStmt);

                    if (nRealOwnerID == m_nUserID)
                    {
                        AfxMessageBox(_T("본인이 등록한 상품은 직접 구매할 수 없습니다."));
                        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
                        return;
                    }
                }
            }
            SQLCHAR query[1000];
            sprintf_s((char*)query, 1000,
                "SET NOCOUNT ON; "
                "UPDATE ITEM "
                "SET BUY_STATUS = '감정대기' "
                "WHERE ITEM_ID = %s; "

                "INSERT INTO TRANSACTIONS "
                "(TRADE_ID, PRICE, PAYMENT_METHOD, TRANSACTION_DATE, ITEM_ID, SELLER_ID, BUYER_ID) "
                "SELECT "
                "(SELECT MAX(TRADE_ID) FROM TRANSACTIONS) + 1, "
                "%s, '%s', GETDATE(), %s, OWN_USER, %d "
                "FROM ITEM WHERE ITEM_ID = %s;",

                m_strTargetItemID,
                m_strItemPrice,
                m_strPaymentMethod,
                m_strTargetItemID,
                m_nUserID,
                m_strTargetItemID
            );

            if (SQLExecDirect(hStmt, query, SQL_NTS) == SQL_SUCCESS)
            {
                AfxMessageBox(_T("결제가 완료되었습니다!\n판매자가 검수 센터로 상품을 발송하면 감정이 시작됩니다."));
                CDialogEx::OnOK();
            }
            else
            {
                // SQLExecDirect 실패 시 에러 메시지 추출
                SQLCHAR sqlState[6], errMsg[256];
                SQLINTEGER nativeErr;
                SQLSMALLINT msgLen;
                SQLError(SQL_NULL_HENV, SQL_NULL_HDBC, hStmt, sqlState, &nativeErr, errMsg, sizeof(errMsg), &msgLen);

                CString strErr;
                strErr.Format(_T("SQL 오류: %s\nSQLState: %s"), errMsg, sqlState);
                AfxMessageBox(strErr);
            }

            SQLCloseCursor(hStmt);
            SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        }
        else
        {
            AfxMessageBox(_T("DB 연결 핸들을 할당할 수 없습니다."));
        }
    }
}
