/*
 * newPartDialog.h
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

#ifndef _NEWPARTSDIALOG_H
#define _NEWPARTSDIALOG_H

#include <boost/regex.hpp>
#include "mecparts.h"

using namespace std;

class NewPartDialog
{
	public:
		NewPartDialog(Glib::RefPtr<Gtk::Builder> pRefBuilder);
		virtual ~NewPartDialog();
		
		string PartNumber() { return m_pNewPartNumber->get_text(); };
		string Description() { return m_pNewPartDescription->get_text(); };
		string Size() { return m_pNewPartSize->get_text(); };
		double Price();
		string Notes() { return m_pNewPartNotes->get_buffer()->get_text(); };
		gint Run(string errorLabel,string currencyCode);
		void Hide();
		
	private:
		Gtk::Dialog *m_pDialog;
	
		bool on_delete_event(GdkEventAny *e);
		void on_partInfo_changed_event();
		Gtk::Label *m_pNewPartErrorLabel;
		Gtk::Entry *m_pNewPartNumber;
		Gtk::Entry *m_pNewPartDescription;
		Gtk::Entry *m_pNewPartSize;
		Gtk::Entry *m_pNewPartPrice;
		Gtk::TextView *m_pNewPartNotes;
		Gtk::Button *m_pNewPartOkButton;
    Gtk::Label *m_pNewPartPriceLabel;
		boost::regex m_pnRegex;
		boost::regex m_whitespaceRegex;
};

#endif // _NEWPARTDIALOG_H
