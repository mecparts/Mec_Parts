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
#include "toMakeStore.h"
#include "pricelistsStore.h"
#include "currenciesStore.h"
#include "config.h"
#include "selectPartsDialog.h"
#include "selectSetDialog.h"
#include "newPartDialog.h"
#include "newSetDialog.h"
#include "newPricelistDialog.h"
#include "newCurrencyDialog.h"
#include "sql.h"

class MainWindow
{
	public:
		MainWindow();
		virtual ~MainWindow();
		
		Gtk::Window *m_pWindow;

		void PopulateCurrencies(string code,string name,double rate);
		void PopulatePricelists(gint64 num,string description,string currencyName,string code,double rate);
		void PopulateParts(gint64 rowId,string partNumber,string description,string size,gdouble price,string notes,string pnPrefix,int pnDigits,string pnSuffix);
		void PopulateSets(string setNumber,string description,int started,int ended,gint64 rowId);
		void PopulateCollection(gint64 rowId,string partNumber,string description,string size,guint count,gdouble price,gdouble total,string pnPrefix,int pnDigits,string pnSuffix,string notes);
		void AddSetPartToPartsList(string partNumber,int count);
		void PopulateToMake(string partNumber,string description,string size,double price,int count,double total,string notes);

	private:
		bool m_initialized;
		Glib::RefPtr<Gtk::Builder> m_refBuilder;
		Config m_cfg;
		Sql *m_pSql;
		map<string,int> m_partsList;
		bool m_bSetsFiltered;
		string SqlEscaped(string value);
		bool on_delete_event(GdkEventAny *e);
		void DisplayPicture(string partNumber,string description,string size,string notes);
		
		void CurrenciesSetup();
		void on_currency_use_toggled(const Glib::ustring &pathStr);
		void on_currencies_button_pressed(GdkEventButton *pEvent);
		void on_currenciesUpdateCurrency_activated_event();
		void currenciesDownloaded(Glib::RefPtr<Gio::AsyncResult>& result);
		void on_currenciesNewCurrency_activated_event();
		void on_currenciesDeleteCurrency_activated_event();
		void on_currencies_name_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_currencies_rate_edited(Glib::ustring pathStr, Glib::ustring text);
		Gtk::TreeView *m_pCurrenciesView;
		Glib::RefPtr<Gtk::ListStore> m_pCurrenciesStore;
		CurrenciesStore m_currenciesStore;
		string m_localeCurrencyCode;
		string m_baseCurrencyCode;
		double m_baseCurrencyRate;
		Gtk::Menu *m_pCurrenciesContextMenu;
		Gtk::MenuItem *m_pCurrenciesUpdateCurrencyMenuItem;
		Gtk::MenuItem *m_pCurrenciesNewCurrencyMenuItem;
		Gtk::MenuItem *m_pCurrenciesDeleteCurrencyMenuItem;
		
		void PricelistsSetup();
		void RefreshPriceLists();
		void on_pricelists_description_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_pricelists_currency_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_pricelists_button_pressed(GdkEventButton *pEvent);
		void on_pricelistsImportPrices_activated_event();
		void on_pricelistsNewPricelist_activated_event();
		void on_pricelistsDeletePricelist_activated_event();
		void on_pricelist_use_toggled(const Glib::ustring &pathStr);
		Gtk::TreeView *m_pPricelistsView;
		Glib::RefPtr<Gtk::ListStore> m_pPricelistsStore;
		gint64 m_pricelistNumber;
		double m_pricelistCurrencyRate;
		string m_pricelistCurrencyCode;
		PricelistsStore m_pricelistsStore;
		Gtk::Menu *m_pPricelistsContextMenu;
		Gtk::MenuItem *m_pPricelistsImportPricesMenuItem;
		Gtk::MenuItem *m_pPricelistsNewPricelistMenuItem;
		Gtk::MenuItem *m_pPricelistsDeletePricelistMenuItem;
		Gtk::FileChooserDialog *m_pPricelistImportCsvDialog;

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
		int m_collectionViewPriceColumnIndex;
		void CollectionSetup();
		void on_collection_set_combobox_changed_event();
		void on_collection_count_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_collection_button_pressed(GdkEventButton *pEvent);
		void on_collectionViewPart_activated_event();
		void on_collectionAddPart_activated_event();
		void on_collectionDeletePart_activated_event();
		void on_collectionAddSet_activated_event();
		void on_collection_partNumber_clicked();
		void on_collection_description_clicked();
		void AddPartToCollection(Gtk::TreeModel::Row partRow,int count,bool updateIfFound);
		void CalculateCollectionTotals();
		void FillCollection();
		
