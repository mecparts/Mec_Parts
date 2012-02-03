/*
 * sql.cpp
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

using namespace std;

Sql::Sql(void *pWnd,string dbName)
{
	m_pWnd = pWnd;
	m_dbName = dbName;
}

Sql::~Sql()
{
}

void Sql::ExecSql(sqlite3* &db,const char *sql,int (*callback)(void *wnd,int argc,char **argv, char **azColName))
{
	char *zErrMsg = NULL;
	int rc;
	
	//cout << sql << endl;
	rc = sqlite3_open(m_dbName.c_str(), &db);
	if( rc ) {
		stringstream err;
		err << "Can't open database: " << sqlite3_errmsg(db);
		throw err.str();
	} 
	rc = sqlite3_exec(db, sql, callback, m_pWnd, &zErrMsg);
	if( rc!=SQLITE_OK ) {
		stringstream err;
		err << "SQL error: " << zErrMsg << ' ' << sql;
		cout << err.str() << endl;
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		throw err.str();
	}
}

void Sql::ExecQuery(const char *sql,int (*callback)(void *wnd,int argc,char **argv, char **azColName))
{
	sqlite3 *db;
	ExecSql(db,sql,callback);
	sqlite3_close(db);
}

void Sql::ExecNonQuery(const char *sql)
{
	sqlite3 *db;
	ExecSql(db,sql,NULL);
	sqlite3_close(db);
}

sqlite3_int64 Sql::ExecInsert(const char *sql)
{
	sqlite3 *db;
	ExecSql(db,sql,NULL);
	sqlite3_int64 rowid = sqlite3_last_insert_rowid(db);
	sqlite3_close(db);
	return rowid;
}

int Sql::ExecUpdate(const char *sql)
{
	sqlite3 *db;
	ExecSql(db,sql,NULL);
	int rowsUpdated = sqlite3_changes(db);
	sqlite3_close(db);
	return rowsUpdated;
}

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
