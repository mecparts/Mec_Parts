/*
 * partprice.h
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

#ifndef _PARTPRICE_H
#define _PARTPRICE_H

#include <string>
#include "mecparts.h"

using namespace std;

class PartPrice
{
	public:
		PartPrice(string num,double price);
		virtual ~PartPrice();
		string PartNum() { return m_partNum; }
		double Price() { return m_price; }
	private:
		string m_partNum;
		double m_price;
};
#endif