		Gtk::TreeView *m_pPartsView;
		PartsStore m_partsStore;
		Glib::RefPtr<Gtk::ListStore> m_pPartsStore;
		void PartsSetup();
		void FillParts();
		bool PartExists(string partNumber);
		void on_parts_description_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_parts_size_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_parts_price_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_parts_notes_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_parts_button_pressed(GdkEventButton *pEvent);
		void on_partsViewPart_activated_event();
		void on_partsNewPart_activated_event();
		void on_partsDeletePart_activated_event();
		void on_partsFilterSets_activated_event();
		void on_partsUnfilterSets_activated_event();
		void on_parts_partNumber_clicked();
		void on_parts_description_clicked();
		void UpdateParts(string num,string field);
		Gtk::Menu *m_pPartsContextMenu;
		Gtk::MenuItem *m_pPartsViewPartMenuItem;
		Gtk::MenuItem *m_pPartsNewPartMenuItem;
		Gtk::MenuItem *m_pPartsDeletePartMenuItem;
		Gtk::MenuItem *m_pPartsFilterSetsMenuItem;
		Gtk::MenuItem *m_pPartsUnfilterSetsMenuItem;
		string m_partsPartNumber;
		string m_partsPartDescription;
		int m_partsViewPriceColumnIndex;

		Gtk::TreeView *m_pSetsView;
		SetsStore m_setsStore;
		Glib::RefPtr<Gtk::ListStore> m_pSetsStore;
		void SetsSetup();
		void on_sets_description_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_sets_started_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_sets_ended_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_sets_button_pressed(GdkEventButton *pEvent);
		void on_setsNewSet_activated_event();
		void on_setsDeleteSet_activated_event();
		void on_sets_setNumber_clicked();
		void on_sets_description_clicked();
		Gtk::Menu *m_pSetsContextMenu;
		Gtk::MenuItem *m_pSetsNewSetMenuItem;
		Gtk::MenuItem *m_pSetsDeleteSetMenuItem;
		
		Gtk::TreeView *m_pToMakeView;
		ToMakeStore m_toMakeStore;
		Glib::RefPtr<Gtk::ListStore> m_pToMakeStore;
		Gtk::Label *m_pToMakeCost;
		Gtk::ComboBox *m_pToMakeHaveComboBox;
		Gtk::ComboBox *m_pToMakeWantComboBox;
		Gtk::Menu *m_pToMakeContextMenu;
		Gtk::MenuItem *m_pToMakeViewPartMenuItem;
		int m_toMakePartsCount;
		double m_toMakeCost;
		int m_toMakeViewPriceColumnIndex;
		void ToMakeSetup();
		void on_toMakeComboBox_changed_event();
		void on_toMake_button_pressed(GdkEventButton *pEvent);
		void on_toMakeViewPart_activated_event();
		void on_toMake_partNumber_clicked();
		void on_toMake_description_clicked();
		void FillToMake0();
		void FillToMake(string haveNum,string wantNum);
		
		SelectPartsDialog *m_pSelectPartsDialog;
		void AddPartCallback(const Gtk::TreeModel::iterator &iter);
		
		SelectSetDialog *m_pSelectSetDialog;
		void AddSetPartsCallback(const Gtk::TreeModel::iterator &iter);

		NewPartDialog *m_pNewPartDialog;
		
		NewSetDialog *m_pNewSetDialog;
		
		NewPricelistDialog *m_pNewPricelistDialog;
		
		NewCurrencyDialog *m_pNewCurrencyDialog;
};

#endif // _MAINWINDOW_H
