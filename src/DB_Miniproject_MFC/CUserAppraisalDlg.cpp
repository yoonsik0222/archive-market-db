// CUserAppraisalDlg.cpp: 구현 파일
//

#include "pch.h"
#include "DB_Miniproject_MFC.h"
#include "afxdialogex.h"
#include "CUserAppraisalDlg.h"


// CUserAppraisalDlg 대화 상자

IMPLEMENT_DYNAMIC(CUserAppraisalDlg, CDialogEx)

CUserAppraisalDlg::CUserAppraisalDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDC_USER_APPRAISAL_DLG, pParent)
{

}

CUserAppraisalDlg::~CUserAppraisalDlg()
{
}

void CUserAppraisalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST3, m_listAppraisal);
}


BEGIN_MESSAGE_MAP(CUserAppraisalDlg, CDialogEx)
END_MESSAGE_MAP()

BOOL CUserAppraisalDlg::OnInitDialog() {

	CDialogEx::OnInitDialog();

	m_listAppraisal.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	m_listAppraisal.InsertColumn(0, _T("상품명(게시글 제목)"), LVCFMT_LEFT, 200);
	m_listAppraisal.InsertColumn(1, _T("감정사 이름"), LVCFMT_CENTER, 100);
	m_listAppraisal.InsertColumn(2, _T("감정 등급"), LVCFMT_CENTER, 100);
	m_listAppraisal.InsertColumn(3, _T("감정 결과"), LVCFMT_CENTER, 100);
	m_listAppraisal.InsertColumn(4, _T("코멘트"), LVCFMT_LEFT, 300);

	// 감정 결과 불러오기

	SQLHDBC hDbc = theApp.m_dbManager.GetConnectionHandle();
	SQLHSTMT hStmt;

	if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS) {
		SQLCHAR query[1000];
		sprintf_s((char*)query, 1000,
			"SELECT I.POST_TITLE, AP.APPI_NAME, A.VERIFIED_GRADE, A.FINAL_RESULT, A.COMMENT "
			"FROM ITEM I, APPRAISAL A, APPRAISER AP "
			"WHERE I.ITEM_ID = A.ITEM_ID "              // 아이템과 감정 내역 연결
			"AND A.APPRAISER_ID = AP.APPRAISER_ID "   // 감정 내역과 감정사 정보 연결
			"AND I.OWN_USER = %d ",                     // 내 상품만 필터링
			m_nUserID);
        if (SQLExecDirect(hStmt, query, SQL_NTS) == SQL_SUCCESS)
        {
            int nRow = 0;

            SQLCHAR outPostTitle[200] = { 0, };
            SQLCHAR outAppraiserName[50] = { 0, };
            SQLCHAR outVerifiedGrade[50] = { 0, };
            SQLCHAR outFinalResult[50] = { 0, };
            SQLCHAR outComment[300] = { 0, };

            // 데이터 매핑
            SQLLEN cbPostTitle, cbAppraiserName, cbVerifiedGrade, cbFinalResult, cbComment;

            SQLBindCol(hStmt, 1, SQL_C_CHAR, outPostTitle, sizeof(outPostTitle), &cbPostTitle);
            SQLBindCol(hStmt, 2, SQL_C_CHAR, outAppraiserName, sizeof(outAppraiserName), &cbAppraiserName);
            SQLBindCol(hStmt, 3, SQL_C_CHAR, outVerifiedGrade, sizeof(outVerifiedGrade), &cbVerifiedGrade);
            SQLBindCol(hStmt, 4, SQL_C_CHAR, outFinalResult, sizeof(outFinalResult), &cbFinalResult);
            SQLBindCol(hStmt, 5, SQL_C_CHAR, outComment, sizeof(outComment), &cbComment);

            while (SQLFetch(hStmt) == SQL_SUCCESS)
            {
                // 포맷팅
                CString strPostTitle((char*)outPostTitle);
                CString strAppraiserName((char*)outAppraiserName);
                CString strVerifiedGrade((char*)outVerifiedGrade);
                CString strFinalResult((char*)outFinalResult);
                CString strComment((char*)outComment);

                // NULL 예외 처리
    
                m_listAppraisal.InsertItem(nRow, strPostTitle);
                m_listAppraisal.SetItemText(nRow, 1, strAppraiserName);
                m_listAppraisal.SetItemText(nRow, 2, strVerifiedGrade);
                m_listAppraisal.SetItemText(nRow, 3, strFinalResult);
                m_listAppraisal.SetItemText(nRow, 4, strComment);

                nRow++;
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

    return TRUE;

}