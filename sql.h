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

class Sql
{
	public:
		Sql(void *pWnd,std::string _dbName);
		virtual ~Sql();
		
		void ExecQuery(std::stringstream *sql,int (*callback)(void *wnd,int argc,char **argv, char **azColName)) { return ExecQuery(sql->str().c_str(),callback); }
		void ExecQuery(std::string sql,int (*callback)(void *wnd,int argc,char **argv, char **azColName)) { return ExecQuery(sql.c_str(),callback); }
		void ExecQuery(const char *sql,int (*callback)(void *wnd,int argc,char **argv, char **azColName));
		
		void ExecNonQuery(std::stringstream *sql) { return ExecNonQuery(sql->str().c_str()); }
		void ExecNonQuery(std::string sql) { return ExecNonQuery(sql.c_str()); }
		void ExecNonQuery(const char *sql);

		sqlite3_int64 ExecInsert(std::stringstream *sql) { return ExecInsert(sql->str().c_str()); }
		sqlite3_int64 ExecInsert(std::string sql) { return ExecInsert(sql.c_str()); }
		sqlite3_int64 ExecInsert(const char *sql);

		int ExecUpdate(std::stringstream *sql) { return ExecUpdate(sql->str().c_str()); }
		int ExecUpdate(std::string sql) { return ExecUpdate(sql.c_str()); }
		int ExecUpdate(const char *sql);

		std::string Escape(std::string value);
	private:
		void ExecSql(sqlite3* &db,const char *sql,int (*callback)(void *wnd,int argc,char **argv, char **azColName));
		void *m_pWnd;
		std::string m_dbName;
};
#endif
