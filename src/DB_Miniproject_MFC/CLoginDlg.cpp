// CLoginDlg.cpp: 구현 파일
//

#include "pch.h"
#include "DB_Miniproject_MFC.h"
#include "afxdialogex.h"
#include "CLoginDlg.h"
#include "CRegisterDlg.h"


// CLoginDlg 대화 상자

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGIN_DLG, pParent)
	, m_strID(_T(""))
	, m_strPW(_T(""))
{

}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, m_strID);
	DDX_Text(pDX, IDC_EDIT3, m_strPW);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_LOGIN, &CLoginDlg::OnBnClickedBtnLogin)
    ON_BN_CLICKED(IDC_BTN_REGISTER, &CLoginDlg::OnBnClickedBtnRegister)
END_MESSAGE_MAP()


// CLoginDlg 메시지 처리기

void CLoginDlg::OnBnClickedBtnLogin()
{
    UpdateData(TRUE);

    if (m_strID.IsEmpty() || m_strPW.IsEmpty()) {
        AfxMessageBox(_T("이메일과 비밀번호를 모두 입력해 주세요."));
        return;
    }

    SQLHSTMT hStmt;
    SQLCHAR query[600];

    /*CString test;
    test.Format(_T("입력값 확인: %s / %s"), szEmail, szPassword);
    AfxMessageBox(test);*/

    SQLHDBC hDbc = theApp.m_dbManager.GetConnectionHandle();

    if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS)
    {
        sprintf_s((char*)query, 600,
            "SELECT USER_ID, USER_NICKNAME FROM USER_INFO "
            "WHERE USER_EMAIL = '%s' AND USER_PASSWORD = '%s' AND IS_DELETED = 0 ",
            (char*)CT2A((LPCTSTR)m_strID),
            (char*)CT2A((LPCTSTR)m_strPW));

        if (SQLExecDirect(hStmt, query, SQL_NTS) == SQL_SUCCESS)
        {

            SQLINTEGER outUserID = 0;
            SQLCHAR outNickname[50] = { 0, };

            SQLBindCol(hStmt, 1, SQL_C_LONG, &outUserID, sizeof(outUserID), NULL);
            SQLBindCol(hStmt, 2, SQL_C_CHAR, outNickname, sizeof(outNickname), NULL);

            SQLRETURN fetchRet = SQLFetch(hStmt);
            if (fetchRet == SQL_SUCCESS)
            {
                m_nUserID = outUserID;
                m_nUserRole = (outUserID == 1) ? 1 : 2;

                CString strNickname = CA2T((char*)outNickname);
                m_strLoginNickname = CString((char*)outNickname);

                CString strWelcome;
                strWelcome.Format(_T("%s님, 환영합니다! 아카이브 패션 플랫폼에 접속합니다."), strNickname);
                AfxMessageBox(strWelcome);;

                SQLCloseCursor(hStmt);
                SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
                EndDialog(IDOK);
                return;
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
    AfxMessageBox(_T("로그인 실패! 이메일 및 비밀번호를 다시 확인하시거나 탈퇴 계정 여부를 검토하십시오."));
}
BOOL CLoginDlg::PreTranslateMessage(MSG* pMsg)
{
    // 사용자가 키보드를 눌렀을 때
    if (pMsg->message == WM_KEYDOWN)
    {
        // 엔터 키를 눌렀을 때 창이 그냥 닫히는 것을 방어
        if (pMsg->wParam == VK_RETURN)
        {
            OnBnClickedBtnLogin();
            return TRUE;
        }

        // ESC 키를 눌렀을 때 창이 닫히는 것도 방어
        if (pMsg->wParam == VK_ESCAPE)
        {
            return TRUE;
        }
    }

    return CDialogEx::PreTranslateMessage(pMsg);
}

void CLoginDlg::OnBnClickedBtnRegister()
{
    CRegisterDlg dlgRegister;
    INT_PTR nResponse = dlgRegister.DoModal();
}
