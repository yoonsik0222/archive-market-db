#pragma once
#include <sql.h>
#include <sqlext.h>

class DBManager
{
private:
    SQLHENV hEnv;    // Environment Handle
    SQLHDBC hDbc;    // Connection Handle

public:
    DBManager();
    ~DBManager();

    bool DB_Connect();      // DB 연결 함수
    void DB_Disconnect();   // DB 해제 함수

    // 다른 다이얼로그 창에서 쿼리를 날릴 때(SQLAllocHandle) 이 연결 핸들이 필요하므로 겟터 제공
    SQLHDBC GetConnectionHandle() { return hDbc; }
};