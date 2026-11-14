#include "pch.h"
#include "DBManager.h"

DBManager::DBManager() {
    hEnv = NULL;
    hDbc = NULL;
}

DBManager::~DBManager() {
    DB_Disconnect(); // 객체가 소멸할 때 안전하게 연결 해제 유도
}

bool DBManager::DB_Connect() {
    SQLRETURN Ret;

    if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) != SQL_SUCCESS) {
        return false;
    }
    if (SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER) != SQL_SUCCESS) {
        return false;
    }
    if (SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc) != SQL_SUCCESS) {
        return false;
    }

    // 계정 매핑
    const char* dataSourceName = "DB01_Server";
    const char* serverId = "sa";
    const char* serverPw = "1234";

    Ret = SQLConnect(hDbc, (SQLCHAR*)dataSourceName, SQL_NTS, (SQLCHAR*)serverId, SQL_NTS, (SQLCHAR*)serverPw, SQL_NTS);

    if ((Ret != SQL_SUCCESS) && (Ret != SQL_SUCCESS_WITH_INFO)) {
        return false;
    }

    return true;
}

void DBManager::DB_Disconnect() {
    if (hDbc) {
        SQLDisconnect(hDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        hDbc = NULL;
    }
    if (hEnv) {
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        hEnv = NULL;
    }
}