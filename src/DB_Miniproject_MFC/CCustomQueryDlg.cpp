// CCustomQueryDlg.cpp: 구현 파일
//

#include "pch.h"
#include "DB_Miniproject_MFC.h"
#include "afxdialogex.h"
#include "CCustomQueryDlg.h"


// CCustomQueryDlg 대화 상자

IMPLEMENT_DYNAMIC(CCustomQueryDlg, CDialogEx)

CCustomQueryDlg::CCustomQueryDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CUSTOM_QUERY_DLG, pParent)
	, m_strQuery(_T(""))
{

}

CCustomQueryDlg::~CCustomQueryDlg()
{
}

void CCustomQueryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strQuery);
	DDX_Control(pDX, IDC_LIST1, m_listResult);
}


BEGIN_MESSAGE_MAP(CCustomQueryDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BTN_QUERY, &CCustomQueryDlg::OnBnClickedBtnQuery)
END_MESSAGE_MAP()

// CCustomQueryDlg 메시지 처리기

void CCustomQueryDlg::OnBnClickedBtnQuery()
{
    UpdateData(TRUE);

    if (m_strQuery.IsEmpty()) {
        AfxMessageBox(_T("SQL 질의를 입력해 주세요."));
        return;
    }

    m_listResult.DeleteAllItems();
    while (m_listResult.DeleteColumn(0));

    SQLHDBC hDbc = theApp.m_dbManager.GetConnectionHandle();
    SQLHSTMT hStmt;

    if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS)
    {
        if (SQLExecDirect(hStmt, (SQLCHAR*)(LPCTSTR)m_strQuery, SQL_NTS) == SQL_SUCCESS) // m_strQuery를 SQLCHAR로 변환하여 직접 쿼리문을 날림
        {
            SQLSMALLINT nCols = 0;
            SQLNumResultCols(hStmt, &nCols);

            if (nCols > 0)      // 분기 : Return 된 값이 있는가? (있으면 SELECT, 없으면 다른 Modifacation Query)
            {
                for (int i = 1; i <= nCols; i++)        // 각 Column 값을 불러옴. (Query에서 탐지 된 실제 nCols 만큼)
                {
                    SQLCHAR colName[50] = { 0, };
                    SQLSMALLINT colNameLen;
                    SQLSMALLINT dataType;
                    SQLULEN colSize;
                    SQLSMALLINT decimalDigits;
                    SQLSMALLINT nullable;

                    // 실제 Column 값을 불러오는 함수
                    SQLDescribeCol(hStmt, i, colName, sizeof(colName), &colNameLen, &dataType, &colSize, &decimalDigits, &nullable);
                    m_listResult.InsertColumn(i - 1, CString((char*)colName), LVCFMT_CENTER, 120);
                }

                
                SQLCHAR* rowData = new SQLCHAR[nCols * 256]; 
                SQLLEN* cbData = new SQLLEN[nCols];          

                for (int i = 0; i < nCols; i++)
                {
                    memset(&rowData[i * 256], 0, 256); 
                    SQLBindCol(hStmt, i + 1, SQL_C_CHAR, &rowData[i * 256], 256, &cbData[i]);
                }

                int nRow = 0;
                while (SQLFetch(hStmt) == SQL_SUCCESS)
                {
                    for (int i = 0; i < nCols; i++)
                    {
                        CString strCell = (cbData[i] == SQL_NULL_DATA) ? _T("NULL") : CString((char*)&rowData[i * 256]);

                        if (i == 0) {
                            m_listResult.InsertItem(nRow, strCell);
                        }
                        else {
                            m_listResult.SetItemText(nRow, i, strCell);
                        }

                        memset(&rowData[i * 256], 0, 256);
                    }
                    nRow++;
                }

               
                delete[] rowData;
                delete[] cbData;

                AfxMessageBox(_T("질의가 성공적으로 수행되었습니다."));
            }
            else
            {
                SQLLEN nRowsAffected = 0;
                SQLRowCount(hStmt, &nRowsAffected);
                CString strMsg;
                strMsg.Format(_T("명령이 성공했습니다. (영향을 받은 행: %d개)"), nRowsAffected);
                AfxMessageBox(strMsg);
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
}
