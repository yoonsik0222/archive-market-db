// CAdminMainDlg.cpp: 구현 파일
//

#include "pch.h"
#include "DB_Miniproject_MFC.h"
#include "afxdialogex.h"
#include "CAdminMainDlg.h"
#include "CAdminAppraisalDlg.h"
#include "CCustomQueryDlg.h"


// CAdminMainDlg 대화 상자

IMPLEMENT_DYNAMIC(CAdminMainDlg, CDialogEx)

CAdminMainDlg::CAdminMainDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ADMIN_MAIN_DLG, pParent)
    , m_strBrandName(_T(""))
    , m_strFoundationYear(_T(""))
    , m_strModelName(_T(""))
    , m_strBrandListName(_T(""))
    , m_strSeason(_T(""))
    , m_strDesigner(_T(""))
{

}

CAdminMainDlg::~CAdminMainDlg()
{
}

void CAdminMainDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT1, m_strBrandName);
    DDX_Text(pDX, IDC_EDIT4, m_strFoundationYear);
    DDX_Text(pDX, IDC_EDIT5, m_strModelName);
    DDX_CBString(pDX, IDC_COMBO2, m_strBrandListName);
    DDX_Control(pDX, IDC_COMBO2, m_cbBrandListCtrl);
    DDX_Text(pDX, IDC_EDIT6, m_strSeason);
    DDX_Text(pDX, IDC_EDIT7, m_strDesigner);
    DDX_Control(pDX, IDC_LIST1, m_listTransaction);
}


BEGIN_MESSAGE_MAP(CAdminMainDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON9, &CAdminMainDlg::OnBnClickedButton9)
    ON_BN_CLICKED(IDC_BUTTON1, &CAdminMainDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON10, &CAdminMainDlg::OnBnClickedButton10)
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_BTN_QUERY, &CAdminMainDlg::OnBnClickedBtnQuery)
END_MESSAGE_MAP()

// CAdminMainDlg 메시지 처리기
BOOL CAdminMainDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_fontForAdmin1.CreateFont(
        -50,                        // 높이 (픽셀 단위, 음수로 쓰면 포인트 단위)
        0,                          // 너비 (0이면 글꼴 비율에 맞춤)
        0,                          // 출력 각도
        0,                          // 기준선 각도
        FW_BOLD,                    // 글꼴 두께 (FW_BOLD 등)
        FALSE,                      // 이탤릭체 여부
        FALSE,                      // 밑줄 여부
        FALSE,                      // 취소선 여부
        DEFAULT_CHARSET,            // 문자셋
        OUT_DEFAULT_PRECIS,         // 출력 정확도
        CLIP_DEFAULT_PRECIS,        // 클리핑 정확도
        DEFAULT_QUALITY,            // 출력 품질
        DEFAULT_PITCH | FF_SWISS,   // 피치 및 패밀리
        _T("맑은 고딕")                  // 글꼴 이름
    );

    
    CWnd* pStatic = GetDlgItem(IDC_ADMIN_TITLE);
    if (pStatic != nullptr)
    {
        pStatic->SetFont(&m_fontForAdmin1);
    }

    m_listTransaction.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

    m_listTransaction.InsertColumn(0, _T("상품 번호"), LVCFMT_CENTER, 100);
    m_listTransaction.InsertColumn(1, _T("게시글명"), LVCFMT_CENTER, 200);
    m_listTransaction.InsertColumn(2, _T("판매자 ID(이름)"), LVCFMT_CENTER, 150);
    m_listTransaction.InsertColumn(3, _T("구매자 ID(이름)"), LVCFMT_CENTER, 150);
    m_listTransaction.InsertColumn(4, _T("결제 일자"), LVCFMT_CENTER, 150);
    m_listTransaction.InsertColumn(5, _T("결제수단"), LVCFMT_CENTER, 100);
    m_listTransaction.InsertColumn(6, _T("가격"), LVCFMT_CENTER, 100);

    RefreshWaitList();

    return TRUE;  // return TRUE unless you set the focus to a control
}

