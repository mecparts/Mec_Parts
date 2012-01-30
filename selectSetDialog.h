#ifndef _SELECTSETSDIALOG_H
#define _SELECTSETSDIALOG_H

#include "mecparts.h"
#include "config.h"

class SelectSetDialog
{
	public:
		SelectSetDialog(Glib::RefPtr<Gtk::Builder> pRefBuilder, Config *pCfg);
		virtual ~SelectSetDialog();
		
		Gtk::Dialog *m_pDialog;
		Glib::RefPtr<Gtk::TreeSelection> Selected() { return m_pSelectSetView->get_selection(); }
		int SelectedCount() { return m_pSelectSetView->get_selection()->count_selected_rows(); }
		void UnselectAll() { return m_pSelectSetView->get_selection()->unselect_all(); }
	
	private:
		Config *m_pCfg;
		Gtk::TreeView *m_pSelectSetView;
		void on_show_event();
		void on_hide_event();
		bool on_delete_event(GdkEventAny *e);
};

#endif // _SELECTSetDIALOG_H
