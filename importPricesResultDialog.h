/*
 * importPricesResultDialog.h: display the results of importing a csv file
 *  containing part prices to a price list
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

#ifndef _IMPORTPRICESRESULTDIALOG_H
#define _IMPORTPRICESRESULTDIALOG_H

#include <vector>
#include <string>
#include "mecparts.h"

using namespace std;

class ImportPricesResultDialog
{
	public:
		ImportPricesResultDialog(Glib::RefPtr<Gtk::Builder> pRefBuilder);
		virtual ~ImportPricesResultDialog();
		
		gint Run(int numAdded,int numUpdated,vector<string> unknownParts);
		void Hide();
	private:
		Gtk::Dialog *m_pDialog;
		bool on_delete_event(GdkEventAny *e);
		Gtk::Label *m_pPartsAddedLabel;
		Gtk::Label *m_pPartsUpdatedLabel;
		Gtk::TextView *m_pUnknownPartsTextView;
		Gtk::Button *m_pImportPricesResultDialogOkButton;
};

#endif // _IMPORTPRICESRESULTDIALOG_H
