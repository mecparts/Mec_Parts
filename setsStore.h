/*
 * setsStores.h
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

#ifndef SETSSTORE_H
#define SETSSTORE_H

#include "mecparts.h"

using namespace std;

//Tree model columns:
class SetsStore : public Gtk::TreeModel::ColumnRecord
{
	public:

		Gtk::TreeModelColumn<string> m_setNumber;
		Gtk::TreeModelColumn<string> m_description;
		Gtk::TreeModelColumn<guint> m_started;
		Gtk::TreeModelColumn<guint> m_ended;
		Gtk::TreeModelColumn<gint64> m_rowId;
		
		SetsStore()
		{
			add(m_setNumber);
			add(m_description);
			add(m_started);
			add(m_ended);
			add(m_rowId);
		}
};
#endif
