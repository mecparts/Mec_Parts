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
#include <vector>
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
#include "importResultDialog.h"
#include "sql.h"
#include "partPrice.h"

class MainWindow
{
	public:
		MainWindow();
		virtual ~MainWindow();
		
		Gtk::Window *m_pWindow;
		Glib::RefPtr<Gtk::StyleContext> m_refStyleContext;
		
		static int PopulateCurrenciesCallback(void *wnd, int argc, char **argv, char **azColName);
		static int PopulatePricelistsCallback(void *wnd, int argc, char **argv, char **azColName);
		static int PopulatePartsCallback(void *wnd, int argc, char **argv, char **azColName);
		static int PopulateSetsCallback(void *wnd, int argc, char **argv, char **azColName);
		static int PopulateCollectionCallback(void *wnd, int argc, char **argv, char **azColName);
		static int AddSetPartCallback(void *wnd,int argc,char **argv, char **azColName);
		static int PopulateToMakeCallback(void *wnd, int argc, char **argv, char **azColName);
		static int RefreshPricesCallback(void *wnd, int argc, char **argv, char **azColName);

		void PopulatePricelists(gint64 num,string description,string currencyName,string code,double rate);
		void PopulateParts(gint64 rowId,string partNumber,string description,string size,gdouble price,string notes,string pnPrefix,int pnDigits,string pnSuffix);
		void PopulateSets(string setNumber,string description,int started,int ended,gint64 rowId);
		void PopulateCollection(gint64 rowId,string partNumber,string description,string size,guint count,gdouble price,gdouble total,string pnPrefix,int pnDigits,string pnSuffix,string notes);
		void AddSetPartToPartsList(string partNumber,int count);
		void PopulateToMake(string partNumber,string description,string size,double price,int count,double total,string notes);
		
	private:
		bool m_initialized;
		string m_baseDir;
		Glib::RefPtr<Gtk::Builder> m_refBuilder;
		Config m_cfg;
		Sql *m_pSql;
		map<string,int> m_partsList;
		vector<PartPrice *> m_partPricesList;
		bool m_bSetsFiltered;
		bool on_delete(GdkEventAny *e);
		void WaitCursor(bool on);
		void TagReadOnlyColumns(Gtk::TreeView *pTreeView);
		void DrawColumn(Gtk::CellRendererText *r,stringstream &text,Gtk::TreeView *t,bool missing);
		void DisplayPicture(string partNumber,string description,string size,string notes);
		Gtk::ComboBox *m_pPricelistComboBox;
		Gtk::ComboBox *m_pCurrencyComboBox;
		Gdk::RGBA m_readOnlyCellBackground;
		
		void CurrenciesSetup();
		void on_currency_combobox_changed();
		void on_currencies_button_pressed(GdkEventButton *pEvent);
		void on_currenciesUpdateCurrency_activated();
		void currenciesDownloaded(Glib::RefPtr<Gio::AsyncResult>& result);
		void on_currenciesNewCurrency_activated();
		void on_currenciesDeleteCurrency_activated();
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
		
	protected:
		Glib::RefPtr<Gtk::ListStore> m_pPricelistsStore;
		PricelistsStore m_pricelistsStore;
	private:
		void PricelistsSetup();
		void RefreshPrices();
		void on_pricelists_description_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_pricelists_currency_changed(const Glib::ustring &pathStr, const Gtk::TreeIter &comboxboxIter);
		void on_pricelists_button_pressed(GdkEventButton *pEvent);
		void on_pricelistsImportPrices_activated();
		void on_pricelistsNewPricelist_activated();
		void on_pricelistsDeletePricelist_activated();
		void on_pricelist_combobox_changed();
		double ToBaseCurrency(double pricelist_price);
		double ToPricelistCurrency(double price);
		Gtk::TreeView *m_pPricelistsView;
		gint64 m_pricelistNumber;
		double m_pricelistCurrencyRate;
		string m_pricelistCurrencyCode;
		Gtk::Menu *m_pPricelistsContextMenu;
		Gtk::MenuItem *m_pPricelistsImportPricesMenuItem;
		Gtk::MenuItem *m_pPricelistsNewPricelistMenuItem;
		Gtk::MenuItem *m_pPricelistsDeletePricelistMenuItem;
		Gtk::FileChooserDialog *m_pImportFileChooserDialog;
		ImportResultDialog *m_pImportResultDialog;

