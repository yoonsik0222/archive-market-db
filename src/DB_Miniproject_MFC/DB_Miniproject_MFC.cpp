
// DB_Miniproject_MFC.cpp: 애플리케이션에 대한 클래스 동작을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "DB_Miniproject_MFC.h"
#include "DB_Miniproject_MFCDlg.h"
#include "CLoginDlg.h"
#include "CAdminMainDlg.h"
#include "CUserMainDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDBMiniprojectMFCApp

BEGIN_MESSAGE_MAP(CDBMiniprojectMFCApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDBMiniprojectMFCApp 생성

CDBMiniprojectMFCApp::CDBMiniprojectMFCApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CDBMiniprojectMFCApp 개체입니다.

CDBMiniprojectMFCApp theApp;


// CDBMiniprojectMFCApp 초기화

BOOL CDBMiniprojectMFCApp::InitInstance()
{
	CWinApp::InitInstance();


	// 대화 상자에 셸 트리 뷰 또는
	// 셸 목록 뷰 컨트롤이 포함되어 있는 경우 셸 관리자를 만듭니다.
	CShellManager *pShellManager = new CShellManager;

	// MFC 컨트롤의 테마를 사용하기 위해 "Windows 원형" 비주얼 관리자 활성화
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 애플리케이션 마법사에서 생성된 애플리케이션"));

	if (!m_dbManager.DB_Connect()) {
		AfxMessageBox(_T("ODBC 데이터베이스 연결 실패!"));

		// 셸 관리자 메모리 해제 후 안전하게 종료 (자동 생성된 하단 코드 활용)
		if (pShellManager != nullptr) delete pShellManager;
		return FALSE;
	}
    else AfxMessageBox(_T("ODBC 데이터베이스 연결 성공"));
    while (1) {
        CLoginDlg loginDlg;
        INT_PTR nResponse = loginDlg.DoModal();

        if (nResponse == IDCANCEL) {
            break;
        }

        if (nResponse == IDOK)
        {
            INT_PTR nMainRes = 0;
            // 로그인 성공 시 세션 권한에 따라 분기
            if (loginDlg.m_nUserRole == 1) { // ADMIN
                CAdminMainDlg adminDlg;
                nMainRes = adminDlg.DoModal();
            }
            else { // 일반 USER
                CUserMainDlg userDlg;
                userDlg.m_nUserID = loginDlg.m_nUserID;
                userDlg.m_strLoginNickname = loginDlg.m_strLoginNickname;
                nMainRes = userDlg.DoModal();
            }
            

            if (nMainRes == IDCANCEL) {
                break;
            }
        }
        
    }
    

    // 위에서 만든 셸 관리자를 삭제합니다.
    if (pShellManager != nullptr)
    {
        delete pShellManager;
    }

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
    ControlBarCleanUp();
#endif

    // [주의] 프로그램이 완전히 끝날 때 m_dbManager 객체가 소멸하면서 
    // DB_Disconnect()가 자동으로 안전하게 호출되어 파이프라인이 닫힙니다.

    return FALSE;

}

