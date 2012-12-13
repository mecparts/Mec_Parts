/*
 * newPartDialog.cpp: create a new part
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
#include "newPartDialog.h"

using namespace std;

NewPartDialog::NewPartDialog(Glib::RefPtr<Gtk::Builder> pRefBuilder) :
	m_pDialog(NULL),
	m_pNewPartErrorLabel(NULL),
	m_pNewPartNumber(NULL),
	m_pNewPartDescription(NULL),
	m_pNewPartSize(NULL),
	m_pNewPartPrice(NULL),
	m_pNewPartNotes(NULL),
	m_pNewPartOkButton(NULL),
	m_pNewPartPriceLabel(NULL)
{
	GET_WIDGET(pRefBuilder,"newPartDialog",m_pDialog)
	m_pDialog->signal_delete_event().connect(sigc::mem_fun(*this,&NewPartDialog::on_delete_event));

	GET_WIDGET(pRefBuilder,"newPartErrorLabel",m_pNewPartErrorLabel)
	GET_WIDGET(pRefBuilder,"newPartPriceLabel",m_pNewPartPriceLabel)
	GET_WIDGET(pRefBuilder,"newPartOkButton",m_pNewPartOkButton)
	GET_WIDGET(pRefBuilder,"partNumberEntry",m_pNewPartNumber)
	m_pNewPartNumber->signal_changed().connect(sigc::mem_fun(*this,&NewPartDialog::on_partInfo_changed_event));

	GET_WIDGET(pRefBuilder,"partDescriptionEntry",m_pNewPartDescription)
	m_pNewPartDescription->signal_changed().connect(sigc::mem_fun(*this,&NewPartDialog::on_partInfo_changed_event));
	GET_WIDGET(pRefBuilder,"partSizeEntry",m_pNewPartSize)
	GET_WIDGET(pRefBuilder,"partPriceEntry",m_pNewPartPrice)
	GET_WIDGET(pRefBuilder,"partNotesTextView",m_pNewPartNotes)
	m_pnRegex.assign("^([A-Za-z]*)([0-9]+)([A-Za-z0-9]*)$");
	m_whitespaceRegex.assign("^\\s*$");
}

NewPartDialog::~NewPartDialog()
{
	delete m_pDialog;
}

bool NewPartDialog::on_delete_event(GdkEventAny *e)
{
	return false;
}

// Use a regex to make sure the ok button isn't enabled until we have
// a valid part number and a non blank description. The part number can
// take the following forms:
//
// 123 prefix='' digits=123 suffix=''
// abc123 prefix='abc' digits=123 suffix=''
// 123abc456def prefix='' digits=123 suffix='abc456def'
// abc123def456ghi prefix='abc' digits=123 suffix='def456ghi'
void NewPartDialog::on_partInfo_changed_event()
{
	m_pNewPartOkButton->set_sensitive(boost::regex_search(PartNumber(),m_pnRegex) && !boost::regex_search(Description(),m_whitespaceRegex));
	m_pNewPartErrorLabel->set_text("");
}

gint NewPartDialog::Run(string errorLabel,string currencyCode)
{
	m_pNewPartPriceLabel->set_text("Price ("+currencyCode+"):");
	m_pNewPartErrorLabel->set_text(errorLabel);
	if( errorLabel.empty() ) {
		m_pNewPartNumber->set_text("");
		m_pNewPartDescription->set_text("");
		m_pNewPartSize->set_text("");
		m_pNewPartPrice->set_text("");
		m_pNewPartNotes->get_buffer()->set_text("");
		m_pNewPartNumber->grab_focus();
	}
	return m_pDialog->run();
}

void NewPartDialog::Hide()
{
	m_pDialog->hide();
}

double NewPartDialog::Price()
{
	string text = m_pNewPartPrice->get_text();
	try {
		return atof(text.c_str());
	}
	catch (exception) {
		return 0.0;
	}
}
