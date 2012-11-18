/*
 * csvReader.cpp
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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "csvReader.h"

using namespace std;

CsvReader::CsvReader(string csvFilename)
{
	m_csvFilename = csvFilename;
	m_ifs.open(m_csvFilename.c_str(),ifstream::in);
}

CsvReader::~CsvReader()
{
	if( m_ifs.is_open() ) {
		m_ifs.close();
	}
}

string CsvReader::Strip(string field)
{
	string output=field;
	char quote=0;

	if( output.length()>1 ) {
		if( output[0]=='"' && output[output.length()-1]=='"' ) {
			quote = '"';
		} else if( output[0]=='\'' && output[output.length()-1]=='\'' ) {
			quote = '\'';
		}
		if( quote!=0 ) {
			output = output.substr(1,output.length()-2);
		}
	}
	if( quote!=0 ) {
		string quotes = string(2,quote);
		size_t index = output.find(quotes);
		while( index != string::npos ) {
			output.replace(index,2,1,quote);
			index = output.find(quotes,index+1);
		}
		index = output.find('\\');
		while( index != string::npos ) {
			output.erase(index,1);
			index = output.find('\\',index+1);
		}
	} else {
		size_t index = output.find('\\');
		while( index != string::npos ) {
			output.erase(index,1);
			index = output.find('\\',index+1);
		}
	}
	return output;
}

void CsvReader::Parse(string line,vector<string> &fields)
{
	int begin;
	int current;
	char quote=0;
	int length = line.length();
	current = 0;
	begin = current;
	fields.clear();
	while( current < length ) {
		if( quote && line[current]==quote && line[current-1]!='\\' && current!=begin ) {
			quote = 0;
		} else if( !quote && (line[current]=='"' || line[current]=='\'') ) {
			quote = line[current];
		}
		if( !quote && (line[current]==',' || current==length-1) ) {
			if( line[current]==',' ) {
				fields.push_back(Strip(line.substr(begin,current-begin)));
			} else {
				fields.push_back(Strip(line.substr(begin,current-begin+1)));
			}
			begin = ++current;
			quote = 0;
			continue;
		}
		++current;
	}
}

bool CsvReader::Parse(vector<string> &fields)
{
	string line;
	if( m_ifs.is_open() && m_ifs.good() ) {
		getline(m_ifs,line);
		size_t eol = line.find_last_of('\n');
		if( eol != string::npos ) {
			line.erase(eol);
		}
		Parse(line,fields);
		return true;
	} else {
		m_ifs.close();
		return false;
	}
}
