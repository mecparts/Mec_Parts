#ifndef _SELECTPARTSDIALOG_H
#define _SELECTPARTSDIALOG_H

#include "mecparts.h"
#include "config.h"

class SelectPartsDialog
{
	public:
		SelectPartsDialog(Glib::RefPtr<Gtk::Builder> pRefBuilder, Config *pCfg);
		virtual ~SelectPartsDialog();
		
		Gtk::Dialog *m_pDialog;
		Glib::RefPtr<Gtk::TreeSelection> Selected() { return m_pSelectPartsView->get_selection(); }
		int SelectedCount() { return m_pSelectPartsView->get_selection()->count_selected_rows(); }
		void UnselectAll() { return m_pSelectPartsView->get_selection()->unselect_all(); }
	
	private:
		Config *m_pCfg;
		Gtk::TreeView *m_pSelectPartsView;
		void on_show_event();
		void on_hide_event();
		bool on_delete_event(GdkEventAny *e);
};

#endif // _SELECTPARTSDIALOG_H
