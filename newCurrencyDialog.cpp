/*
 * newCurrencyDialog.cpp: add a new currency
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
#include "newCurrencyDialog.h"

using namespace std;

NewCurrencyDialog::NewCurrencyDialog(Glib::RefPtr<Gtk::Builder> pRefBuilder) :
	m_pDialog(NULL),
	m_pNewCurrencyErrorLabel(NULL),
	m_pNewCurrencyDescription(NULL),
	m_pNewCurrencyCode(NULL),
	m_pNewCurrencyRate(NULL),
	m_pNewCurrencyOkButton(NULL)
{
	
	GET_WIDGET(pRefBuilder,"newCurrencyDialog",m_pDialog)
	m_pDialog->signal_delete_event().connect(sigc::mem_fun(*this,&NewCurrencyDialog::on_delete_event));

	GET_WIDGET(pRefBuilder,"newCurrencyErrorLabel",m_pNewCurrencyErrorLabel)
	GET_WIDGET(pRefBuilder,"newCurrencyOkButton",m_pNewCurrencyOkButton)
	GET_WIDGET(pRefBuilder,"currencyDescriptionEntry",m_pNewCurrencyDescription)
	m_pNewCurrencyDescription->signal_changed().connect(sigc::mem_fun(*this,&NewCurrencyDialog::on_input_changed_event));
	GET_WIDGET(pRefBuilder,"currencyCodeEntry",m_pNewCurrencyCode);
	m_pNewCurrencyCode->signal_changed().connect(sigc::mem_fun(*this,&NewCurrencyDialog::on_code_changed_event));
	GET_WIDGET(pRefBuilder,"currencyRateEntry",m_pNewCurrencyRate);
	m_pNewCurrencyRate->signal_changed().connect(sigc::mem_fun(*this,&NewCurrencyDialog::on_input_changed_event));
	m_codeRegex.assign("^[A-Z]{3}$");
	m_whitespaceRegex.assign("^\\s*$");
}

NewCurrencyDialog::~NewCurrencyDialog()
{
	delete m_pDialog;
}

bool NewCurrencyDialog::on_delete_event(GdkEventAny *e)
{
	return false;
}

void NewCurrencyDialog::on_code_changed_event()
{
	string code = CurrencyCode();
	transform(code.begin(), code.end(), code.begin(), (int(*)(int)) toupper);
	m_pNewCurrencyCode->set_text(code);
	on_input_changed_event();
}

// the minimum requirements for allowing the creation of a new currency are:
// a non blank currency name
// a 3 letter currency code
// an positive exchange rate
void NewCurrencyDialog::on_input_changed_event()
{
	m_pNewCurrencyOkButton->set_sensitive(!boost::regex_search(Description(),m_whitespaceRegex) && boost::regex_search(CurrencyCode(),m_codeRegex) && CurrencyRate()>0);
	m_pNewCurrencyErrorLabel->set_text("");
}

gint NewCurrencyDialog::Run(string errorLabel)
{
	m_pNewCurrencyErrorLabel->set_text(errorLabel);
	if( errorLabel.empty() ) {
	  m_pNewCurrencyDescription->set_text("");
	  m_pNewCurrencyCode->set_text("");
	  m_pNewCurrencyRate->set_text("");
	  m_pNewCurrencyDescription->grab_focus();
	}
	return m_pDialog->run();
}

void NewCurrencyDialog::Hide()
{
	m_pDialog->hide();
}
