/*
 * newSetDialog.cpp: create a new set
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
#include "newSetDialog.h"

using namespace std;

NewSetDialog::NewSetDialog(Glib::RefPtr<Gtk::Builder> pRefBuilder) :
	m_pDialog(NULL),
	m_pNewSetErrorLabel(NULL),
	m_pNewSetNumber(NULL),
	m_pNewSetDescription(NULL),
	m_pNewSetStarted(NULL),
	m_pNewSetEnded(NULL),
	m_pNewSetOkButton(NULL)
{
	GET_WIDGET(pRefBuilder,"newSetDialog",m_pDialog)
	m_pDialog->signal_delete_event().connect(sigc::mem_fun(*this,&NewSetDialog::on_delete_event));

	GET_WIDGET(pRefBuilder,"newSetErrorLabel",m_pNewSetErrorLabel)
	GET_WIDGET(pRefBuilder,"newSetOkButton",m_pNewSetOkButton)
	GET_WIDGET(pRefBuilder,"setNumberEntry",m_pNewSetNumber)
	m_pNewSetNumber->signal_changed().connect(sigc::mem_fun(*this,&NewSetDialog::on_setInfo_changed_event));

	GET_WIDGET(pRefBuilder,"setDescriptionEntry",m_pNewSetDescription)
	m_pNewSetDescription->signal_changed().connect(sigc::mem_fun(*this,&NewSetDialog::on_setInfo_changed_event));
	GET_WIDGET(pRefBuilder,"startedEntry",m_pNewSetStarted)
	GET_WIDGET(pRefBuilder,"endedEntry",m_pNewSetEnded)
	m_whitespaceRegex.assign("^\\s*$");
}

NewSetDialog::~NewSetDialog()
{
	delete m_pDialog;
}

bool NewSetDialog::on_delete_event(GdkEventAny *e)
{
	return false;
}

// Minimum requirements for creating a new set are a non blank set number and description
void NewSetDialog::on_setInfo_changed_event()
{
	m_pNewSetOkButton->set_sensitive(!boost::regex_search(SetNumber(),m_whitespaceRegex) && !boost::regex_search(Description(),m_whitespaceRegex));
	m_pNewSetErrorLabel->set_text("");
}

// clear out any prior user text, set the cursor to the first field and run the dialog
gint NewSetDialog::Run(string errorLabel)
{
	m_pNewSetErrorLabel->set_text(errorLabel);
	if( errorLabel.empty() ) {
		m_pNewSetNumber->set_text("");
		m_pNewSetDescription->set_text("");
		m_pNewSetStarted->set_text("");
		m_pNewSetEnded->set_text("");
		m_pNewSetNumber->grab_focus();
	}
	return m_pDialog->run();
}

void NewSetDialog::Hide()
{
	m_pDialog->hide();
}

int NewSetDialog::Started()
{
	string text = m_pNewSetStarted->get_text();
	int year;
	try {
		year = atoi(text.c_str());
	}
	catch(exception &) {
		year = 9999;
	}
	return year==0 ? 9999 : year;
}

int NewSetDialog::Ended()
{
	string text = m_pNewSetEnded->get_text();
	int year;
	try {
		year = atoi(text.c_str());
	}
	catch(exception &) {
		year = 9999;
	}
	return year==0 ? 9999 : year;
}

