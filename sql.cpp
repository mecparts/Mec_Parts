/*
 * sql.cpp: an interface class to the sqlite3 library
 * 
 * Copyright 2012 Wayne Hortensius <whortens@shaw.ca>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "sql.h"
#include <sstream>
#include <iomanip>
#include <time.h>

using namespace std;

// constructor: accepts the name of the SQLite database and a generic pointer.
// I tend to put a pointer to the class that's going to be making the SQL
// calls. That way, the callback (which has to be a static function) will have
// access to the non-static member data if it so choses.
Sql::Sql(void *pWnd,string dbName)
{
	m_pWnd = pWnd;
	m_dbName = dbName;
}

// destructor
Sql::~Sql()
{
}

// ExecSql: execute a SQL statement. This is an internal private routine and
// should not be made public and called from user code. Why? Read the comments
// at the end of the routine!
//
// db: a reference to an sqlite3 pointer. The caller will use this to close
//     the database when it's through with it.
// sql: a pointer to the SQL string to be executed
// callback: a pointer to the user's callback function
void Sql::ExecSql(sqlite3* &db,const char *sql,int (*callback)(void *wnd,int argc,char **argv, char **azColName))
{
	char *zErrMsg = NULL;
	int rc;

#ifdef DEBUG  
	time_t rawtime;
	struct tm *ti;
	time(&rawtime);
	ti = localtime(&rawtime);
	
	cout << setw(2) << ti->tm_hour << ':' << setfill('0') << setw(2) << ti->tm_min << ':' << setw(2) << ti->tm_sec << "  " << sql << endl;
#endif
	// open the SQL database
	rc = sqlite3_open(m_dbName.c_str(), &db);
	if( rc ) {
		// well, THAT didn't work!
		stringstream err;
		err << "Can't open database: " << sqlite3_errmsg(db);
		throw err.str();
	}
	// execute the SQL statement with the (optional) callback function receiving
	// any data to be returned.
	rc = sqlite3_exec(db, sql, callback, m_pWnd, &zErrMsg);
	if( rc!=SQLITE_OK ) {
		// problem!
		stringstream err;
		err << "SQL error: " << zErrMsg << endl << "Command was: " << sql;
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		throw err.str();
	}
	// Noticed that I didn't close the SQL database eh?
	// That's because the caller may want to make another sqlite3 call before
	// closing the database itself.
}

// ExecQuery: execute a SQL SELECT statement and return the rows, one at a time,
// to the user's callback function. A pointer 
void Sql::ExecQuery(const char *sql,int (*callback)(void *wnd,int argc,char **argv, char **azColName))
{
	sqlite3 *db;
	ExecSql(db,sql,callback);
	sqlite3_close(db);
}

// ExecNonQuery: execute a general SQL statement that does not return any
// data. This could be a DELETE, DROP, INSERT, UPDATE, etc.
void Sql::ExecNonQuery(const char *sql)
{
	sqlite3 *db;
	ExecSql(db,sql,NULL);
	sqlite3_close(db);
}

// ExecInsert: execute a SQL INSERT statement. Returns the new row id.
sqlite3_int64 Sql::ExecInsert(const char *sql)
{
	sqlite3 *db;
	ExecSql(db,sql,NULL);
	sqlite3_int64 rowid = sqlite3_last_insert_rowid(db);
	sqlite3_close(db);
	return rowid;
}

// ExecUpdate: execute a SQL UPDATE statement. Returns the number of rows
// actually updated.
int Sql::ExecUpdate(const char *sql)
{
	sqlite3 *db;
	ExecSql(db,sql,NULL);
	int rowsUpdated = sqlite3_changes(db);
	sqlite3_close(db);
	return rowsUpdated;
}

// Escape: double up any single quotes found.
string Sql::Escape(string value)
{
	string _value;
	_value = value;
	size_t index = _value.find("'");
	while( index != string::npos ) {
		_value.replace(index,1,"''");
		index = _value.find("'",index+2);
	}
	return _value;
}
