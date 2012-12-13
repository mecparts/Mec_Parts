/*
 * newCurrencyDialog.h
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

#ifndef _NEWCURRENCYDIALOG_H
#define _NEWCURRENCYDIALOG_H

#include <boost/regex.hpp>
#include "mecparts.h"
#include "currenciesStore.h"

using namespace std;

class NewCurrencyDialog
{
	public:
		NewCurrencyDialog(Glib::RefPtr<Gtk::Builder> pRefBuilder);
		virtual ~NewCurrencyDialog();
		
		Gtk::Dialog *m_pDialog;
	
		string Description() { return m_pNewCurrencyDescription->get_text(); }
		string CurrencyCode() { return m_pNewCurrencyCode->get_text(); }
		double CurrencyRate() { return atof(m_pNewCurrencyRate->get_text().c_str()); }
		gint Run(string errorLabel);
		void Hide();
		
	private:
		bool on_delete(GdkEventAny *e);
		void on_code_changed();
		void on_input_changed();
		Gtk::Label *m_pNewCurrencyErrorLabel;
		Gtk::Entry *m_pNewCurrencyDescription;
		Gtk::Entry *m_pNewCurrencyCode;
		Gtk::Entry *m_pNewCurrencyRate;
		Gtk::Button *m_pNewCurrencyOkButton;
		CurrenciesStore m_currenciesStore;
		boost::regex m_codeRegex;
		boost::regex m_whitespaceRegex;
};

#endif // _NEWCURRENCYDIALOG_H
