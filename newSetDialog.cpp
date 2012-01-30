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
	m_pNewSetNumber->signal_changed().connect(sigc::mem_fun(*this,&NewSetDialog::on_setNum_changed_event));

	GET_WIDGET(pRefBuilder,"setDescriptionEntry",m_pNewSetDescription)
	GET_WIDGET(pRefBuilder,"startedEntry",m_pNewSetStarted)
	GET_WIDGET(pRefBuilder,"endedEntry",m_pNewSetEnded)
}

NewSetDialog::~NewSetDialog()
{
	delete m_pDialog;
}

bool NewSetDialog::on_delete_event(GdkEventAny *e)
{
  return false;
}

void NewSetDialog::on_setNum_changed_event()
{
	m_pNewSetOkButton->set_sensitive(!SetNumber().empty());
	m_pNewSetErrorLabel->set_text("");
}

void NewSetDialog::ClearInput()
{
	m_pNewSetErrorLabel->set_text("");
	m_pNewSetNumber->set_text("");
	m_pNewSetDescription->set_text("");
	m_pNewSetStarted->set_text("");
	m_pNewSetEnded->set_text("");
}

int NewSetDialog::Started()
{
	string text = m_pNewSetStarted->get_text();
	int year;
	try {
		year = atoi(text.c_str());
	}
	catch(exception) {
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
	catch(exception) {
		year = 9999;
	}
	return year==0 ? 9999 : year;
}

