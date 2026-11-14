// CUserMainDlg.cpp: 구현 파일
//

#include "pch.h"
#include "DB_Miniproject_MFC.h"
#include "afxdialogex.h"
#include "CUserMainDlg.h"
#include "CUserSearchDlg.h"
#include "CUserAppraisalDlg.h"
#include "CUserInsertDlg.h"
#include "CUserPurchaseDlg.h"


// CUserMainDlg 대화 상자

IMPLEMENT_DYNAMIC(CUserMainDlg, CDialogEx)

CUserMainDlg::CUserMainDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_USER_MAIN_DLG, pParent)
{

}

CUserMainDlg::~CUserMainDlg()
{
}

void CUserMainDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_listItems);
}


BEGIN_MESSAGE_MAP(CUserMainDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON1, &CUserMainDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, &CUserMainDlg::OnBnClickedButton2)
    ON_BN_CLICKED(IDC_BUTTON4, &CUserMainDlg::OnBnClickedButton4)
    ON_BN_CLICKED(IDC_BUTTON5, &CUserMainDlg::OnBnClickedButton5)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, &CUserMainDlg::OnNMCustomdrawList1)
    ON_BN_CLICKED(IDC_BUTTON7, &CUserMainDlg::OnBnClickedButton7)
    ON_BN_CLICKED(IDC_BUTTON8, &CUserMainDlg::OnBnClickedButton8)
    ON_BN_CLICKED(IDC_BUTTON6, &CUserMainDlg::OnBnClickedButton6)
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// CUserMainDlg 메시지 처리기

BOOL CUserMainDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_fontForUserName.CreateFont(
        20,                         // 높이 (픽셀 단위, 음수로 쓰면 포인트 단위)
        0,                          // 너비 (0이면 글꼴 비율에 맞춤)
        0,                          // 출력 각도
        0,                          // 기준선 각도
        FW_NORMAL,                  // 글꼴 두께 (FW_BOLD 등)
        FALSE,                      // 이탤릭체 여부
        FALSE,                      // 밑줄 여부
        FALSE,                      // 취소선 여부
        DEFAULT_CHARSET,            // 문자셋
        OUT_DEFAULT_PRECIS,         // 출력 정확도
        CLIP_DEFAULT_PRECIS,        // 클리핑 정확도
        DEFAULT_QUALITY,            // 출력 품질
        DEFAULT_PITCH | FF_SWISS,   // 피치 및 패밀리
        _T("굴림")                  // 글꼴 이름
    );


    CWnd* pStatic = GetDlgItem(IDC_USER_NAME);
    if (pStatic != nullptr)
    {
        pStatic->SetFont(&m_fontForUserName);
    }

    // 환영 텍스트 변경
    CString strWelcomeMsg;
    strWelcomeMsg.Format(_T("%s님, 환영합니다."), m_strLoginNickname);
    SetDlgItemText(IDC_USER_NAME, strWelcomeMsg);

    // 리스트 리셋
    m_listItems.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

    m_listItems.InsertColumn(0, _T("게시글 제목"), LVCFMT_LEFT, 200);
    m_listItems.InsertColumn(1, _T("브랜드"), LVCFMT_CENTER, 100);
    m_listItems.InsertColumn(2, _T("모델명"), LVCFMT_LEFT, 150);
    m_listItems.InsertColumn(3, _T("시즌"), LVCFMT_LEFT, 80);
    m_listItems.InsertColumn(4, _T("가격(원)"), LVCFMT_RIGHT, 100);
    m_listItems.InsertColumn(5, _T("사이즈"), LVCFMT_CENTER, 80);
    m_listItems.InsertColumn(6, _T("상태"), LVCFMT_CENTER, 80);
    m_listItems.InsertColumn(7, _T("원판(복각) 모델"), LVCFMT_LEFT, 150);
    m_listItems.InsertColumn(8, _T("숨김ID"), LVCFMT_LEFT, 0);

    return TRUE;  // return TRUE unless you set the focus to a control
}

