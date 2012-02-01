/*
 * selectSetDialog.h
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
