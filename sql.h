/*
 * sql.h
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

#ifndef _MECPARTS_SQL_H
#define _MECPARTS_SQL_H

#include <sqlite3.h>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

class Sql
{
	public:
		Sql(void *pWnd,string _dbName);
		virtual ~Sql();
		
		void ExecQuery(stringstream *sql,int (*callback)(void *wnd,int argc,char **argv, char **azColName)) { return ExecQuery(sql->str().c_str(),callback); }
		void ExecQuery(string sql,int (*callback)(void *wnd,int argc,char **argv, char **azColName)) { return ExecQuery(sql.c_str(),callback); }
		void ExecQuery(const char *sql,int (*callback)(void *wnd,int argc,char **argv, char **azColName));
		
		void ExecNonQuery(stringstream *sql) { return ExecNonQuery(sql->str().c_str()); }
		void ExecNonQuery(string sql) { return ExecNonQuery(sql.c_str()); }
		void ExecNonQuery(const char *sql);

		sqlite3_int64 ExecInsert(stringstream *sql) { return ExecInsert(sql->str().c_str()); }
		sqlite3_int64 ExecInsert(string sql) { return ExecInsert(sql.c_str()); }
		sqlite3_int64 ExecInsert(const char *sql);

		int ExecUpdate(stringstream *sql) { return ExecUpdate(sql->str().c_str()); }
		int ExecUpdate(string sql) { return ExecUpdate(sql.c_str()); }
		int ExecUpdate(const char *sql);

		string Escape(string value);
	private:
		void ExecSql(sqlite3* &db,const char *sql,int (*callback)(void *wnd,int argc,char **argv, char **azColName));
		void *m_pWnd;
		string m_dbName;
};
#endif
