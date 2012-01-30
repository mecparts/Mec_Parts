#ifndef _NEWSETSDIALOG_H
#define _NEWSETSDIALOG_H

#include "mecparts.h"

using namespace std;

class NewSetDialog
{
	public:
		NewSetDialog(Glib::RefPtr<Gtk::Builder> pRefBuilder);
		virtual ~NewSetDialog();
		
		Gtk::Dialog *m_pDialog;
	
		void ErrorLabel(string text) { m_pNewSetErrorLabel->set_text(text); }
		string SetNumber() { return m_pNewSetNumber->get_text(); };
		string Description() { return m_pNewSetDescription->get_text(); };
		int Started();
		int Ended();
		void ClearInput();
		
	private:
		bool on_delete_event(GdkEventAny *e);
		void on_setNum_changed_event();
		Gtk::Label *m_pNewSetErrorLabel;
		Gtk::Entry *m_pNewSetNumber;
		Gtk::Entry *m_pNewSetDescription;
		Gtk::Entry *m_pNewSetStarted;
		Gtk::Entry *m_pNewSetEnded;
		Gtk::Button *m_pNewSetOkButton;
};

#endif // _NEWSETDIALOG_H
