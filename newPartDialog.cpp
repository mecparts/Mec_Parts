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
	m_pNewPartOkButton(NULL)
{
	GET_WIDGET(pRefBuilder,"newPartDialog",m_pDialog)
	m_pDialog->signal_delete_event().connect(sigc::mem_fun(*this,&NewPartDialog::on_delete_event));

	GET_WIDGET(pRefBuilder,"newPartErrorLabel",m_pNewPartErrorLabel)
	GET_WIDGET(pRefBuilder,"newPartOkButton",m_pNewPartOkButton)
	GET_WIDGET(pRefBuilder,"partNumberEntry",m_pNewPartNumber)
	m_pNewPartNumber->signal_changed().connect(sigc::mem_fun(*this,&NewPartDialog::on_partNum_changed_event));

	GET_WIDGET(pRefBuilder,"descriptionEntry",m_pNewPartDescription)
	GET_WIDGET(pRefBuilder,"sizeEntry",m_pNewPartSize)
	GET_WIDGET(pRefBuilder,"priceEntry",m_pNewPartPrice)
	GET_WIDGET(pRefBuilder,"notesTextView",m_pNewPartNotes)
}

NewPartDialog::~NewPartDialog()
{
	delete m_pDialog;
}

bool NewPartDialog::on_delete_event(GdkEventAny *e)
{
  return false;
}

void NewPartDialog::on_partNum_changed_event()
{
	m_pNewPartOkButton->set_sensitive(!PartNumber().empty());
	m_pNewPartErrorLabel->set_text("");
}

void NewPartDialog::ClearInput()
{
	m_pNewPartErrorLabel->set_text("");
	m_pNewPartNumber->set_text("");
	m_pNewPartDescription->set_text("");
	m_pNewPartSize->set_text("");
	m_pNewPartPrice->set_text("");
	m_pNewPartNotes->get_buffer()->set_text("");
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
};
