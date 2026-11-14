// CAdminAppraisalDlg.cpp: 구현 파일
//

#include "pch.h"
#include "DB_Miniproject_MFC.h"
#include "afxdialogex.h"
#include "CAdminAppraisalDlg.h"


// CAdminAppraisalDlg 대화 상자

IMPLEMENT_DYNAMIC(CAdminAppraisalDlg, CDialogEx)

CAdminAppraisalDlg::CAdminAppraisalDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_ADMIN_APPRAISAL_DLG, pParent)
	, m_strGrade(_T(""))
	, m_strResult(_T(""))
	, m_strComment(_T(""))
{

}

CAdminAppraisalDlg::~CAdminAppraisalDlg()
{
}

void CAdminAppraisalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cbGradeCtrl);
	DDX_CBString(pDX, IDC_COMBO1, m_strGrade);
	DDX_Control(pDX, IDC_COMBO3, m_cbResultCtrl);
	DDX_CBString(pDX, IDC_COMBO3, m_strResult);
	DDX_Control(pDX, IDC_COMBO4, m_cbAppraiserCtrl);
	DDX_Text(pDX, IDC_EDIT1, m_strComment);
}


BEGIN_MESSAGE_MAP(CAdminAppraisalDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &CAdminAppraisalDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CAdminAppraisalDlg 메시지 처리기
BOOL CAdminAppraisalDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();

    m_cbGradeCtrl.AddString(_T("S"));
	m_cbGradeCtrl.AddString(_T("A"));
	m_cbGradeCtrl.AddString(_T("B"));
	m_cbGradeCtrl.AddString(_T("F"));
	m_cbGradeCtrl.SetCurSel(0);

	m_cbResultCtrl.AddString(_T("PASS"));
	m_cbResultCtrl.AddString(_T("FAIL"));
	m_cbResultCtrl.SetCurSel(0);

    SQLHDBC hDbc = theApp.m_dbManager.GetConnectionHandle();
    SQLHSTMT hStmt;
    SQLCHAR query[500];

    if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS)
    {
        sprintf_s((char*)query, 500,
            "SELECT APPRAISER_ID, APPI_NAME FROM APPRAISER WHERE APPRAISER_ID <> 0 "
        );

        if (SQLExecDirect(hStmt, query, SQL_NTS) == SQL_SUCCESS)
        {
            SQLINTEGER outID = 0;
            SQLCHAR outName[50] = { 0, };
            SQLLEN cbID, cbName;

            SQLBindCol(hStmt, 1, SQL_C_LONG, &outID, 0, &cbID);
            SQLBindCol(hStmt, 2, SQL_C_CHAR, outName, sizeof(outName), &cbName);

            while (SQLFetch(hStmt) == SQL_SUCCESS)
            {
                // 콤보박스에 이름을 띄우고, 뒤에 Hidden ID.
                int nIndex = m_cbAppraiserCtrl.AddString(CString((char*)outName));
                m_cbAppraiserCtrl.SetItemData(nIndex, (DWORD_PTR)outID);
            }
        }
        SQLCloseCursor(hStmt);
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    }
    m_cbAppraiserCtrl.SetCurSel(0);

    return TRUE;
}
void CAdminAppraisalDlg::OnBnClickedOk()
{
    UpdateData(TRUE);

    if (m_strComment.IsEmpty()) {
        AfxMessageBox(_T("코멘트를 반드시 작성해 주세요."));
        return;
    }

    // 숨겨둔 감정사 ID 꺼내오기
    int nAppraiserID = (int)m_cbAppraiserCtrl.GetItemData(m_cbAppraiserCtrl.GetCurSel());

    SQLHSTMT hStmt;
    SQLHDBC hDbc = theApp.m_dbManager.GetConnectionHandle();

    if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS)
    {
        SQLCHAR query[2000];

        if (m_strResult == _T("PASS"))
        {
            sprintf_s((char*)query, 2000,
                "SET NOCOUNT ON; "
                // 감정 내역 저장
                "INSERT INTO APPRAISAL (ITEM_ID, APPRAISAL_NO, VERIFIED_GRADE, FINAL_RESULT, COMMENT, APPRAISER_ID) "
                "SELECT %s, (SELECT COUNT(*) FROM APPRAISAL WHERE ITEM_ID = %s) + 1, "
                "'%s', '%s', '%s', %d; "
                // 아이템 상태 '판매완료'로 확정
                "UPDATE ITEM SET BUY_STATUS = '판매완료' WHERE ITEM_ID = %s; ",

                m_strTargetItemID, m_strTargetItemID, // INSERT의 ITEM_ID 두 번
                m_strGrade, m_strResult, m_strComment, nAppraiserID,
                m_strTargetItemID // UPDATE의 ITEM_ID
            );
        }
        else
        {
            sprintf_s((char*)query, 2000,
                "SET NOCOUNT ON; "
                // 감정 내역
                "INSERT INTO APPRAISAL (ITEM_ID, APPRAISAL_NO, VERIFIED_GRADE, FINAL_RESULT, COMMENT, APPRAISER_ID) "
                "SELECT %s, (SELECT COUNT(*) FROM APPRAISAL WHERE ITEM_ID = %s) + 1, "
                "'%s', '%s', '%s', %d; "
                // 결제 내역(TRANSACTIONS) 삭제 (환불 처리)
                "DELETE FROM TRANSACTIONS WHERE ITEM_ID = %s; "
                // 아이템을 다시 시장에 내놓기 (판매중)
                "UPDATE ITEM SET BUY_STATUS = '판매중' WHERE ITEM_ID = %s; ",

                m_strTargetItemID, m_strTargetItemID,
                m_strGrade, m_strResult, m_strComment, nAppraiserID,
                m_strTargetItemID, // DELETE용 ID
                m_strTargetItemID  // UPDATE용 ID
            );
        }

        // 쿼리 실행
        if (SQLExecDirect(hStmt, query, SQL_NTS) == SQL_SUCCESS) {
            AfxMessageBox(_T("감정 처리가 완료되었습니다."));
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
}
