#ifndef ___SQL_H___
#define ___SQL_H___

#include <iostream>
#include <vector>
#include <sqlite3.h>

using std::cout;
using std::endl;
using std::string;
using std::vector;


namespace myprosql{

class SQL{
public:
    SQL():zErrMsg(0){
    	rc = sqlite3_open_v2("mybase", &db,  SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    	if( rc ){
    	  cout << "Can't open database: " << sqlite3_errmsg(db) << endl;
    	  sqlite3_close(db);
    	  // return 1;
    	}
    	std::vector<string> vRequest {R"(
    	            CREATE TABLE IF NOT EXISTS COMPANY(
    	               ID INTEGER PRIMARY KEY   AUTOINCREMENT,
    	               TIME             text,
    	               EVENTS           CHAR(50)
    	            );)", 
    	            R"( INSERT INTO COMPANY (TIME, EVENTS)
    	                VALUES (DATETIME('now','localtime'),'start server');)"};

    	for (auto i: vRequest) {
    	    rc = sqlite3_exec(db, i.c_str(), 0, 0, &zErrMsg);
    	    if( rc!=SQLITE_OK ){
    	      cout << "Can't open database: " << zErrMsg << endl;
    	      sqlite3_free(zErrMsg);
    	    }
    	}
    }
    ~SQL(){

    	sqlite3_close(db);
    }
    void writeEvent(char *requets, std::size_t size){
    	std::string reqSorces = R"( INSERT INTO COMPANY (TIME, EVENTS)
    	        VALUES (DATETIME('now','localtime'),'');)";

    	char oldChars[50]={0};


    	int newLen = 0;
    	for (unsigned int j = 0; j < size; j++) {
    	    char ch = requets[j];
    	    if ((ch >= 'a' && ch <= 'z')||(ch >= 'A' && ch <= 'Z')||(ch >= '0' && ch <= '9') || ch == ' ') {
    	        oldChars[newLen] = ch;
    	        newLen++;
    	    }
    	}


    	reqSorces.insert(86,oldChars,newLen);

    	cout << "sql: " << reqSorces << endl;

    	rc = sqlite3_exec(db, reqSorces.c_str(), 0, 0, &zErrMsg);
    	if( rc!=SQLITE_OK ){
    	  cout << "Can't open database: " << zErrMsg << endl;
    	  sqlite3_free(zErrMsg);
    	}	
    }
private:
	sqlite3 *db;
	int rc;
	char *zErrMsg = 0;
};

} /*namecpase myprosql*/

#endif /* ___SQL_H___ */