#ifndef _NEWPARTSDIALOG_H
#define _NEWPARTSDIALOG_H

#include "mecparts.h"

using namespace std;

class NewPartDialog
{
	public:
		NewPartDialog(Glib::RefPtr<Gtk::Builder> pRefBuilder);
		virtual ~NewPartDialog();
		
		Gtk::Dialog *m_pDialog;
	
		void ErrorLabel(string text) { m_pNewPartErrorLabel->set_text(text); }
		string PartNumber() { return m_pNewPartNumber->get_text(); };
		string Description() { return m_pNewPartDescription->get_text(); };
		string Size() { return m_pNewPartSize->get_text(); };
		double Price();
		string Notes() { return m_pNewPartNotes->get_buffer()->get_text(); };
		void ClearInput();
		
	private:
		bool on_delete_event(GdkEventAny *e);
		void on_partNum_changed_event();
		Gtk::Label *m_pNewPartErrorLabel;
		Gtk::Entry *m_pNewPartNumber;
		Gtk::Entry *m_pNewPartDescription;
		Gtk::Entry *m_pNewPartSize;
		Gtk::Entry *m_pNewPartPrice;
		Gtk::TextView *m_pNewPartNotes;
		Gtk::Button *m_pNewPartOkButton;
};

#endif // _NEWPARTDIALOG_H