	protected:
		CollectionStore m_collectionStore;
		Glib::RefPtr<Gtk::ListStore> m_pCollectionStore;
		int m_collectionPartsCount;
		double m_collectionCost;
	private:
		Gtk::TreeView *m_pCollectionView;
		Gtk::CellRendererText *m_pCollectionCountCellRenderer;
		Gtk::CellRendererText *m_pCollectionPriceCellRenderer;
		Gtk::Label *m_pCollectionCountCost;
		Gtk::Button *m_pCollectionManualButton;
		Gtk::ComboBox *m_pCollectionSetComboBox;
		Gtk::Menu *m_pCollectionContextMenu;
		Gtk::ScrolledWindow *m_pCollectionScrolledWindow;
		Gtk::MenuItem *m_pCollectionViewPartMenuItem;
		Gtk::MenuItem *m_pCollectionAddPartMenuItem;
		Gtk::MenuItem *m_pCollectionDeletePartMenuItem;
		Gtk::MenuItem *m_pCollectionAddSetMenuItem;
		Gtk::MenuItem *m_pCollectionImportPartsMenuItem;
		string m_collectionNumber;
		string m_collectionDescription;
		int m_collectionViewPriceColumnIndex;
		void CollectionSetup();
		void on_collection_set_combobox_changed();
		void on_collection_manual_button_clicked();
		void on_collection_count_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_collection_count_column_drawn(Gtk::CellRenderer *r,const Gtk::TreeModel::iterator &i);
		void on_collection_price_column_drawn(Gtk::CellRenderer *r,const Gtk::TreeModel::iterator &i);
		void on_collection_button_pressed(GdkEventButton *pEvent);
		void on_collectionViewPart_activated();
		void on_collectionAddPart_activated();
		void on_collectionDeletePart_activated();
		void on_collectionAddSet_activated();
		void on_collectionImportParts_activated();
		void on_collection_partNumber_clicked();
		void on_collection_description_clicked();
		void on_collection_size_clicked();
		void on_collection_notes_clicked();
		bool AddPartToCollection(Gtk::TreeModel::Row partRow,int count,bool updateIfFound);
		void CalculateCollectionTotals();

	protected:    
		Glib::RefPtr<Gtk::ListStore> m_pPartsStore;
		PartsStore m_partsStore;
	private:
		Gtk::TreeView *m_pPartsView;
		void PartsSetup();
		bool PartExists(string partNumber);
		bool PartExists(string partNumber,Gtk::TreeModel::Row &partRow);
		void on_parts_description_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_parts_size_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_parts_price_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_parts_notes_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_parts_button_pressed(GdkEventButton *pEvent);
		void on_partsViewPart_activated();
		void on_partsNewPart_activated();
		void on_partsDeletePart_activated();
		void on_partsFilterSets_activated();
		void on_partsUnfilterSets_activated();
		void on_parts_partNumber_clicked();
		void on_parts_description_clicked();
		void on_parts_size_clicked();
		void on_parts_notes_clicked();
		void on_parts_price_column_drawn(Gtk::CellRenderer *r,const Gtk::TreeModel::iterator &i);
		void UpdateParts(string num,string field);
		Gtk::Menu *m_pPartsContextMenu;
		Gtk::MenuItem *m_pPartsViewPartMenuItem;
		Gtk::MenuItem *m_pPartsNewPartMenuItem;
		Gtk::MenuItem *m_pPartsDeletePartMenuItem;
		Gtk::MenuItem *m_pPartsFilterSetsMenuItem;
		Gtk::MenuItem *m_pPartsUnfilterSetsMenuItem;
		int m_partsViewPriceColumnIndex;
		Gtk::CellRendererText *m_pPartsPriceCellRenderer;
		Gdk::RGBA m_missingValueCellForeColor;

	protected:
		SetsStore m_setsStore;
		Glib::RefPtr<Gtk::ListStore> m_pSetsStore;
	private:
		Gtk::TreeView *m_pSetsView;
		void SetsSetup();
		void on_sets_description_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_sets_started_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_sets_ended_edited(Glib::ustring pathStr, Glib::ustring text);
		void on_sets_button_pressed(GdkEventButton *pEvent);
		void on_setsNewSet_activated();
		void on_setsDeleteSet_activated();
		void on_setsViewManual_activated();
		void on_sets_setNumber_clicked();
		void on_sets_description_clicked();
		Gtk::Menu *m_pSetsContextMenu;
		Gtk::MenuItem *m_pSetsNewSetMenuItem;
		Gtk::MenuItem *m_pSetsDeleteSetMenuItem;
		Gtk::MenuItem *m_pSetsViewManualMenuItem;
		
	private:
		Gtk::ScrolledWindow *m_pToMakeScrolledWindow;
		Gtk::TreeView *m_pToMakeView;
		Gtk::CellRendererText *m_pToMakePriceCellRenderer;
		ToMakeStore m_toMakeStore;
		Glib::RefPtr<Gtk::ListStore> m_pToMakeStore;
		Gtk::Label *m_pToMakeCost;
		Gtk::ComboBox *m_pToMakeHaveComboBox;
		Gtk::ComboBox *m_pToMakeWantComboBox;
		Gtk::Menu *m_pToMakeContextMenu;
		Gtk::MenuItem *m_pToMakeViewPartMenuItem;
		double m_toMakeCost;
		int m_toMakeViewPriceColumnIndex;
		void ToMakeSetup();
		void on_toMake_price_column_drawn(Gtk::CellRenderer *r,const Gtk::TreeModel::iterator &iter);
		void on_toMakeComboBox_changed();
		void on_toMake_button_pressed(GdkEventButton *pEvent);
		void on_toMakeViewPart_activated();
		void on_toMake_partNumber_clicked();
		void on_toMake_description_clicked();
		void on_toMake_size_clicked();
		void on_toMake_notes_clicked();
		void RefreshToMake();
		void FillToMake();
		void CalculateToMakeTotals();
		
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
