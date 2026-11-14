// CUserInsertDlg.cpp: 구현 파일
//

#include "pch.h"
#include "DB_Miniproject_MFC.h"
#include "afxdialogex.h"
#include "CUserInsertDlg.h"


// CUserInsertDlg 대화 상자

IMPLEMENT_DYNAMIC(CUserInsertDlg, CDialogEx)

CUserInsertDlg::CUserInsertDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_USER_INSERT_DLG, pParent)
	, m_strPostTitle(_T(""))
	, m_strModelNames(_T(""))
	, m_strItemSize(_T(""))
	, m_strConditions(_T(""))
	, m_nPrice(0)
	, m_strBrandName(_T(""))
	, m_strSeason(_T(""))
	, m_strDesigner(_T(""))
	, m_strOriginalModel(_T(""))
{

}

CUserInsertDlg::~CUserInsertDlg()
{
}

void CUserInsertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_POST_TITLE, m_strPostTitle);
	DDX_Text(pDX, IDC_MODEL_NAMES, m_strModelNames);
	DDX_Text(pDX, IDC_SIZE, m_strItemSize);
	DDX_CBString(pDX, IDC_CONDITIONS, m_strConditions);
	DDX_Control(pDX, IDC_CONDITIONS, m_cbConditionsCtrl);
	DDX_Text(pDX, IDC_PRICE, m_nPrice);
	DDX_CBString(pDX, IDC_BRAND, m_strBrandName);
	DDX_Control(pDX, IDC_BRAND, m_cbBrandCtrl);
	DDX_Text(pDX, IDC_SEASON, m_strSeason);
	DDX_Text(pDX, IDC_DESIGNER, m_strDesigner);
	DDX_CBString(pDX, IDC_ORIGINAL_MODEL, m_strOriginalModel);
	DDX_Control(pDX, IDC_ORIGINAL_MODEL, m_cbOriginalModelCtrl);
}


BEGIN_MESSAGE_MAP(CUserInsertDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CUserInsertDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CUserInsertDlg 메시지 처리기

BOOL CUserInsertDlg::OnInitDialog() {

    CDialogEx::OnInitDialog();

    m_cbConditionsCtrl.AddString(_T("새상품"));
    m_cbConditionsCtrl.AddString(_T("사용감 없음"));
    m_cbConditionsCtrl.AddString(_T("약간의 오염"));
    m_cbConditionsCtrl.AddString(_T("심한 오염"));
    m_cbConditionsCtrl.SetCurSel(0);

    m_cbOriginalModelCtrl.AddString(_T("해당 없음"));

    SQLHDBC hDbc = theApp.m_dbManager.GetConnectionHandle();
    SQLHSTMT hStmt;

    if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS)
    {
        SQLCHAR query[500] = { 0, };
        sprintf_s((char*)query, 500,
            "SELECT BRAND_NAME FROM BRAND WHERE BRAND_NAME <> 'NO OR DELETED' ORDER BY BRAND_NAME ASC "
        );

        if (SQLExecDirect(hStmt, query, SQL_NTS) == SQL_SUCCESS)
        {
            SQLCHAR outBrand[50] = { 0, };
            SQLLEN cbBrand;

            SQLBindCol(hStmt, 1, SQL_C_CHAR, outBrand, sizeof(outBrand), &cbBrand);

            while (SQLFetch(hStmt) == SQL_SUCCESS)
            {
                m_cbBrandCtrl.AddString(CString((char*)outBrand));
            }
        }
        SQLCloseCursor(hStmt);

        sprintf_s((char*)query, 500,
            "SELECT MODEL_NAMES "
            "FROM ITEM "
            "WHERE ITEM_ID BETWEEN 0 AND 1000 "
            "ORDER BY MODEL_NAMES ASC"
            );

        if (SQLExecDirect(hStmt, query, SQL_NTS) == SQL_SUCCESS)
        {
            SQLCHAR outModel[100] = { 0, };
            SQLLEN cbModel;

            SQLBindCol(hStmt, 1, SQL_C_CHAR, outModel, sizeof(outModel), &cbModel);

            while (SQLFetch(hStmt) == SQL_SUCCESS)
            {
                m_cbOriginalModelCtrl.AddString(CString((char*)outModel));
            }
        }
        SQLCloseCursor(hStmt);
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    }

    // 기본 선택값 세팅 (브랜드는 첫 번째 데이터, 복각 모델은 '해당 없음')
    m_cbBrandCtrl.SetCurSel(0);
    m_cbOriginalModelCtrl.SetCurSel(0);

    return TRUE;

}

void CUserInsertDlg::OnBnClickedOk()
{
    UpdateData(TRUE);

    if (m_strPostTitle.IsEmpty() || m_strModelNames.IsEmpty() || m_strItemSize.IsEmpty() ||
        m_strConditions.IsEmpty() || m_strBrandName.IsEmpty() || m_nPrice <= 0)
    {
        AfxMessageBox(_T("필수 입력 사항을 모두 올바르게 입력해 주세요. (가격은 0보다 커야 합니다)"));
        return;
    }

    // 선택 입력 사항의 NULL 처리 로직 세팅
    CString strSeasonVal = m_strSeason.IsEmpty() ? _T("NULL") : _T("'") + m_strSeason + _T("'");
    CString strDesignerVal = m_strDesigner.IsEmpty() ? _T("NULL") : _T("'") + m_strDesigner + _T("'");

    CString strOriginalModelVal = m_strOriginalModel.IsEmpty() ? _T("NULL") :
        _T("(SELECT ITEM_ID FROM ITEM WHERE MODEL_NAMES = '") + m_strOriginalModel + _T("')");


    SQLHSTMT hStmt;
    SQLHDBC hDbc = theApp.m_dbManager.GetConnectionHandle();
    SQLCHAR query[2000] = { 0, };

    if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS)
    {
        sprintf_s((char*)query, 2000,
            "INSERT INTO ITEM "
            "(ITEM_ID, MODEL_NAMES, POST_TITLE, SEASON, ITEM_SIZE, USER_CONDITIONS, PRICE, BUY_STATUS, DESIGNER, BRAND_ID, OWN_USER, ORIGINAL_MODEL) "
            "SELECT "
            "(SELECT MAX(ITEM_ID) FROM ITEM) + 1, "
            "'%s', '%s', %s, '%s', '%s', %d, '판매중', %s, "
            "(SELECT BRAND_ID FROM BRAND WHERE BRAND_NAME = '%s'), %d, %s ",
            m_strModelNames,
            m_strPostTitle,
            strSeasonVal,
            m_strItemSize,
            m_strConditions,
            m_nPrice,
            strDesignerVal,
            m_strBrandName,
            m_nUserID,
            strOriginalModelVal
        );

        if (SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS) == SQL_SUCCESS)
        {
            AfxMessageBox(_T("상품이 성공적으로 등록되었습니다!"));
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