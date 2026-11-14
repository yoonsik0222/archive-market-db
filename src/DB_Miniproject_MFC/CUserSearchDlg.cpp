// CUserSearchDlg.cpp: 구현 파일
//

#include "pch.h"
#include "DB_Miniproject_MFC.h"
#include "afxdialogex.h"
#include "CUserSearchDlg.h"


// CUserSearchDlg 대화 상자

IMPLEMENT_DYNAMIC(CUserSearchDlg, CDialogEx)

CUserSearchDlg::CUserSearchDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_USER_SEARCH_DLG, pParent)
	, m_strKeyword(_T(""))
	, m_strBrand(_T(""))
	, m_strCondition(_T(""))
	, m_nMinPrice(0)
	, m_nMaxPrice(0)
    , m_bIsVintageOnly(FALSE)
{

}

CUserSearchDlg::~CUserSearchDlg()
{
}

void CUserSearchDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_KEYWORD, m_strKeyword);
    DDX_CBString(pDX, IDC_BRAND_LIST, m_strBrand);
    DDX_CBString(pDX, IDC_CONDITION_LIST, m_strCondition);
    DDX_Text(pDX, IDC_MIN_PRICE, m_nMinPrice);
    DDX_Text(pDX, IDC_MAX_PRICE, m_nMaxPrice);
    DDX_Control(pDX, IDC_BRAND_LIST, m_cbBrandCtrl);
    DDX_Control(pDX, IDC_CONDITION_LIST, m_cbConditionCtrl);
    DDX_Check(pDX, IDC_CHECK1, m_bIsVintageOnly);
}


BEGIN_MESSAGE_MAP(CUserSearchDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &CUserSearchDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CUserSearchDlg 메시지 처리기


BOOL CUserSearchDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_cbBrandCtrl.AddString(_T("전체"));
    
    SQLHDBC hDbc = theApp.m_dbManager.GetConnectionHandle();
    SQLHSTMT hStmt;

    if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS)
    {
        SQLCHAR query[500] = { 0, };
        sprintf_s((char*)query, 500,
            "SELECT BRAND_NAME FROM BRAND ",
            "WHERE BRAND_NAME <> 'NO OR DELETED' "
            "ORDER BY BRAND_NAME ASC "
            );

        if (SQLExecDirect(hStmt, query, SQL_NTS) == SQL_SUCCESS)
        {
            SQLCHAR outBrandName[50] = { 0, };
            SQLLEN cbBrandName;

            SQLBindCol(hStmt, 1, SQL_C_CHAR, outBrandName, sizeof(outBrandName), &cbBrandName);

            while (SQLFetch(hStmt) == SQL_SUCCESS)
            {
                CString strBrand((char*)outBrandName);
                m_cbBrandCtrl.AddString(strBrand);
            }
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

    m_cbBrandCtrl.SetCurSel(0);


    // 상태 기본 설정
    m_cbConditionCtrl.AddString(_T("전체"));
    m_cbConditionCtrl.AddString(_T("새상품"));
    m_cbConditionCtrl.AddString(_T("사용감 없음"));
    m_cbConditionCtrl.AddString(_T("약간의 오염"));
    m_cbConditionCtrl.AddString(_T("심한 오염"));
    m_cbConditionCtrl.SetCurSel(0);

    return TRUE;
}
void CUserSearchDlg::OnBnClickedOk()
{
    UpdateData(TRUE);
    CDialogEx::OnOK();
}
