// CRegisterDlg.cpp: 구현 파일
//

#include "pch.h"
#include "DB_Miniproject_MFC.h"
#include "afxdialogex.h"
#include "CRegisterDlg.h"


// CRegisterDlg 대화 상자

IMPLEMENT_DYNAMIC(CRegisterDlg, CDialogEx)

CRegisterDlg::CRegisterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REGISTER_DLG, pParent)
	, m_strEmail(_T(""))
	, m_strNickname(_T(""))
	, m_strPassword(_T(""))
{

}

CRegisterDlg::~CRegisterDlg()
{
}

void CRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strEmail);
	DDX_Text(pDX, IDC_EDIT4, m_strNickname);
	DDX_Text(pDX, IDC_EDIT5, m_strPassword);
}


BEGIN_MESSAGE_MAP(CRegisterDlg, CDialogEx)
	ON_BN_CLICKED(IDOK3, &CRegisterDlg::OnBnClickedOk3)
	ON_BN_CLICKED(IDOK4, &CRegisterDlg::OnBnClickedOk4)
	ON_BN_CLICKED(IDOK, &CRegisterDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CRegisterDlg 메시지 처리기

void CRegisterDlg::OnBnClickedOk3()
{
	UpdateData(TRUE);

	if (m_strEmail.IsEmpty()) {
		AfxMessageBox(_T("이메일을 입력해 주세요."));
		return;
	}

	SQLHDBC hDbc = theApp.m_dbManager.GetConnectionHandle();
	SQLHSTMT hStmt;

	if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS) {
		SQLCHAR query[500];

		sprintf_s((char*)query, 500, 
			"SELECT COUNT(*) FROM USER_INFO WHERE USER_EMAIL = '%s'",
			m_strEmail
		);
		if (SQLExecDirect(hStmt, query, SQL_NTS) == SQL_SUCCESS) {
			SQLINTEGER nCount = 0;
			SQLLEN cbCount;

			SQLBindCol(hStmt, 1, SQL_C_LONG, &nCount, 0, &cbCount);

			if (SQLFetch(hStmt) == SQL_SUCCESS) {
				if (nCount == 0) {
					AfxMessageBox(_T("사용 가능한 이메일입니다."));
					m_strCheckedEmail = m_strEmail;
				}
				else {
					AfxMessageBox(_T("이미 사용 중인 이메일 입니다. 다른 이메일을 입력해주세요."));
					m_strCheckedEmail = _T("");
				}
			}
		}
		SQLCloseCursor(hStmt);
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	}

}

void CRegisterDlg::OnBnClickedOk4()
{
	UpdateData(TRUE);

	if (m_strEmail.IsEmpty()) {
		AfxMessageBox(_T("닉네임을 입력해 주세요."));
		return;
	}

	SQLHDBC hDbc = theApp.m_dbManager.GetConnectionHandle();
	SQLHSTMT hStmt;

	if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS) {
		SQLCHAR query[500];

		sprintf_s((char*)query, 500, 
			"SELECT COUNT(*) FROM USER_INFO WHERE USER_NICKNAME = '%s'", 
			m_strNickname
		);
		if (SQLExecDirect(hStmt, query, SQL_NTS) == SQL_SUCCESS) {

			SQLINTEGER nCount = 0;
			SQLLEN cbCount;

			SQLGetData(hStmt, 1, SQL_C_LONG, &nCount, 0, &cbCount);

			if (SQLFetch(hStmt) == SQL_SUCCESS) {
				
				if (nCount == 0) {
					AfxMessageBox(_T("사용 가능한 닉네임 입니다."));
					m_strCheckedNickname = m_strNickname;
				}
				else {
					AfxMessageBox(_T("이미 사용 중인 닉네임 입니다. 다른 닉네임을 입력해주세요."));
					m_strCheckedNickname = _T("");
				}
			}
		}
		SQLCloseCursor(hStmt);
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	}
}

void CRegisterDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	if (m_strEmail.IsEmpty() || m_strNickname.IsEmpty() || m_strPassword.IsEmpty()) {
		AfxMessageBox(_T("모든 항목을 입력해주세요."));
		return;
	}

	if (m_strEmail != m_strCheckedEmail) {
		AfxMessageBox(_T("이메일 중복 검사를 진행해주세요."));
		return;
	}

	if (m_strNickname != m_strCheckedNickname) {
		AfxMessageBox(_T("닉네임 중복 검사를 진행해주세요."));
		return;
	}

	if (m_strPassword.Find(_T("'")) != -1 ||
		m_strPassword.Find(_T(";")) != -1 ||
		m_strPassword.Find(_T("-")) != -1)
		{
			AfxMessageBox(_T("비밀번호에는 작은 따음표('), 세미콜론(;), 하이픈(-)이 포함될 수 없습니다."));
			return;
		}

	SQLHDBC hDbc = theApp.m_dbManager.GetConnectionHandle();
	SQLHSTMT hStmt;

	if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS) {
		SQLCHAR query[1000];
		sprintf_s((char*)query, 1000,
			"INSERT INTO USER_INFO "
			"(USER_ID, USER_EMAIL, USER_NICKNAME, USER_PASSWORD, IS_DELETED) "
			"SELECT (SELECT MAX(USER_ID) FROM USER_INFO) + 1, "
			"'%s', '%s', '%s', 0",
			m_strCheckedEmail, m_strCheckedNickname, m_strPassword
		);

		if (SQLExecDirect(hStmt, query, SQL_NTS) == SQL_SUCCESS) {
			AfxMessageBox(_T("회원가입이 완료되었습니다! 창을 닫고 로그인해주세요."));
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