// 판매 완료된 물건 열 색 처리
void CUserMainDlg::OnNMCustomdrawList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMLVCUSTOMDRAW pLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
    *pResult = CDRF_DODEFAULT; // 기본적으로는 윈도우가 알아서 그리게 둠

    // 표를 그리기 직전 단계인가?
    if (pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
    {
        *pResult = CDRF_NOTIFYITEMDRAW; // 행마다 그릴 때 다시 보고
    }
    // 각각의 행을 그리는 단계인가?
    else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
    {
        int nRow = (int)pLVCD->nmcd.dwItemSpec; // 현재 그리고 있는 행 번호

        DWORD_PTR dwData = m_listItems.GetItemData(nRow);

        if (dwData == 1) // 판매완료
        {
            // clrText 에 RGB 할당시 텍스트 색깔 변경
            pLVCD->clrTextBk = RGB(240, 240, 240); // 배경을 연한 회색으로
        }
        else if (dwData == 2) // 감정 대기 상품
        {
            pLVCD->clrTextBk = RGB(233, 236, 105); // 배경을 연한 노랑으로
        }
        else // 판매 중인 상품이라면?
        {
            pLVCD->clrText = RGB(0, 0, 0); // 기본 검은색
        }

        *pResult = CDRF_NEWFONT; // 폰트/색상 변경을 적용.
    }
}

