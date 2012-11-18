/*
 * collectionStore.h
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

#ifndef COLLECTIONSTORE_H
#define COLLECTIONSTORE_H

#include "mecparts.h"

using namespace std;

//Tree model columns:
class CollectionStore : public Gtk::TreeModel::ColumnRecord
{
	public:

		Gtk::TreeModelColumn<string> m_partNumber;
		Gtk::TreeModelColumn<string> m_description;
		Gtk::TreeModelColumn<string> m_size;
		Gtk::TreeModelColumn<guint> m_count;
		Gtk::TreeModelColumn<gdouble> m_price;
		Gtk::TreeModelColumn<gdouble> m_total;
		Gtk::TreeModelColumn<gint64> m_rowId;
		Gtk::TreeModelColumn<string> m_pnPrefix;
		Gtk::TreeModelColumn<guint> m_pnDigits;
		Gtk::TreeModelColumn<string> m_pnSuffix;
		Gtk::TreeModelColumn<string> m_notes;

		CollectionStore()
		{
			add(m_partNumber);
			add(m_description);
			add(m_size);
			add(m_count);
			add(m_price);
			add(m_total);
			add(m_rowId);
			add(m_pnPrefix);
			add(m_pnDigits);
			add(m_pnSuffix);
			add(m_notes);
		}
};
#endif