void CAdminMainDlg::RefreshWaitList()
{
    m_listTransaction.DeleteAllItems();
    m_cbBrandListCtrl.ResetContent();

    SQLHDBC hDbc = theApp.m_dbManager.GetConnectionHandle();
    SQLHSTMT hStmt;
    SQLCHAR query[500] = { 0, };

    if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS)
    {
        sprintf_s((char*)query, 500,
            "SELECT BRAND_NAME FROM BRAND ORDER BY BRAND_NAME ASC"
            );

        if (SQLExecDirect(hStmt, query, SQL_NTS) == SQL_SUCCESS)
        {
            SQLCHAR outBrand[50] = { 0, };
            SQLLEN cbBrand;

            SQLBindCol(hStmt, 1, SQL_C_CHAR, outBrand, sizeof(outBrand), &cbBrand);

            while (SQLFetch(hStmt) == SQL_SUCCESS)
            {
                m_cbBrandListCtrl.AddString(CString((char*)outBrand));
            }
        }

        SQLCloseCursor(hStmt);

        if (m_cbBrandListCtrl.GetCount() > 0) {
            m_cbBrandListCtrl.SetCurSel(0);
        }
        
        sprintf_s((char*)query, 500,
            "SELECT I.ITEM_ID, I.POST_TITLE, "
            "US.USER_NICKNAME, "
            "UB.USER_NICKNAME, "
            "T.TRANSACTION_DATE, T.PAYMENT_METHOD, "
            "I.PRICE "
            "FROM ITEM I, TRANSACTIONS T, USER_INFO US, USER_INFO UB "
            "WHERE I.ITEM_ID = T.ITEM_ID "
            "AND T.SELLER_ID = US.USER_ID "
            "AND T.BUYER_ID = UB.USER_ID "
            "AND I.BUY_STATUS = '감정대기' "
            "ORDER BY T.TRANSACTION_DATE ASC"
        );

        if (SQLExecDirect(hStmt, query, SQL_NTS) == SQL_SUCCESS)
        {
            int nRow = 0;

            SQLCHAR outItemID[50] = { 0, };
            SQLCHAR outPostTitle[200] = { 0, };
            SQLCHAR outSeller[100] = { 0, };
            SQLCHAR outBuyer[100] = { 0, };
            SQLCHAR outPaymentMethod[50] = { 0, };
            SQLINTEGER outPrice = 0;
            SQL_TIMESTAMP_STRUCT outDate;
            SQLLEN cbItemID, cbPostTitle, cbSeller, cbBuyer, cbPaymentMethod, cbPrice, cbDate;

            SQLBindCol(hStmt, 1, SQL_C_CHAR, outItemID, sizeof(outItemID), &cbItemID);
            SQLBindCol(hStmt, 2, SQL_C_CHAR, outPostTitle, sizeof(outPostTitle), &cbPostTitle);
            SQLBindCol(hStmt, 3, SQL_C_CHAR, outSeller, sizeof(outSeller), &cbSeller);
            SQLBindCol(hStmt, 4, SQL_C_CHAR, outBuyer, sizeof(outBuyer), &cbBuyer);
            SQLBindCol(hStmt, 5, SQL_C_TYPE_TIMESTAMP, &outDate, sizeof(outDate), &cbDate);
            SQLBindCol(hStmt, 6, SQL_C_CHAR, outPaymentMethod, sizeof(outPaymentMethod), &cbPaymentMethod);
            SQLBindCol(hStmt, 7, SQL_C_LONG, &outPrice, 0, &cbPrice);

            while (SQLFetch(hStmt) == SQL_SUCCESS)
            {
                CString strItemID((char*)outItemID);
                CString strPostTitle((char*)outPostTitle);
                CString strSeller((char*)outSeller);
                CString strBuyer((char*)outBuyer);
                CString strPaymentMethod((char*)outPaymentMethod);

                CString strPrice;
                strPrice.Format(_T("%d 원"), outPrice);

                CString strDate;
                if (cbDate != SQL_NULL_DATA) {
                    strDate.Format(_T("%04d-%02d-%02d %02d:%02d"),
                        outDate.year, outDate.month, outDate.day, outDate.hour, outDate.minute);
                }
                else {
                    strDate = _T("-");
                }

                m_listTransaction.InsertItem(nRow, strItemID);
                m_listTransaction.SetItemText(nRow, 1, strPostTitle);
                m_listTransaction.SetItemText(nRow, 2, strSeller);
                m_listTransaction.SetItemText(nRow, 3, strBuyer);
                m_listTransaction.SetItemText(nRow, 4, strDate);
                m_listTransaction.SetItemText(nRow, 5, strPaymentMethod);
                m_listTransaction.SetItemText(nRow, 6, strPrice);

                nRow++;
            }
        }

        SQLCloseCursor(hStmt);
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    }
}

