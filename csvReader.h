/*
 * csvReader.h
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

#include <string>
#include <vector>
#include <fstream>

#ifndef _CSVREADER_H
#define _CSVREADER_H

using namespace std;

class CsvReader
{
	public:
		CsvReader(string csvFilename);
		virtual ~CsvReader();
		bool Parse(vector<string> &fields);

	private:
		ifstream m_ifs;
		string m_csvFilename;

		void Parse(string line,vector<string> &fields);
		string Strip(string field);
};
#endif