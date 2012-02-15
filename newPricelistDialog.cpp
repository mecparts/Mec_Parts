/*
 * newPricelistDialog.cpp
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
#include "newPricelistDialog.h"

using namespace std;

NewPricelistDialog::NewPricelistDialog(Glib::RefPtr<Gtk::Builder> pRefBuilder) :
	m_pDialog(NULL),
	m_pNewPricelistErrorLabel(NULL),
	m_pNewPricelistDescription(NULL),
	m_pNewPricelistCurrencyComboBox(NULL),
	m_pNewPricelistOkButton(NULL)
{
	
	GET_WIDGET(pRefBuilder,"newPricelistDialog",m_pDialog)
	m_pDialog->signal_delete_event().connect(sigc::mem_fun(*this,&NewPricelistDialog::on_delete_event));

	GET_WIDGET(pRefBuilder,"newPricelistErrorLabel",m_pNewPricelistErrorLabel)
	GET_WIDGET(pRefBuilder,"newPricelistOkButton",m_pNewPricelistOkButton)
	GET_WIDGET(pRefBuilder,"pricelistDescriptionEntry",m_pNewPricelistDescription)
	m_pNewPricelistDescription->signal_changed().connect(sigc::mem_fun(*this,&NewPricelistDialog::on_input_changed_event));
	GET_WIDGET(pRefBuilder,"pricelistCurrencyComboBox",m_pNewPricelistCurrencyComboBox);
	m_pNewPricelistCurrencyComboBox->pack_start(m_currenciesStore.m_name);
	m_pNewPricelistCurrencyComboBox->signal_changed().connect(sigc::mem_fun(*this,&NewPricelistDialog::on_input_changed_event));
}

NewPricelistDialog::~NewPricelistDialog()
{
	delete m_pDialog;
}

bool NewPricelistDialog::on_delete_event(GdkEventAny *e)
{
  return false;
}

void NewPricelistDialog::on_input_changed_event()
{
	m_pNewPricelistOkButton->set_sensitive(m_pNewPricelistCurrencyComboBox->get_active_row_number() != -1 && !Description().empty());
	m_pNewPricelistErrorLabel->set_text("");
}

void NewPricelistDialog::ClearInput()
{
	m_pNewPricelistErrorLabel->set_text("");
	m_pNewPricelistDescription->set_text("");
	m_pNewPricelistCurrencyComboBox->unset_active();
}

string NewPricelistDialog::CurrencyCode()
{
	string code = "";
  Gtk::TreeModel::iterator iter = m_pNewPricelistCurrencyComboBox->get_active();
  if(iter) {
    Gtk::TreeModel::Row row = *iter;
    if(row) {
			code = row[m_currenciesStore.m_code];
		}
	}
	return code;
}

string NewPricelistDialog::CurrencyName()
{
	string name = "";
  Gtk::TreeModel::iterator iter = m_pNewPricelistCurrencyComboBox->get_active();
  if(iter) {
    Gtk::TreeModel::Row row = *iter;
    if(row) {
			name = row[m_currenciesStore.m_name];
		}
	}
	return name;
}

double NewPricelistDialog::CurrencyRate()
{
	double rate = 1;
  Gtk::TreeModel::iterator iter = m_pNewPricelistCurrencyComboBox->get_active();
  if(iter) {
    Gtk::TreeModel::Row row = *iter;
    if(row) {
			rate = row[m_currenciesStore.m_rate];
		}
	}
	return rate;
}
