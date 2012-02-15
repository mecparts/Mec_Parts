/*
 * newCurrencyDialog.cpp
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
#include <boost/regex.hpp>
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

void NewCurrencyDialog::on_input_changed_event()
{
	boost::regex re("^[A-Z]{3}$");
	m_pNewCurrencyOkButton->set_sensitive(!Description().empty() && boost::regex_search(CurrencyCode(),re) && CurrencyRate()>0);
	m_pNewCurrencyErrorLabel->set_text("");
}

void NewCurrencyDialog::ClearInput()
{
	m_pNewCurrencyErrorLabel->set_text("");
	m_pNewCurrencyDescription->set_text("");
	m_pNewCurrencyCode->set_text("");
	m_pNewCurrencyRate->set_text("");
}