void CAdminMainDlg::OnBnClickedButton9()
{
    int nSelectedItem = m_listTransaction.GetNextItem(-1, LVNI_SELECTED);
    if (nSelectedItem == -1) {
        AfxMessageBox(_T("감정할 매물을 리스트에서 먼저 선택해 주세요."));
        return;
    }

    CAdminAppraisalDlg dlgAppraisal;
    dlgAppraisal.m_strTargetItemID = m_listTransaction.GetItemText(nSelectedItem, 0);

    if (dlgAppraisal.DoModal() == IDOK)
    {
        RefreshWaitList();
    }
}

// 브랜드 등록
void CAdminMainDlg::OnBnClickedButton1()
{
    UpdateData(TRUE);

    if (m_strBrandName.IsEmpty()) {
        AfxMessageBox(_T("브랜드 이름을 입력해 주세요."));
        return;
    }

    // 창립연도가 비어있으면 NULL 처리, 아니면 숫자 그대로 입력
    CString strYear = m_strFoundationYear.IsEmpty() ? _T("NULL") : m_strFoundationYear;

    SQLHSTMT hStmt;
    SQLHDBC hDbc = theApp.m_dbManager.GetConnectionHandle();

    if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS)
    {
        SQLCHAR query[500];
        sprintf_s((char*)query, 500,
            "INSERT INTO BRAND (BRAND_ID, BRAND_NAME, FOUNDATION_YEAR) "
            "SELECT (SELECT MAX(BRAND_ID) FROM BRAND) + 1, "
            "'%s', %s",
            m_strBrandName, strYear);

        if (SQLExecDirect(hStmt, query, SQL_NTS) == SQL_SUCCESS)
        {
            AfxMessageBox(_T("새로운 브랜드가 등록되었습니다."));

            // 입력창 초기화
            m_strBrandName = _T("");
            m_strFoundationYear = _T("");
            UpdateData(FALSE);


            m_cbBrandListCtrl.ResetContent();
            RefreshWaitList();
            
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

// 복각 모델 등록
void CAdminMainDlg::OnBnClickedButton10()
{
    UpdateData(TRUE);

    if (m_strModelName.IsEmpty() || m_strBrandListName.IsEmpty()) {
        AfxMessageBox(_T("모델명과 브랜드를 선택/입력해 주세요."));
        return;
    }

    // 선택 입력 사항 NULL 처리
    CString strSeasonVal = m_strSeason.IsEmpty() ? _T("NULL") : _T("'") + m_strSeason + _T("'");
    CString strDesignerVal = m_strDesigner.IsEmpty() ? _T("NULL") : _T("'") + m_strDesigner + _T("'");

    SQLHSTMT hStmt;
    SQLHDBC hDbc = theApp.m_dbManager.GetConnectionHandle();

    if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS)
    {
        SQLCHAR query[1000];
        sprintf_s((char*)query, 1000,
            "INSERT INTO ITEM "
            "(ITEM_ID, MODEL_NAMES, POST_TITLE, SEASON, ITEM_SIZE, USER_CONDITIONS, PRICE, BUY_STATUS, DESIGNER, BRAND_ID, OWN_USER) "
            "SELECT "
            "(SELECT MAX(ITEM_ID) FROM ITEM WHERE ITEM_ID < 1000) + 1, "
            "'%s', '[REF] %s', %s, "
            "'FREE', '새상품', 1, '숨김', %s, "
            "(SELECT BRAND_ID FROM BRAND WHERE BRAND_NAME = '%s'), 1 ",

            m_strModelName, m_strModelName, strSeasonVal, strDesignerVal, m_strBrandListName);

        if (SQLExecDirect(hStmt, query, SQL_NTS) == SQL_SUCCESS)
        {
            AfxMessageBox(_T("복각 모델 마스터 데이터가 성공적으로 등록되었습니다."));

            // 입력창 초기화
            m_strModelName = _T("");
            m_strSeason = _T("");
            m_strDesigner = _T("");
            m_cbBrandListCtrl.SetCurSel(0);
            UpdateData(FALSE);
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

BOOL CAdminMainDlg::PreTranslateMessage(MSG* pMsg)
{
    if(pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_ESCAPE) {
            return TRUE;
        }
        if (pMsg->wParam == VK_RETURN) {
            return TRUE;
        }
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}



void CAdminMainDlg::OnClose()
{
    EndDialog(IDRETRY);
}

void CAdminMainDlg::OnBnClickedBtnQuery()
{
    CCustomQueryDlg dlg;
    dlg.DoModal();
    RefreshWaitList();
}
