/*
 * mainWindow.h
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

#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

#include "mecparts.h"
#include <map>
#include "collectionStore.h"
#include "partsStore.h"
#include "setsStore.h"
#include "neededStore.h"
#include "pricelistsStore.h"
#include "config.h"
#include "selectPartsDialog.h"
#include "selectSetDialog.h"
#include "newPartDialog.h"
#include "newSetDialog.h"
#include "sql.h"

class MainWindow
{
	public:
		MainWindow();
		virtual ~MainWindow();
		
		Gtk::Window *m_pWindow;

		void PopulatePricelists(gint64 num,string description);
		void PopulateParts(gint64 rowId,string partNumber,string description,string size,gdouble price,string notes,string pnPrefix,int pnDigits,string pnSuffix);
		void PopulateSets(string setNumber,string description,int started,int ended,gint64 rowId);
		void PopulateCollection(gint64 rowId,string partNumber,string description,string size,guint count,gdouble price,gdouble total,string pnPrefix,int pnDigits,string pnSuffix);
		void AddSetPartToPartsList(string partNumber,int count);
		void PopulateNeeded(string partNumber,string description,string size,double price,int count,double total);

	private:
		bool m_initialized;
		Glib::RefPtr<Gtk::Builder> m_refBuilder;
		Config m_cfg;
		Sql *m_pSql;
		map<string,int> m_partsList;
		bool m_bSetsFiltered;
		string SqlEscaped(string value);
		Gtk::MenuItem *m_pNewDatabaseMenuItem;
		Gtk::MenuItem *m_pOpenDatabaseMenuItem;
		Gtk::MenuItem *m_pProgramQuitMenuItem;
		Gtk::MenuItem *m_pImportPricelistMenuItem;
		Gtk::MenuItem *m_pDeletePricelistMenuItem;
		Gtk::MenuItem *m_pAboutMenuItem;
		void on_program_quit();
		bool on_delete_event(GdkEventAny *e);
		void DisplayPicture(string partNumber,string description,string size);
		
		void on_new_database();
		void on_open_database();
		void on_about();
		
		void PricelistsSetup();
		Gtk::Menu *m_pPricelistMenu;
		Glib::RefPtr<Gtk::ListStore> m_pPricelistStore;
		gint64 m_pricelistNumber;
		PricelistsStore m_pricelistStore;
		Gtk::RadioMenuItem *m_pFirstPricelistMenuItem;
		void on_import_pricelist();
		void on_delete_pricelist();
		void on_pricelist_toggled_event(Gtk::RadioMenuItem *pMenuItem,gint64 rowId);
		
		Gtk::TreeView *m_pCollectionView;
		CollectionStore m_collectionStore;
		Glib::RefPtr<Gtk::ListStore> m_pCollectionStore;
		Gtk::Label *m_pCollectionCountCost;
		Gtk::ComboBox *m_pCollectionSetComboBox;
		Gtk::Menu *m_pCollectionContextMenu;
		Gtk::MenuItem *m_pCollectionViewPartMenuItem;
		Gtk::MenuItem *m_pCollectionAddPartMenuItem;
		Gtk::MenuItem *m_pCollectionDeletePartMenuItem;
		Gtk::MenuItem *m_pCollectionAddSetMenuItem;
		string m_collectionNumber;
		string m_collectionDescription;
		string m_collectionPartNumber;
		string m_collectionPartDescription;
		int m_collectionPartsCount;
		double m_collectionCost;
		void CollectionSetup();
		void on_collection_set_combobox_changed_event();
		void on_collection_count_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_collection_selection_changed_event();
		void on_collection_button_pressed(GdkEventButton *pEvent);
		void on_collectionViewPart_activated_event();
		void on_collectionAddPart_activated_event();
		void on_collectionDeletePart_activated_event();
		void on_collectionAddSet_activated_event();
		void AddPartToCollection(Gtk::TreeModel::Row partRow,int count,bool updateIfFound);
		void CalculateCollectionTotals();
		void FillCollection();
		
		Gtk::TreeView *m_pPartsView;
		PartsStore m_partsStore;
		Glib::RefPtr<Gtk::ListStore> m_pPartsStore;
		void PartsSetup();
		void FillParts();
		void on_parts_description_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_parts_size_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_parts_price_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_parts_notes_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_parts_selection_changed_event();
		void on_parts_button_pressed(GdkEventButton *pEvent);
		void on_partsViewPart_activated_event();
		void on_partsNewPart_activated_event();
		void on_partsDeletePart_activated_event();
		void on_partsFilterSets_activated_event();
		void on_partsUnfilterSets_activated_event();
		void UpdateParts(string num,string field);
		Gtk::Menu *m_pPartsContextMenu;
		Gtk::MenuItem *m_pPartsViewPartMenuItem;
		Gtk::MenuItem *m_pPartsNewPartMenuItem;
		Gtk::MenuItem *m_pPartsDeletePartMenuItem;
		Gtk::MenuItem *m_pPartsFilterSetsMenuItem;
		Gtk::MenuItem *m_pPartsUnfilterSetsMenuItem;
		string m_partsPartNumber;
		string m_partsPartDescription;

		Gtk::TreeView *m_pSetsView;
		SetsStore m_setsStore;
		Glib::RefPtr<Gtk::ListStore> m_pSetsStore;
		void SetsSetup();
		void on_sets_description_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_sets_started_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_sets_ended_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_sets_selection_changed_event();
		void on_sets_button_pressed(GdkEventButton *pEvent);
		void on_setsNewSet_activated_event();
		void on_setsDeleteSet_activated_event();
		Gtk::Menu *m_pSetsContextMenu;
		Gtk::MenuItem *m_pSetsNewSetMenuItem;
		Gtk::MenuItem *m_pSetsDeleteSetMenuItem;
		
		Gtk::TreeView *m_pNeededView;
		NeededStore m_neededStore;
		Glib::RefPtr<Gtk::ListStore> m_pNeededStore;
		Gtk::Label *m_pNeededCost;
		Gtk::ComboBox *m_pNeededHaveComboBox;
		Gtk::ComboBox *m_pNeededWantComboBox;
		Gtk::Menu *m_pNeededContextMenu;
		Gtk::MenuItem *m_pNeededViewPartMenuItem;
		int m_neededPartsCount;
		double m_neededCost;
		void NeededSetup();
		void on_neededComboBox_changed_event();
		void on_needed_button_pressed(GdkEventButton *pEvent);
		void on_neededViewPart_activated_event();
		void FillNeeded(string haveNum,string wantNum);
		
		SelectPartsDialog *m_pSelectPartsDialog;
		void AddPartCallback(const Gtk::TreeModel::iterator &iter);
		
		SelectSetDialog *m_pSelectSetDialog;
		void AddSetPartsCallback(const Gtk::TreeModel::iterator &iter);

		NewPartDialog *m_pNewPartDialog;
		
		NewSetDialog *m_pNewSetDialog;
		
};

#endif // _MAINWINDOW_H
