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
#ifndef _CSVREADER_H
#define _CSVREADER_H

#include <string>
#include <vector>
#include <fstream>

class CsvReader
{
	public:
		CsvReader(std::string csvFilename);
		virtual ~CsvReader();
		bool Parse(std::vector<std::string> &fields);

	private:
		std::ifstream m_ifs;
		std::string m_csvFilename;

		void Parse(std::string line,std::vector<std::string> &fields);
		std::string Strip(std::string field);
};
#endif