// '최신 매물 조회' 버튼
void CUserMainDlg::OnBnClickedButton1()
{
    m_listItems.DeleteAllItems();

    SQLHSTMT hStmt;
    SQLHDBC hDbc = theApp.m_dbManager.GetConnectionHandle();

    if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS)
    {
        SQLCHAR query[1000];
        sprintf_s((char*)query, 1000,
            "SELECT I.POST_TITLE, B.BRAND_NAME, I.MODEL_NAMES, I.SEASON, I.PRICE, I.ITEM_SIZE, I.USER_CONDITIONS, "
            // 서브 쿼리로 복각 모델 이름만 가져오기.
            "(SELECT I2.MODEL_NAMES FROM ITEM I2 WHERE I2.ITEM_ID = I.ORIGINAL_MODEL), I.ITEM_ID "
            "FROM ITEM I, BRAND B "
            "WHERE I.BRAND_ID = B.BRAND_ID "
            "AND I.BUY_STATUS <> '판매완료' AND I.BUY_STATUS <> '숨김' AND I.BUY_STATUS <> '감정대기' "
            "ORDER BY I.ITEM_ID DESC");

        if (SQLExecDirect(hStmt, query, SQL_NTS) == SQL_SUCCESS)
        {
            int nRow = 0;

            SQLCHAR outPostTitle[200] = { 0, };
            SQLCHAR outBrandName[50] = { 0, };
            SQLCHAR outModelNames[100] = { 0, };
            SQLCHAR outSeason[50] = { 0, };
            SQLINTEGER outPrice = 0;
            SQLCHAR outItemSize[50] = { 0, };
            SQLCHAR outCondition[50] = { 0, };
            SQLCHAR outOriginalModel[100] = { 0, };
            SQLCHAR outItemID[50] = { 0, };

            SQLLEN cbPostTitle, cbBrandName, cbModelNames, cbSeason, cbPrice, cbItemSize, cbCondition, cbOriginalModel, cbItemID;

            // 데이터 매핑
            SQLBindCol(hStmt, 1, SQL_C_CHAR, outPostTitle, sizeof(outPostTitle), &cbPostTitle);
            SQLBindCol(hStmt, 2, SQL_C_CHAR, outBrandName, sizeof(outBrandName), &cbBrandName);
            SQLBindCol(hStmt, 3, SQL_C_CHAR, outModelNames, sizeof(outModelNames), &cbModelNames);
            SQLBindCol(hStmt, 4, SQL_C_CHAR, outSeason, sizeof(outSeason), &cbSeason);
            SQLBindCol(hStmt, 5, SQL_C_LONG, &outPrice, 0, &cbPrice);
            SQLBindCol(hStmt, 6, SQL_C_CHAR, outItemSize, sizeof(outItemSize), &cbItemSize);
            SQLBindCol(hStmt, 7, SQL_C_CHAR, outCondition, sizeof(outCondition), &cbCondition);
            SQLBindCol(hStmt, 8, SQL_C_CHAR, outOriginalModel, sizeof(outOriginalModel), &cbOriginalModel);
            SQLBindCol(hStmt, 9, SQL_C_CHAR, outItemID, sizeof(outItemID), &cbItemID);

            while (SQLFetch(hStmt) == SQL_SUCCESS)
            {
                // 포맷팅
                CString strPostTitle((char*)outPostTitle);
                CString strBrandName((char*)outBrandName);
                CString strModelNames((char*)outModelNames);
                CString strItemSize((char*)outItemSize);
                CString strCondition((char*)outCondition);
                CString strItemID((char*)outItemID);
                CString strPrice;
                strPrice.Format(_T("%d"), outPrice);

                // NULL 예외 처리
                CString strOriginalModel;
                if (cbOriginalModel == SQL_NULL_DATA) {
                    strOriginalModel = _T("-");
                }
                else {
                    strOriginalModel = CString((char*)outOriginalModel);
                }

                CString strSeason;
                if (cbSeason == SQL_NULL_DATA) {
                    strSeason = _T("-");
                }
                else {
                    strSeason = CString((char*)outSeason);
                }

                m_listItems.InsertItem(nRow, strPostTitle);
                m_listItems.SetItemText(nRow, 1, strBrandName);
                m_listItems.SetItemText(nRow, 2, strModelNames);
                m_listItems.SetItemText(nRow, 3, strSeason);
                m_listItems.SetItemText(nRow, 4, strPrice);
                m_listItems.SetItemText(nRow, 5, strItemSize);
                m_listItems.SetItemText(nRow, 6, strCondition);
                m_listItems.SetItemText(nRow, 7, strOriginalModel);
                m_listItems.SetItemText(nRow, 8, strItemID);

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
    else {
        AfxMessageBox(_T("Allocate Handle Fail"));
    }
}

// '상품 검색' 기능
void CUserMainDlg::OnBnClickedButton2()
{
    CUserSearchDlg dlgSearch;

    if (dlgSearch.DoModal() == IDOK)
    {
        // 리스트 컨트롤 초기화
        m_listItems.DeleteAllItems();

        SQLHSTMT hStmt;
        SQLHDBC hDbc = theApp.m_dbManager.GetConnectionHandle();

        if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS)
        {
            CString strQuery = _T("SELECT I.POST_TITLE, B.BRAND_NAME, I.MODEL_NAMES, I.SEASON, I.PRICE, ")
                _T("I.ITEM_SIZE, I.USER_CONDITIONS, ")
                _T("(SELECT I2.MODEL_NAMES FROM ITEM I2 WHERE I2.ITEM_ID = I.ORIGINAL_MODEL), I.ITEM_ID ")
                _T("FROM ITEM I, BRAND B ")
                _T("WHERE I.BRAND_ID = B.BRAND_ID ")
                _T("  AND I.BUY_STATUS <> '판매완료' AND I.BUY_STATUS <> '숨김' AND I.BUY_STATUS <> '감정대기' ");

            // User가 입력한 필터 값에 따라 append. 따라서 기존 고정 query가 아닌 CString에 값을 추가하여 변환
           
            // 키워드 통합 검색 (제목 또는 모델명)
            if (!dlgSearch.m_strKeyword.IsEmpty())
            {
                CString strFilter;
                strFilter.Format(_T(" AND (I.POST_TITLE LIKE '%%%s%%' OR I.MODEL_NAMES LIKE '%%%s%%') "),
                    dlgSearch.m_strKeyword, dlgSearch.m_strKeyword);
                strQuery += strFilter;
            }

            // 브랜드 필터
            if (!dlgSearch.m_strBrand.IsEmpty() && dlgSearch.m_strBrand != _T("전체"))
            {
                CString strFilter;
                strFilter.Format(_T(" AND B.BRAND_NAME = '%s' "), dlgSearch.m_strBrand);
                strQuery += strFilter;
            }

            // 상태 필터 
            if (!dlgSearch.m_strCondition.IsEmpty() && dlgSearch.m_strCondition != _T("전체"))
            {
                CString strFilter;
                strFilter.Format(_T(" AND I.USER_CONDITIONS = '%s' "), dlgSearch.m_strCondition);
                strQuery += strFilter;
            }

            // 가격 범위 필터 (최소/최대값이 0보다 클 때만)
            if (dlgSearch.m_nMinPrice > 0 || dlgSearch.m_nMaxPrice > 0)
            {
                // 최대값이 입력되지 않았으면 무한대(충분히 큰 수)로 간주
                int nMax = (dlgSearch.m_nMaxPrice > 0) ? dlgSearch.m_nMaxPrice : 999999999;
                CString strFilter;
                strFilter.Format(_T(" AND I.PRICE BETWEEN %d AND %d "), dlgSearch.m_nMinPrice, nMax);
                strQuery += strFilter;
            }

            // 복각/빈티지 전용 필터
            if (dlgSearch.m_bIsVintageOnly)
            {
                strQuery += _T(" AND I.ORIGINAL_MODEL IS NOT NULL ");
            }

            // 마지막으로 정렬 순서 붙이기
            strQuery += _T(" ORDER BY I.ITEM_ID DESC");

            char query[2000] = { 0, };
            strcpy_s(query, sizeof(query), CT2A((LPCTSTR)strQuery));

            if (SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS) == SQL_SUCCESS)
            {
               
                int nRow = 0;

                SQLCHAR outPostTitle[200] = { 0, };
                SQLCHAR outBrandName[50] = { 0, };
                SQLCHAR outModelNames[100] = { 0, };
                SQLCHAR outSeason[50] = { 0, };
                SQLINTEGER outPrice = 0;
                SQLCHAR outItemSize[50] = { 0, };
                SQLCHAR outCondition[50] = { 0, };
                SQLCHAR outOriginalModel[100] = { 0, };
                SQLCHAR outItemID[50] = { 0, };

                SQLLEN cbPostTitle, cbBrandName, cbModelNames, cbSeason, cbPrice, cbItemSize, cbCondition, cbOriginalModel, cbItemID;

                // 데이터 매핑
                SQLBindCol(hStmt, 1, SQL_C_CHAR, outPostTitle, sizeof(outPostTitle), &cbPostTitle);
                SQLBindCol(hStmt, 2, SQL_C_CHAR, outBrandName, sizeof(outBrandName), &cbBrandName);
                SQLBindCol(hStmt, 3, SQL_C_CHAR, outModelNames, sizeof(outModelNames), &cbModelNames);
                SQLBindCol(hStmt, 4, SQL_C_CHAR, outSeason, sizeof(outSeason), &cbSeason);
                SQLBindCol(hStmt, 5, SQL_C_LONG, &outPrice, 0, &cbPrice);
                SQLBindCol(hStmt, 6, SQL_C_CHAR, outItemSize, sizeof(outItemSize), &cbItemSize);
                SQLBindCol(hStmt, 7, SQL_C_CHAR, outCondition, sizeof(outCondition), &cbCondition);
                SQLBindCol(hStmt, 8, SQL_C_CHAR, outOriginalModel, sizeof(outOriginalModel), &cbOriginalModel);
                SQLBindCol(hStmt, 9, SQL_C_CHAR, outItemID, sizeof(outItemID), &cbItemID);

                while (SQLFetch(hStmt) == SQL_SUCCESS)
                {
                    // 포맷팅
                    CString strPostTitle((char*)outPostTitle);
                    CString strBrandName((char*)outBrandName);
                    CString strModelNames((char*)outModelNames);
                    CString strItemSize((char*)outItemSize);
                    CString strCondition((char*)outCondition);
                    CString strItemID((char*)outItemID);
                    CString strPrice;
                    strPrice.Format(_T("%d"), outPrice);

                    // NULL 예외처리
                    CString strOriginalModel;
                    if (cbOriginalModel == SQL_NULL_DATA) {
                        strOriginalModel = _T("-"); // 비어있으면 하이픈(-) 출력
                    }
                    else {
                        strOriginalModel = CString((char*)outOriginalModel);
                    }
                    CString strSeason;
                    if (cbSeason == SQL_NULL_DATA) {
                        strSeason = _T("-");
                    }
                    else {
                        strSeason = CString((char*)outSeason);
                    }

                    m_listItems.InsertItem(nRow, strPostTitle);
                    m_listItems.SetItemText(nRow, 1, strBrandName);
                    m_listItems.SetItemText(nRow, 2, strModelNames);
                    m_listItems.SetItemText(nRow, 3, strSeason);
                    m_listItems.SetItemText(nRow, 4, strPrice);
                    m_listItems.SetItemText(nRow, 5, strItemSize);
                    m_listItems.SetItemText(nRow, 6, strCondition);
                    m_listItems.SetItemText(nRow, 7, strOriginalModel);
                    m_listItems.SetItemText(nRow, 8, strItemID);

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
    }
}

// '내 상품 조회'
void CUserMainDlg::OnBnClickedButton4()
{
    m_listItems.DeleteAllItems();

    SQLHSTMT hStmt;
    SQLHDBC hDbc = theApp.m_dbManager.GetConnectionHandle();

    if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS)
    {
        SQLCHAR query[1000];
        sprintf_s((char*)query, 1000,
            "SELECT I.POST_TITLE, B.BRAND_NAME, I.MODEL_NAMES, I.SEASON, I.PRICE, I.ITEM_SIZE, I.USER_CONDITIONS, "
            // 서브 쿼리로 복각 모델 이름만 가져오기.
            "(SELECT I2.MODEL_NAMES FROM ITEM I2 WHERE I2.ITEM_ID = I.ORIGINAL_MODEL), I.BUY_STATUS, I.ITEM_ID "
            "FROM ITEM I, BRAND B "
            "WHERE I.BRAND_ID = B.BRAND_ID "
            "AND I.OWN_USER = %d "
            "ORDER BY I.ITEM_ID DESC", m_nUserID);

        if (SQLExecDirect(hStmt, query, SQL_NTS) == SQL_SUCCESS)
        {
            int nRow = 0;

            SQLCHAR outPostTitle[200] = { 0, };
            SQLCHAR outBrandName[50] = { 0, };
            SQLCHAR outModelNames[100] = { 0, };
            SQLCHAR outSeason[50] = { 0, };
            SQLINTEGER outPrice = 0;
            SQLCHAR outItemSize[50] = { 0, };
            SQLCHAR outCondition[50] = { 0, };
            SQLCHAR outOriginalModel[100] = { 0, };
            SQLCHAR outBuyStatus[50] = { 0, };
            SQLCHAR outItemID[50] = { 0, };

            SQLLEN cbPostTitle, cbBrandName, cbModelNames, cbSeason, cbPrice, cbItemSize, cbCondition, cbOriginalModel, cbBuyStatus, cbItemID;

            // 데이터 매핑
            SQLBindCol(hStmt, 1, SQL_C_CHAR, outPostTitle, sizeof(outPostTitle), &cbPostTitle);
            SQLBindCol(hStmt, 2, SQL_C_CHAR, outBrandName, sizeof(outBrandName), &cbBrandName);
            SQLBindCol(hStmt, 3, SQL_C_CHAR, outModelNames, sizeof(outModelNames), &cbModelNames);
            SQLBindCol(hStmt, 4, SQL_C_CHAR, outSeason, sizeof(outSeason), &cbSeason);
            SQLBindCol(hStmt, 5, SQL_C_LONG, &outPrice, 0, &cbPrice);
            SQLBindCol(hStmt, 6, SQL_C_CHAR, outItemSize, sizeof(outItemSize), &cbItemSize);
            SQLBindCol(hStmt, 7, SQL_C_CHAR, outCondition, sizeof(outCondition), &cbCondition);
            SQLBindCol(hStmt, 8, SQL_C_CHAR, outOriginalModel, sizeof(outOriginalModel), &cbOriginalModel);
            SQLBindCol(hStmt, 9, SQL_C_CHAR, outBuyStatus, sizeof(outBuyStatus), &cbBuyStatus);
            SQLBindCol(hStmt, 10, SQL_C_CHAR, outItemID, sizeof(outItemID), &cbItemID);

            while (SQLFetch(hStmt) == SQL_SUCCESS)
            {
                // 포맷팅
                CString strPostTitle((char*)outPostTitle);
                CString strBrandName((char*)outBrandName);
                CString strModelNames((char*)outModelNames);
                CString strItemSize((char*)outItemSize);
                CString strCondition((char*)outCondition);
                CString strPrice;
                strPrice.Format(_T("%d"), outPrice);
                CString strItemID((char*)outItemID);
                

                // NULL 예외 처리
                CString strOriginalModel;
                if (cbOriginalModel == SQL_NULL_DATA) {
                    strOriginalModel = _T("-"); // 비어있으면 하이픈(-) 출력
                }
                else {
                    strOriginalModel = CString((char*)outOriginalModel);
                }
                CString strSeason;
                if (cbSeason == SQL_NULL_DATA) {
                    strSeason = _T("-");
                }
                else {
                    strSeason = CString((char*)outSeason);
                }
                

                int nIndex = m_listItems.InsertItem(nRow, strPostTitle);
                m_listItems.SetItemText(nRow, 1, strBrandName);
                m_listItems.SetItemText(nRow, 2, strModelNames);
                m_listItems.SetItemText(nRow, 3, strSeason);
                m_listItems.SetItemText(nRow, 4, strPrice);
                m_listItems.SetItemText(nRow, 5, strItemSize);
                m_listItems.SetItemText(nRow, 6, strCondition);
                m_listItems.SetItemText(nRow, 7, strOriginalModel);
                m_listItems.SetItemText(nRow, 8, strItemID);

                CString strStatus((char*)outBuyStatus);
                if (strStatus == _T("판매완료")) {
                    m_listItems.SetItemData(nIndex, 1); // 1 = 판매완료
                }
                else if (strStatus == _T("감정대기")) {
                    m_listItems.SetItemData(nIndex, 2); // 2 = 감정대기
                }
                else {
                    m_listItems.SetItemData(nIndex, 0); // 0 = 판매중
                }

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
    else {
        AfxMessageBox(_T("Allocate Handle Fail"));
    }
}

void CUserMainDlg::OnBnClickedButton5()
{
    CUserAppraisalDlg dlgAppraisal;
    dlgAppraisal.m_nUserID = m_nUserID;
    INT_PTR nResponse = dlgAppraisal.DoModal();
}


// '상품 삭제'
void CUserMainDlg::OnBnClickedButton7()
{
    // 표에서 현재 클릭(선택)된 항목의 인덱스 리턴
    int nSelectedItem = m_listItems.GetNextItem(-1, LVNI_SELECTED);

    if (nSelectedItem == -1) {
        AfxMessageBox(_T("삭제할 상품을 먼저 리스트에서 선택해 주세요."));
        return;
    }

    DWORD_PTR dwBuyStatus = m_listItems.GetItemData(nSelectedItem);

    if (dwBuyStatus == 1) // 1 = 판매완료
    {
        AfxMessageBox(_T("이미 판매 완료된 상품은 삭제할 수 없습니다."));
        return;
    }
    else if (dwBuyStatus == 2) // 2 = 감정대기
    {
        AfxMessageBox(_T("감정 대기 중인 상품은 삭제할 수 없습니다."));
        return;
    }

    CString strTargetID = m_listItems.GetItemText(nSelectedItem, 8);
    
    if (AfxMessageBox(_T("정말 이 상품을 삭제하시겠습니까?\n(데이터베이스에서 영구적으로 삭제됩니다.)"), MB_YESNO | MB_ICONQUESTION) == IDYES)
    {
        SQLHSTMT hStmt;
        SQLHDBC hDbc = theApp.m_dbManager.GetConnectionHandle();

        if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS)
        {
            SQLCHAR query[500];
            sprintf_s((char*)query, 500,
                "DELETE FROM ITEM "
                "WHERE ITEM_ID = %s AND OWN_USER = %d",
                strTargetID, m_nUserID);

            if (SQLExecDirect(hStmt, query, SQL_NTS) == SQL_SUCCESS)
            {
                AfxMessageBox(_T("상품이 성공적으로 삭제되었습니다."));

                OnBnClickedButton4();
            }
            else
            {
                AfxMessageBox(_T("내 상품이 아닌 상품은 삭제할 수 없습니다."));
            }

            SQLCloseCursor(hStmt);
            SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        }
        else
        {
            AfxMessageBox(_T("Allocate Handle Fail"));
        }
    }
}

// 상품 등록
void CUserMainDlg::OnBnClickedButton8()
{
    CUserInsertDlg dlgInsert;
    dlgInsert.m_nUserID = m_nUserID;
    INT_PTR nResponse = dlgInsert.DoModal();
}

// 상품 구매
void CUserMainDlg::OnBnClickedButton6()
{
    // 선택한 매물 확인
    int nSelectedItem = m_listItems.GetNextItem(-1, LVNI_SELECTED);
    if (nSelectedItem == -1) {
        AfxMessageBox(_T("구매하실 상품을 리스트에서 먼저 선택해 주세요."));
        return;
    }

    // 상태(판매완료 여부) 검사
    if (m_listItems.GetItemData(nSelectedItem) == 1) {
        AfxMessageBox(_T("이미 판매가 완료된 상품입니다."));
        return;
    }
    else if (m_listItems.GetItemData(nSelectedItem) == 2) // 2 = 감정 대기
    {
        AfxMessageBox(_T("감정 대기 중인 상품은 구매할 수 없습니다."));
        return;
    }

    CUserPurchaseDlg dlgPurchase;
    dlgPurchase.m_strTargetItemID = m_listItems.GetItemText(nSelectedItem, 8);
    dlgPurchase.m_strItemTitle = m_listItems.GetItemText(nSelectedItem, 0);
    dlgPurchase.m_strItemPrice = m_listItems.GetItemText(nSelectedItem, 4); 
    dlgPurchase.m_nUserID = m_nUserID;

    if (dlgPurchase.DoModal() == IDOK)
    {
        OnBnClickedButton1();
    }
}

BOOL CUserMainDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN)
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

void CUserMainDlg::OnClose()
{
    EndDialog(IDRETRY);
}
