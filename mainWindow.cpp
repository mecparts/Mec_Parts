/*
 * mainWindow.cpp
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

#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <assert.h>
#include <locale.h>
#include <boost/regex.hpp>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include "mainWindow.h"

#ifndef _Callback_Definitions
static int PopulateCurrenciesCallback(void *wnd, int argc, char **argv, char **azColName);
static int PopulatePricelistsCallback(void *wnd, int argc, char **argv, char **azColName);
static int PopulatePartsCallback(void *wnd, int argc, char **argv, char **azColName);
static int PopulateSetsCallback(void *wnd, int argc, char **argv, char **azColName);
static int PopulateCollectionCallback(void *wnd, int argc, char **argv, char **azColName);
static int AddSetPartCallback(void *wnd,int argc,char **argv, char **azColName);
static int PopulateToMakeCallback(void *wnd, int argc, char **argv, char **azColName);
#endif
using namespace std;

#ifndef _Constructor_Destructor
MainWindow::MainWindow() :
		m_pWindow(NULL),
		m_initialized(false),
		m_refBuilder(NULL),
		m_pSql(NULL),
		m_bSetsFiltered(false),
		m_pProgramQuitMenuItem(NULL),
		m_pAboutMenuItem(NULL),
		m_pCurrenciesView(NULL),
		m_pCurrenciesStore(NULL),
		m_localeCurrencyCode(""),
		m_baseCurrencyCode(""),
		m_baseCurrencyRate(0),
		m_pCurrenciesContextMenu(NULL),
		m_pCurrenciesUpdateCurrencyMenuItem(NULL),
		m_pCurrenciesNewCurrencyMenuItem(NULL),
		m_pCurrenciesDeleteCurrencyMenuItem(NULL),
		m_pPricelistsView(NULL),
		m_pPricelistsStore(NULL),
		m_pricelistNumber(0),
		m_pricelistCurrencyRate(0),
		m_pricelistCurrencyCode(""),
		m_pPricelistsContextMenu(NULL),
		m_pPricelistsImportPricelistMenuItem(NULL),
		m_pPricelistsNewPricelistMenuItem(NULL),
		m_pPricelistsDeletePricelistMenuItem(NULL),
		m_pPricelistImportCsvDialog(NULL),
		m_pCollectionView(NULL),
		m_pCollectionStore(NULL),
		m_pCollectionCountCost(NULL),
		m_pCollectionSetComboBox(NULL),
		m_pCollectionContextMenu(NULL),
		m_pCollectionViewPartMenuItem(NULL),
		m_pCollectionAddPartMenuItem(NULL),
		m_pCollectionDeletePartMenuItem(NULL),
		m_pCollectionAddSetMenuItem(NULL),
		m_collectionNumber(""),
		m_collectionDescription(""),
		m_collectionPartNumber(""),
		m_collectionPartDescription(""),
		m_collectionPartsCount(0),
		m_collectionCost(0),
		m_collectionViewPriceColumnIndex(0),
		m_pPartsView(NULL),
		m_pPartsStore(NULL),
		m_pPartsContextMenu(NULL),
		m_pPartsViewPartMenuItem(NULL),
		m_pPartsNewPartMenuItem(NULL),
		m_pPartsDeletePartMenuItem(NULL),
		m_pPartsFilterSetsMenuItem(NULL),
		m_pPartsUnfilterSetsMenuItem(NULL),
		m_partsPartNumber(""),
		m_partsPartDescription(""),
		m_partsViewPriceColumnIndex(0),
		m_pSetsView(NULL),
		m_pSetsStore(NULL),
		m_pSetsContextMenu(NULL),
		m_pSetsNewSetMenuItem(NULL),
		m_pSetsDeleteSetMenuItem(NULL),
		m_pToMakeView(NULL),
		m_pToMakeStore(NULL),
		m_pToMakeCost(NULL),
		m_pToMakeHaveComboBox(NULL),
		m_pToMakeWantComboBox(NULL),
		m_pToMakeContextMenu(NULL),
		m_pToMakeViewPartMenuItem(NULL),
		m_toMakePartsCount(0),
		m_toMakeCost(0),
		m_toMakeViewPriceColumnIndex(0),
		m_pSelectPartsDialog(NULL),
		m_pSelectSetDialog(NULL),
		m_pNewPartDialog(NULL),
		m_pNewSetDialog(NULL),
		m_pNewPricelistDialog(NULL),
		m_pNewCurrencyDialog(NULL)
{
	m_pSql = new Sql(this,"meccano.db");

	// Load the GtkBuilder file and instantiate its widgets
	m_refBuilder = Gtk::Builder::create();
	if( !m_refBuilder ) {
		throw "null m_refBuilder";
	}
	try {
		m_refBuilder->add_from_file("mecparts.ui");
	}
	catch( const Glib::FileError &ex ) {
		throw "FileError: "+ex.what();
	}
	catch(const  Glib::MarkupError &ex) {
    throw "MarkupError: "+ex.what();
  }
	catch( const Gtk::BuilderError &ex ) {
		throw "BuilderError: "+ex.what();
	}
	
	GET_WIDGET(m_refBuilder,"mainWindow",m_pWindow)
	m_pWindow->signal_delete_event().connect(sigc::mem_fun(*this,&MainWindow::on_delete_event));

	m_cfg.load_cfg();
	int w,h;
	m_cfg.get_mainWindow_size(w,h);
	m_pWindow->set_default_size(w,h);

	int x,y;
	m_cfg.get_mainWindow_pos(x,y);
	if( x != -1 && x != -1 ) {
		m_pWindow->move(x,y);
	}

	CurrenciesSetup();
	PricelistsSetup();
	PartsSetup();
	SetsSetup();
	CollectionSetup();
	ToMakeSetup();

	m_pSelectPartsDialog = new SelectPartsDialog(m_refBuilder,&m_cfg);
	if( !m_pSelectPartsDialog ) {
		throw "Could not get selectPartsDialog";
	}
		
	m_pSelectSetDialog = new SelectSetDialog(m_refBuilder,&m_cfg);
	if( !m_pSelectSetDialog ) {
		throw "Could not get selectSetDialog";
	}

	m_pNewPartDialog = new NewPartDialog(m_refBuilder);
	if( !m_pNewPartDialog ) {
		throw "Could not get newPartDialog";
	}

	m_initialized = true;

	m_pCollectionView->get_column(m_collectionViewPriceColumnIndex)->set_title(m_baseCurrencyCode);
	m_pPartsView->get_column(m_partsViewPriceColumnIndex)->set_title(m_baseCurrencyCode);
	m_pToMakeView->get_column(m_toMakeViewPriceColumnIndex)->set_title(m_baseCurrencyCode);

	if( m_pSetsStore->children().size() > 0 ) {
		m_pCollectionSetComboBox->set_active(0);
	}
}

MainWindow::~MainWindow()
{
	delete m_pWindow;
}
#endif
#ifndef _General_Routines
bool MainWindow::on_delete_event(GdkEventAny *e)
{
	int width,height,x,y;
	m_pWindow->get_size(width,height);
	m_cfg.set_mainWindow_size(width,height);
	m_pWindow->get_position(x,y);
	m_cfg.set_mainWindow_pos(x,y);
	m_cfg.save_cfg();
  return false;
}

void MainWindow::on_program_quit()
{
	Gtk::Main::quit();
}

void MainWindow::on_about()
{
	Gtk::AboutDialog *pAbout = NULL;
	GET_WIDGET(m_refBuilder,"aboutDialog",pAbout);
	if( pAbout->run() == Gtk::RESPONSE_OK ) {
		pAbout->hide();
	} else {
		pAbout->hide();
	}
}
#endif
#ifndef _Collection_Routines
void MainWindow::CollectionSetup()
{
  GET_WIDGET(m_refBuilder,"collectionView",m_pCollectionView)
  m_pCollectionView->signal_button_press_event().connect_notify(sigc::mem_fun(*this,&MainWindow::on_collection_button_pressed));

	GET_WIDGET(m_refBuilder,"collectionCountCost",m_pCollectionCountCost);
		
  GET_WIDGET(m_refBuilder,"collectionView",m_pCollectionView)
	
	GET_OBJECT(m_refBuilder,"collectionStore",m_pCollectionStore,ListStore);
		
	m_collectionViewPriceColumnIndex = m_pCollectionView->append_column_numeric("Price",m_collectionStore.m_price,"%.2lf")-1;
	m_pCollectionView->append_column_numeric("Total",m_collectionStore.m_total,"%.2lf");

	Gtk::CellRendererText *pCellRenderer;
	GET_TEXT_RENDERER("collectionCountCellRenderer",pCellRenderer,m_pCollectionView,m_collectionStore.m_count.index())
	pCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_collection_count_edited));

	GET_TEXT_RENDERER("collectionPriceCellRenderer",pCellRenderer,m_pCollectionView,m_collectionStore.m_price.index())
	string propertyName = "background-gdk"; Gdk::Color bkColour= Gdk::Color(string("grey88"));
	pCellRenderer->set_property(propertyName,bkColour);
		
	GET_TEXT_RENDERER("collectionTotalCellRenderer",pCellRenderer,m_pCollectionView,m_collectionStore.m_total.index())
	pCellRenderer->set_property(propertyName,bkColour);
	
	GET_WIDGET(m_refBuilder,"collectionSetComboBox",m_pCollectionSetComboBox);
	m_pCollectionSetComboBox->pack_start(m_setsStore.m_description);
	m_pCollectionSetComboBox->signal_changed().connect(sigc::mem_fun(*this,&MainWindow::on_collection_set_combobox_changed_event));

	GET_WIDGET(m_refBuilder,"collectionContextMenu",m_pCollectionContextMenu)
		
	GET_WIDGET(m_refBuilder,"collectionViewPartMenuItem",m_pCollectionViewPartMenuItem)
	m_pCollectionViewPartMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_collectionViewPart_activated_event));

	GET_WIDGET(m_refBuilder,"collectionAddPartMenuItem",m_pCollectionAddPartMenuItem)
	m_pCollectionAddPartMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_collectionAddPart_activated_event));

	GET_WIDGET(m_refBuilder,"collectionDeletePartMenuItem",m_pCollectionDeletePartMenuItem)
	m_pCollectionDeletePartMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_collectionDeletePart_activated_event));

	GET_WIDGET(m_refBuilder,"collectionAddSetMenuItem",m_pCollectionAddSetMenuItem)
	m_pCollectionAddSetMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_collectionAddSet_activated_event));

	m_pCollectionView->get_column(m_collectionStore.m_partNumber.index())->signal_clicked().connect_notify(sigc::mem_fun(*this,&MainWindow::on_collection_partNumber_clicked));
	m_pCollectionView->get_column(m_collectionStore.m_description.index())->signal_clicked().connect_notify(sigc::mem_fun(*this,&MainWindow::on_collection_description_clicked));
}

void MainWindow::on_collection_button_pressed(GdkEventButton *pEvent)
{
	if( pEvent->type==GDK_BUTTON_PRESS && pEvent->button==3 ) {
		Gtk::TreeModel::Path path;
		if( m_pCollectionView->get_path_at_pos((int)pEvent->x,(int)pEvent->y,path) ) {
			m_pCollectionView->set_cursor(path);
		}
		bool partSelected = m_pCollectionView->get_selection()->count_selected_rows() != 0;
		m_pCollectionViewPartMenuItem->set_sensitive(partSelected);
		m_pCollectionDeletePartMenuItem->set_sensitive(partSelected);
		m_collectionPartNumber = m_collectionPartDescription = "";
		Gtk::TreeModel::iterator iter = m_pCollectionView->get_selection()->get_selected();
		if( iter ) {
			Gtk::TreeModel::Row row = *iter;
			m_collectionPartNumber = row[m_collectionStore.m_partNumber];
			m_collectionPartDescription = row[m_collectionStore.m_description];
		}
		m_pCollectionContextMenu->popup(pEvent->button,pEvent->time);
	}
}

void MainWindow::on_collection_set_combobox_changed_event()
{
	m_pCollectionStore->clear();
	m_collectionPartsCount = 0;
	m_collectionCost = 0;
	m_pCollectionCountCost->set_text("");
  Gtk::TreeModel::iterator iter = m_pCollectionSetComboBox->get_active();
  if(iter) {
    Gtk::TreeModel::Row row = *iter;
    if(row) {
			m_collectionNumber = row[m_setsStore.m_setNumber];
			m_collectionDescription = row[m_setsStore.m_description];
			FillCollection();
		}
	}
}

void MainWindow::on_collectionViewPart_activated_event()
{
	Gtk::TreeModel::iterator iter = m_pCollectionView->get_selection()->get_selected();
	if( iter ) {
		Gtk::TreeModel::Row row = *iter;
		string partNumber = row[m_collectionStore.m_partNumber];
		string description = row[m_collectionStore.m_description];
		string size = row[m_collectionStore.m_size];
		DisplayPicture(partNumber,description,size);
	}
}

void MainWindow::on_collectionAddPart_activated_event()
{
	m_pSelectPartsDialog->UnselectAll();
	if( m_pSelectPartsDialog->m_pDialog->run() == Gtk::RESPONSE_OK ) {
		Glib::RefPtr<Gtk::TreeSelection> pSelected = m_pSelectPartsDialog->Selected();
		if( m_pSelectPartsDialog->SelectedCount() > 0 ) {
			pSelected->selected_foreach_iter(sigc::mem_fun(*this,&MainWindow::AddPartCallback));
		}
	}
	m_pSelectPartsDialog->m_pDialog->hide();
}

void MainWindow::AddPartCallback(const Gtk::TreeModel::iterator &iter)
{
	Gtk::TreeModel::Row partRow = *iter;
	AddPartToCollection(partRow,0,false);
}

void MainWindow::on_collectionDeletePart_activated_event()
{
	Gtk::TreeModel::iterator iter = m_pCollectionView->get_selection()->get_selected();
	if( iter ) {
		Gtk::MessageDialog areYouSure(*m_pWindow,"Are you sure?",false,Gtk::MESSAGE_QUESTION,Gtk::BUTTONS_OK_CANCEL);
		Gtk::TreeModel::Row row = *iter;
		string partNum = row[m_collectionStore.m_partNumber];
		string partDescription = row[m_collectionStore.m_description];
		areYouSure.set_secondary_text("Delete part # "+partNum+" ("+partDescription+") from collection \""+m_collectionDescription+"\"?");
		if( areYouSure.run() == Gtk::RESPONSE_OK ) {
			m_pCollectionStore->erase(iter);
			stringstream sql;
			sql
				<< "DELETE FROM set_parts WHERE part_num='" << m_pSql->Escape(partNum) 
				<< "' AND set_num='" << m_pSql->Escape(m_collectionNumber) << "'";
			m_pSql->ExecNonQuery(&sql);
			CalculateCollectionTotals();
			FillToMake0();
		}
	}
}

void MainWindow::on_collectionAddSet_activated_event()
{
	m_pSelectSetDialog->UnselectAll();
	if( m_pSelectSetDialog->m_pDialog->run() == Gtk::RESPONSE_OK ) {
		m_pSelectSetDialog->m_pDialog->hide();
		Glib::RefPtr<Gdk::Window> refWindow = m_pWindow->get_window();
		if( refWindow ) {
			Gdk::Cursor waitCursor(Gdk::WATCH);
			refWindow->set_cursor(waitCursor);
			Gdk::flush();
			while(Gtk::Main::events_pending()) {
				Gtk::Main::iteration();
			}
		}
		Glib::RefPtr<Gtk::TreeSelection> pSelected = m_pSelectSetDialog->Selected();
		if( m_pSelectSetDialog->SelectedCount() > 0 ) {
			pSelected->selected_foreach_iter(sigc::mem_fun(*this,&MainWindow::AddSetPartsCallback));
		}
		CalculateCollectionTotals();
		FillToMake0();
		if( refWindow ) {
			refWindow->set_cursor();
			Gdk::flush();
			while(Gtk::Main::events_pending()) {
				Gtk::Main::iteration();
			}
		}
	} else {
		m_pSelectSetDialog->m_pDialog->hide();
	}
}

void MainWindow::AddSetPartToPartsList(string partNumber,int count)
{
	m_partsList[partNumber] = count;
}

void MainWindow::AddSetPartsCallback(const Gtk::TreeModel::iterator &iter)
{
	Gtk::TreeModel::Row setRow = *iter;
	string setNumber = setRow[m_setsStore.m_setNumber];
	m_partsList.clear();
	stringstream sql;
	sql << "SELECT part_num,count FROM set_parts WHERE set_num='" << m_pSql->Escape(setNumber) << "'";
	m_pSql->ExecQuery(&sql,AddSetPartCallback);
	
	for( map<string,int>::iterator iter=m_partsList.begin(); iter!=m_partsList.end(); ++iter ) {
		string partNumber = (*iter).first;
		int count = (*iter).second;
		Gtk::TreeModel::Children partRows = m_pPartsStore->children();
		for( Gtk::TreeModel::iterator r = partRows.begin(); r!=partRows.end(); ++r ) {
			Gtk::TreeModel::Row partRow = *r;
			string partRowNumber = partRow[m_partsStore.m_partNumber];
			if( partRowNumber == partNumber ) {
				AddPartToCollection(partRow,count,true);
				break;
			}
		}
	}
}

void MainWindow::AddPartToCollection(Gtk::TreeModel::Row partRow,int count,bool updateExistingRow)
{
	gint64 partRowId = partRow[m_partsStore.m_rowId];
	string partRowPrefix = partRow[m_partsStore.m_pnPrefix];
	int partRowDigits = partRow[m_partsStore.m_pnDigits];
	string partRowSuffix = partRow[m_partsStore.m_pnSuffix];
	Gtk::TreeModel::Children rows = m_pCollectionStore->children();
	bool inserted = false;
	Gtk::TreeModel::Row newRow;
	for( Gtk::TreeModel::iterator r = rows.begin(); r != rows.end(); ++r ) {
		Gtk::TreeModel::Row row = *r;
		gint64 collectionRowId = row[m_collectionStore.m_rowId];
		if( partRowId == collectionRowId ) {
			if( updateExistingRow ) {
				row[m_collectionStore.m_count] = row[m_collectionStore.m_count]+count;
				row[m_collectionStore.m_total] = row[m_collectionStore.m_count]*row[m_collectionStore.m_price];
				stringstream sql;
				string partNumber = partRow[m_partsStore.m_partNumber];
				sql << "UPDATE set_parts SET count=count+" << count << " WHERE set_num='" << m_pSql->Escape(m_collectionNumber) << "' AND part_num='" << m_pSql->Escape(partNumber) << "'";
				m_pSql->ExecNonQuery(&sql);
			}
			return;
		} else {
			string collectionRowPrefix = row[m_collectionStore.m_pnPrefix];
			int collectionRowDigits = row[m_collectionStore.m_pnDigits];
			string collectionRowSuffix = row[m_collectionStore.m_pnSuffix];
			if( partRowPrefix < collectionRowPrefix || (partRowPrefix == collectionRowPrefix && partRowDigits < collectionRowDigits) || (partRowPrefix==collectionRowPrefix && partRowDigits==collectionRowDigits && partRowSuffix < collectionRowSuffix) ) {
				newRow = *(m_pCollectionStore->insert(r));
				inserted = true;
				break;
			}
		}
	}
	if( !inserted ) {
		newRow = *(m_pCollectionStore->append());
	}
	newRow[m_collectionStore.m_rowId] = partRowId;
	string temp;
	newRow[m_collectionStore.m_partNumber] = temp = partRow[m_partsStore.m_partNumber];
	newRow[m_collectionStore.m_description] = temp = partRow[m_partsStore.m_description];
	newRow[m_collectionStore.m_size] = temp = partRow[m_partsStore.m_size];
	newRow[m_collectionStore.m_count] = count;
	double price;
	newRow[m_collectionStore.m_price] = price = partRow[m_partsStore.m_price];
	newRow[m_collectionStore.m_total] = price * count;
	newRow[m_collectionStore.m_pnPrefix] = partRowPrefix;
	newRow[m_collectionStore.m_pnDigits] = partRowDigits;
	newRow[m_collectionStore.m_pnSuffix] = partRowSuffix;
	string partNumber = partRow[m_partsStore.m_partNumber];
	stringstream sql;
	sql 
		<< "INSERT INTO set_parts(set_num,part_num,count) VALUES ('" 
		<< m_pSql->Escape(m_collectionNumber) << "','" 
		<< m_pSql->Escape(partNumber) << "'," << count << ")";
	m_pSql->ExecNonQuery(&sql);
}

void MainWindow::FillCollection()
{
	if( m_pCollectionStore->children().size() > 0 ) {
		m_pCollectionStore->clear();
		m_collectionPartsCount = 0;
		m_collectionCost = 0;
		m_pCollectionCountCost->set_text("");
	}
	stringstream cmd;
	cmd 
		<< "SELECT p.rowid,p.num,p.description,p.size,sp.count,IFNULL(pp.price,0) price,"
		<< "IFNULL(pp.price,0)*sp.count total,p.pnPrefix a,p.pnDigits b,p.pnSuffix c "
		<< "FROM parts p JOIN set_parts sp ON p.num=sp.part_num AND sp.set_num='" << m_collectionNumber <<"' "
		<< "LEFT OUTER JOIN v_part_prices pp ON p.num=pp.part_num AND pp.pricelist_num=" << m_pricelistNumber
		<< " ORDER BY p.pnPrefix,p.pnDigits,p.pnSuffix";
	m_pSql->ExecQuery(&cmd, PopulateCollectionCallback);
	
	stringstream temp;
	temp << "# Parts: " << m_collectionPartsCount << "  Total Cost: " << setiosflags(ios::fixed) << setprecision(2) << m_collectionCost;
	m_pCollectionCountCost->set_text(temp.str());
}

void MainWindow::PopulateCollection(gint64 rowId,string partNumber,string description,string size,guint count,gdouble price,gdouble total,string pnPrefix,int pnDigits,string pnSuffix)
{
	Gtk::TreeModel::Row row = *(m_pCollectionStore->append());
	row[m_collectionStore.m_rowId] = rowId;
	row[m_collectionStore.m_partNumber] = partNumber;
	row[m_collectionStore.m_description] = description;
	row[m_collectionStore.m_size] = size;
	row[m_collectionStore.m_count] = count;
	row[m_collectionStore.m_price] = price;
	row[m_collectionStore.m_total] = total;
	row[m_collectionStore.m_pnPrefix] = pnPrefix;
	row[m_collectionStore.m_pnDigits] = pnDigits;
	row[m_collectionStore.m_pnSuffix] = pnSuffix;
	m_collectionPartsCount += count;
	m_collectionCost += total;
}

void MainWindow::on_collection_partNumber_clicked()
{
	m_pCollectionView->set_search_column(m_collectionStore.m_partNumber.index());
	m_pCollectionView->grab_focus();
}

void MainWindow::on_collection_description_clicked()
{
	m_pCollectionView->set_search_column(m_collectionStore.m_description.index());
	m_pCollectionView->grab_focus();
}

void MainWindow::on_collection_count_edited(Glib::ustring pathStr, Glib::ustring text)
{
	Gtk::TreeIter iter = m_pCollectionStore->get_iter(pathStr);
	if( !iter ) {
		throw "No iter for "+pathStr;
	}
	int count = atoi(text.c_str());
	double price = (*iter)[m_collectionStore.m_price];
	string num = (*iter)[m_collectionStore.m_partNumber];
	(*iter)[m_collectionStore.m_count] = count;
	(*iter)[m_collectionStore.m_total] = count * price;
	stringstream sql;
	sql
		<< "UPDATE set_parts SET count=" << count 
		<< " WHERE part_num='" << m_pSql->Escape(num) 
		<< "' AND set_num='" << m_pSql->Escape(m_collectionNumber) << "'";
	m_pSql->ExecNonQuery(&sql);
	CalculateCollectionTotals();
	FillToMake0();
}

void MainWindow::CalculateCollectionTotals()
{
	Gtk::TreeModel::Children rows = m_pCollectionStore->children();
	m_collectionPartsCount = 0;
	m_collectionCost = 0;
	for( Gtk::TreeModel::iterator r = rows.begin(); r != rows.end(); ++r ) {
		Gtk::TreeModel::Row row = *r;
		m_collectionPartsCount += row[m_collectionStore.m_count];
		m_collectionCost += row[m_collectionStore.m_total];
	}
	stringstream temp;
	temp << "# Parts: " << m_collectionPartsCount << "  Total Cost: " << setiosflags(ios::fixed) << setprecision(2) << m_collectionCost;
	m_pCollectionCountCost->set_text(temp.str());
}
#endif
#ifndef _Parts_Routines
void MainWindow::PartsSetup()
{
	GET_OBJECT(m_refBuilder,"partsStore",m_pPartsStore,ListStore)

	GET_WIDGET(m_refBuilder,"partsView",m_pPartsView)
	m_partsViewPriceColumnIndex = m_pPartsView->append_column_numeric_editable("Price",m_partsStore.m_price,"%.2lf")-1;
	m_pPartsView->append_column_editable("Notes",m_partsStore.m_notes);

	Gtk::CellRendererText *pCellRenderer;
	GET_TEXT_RENDERER("partsDescriptionCellRenderer",pCellRenderer,m_pPartsView,m_partsStore.m_description.index())
	pCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_parts_description_edited));

	GET_TEXT_RENDERER("partsSizeCellRenderer",pCellRenderer,m_pPartsView,m_partsStore.m_size.index())
	pCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_parts_size_edited));

	GET_TEXT_RENDERER("partsPriceCellRenderer",pCellRenderer,m_pPartsView,m_partsStore.m_price.index())
	pCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_parts_price_edited));

	GET_TEXT_RENDERER("partsNotesCellRenderer",pCellRenderer,m_pPartsView,m_partsStore.m_notes.index())
	pCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_parts_notes_edited));

  m_pPartsView->signal_button_press_event().connect_notify(sigc::mem_fun(*this,&MainWindow::on_parts_button_pressed));

	m_pPartsView->get_column(m_partsStore.m_partNumber.index())->signal_clicked().connect_notify(sigc::mem_fun(*this,&MainWindow::on_parts_partNumber_clicked));
	m_pPartsView->get_column(m_partsStore.m_description.index())->signal_clicked().connect_notify(sigc::mem_fun(*this,&MainWindow::on_parts_description_clicked));

	GET_WIDGET(m_refBuilder,"partsContextMenu",m_pPartsContextMenu)
	GET_WIDGET(m_refBuilder,"partsViewPartMenuItem",m_pPartsViewPartMenuItem)
	m_pPartsViewPartMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_partsViewPart_activated_event));

	GET_WIDGET(m_refBuilder,"partsNewPartMenuItem",m_pPartsNewPartMenuItem)
	m_pPartsNewPartMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_partsNewPart_activated_event));

	GET_WIDGET(m_refBuilder,"partsDeletePartMenuItem",m_pPartsDeletePartMenuItem)
	m_pPartsDeletePartMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_partsDeletePart_activated_event));

	GET_WIDGET(m_refBuilder,"partsFilterSetsMenuItem",m_pPartsFilterSetsMenuItem)
	m_pPartsFilterSetsMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_partsFilterSets_activated_event));

	GET_WIDGET(m_refBuilder,"partsUnfilterSetsMenuItem",m_pPartsUnfilterSetsMenuItem)
	m_pPartsUnfilterSetsMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_partsUnfilterSets_activated_event));

	FillParts();
}

void MainWindow::on_parts_partNumber_clicked()
{
	m_pPartsView->set_search_column(m_partsStore.m_partNumber.index());
	m_pPartsView->grab_focus();
}

void MainWindow::on_parts_description_clicked()
{
	m_pPartsView->set_search_column(m_partsStore.m_description.index());
	m_pPartsView->grab_focus();
}

void MainWindow::FillParts()
{
	if( m_pPartsStore->children().size() > 0 ) {
		m_pPartsStore->clear();
	}
	stringstream cmd;
	cmd 
		<< "SELECT p.rowid,p.num,p.description,p.size,IFNULL(pp.price,0) price,"
		<< "p.notes,p.pnPrefix a,p.pnDigits b,p.pnSuffix c "
		<< "FROM parts p LEFT OUTER JOIN v_part_prices pp ON p.num=pp.part_num AND pp.pricelist_num=" << m_pricelistNumber
		<< " ORDER BY p.pnPrefix,p.pnDigits,p.pnSuffix";
	m_pSql->ExecQuery(&cmd, PopulatePartsCallback);
}

void MainWindow::PopulateParts(gint64 rowId,string partNumber,string description,string size,gdouble price,string notes,string pnPrefix,int pnDigits,string pnSuffix)
{
	Gtk::TreeModel::Row row = *(m_pPartsStore->append());
	row[m_partsStore.m_rowId] = rowId;
	row[m_partsStore.m_partNumber] = partNumber;
	row[m_partsStore.m_description] = description;
	row[m_partsStore.m_size] = size;
	row[m_partsStore.m_price] = price;
	row[m_partsStore.m_notes] = notes;
	row[m_partsStore.m_pnPrefix] = pnPrefix;
	row[m_partsStore.m_pnDigits] = pnDigits;
	row[m_partsStore.m_pnSuffix] = pnSuffix;
}

#ifndef _Parts_Fields_Edited
void MainWindow::on_parts_description_edited(Glib::ustring pathStr, Glib::ustring text)
{
	Gtk::TreeIter iter = m_pPartsStore->get_iter(pathStr);
	if( !iter ) {
		throw "No iter for "+pathStr;
	}
	string num = (*iter)[m_partsStore.m_partNumber];
	(*iter)[m_partsStore.m_description] = text;
	stringstream sql;
	sql
		<< "UPDATE parts SET description='" << m_pSql->Escape(text)
		<< "' WHERE num='" << m_pSql->Escape(num) << "'";
	m_pSql->ExecNonQuery(&sql);
	FillCollection();
	FillToMake0();
}

void MainWindow::on_parts_size_edited(Glib::ustring pathStr, Glib::ustring text)
{
	Gtk::TreeIter iter = m_pPartsStore->get_iter(pathStr);
	if( !iter ) {
		throw "No iter for "+pathStr;
	}
	string num = (*iter)[m_partsStore.m_partNumber];
	(*iter)[m_partsStore.m_size] = text;
	stringstream sql;
	sql
		<< "UPDATE parts SET size='" << m_pSql->Escape(text)
		<< "' WHERE num='" << m_pSql->Escape(num) << "'";
	m_pSql->ExecNonQuery(&sql);
	FillCollection();
	FillToMake0();
}

void MainWindow::on_parts_price_edited(Glib::ustring pathStr, Glib::ustring text)
{
	Gtk::TreeIter iter = m_pPartsStore->get_iter(pathStr);
	if( !iter ) {
		throw "No iter for "+pathStr;
	}
	double price = atof(text.c_str());
	string num = (*iter)[m_collectionStore.m_partNumber];
	(*iter)[m_partsStore.m_price] = price;
	stringstream sql;
	sql
		<< "UPDATE part_prices SET price=" << price/m_baseCurrencyRate*m_pricelistCurrencyRate 
		<< " WHERE part_num='" << m_pSql->Escape(num) << "' AND pricelist_num=" << m_pricelistNumber;
	if( m_pSql->ExecUpdate(&sql) == 0 ) {
		sql.str("");
		sql
			<< "INSERT INTO part_prices(pricelist_num,part_num,price) VALUES ("
			<< m_pricelistNumber
			<< ",'" << m_pSql->Escape(num) << "',"
			<< price/m_baseCurrencyRate*m_pricelistCurrencyRate << ")";
		m_pSql->ExecInsert(&sql);
	}
	FillCollection();
	FillToMake0();
}

void MainWindow::on_parts_notes_edited(Glib::ustring pathStr, Glib::ustring text)
{
	Gtk::TreeIter iter = m_pPartsStore->get_iter(pathStr);
	if( !iter ) {
		throw "No iter for "+pathStr;
	}
	string num = (*iter)[m_partsStore.m_partNumber];
	(*iter)[m_partsStore.m_notes] = text;
	stringstream sql;
	sql
		<< "UPDATE parts SET notes='" << m_pSql->Escape(text)
		<< "' WHERE num='" << m_pSql->Escape(num) << "'";
	m_pSql->ExecNonQuery(&sql);
	FillCollection();
	FillToMake0();
}
#endif
void MainWindow::on_parts_button_pressed(GdkEventButton *pEvent)
{
	if( pEvent->type==GDK_BUTTON_PRESS && pEvent->button==3 ) {
		Gtk::TreeModel::Path path;
		m_pPartsViewPartMenuItem->set_sensitive(false);
		m_pPartsDeletePartMenuItem->set_sensitive(false);
		m_pPartsFilterSetsMenuItem->set_sensitive(false);
		m_pPartsUnfilterSetsMenuItem->set_sensitive(m_bSetsFiltered);
		m_partsPartNumber = m_partsPartDescription = "";
		if( m_pPartsView->get_path_at_pos((int)pEvent->x,(int)pEvent->y,path) ) {
			m_pPartsView->set_cursor(path);
			m_pPartsViewPartMenuItem->set_sensitive(true);
			m_pPartsDeletePartMenuItem->set_sensitive(true);
			m_pPartsFilterSetsMenuItem->set_sensitive(true);
			Gtk::TreeModel::iterator iter = m_pPartsView->get_selection()->get_selected();
			if( iter ) {
				Gtk::TreeModel::Row row = *iter;
				m_partsPartNumber = row[m_partsStore.m_partNumber];
				m_partsPartDescription = row[m_partsStore.m_description];
			}
		}
		m_pPartsContextMenu->popup(pEvent->button,pEvent->time);
	}
}

void MainWindow::on_partsViewPart_activated_event()
{
	Gtk::TreeModel::iterator iter = m_pPartsView->get_selection()->get_selected();
	if( iter ) {
		Gtk::TreeModel::Row row = *iter;
		string partNumber = row[m_partsStore.m_partNumber];
		string description = row[m_partsStore.m_description];
		string size = row[m_partsStore.m_size];
		DisplayPicture(partNumber,description,size);
	}
}

void MainWindow::DisplayPicture(string partNumber,string description,string size)
{
	Gtk::MessageDialog pictureDialog(*m_pWindow,partNumber,false,Gtk::MESSAGE_INFO,Gtk::BUTTONS_OK);
	pictureDialog.set_secondary_text(description+", "+size);
	Gtk::Image *pImage = NULL;
	string imageExtensions[] = {".jpg",".png",".gif",".bmp",".jpeg",".pcx",".tif",".tiff"};
	int numImageExtensions = sizeof(imageExtensions)/sizeof(string);
	bool foundFile = false;
	string fileName;
	for( int i=0; i<numImageExtensions; ++i ) {
		fileName = "Pictures/"+partNumber+imageExtensions[i];
		if( Glib::file_test(fileName,Glib::FILE_TEST_EXISTS) ) {
			pImage = new Gtk::Image(Gdk::Pixbuf::create_from_file(fileName, 300, 300, true));
			pictureDialog.set_image(*pImage);
			foundFile = true;
			break;
		}
	}
	if( !foundFile ) {
		pImage = new Gtk::Image(Gtk::Stock::MISSING_IMAGE,Gtk::ICON_SIZE_DIALOG);
		pictureDialog.set_image(*pImage);
	}
	pImage->show();
	pictureDialog.run();
	delete pImage;
}

void MainWindow::on_partsNewPart_activated_event()
{
	m_pNewPartDialog->ClearInput();
	while( true ) {
		if( m_pNewPartDialog->m_pDialog->run() == Gtk::RESPONSE_OK ) {
			boost::regex pnRegex("([A-Za-z]*+)([0-9]*+)([A-Za-z0-9]*+)");
			boost::smatch matches;
			string new_partNum = m_pNewPartDialog->PartNumber();
			if( !boost::regex_match(new_partNum,matches,pnRegex) || matches.size()!=4) {
				throw "Regex failed";
			}
			string new_pnPrefix = matches[1];
			int new_pnDigits = atoi(matches[2].str().c_str());
			string new_pnSuffix = matches[3];
//			cout << '[' << new_pnPrefix << "] [" << new_pnDigits << "] [" << new_pnSuffix << ']' << endl;
//			return;
			Gtk::TreeModel::Row newRow;
			bool alreadyExists = false;
			bool inserted = false;
			Gtk::TreeModel::Children partsRows = m_pPartsStore->children();
			for( Gtk::TreeModel::iterator iter = partsRows.begin(); iter!= partsRows.end(); ++iter ) {
				Gtk::TreeModel::Row partsRow = *iter;
				string partNum = partsRow[m_partsStore.m_partNumber];
				string pnPrefix = partsRow[m_partsStore.m_pnPrefix];
				int pnDigits = partsRow[m_partsStore.m_pnDigits];
				string pnSuffix = partsRow[m_partsStore.m_pnSuffix];
				if( new_partNum == partNum ) {
					m_pNewPartDialog->ErrorLabel("Part number "+m_pNewPartDialog->PartNumber()+" already exists!");
					alreadyExists = true;
					break;
				} else if( new_pnPrefix < pnPrefix || (new_pnPrefix == pnPrefix && new_pnDigits < pnDigits) || (new_pnPrefix==pnPrefix && new_pnDigits==pnDigits && new_pnSuffix <= pnSuffix) ) {
					newRow = *(m_pPartsStore->insert(iter));
					inserted = true;
					break;
				}
			}
			if( !alreadyExists ) {
				m_pNewPartDialog->ErrorLabel("");
				if( !inserted ) {
					newRow = *(m_pPartsStore->append());
				}
				stringstream sql;
				sql <<
					"INSERT INTO parts(pnPrefix,pnDigits,pnSuffix,num,description,size,notes) VALUES ("
					<< "'" << m_pSql->Escape(new_pnPrefix) << "',"
					<< new_pnDigits << ","
					<< "'" << m_pSql->Escape(new_pnSuffix) << "'," 
					<< "'" << new_partNum << "',"
					<< "'" << m_pNewPartDialog->Description() << "',"
					<< "'" << m_pNewPartDialog->Size() << "',"
					<< "'" << m_pNewPartDialog->Notes() << "')";
				newRow[m_partsStore.m_rowId] = m_pSql->ExecInsert(&sql);
				newRow[m_partsStore.m_partNumber] = new_partNum;
				newRow[m_partsStore.m_pnPrefix] = new_pnPrefix;
				newRow[m_partsStore.m_pnDigits] = new_pnDigits;
				newRow[m_partsStore.m_pnSuffix] = new_pnSuffix;
				newRow[m_partsStore.m_description] = m_pNewPartDialog->Description();
				newRow[m_partsStore.m_size] = m_pNewPartDialog->Size();
				newRow[m_partsStore.m_notes] = m_pNewPartDialog->Notes();
				newRow[m_partsStore.m_price] = m_pNewPartDialog->Price();
				sql.str("");
				sql
					<< "INSERT INTO part_prices(pricelist_num,part_num,price) VALUES ("
					<< m_pricelistNumber << ","
					<< "'" << m_pSql->Escape(new_partNum) << "',"
					<< m_pNewPartDialog->Price()/m_baseCurrencyRate*m_pricelistCurrencyRate << ")";
				m_pSql->ExecInsert(&sql);
				break;
			}
		} else {
			break;
		}
	}
	m_pNewPartDialog->m_pDialog->hide();
}

void MainWindow::on_partsDeletePart_activated_event()
{
	Gtk::TreeModel::iterator iter = m_pPartsView->get_selection()->get_selected();
	if( iter ) {
		Gtk::MessageDialog areYouSure(*m_pWindow,"Are you sure?",false,Gtk::MESSAGE_QUESTION,Gtk::BUTTONS_OK_CANCEL);
		Gtk::TreeModel::Row row = *iter;
		string partNum = row[m_partsStore.m_partNumber];
		string partDescription = row[m_partsStore.m_description];
		areYouSure.set_secondary_text("Delete part # "+partNum+" ("+partDescription+")? This will remove the part from all collections.");
		if( areYouSure.run() == Gtk::RESPONSE_OK ) {
			m_pPartsStore->erase(iter);
			stringstream sql;
			sql 
				<< "DELETE FROM set_parts WHERE part_num='" << m_pSql->Escape(partNum) << "'; "
				<< "DELETE FROM part_prices WHERE part_num='" << m_pSql->Escape(partNum) << "'; "
				<< "DELETE FROM parts WHERE num='" << m_pSql->Escape(partNum) << "'";
			m_pSql->ExecNonQuery(&sql);
			FillCollection();
			FillToMake0();
		}
	}
}

void MainWindow::on_partsFilterSets_activated_event()
{
	Gtk::TreeModel::iterator iter = m_pPartsView->get_selection()->get_selected();
	if( iter ) {
		m_bSetsFiltered = true;
		Gtk::TreeModel::Row row = *iter;
		string partNumber = row[m_partsStore.m_partNumber];
		stringstream sql;
		sql << "select s.num,s.description,ifnull(s.started,9999),ifnull(s.ended,9999),s.rowid FROM sets s,set_parts sp WHERE s.num=sp.set_num AND sp.part_num='" << partNumber << "' ORDER BY s.started DESC,s.description";
		m_pSetsStore->clear();
		m_pSql->ExecQuery(&sql,PopulateSetsCallback);
	}
}

void MainWindow::on_partsUnfilterSets_activated_event()
{
	m_bSetsFiltered = false;
	m_pSetsStore->clear();
	const char *cmd = 
		"select num,description,ifnull(started,9999),ifnull(ended,9999),rowid FROM sets ORDER BY started DESC,description";
	m_pSql->ExecQuery(cmd,PopulateSetsCallback);
}
#endif
#ifndef _ToMake_Routines
void MainWindow::ToMakeSetup()
{
  GET_WIDGET(m_refBuilder,"toMakeView",m_pToMakeView)
   m_pToMakeView->signal_button_press_event().connect_notify(sigc::mem_fun(*this,&MainWindow::on_toMake_button_pressed));

	GET_WIDGET(m_refBuilder,"toMakeCost",m_pToMakeCost);
	
	GET_OBJECT(m_refBuilder,"toMakeStore",m_pToMakeStore,ListStore)
		
	m_toMakeViewPriceColumnIndex = m_pToMakeView->append_column_numeric("Price",m_toMakeStore.m_price,"%.2lf")-1;
	m_pToMakeView->append_column_numeric("Total",m_toMakeStore.m_total,"%.2lf");

	Gtk::CellRendererText *pCellRenderer = NULL;
	GET_TEXT_RENDERER("toMakePriceCellRenderer",pCellRenderer,m_pToMakeView,m_toMakeStore.m_price.index())
	string propertyName = "background-gdk"; Gdk::Color bkColour= Gdk::Color(string("grey88"));
	pCellRenderer->set_property(propertyName,bkColour);

	GET_TEXT_RENDERER("toMakeTotalCellRenderer",pCellRenderer,m_pToMakeView,m_toMakeStore.m_total.index())
	pCellRenderer->set_property(propertyName,bkColour);
	
	GET_WIDGET(m_refBuilder,"toMakeHaveComboBox",m_pToMakeHaveComboBox);
	m_pToMakeHaveComboBox->pack_start(m_setsStore.m_description);
	m_pToMakeHaveComboBox->signal_changed().connect(sigc::mem_fun(*this,&MainWindow::FillToMake0));

	GET_WIDGET(m_refBuilder,"toMakeWantComboBox",m_pToMakeWantComboBox);
	m_pToMakeWantComboBox->pack_start(m_setsStore.m_description);
	m_pToMakeWantComboBox->signal_changed().connect(sigc::mem_fun(*this,&MainWindow::FillToMake0));

	GET_WIDGET(m_refBuilder,"toMakeContextMenu",m_pToMakeContextMenu)
		
	GET_WIDGET(m_refBuilder,"toMakeViewPartMenuItem",m_pToMakeViewPartMenuItem)
	m_pToMakeViewPartMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_toMakeViewPart_activated_event));

	m_pToMakeView->get_column(m_toMakeStore.m_partNumber.index())->signal_clicked().connect_notify(sigc::mem_fun(*this,&MainWindow::on_toMake_partNumber_clicked));
	m_pToMakeView->get_column(m_toMakeStore.m_description.index())->signal_clicked().connect_notify(sigc::mem_fun(*this,&MainWindow::on_toMake_description_clicked));
}

void MainWindow::on_toMake_button_pressed(GdkEventButton *pEvent)
{
	if( pEvent->type==GDK_BUTTON_PRESS && pEvent->button==3 ) {
		Gtk::TreeModel::Path path;
		if( m_pToMakeView->get_path_at_pos((int)pEvent->x,(int)pEvent->y,path) ) {
			m_pToMakeView->set_cursor(path);
			m_pToMakeContextMenu->popup(pEvent->button,pEvent->time);
		}
	}
}

void MainWindow::on_toMakeViewPart_activated_event()
{
	Gtk::TreeModel::iterator iter = m_pToMakeView->get_selection()->get_selected();
	if( iter ) {
		Gtk::TreeModel::Row row = *iter;
		string partNumber = row[m_toMakeStore.m_partNumber];
		string description = row[m_toMakeStore.m_description];
		string size = row[m_toMakeStore.m_size];
		DisplayPicture(partNumber,description,size);
	}
}

void MainWindow::on_toMake_partNumber_clicked()
{
	m_pToMakeView->set_search_column(m_toMakeStore.m_partNumber.index());
	m_pToMakeView->grab_focus();
}

void MainWindow::on_toMake_description_clicked()
{
	m_pToMakeView->set_search_column(m_toMakeStore.m_description.index());
	m_pToMakeView->grab_focus();
}


void MainWindow::FillToMake0()
{
	m_pToMakeStore->clear();
	m_toMakePartsCount = 0;
	m_toMakeCost = 0;
	m_pToMakeCost->set_text("");
  Gtk::TreeModel::iterator iterHave = m_pToMakeHaveComboBox->get_active();
	Gtk::TreeModel::iterator iterWant = m_pToMakeWantComboBox->get_active();
  if(iterHave && iterWant)
  {
    Gtk::TreeModel::Row haveRow = *iterHave;
    Gtk::TreeModel::Row wantRow = *iterWant;
    if(haveRow && wantRow)
    {
			string haveNum = haveRow[m_setsStore.m_setNumber];
			string wantNum = wantRow[m_setsStore.m_setNumber];
			FillToMake(haveNum,wantNum);
		}
	}
}

void MainWindow::FillToMake(string haveNum,string wantNum)
{
	if( m_pToMakeStore->children().size() > 0 ) {
		m_pToMakeStore->clear();
		m_toMakePartsCount = 0;
		m_toMakeCost = 0;
		m_pToMakeCost->set_text("");
	}
	stringstream cmd;
	cmd
	  << "DROP VIEW IF EXISTS v_set_parts; CREATE TEMP VIEW v_set_parts AS SELECT "
	  << "s.num set_num,s.description set_description,p.num part_num,p.description part_description,p.size,"
	  << "sp.count,IFNULL(pp.price,0) price,sp.count*IFNULL(pp.price,0) total " 
		<< "FROM sets s INNER join set_parts sp ON s.num=sp.set_num JOIN parts p ON p.num=sp.part_num "
		<< "LEFT OUTER JOIN v_part_prices pp ON p.num=pp.part_num AND pp.pricelist_num=" << m_pricelistNumber
		<< " ORDER BY p.pnPrefix,p.pnDigits,p.pnSuffix; "
		<< "DROP TABLE IF EXISTS have; DROP TABLE IF EXISTS want; "
		<< "CREATE TEMP TABLE have AS SELECT * FROM v_set_parts WHERE set_num='" << m_pSql->Escape(haveNum) << "'; "
		<< "CREATE TEMP TABLE want AS SELECT * FROM v_set_parts WHERE set_num='" << m_pSql->Escape(wantNum) << "'; "
		<< "SELECT w.part_num,w.part_description,w.size,w.price,w.count-IFNULL(h.count,0),w.price*(w.count-IFNULL(h.count,0)) "
		<< "FROM want w LEFT OUTER JOIN have h ON w.part_num=h.part_num WHERE w.count-IFNULL(h.count,0)>0";
	m_pSql->ExecQuery(&cmd, PopulateToMakeCallback);
	
	stringstream temp;
	temp << setiosflags(ios::fixed) << setprecision(2) << m_toMakeCost;
	m_pToMakeCost->set_text(temp.str());
}

void MainWindow::PopulateToMake(string partNumber,string description,string size,double price,int count,double total)
{
	Gtk::TreeModel::Row row = *(m_pToMakeStore->append());
	row[m_toMakeStore.m_partNumber] = partNumber;
	row[m_toMakeStore.m_description] = description;
	row[m_toMakeStore.m_size] = size;
	row[m_toMakeStore.m_count] = count;
	row[m_toMakeStore.m_price] = price;
	row[m_toMakeStore.m_total] = total;
	m_toMakePartsCount += count;
	m_toMakeCost += total;
}

#endif
#ifndef _Sets_Routines
void MainWindow::SetsSetup()
{
	GET_OBJECT(m_refBuilder,"setsStore",m_pSetsStore,ListStore)

  GET_WIDGET(m_refBuilder,"setsView",m_pSetsView)
	m_pSetsView->append_column_numeric_editable("Started",m_setsStore.m_started,"%d");
	m_pSetsView->append_column_numeric_editable("Ended",m_setsStore.m_ended,"%d");
		
	Gtk::CellRendererText *pCellRenderer;
	GET_TEXT_RENDERER("setsDescriptionCellRenderer",pCellRenderer,m_pSetsView,m_setsStore.m_description.index())
	pCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_sets_description_edited));

	GET_TEXT_RENDERER("setsStartedCellRenderer",pCellRenderer,m_pSetsView,m_setsStore.m_started.index())
	pCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_sets_started_edited));

	GET_TEXT_RENDERER("setsEndedCellRenderer",pCellRenderer,m_pSetsView,m_setsStore.m_ended.index())
	pCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_sets_ended_edited));

	m_pSetsView->signal_button_press_event().connect_notify(sigc::mem_fun(*this,&MainWindow::on_sets_button_pressed));

	GET_WIDGET(m_refBuilder,"setsContextMenu",m_pSetsContextMenu)
	GET_WIDGET(m_refBuilder,"setsNewSetMenuItem",m_pSetsNewSetMenuItem)
	m_pSetsNewSetMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_setsNewSet_activated_event));

	GET_WIDGET(m_refBuilder,"setsDeleteSetMenuItem",m_pSetsDeleteSetMenuItem)
	m_pSetsDeleteSetMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_setsDeleteSet_activated_event));

	m_pSetsView->get_column(m_setsStore.m_setNumber.index())->signal_clicked().connect_notify(sigc::mem_fun(*this,&MainWindow::on_sets_setNumber_clicked));
	m_pSetsView->get_column(m_setsStore.m_description.index())->signal_clicked().connect_notify(sigc::mem_fun(*this,&MainWindow::on_sets_description_clicked));

	m_pNewSetDialog = new NewSetDialog(m_refBuilder);
	if( !m_pNewSetDialog ) {
		throw "Could not get newSetDialog";
	}

	const char *cmd = 
		"select num,description,ifnull(started,9999),ifnull(ended,9999),rowid FROM sets ORDER BY started DESC,description";
	m_pSql->ExecQuery(cmd,PopulateSetsCallback);
}

void MainWindow::PopulateSets(string setNumber,string description,int started,int ended,gint64 rowid)
{
	Gtk::TreeModel::Row row = *(m_pSetsStore->append());
	row[m_setsStore.m_rowId] = rowid;
	row[m_setsStore.m_setNumber] = setNumber;
	row[m_setsStore.m_description] = description;
	row[m_setsStore.m_started] = started;
	row[m_setsStore.m_ended] = ended;
}
#ifndef _Sets_Fields_Edited
void MainWindow::on_sets_description_edited(Glib::ustring pathStr, Glib::ustring text)
{
	Gtk::TreeIter iter = m_pSetsStore->get_iter(pathStr);
	if( !iter ) {
		throw "No iter for "+pathStr;
	}
	string num = (*iter)[m_setsStore.m_setNumber];
	(*iter)[m_setsStore.m_description] = text;
	stringstream sql;
	sql
		<< "UPDATE sets SET description='" << m_pSql->Escape(text)
		<< "' WHERE num='" << m_pSql->Escape(num) << "'";
	m_pSql->ExecNonQuery(&sql);
}

void MainWindow::on_sets_started_edited(Glib::ustring pathStr, Glib::ustring text)
{
	Gtk::TreeIter iter = m_pSetsStore->get_iter(pathStr);
	if( !iter ) {
		throw "No iter for "+pathStr;
	}
	string num = (*iter)[m_setsStore.m_setNumber];
	(*iter)[m_setsStore.m_started] = atoi(text.c_str());
	stringstream sql;
	sql
		<< "UPDATE sets SET started=" << atoi(text.c_str())
		<< " WHERE num='" << m_pSql->Escape(num) << "'";
	m_pSql->ExecNonQuery(&sql);
}

void MainWindow::on_sets_ended_edited(Glib::ustring pathStr, Glib::ustring text)
{
	Gtk::TreeIter iter = m_pSetsStore->get_iter(pathStr);
	if( !iter ) {
		throw "No iter for "+pathStr;
	}
	string num = (*iter)[m_setsStore.m_setNumber];
	(*iter)[m_setsStore.m_ended] = atoi(text.c_str());
	stringstream sql;
	sql
		<< "UPDATE sets SET ended=" << atoi(text.c_str())
		<< " WHERE num='" << m_pSql->Escape(num) << "'";
	m_pSql->ExecNonQuery(&sql);
}
#endif

void MainWindow::on_sets_button_pressed(GdkEventButton *pEvent)
{
	if( pEvent->type==GDK_BUTTON_PRESS && pEvent->button==3 ) {
		Gtk::TreeModel::Path path;
		if( m_pSetsView->get_path_at_pos((int)pEvent->x,(int)pEvent->y,path) ) {
			m_pSetsView->set_cursor(path);
		}
		m_pSetsDeleteSetMenuItem->set_sensitive(m_pSetsView->get_selection()->count_selected_rows() != 0);
		m_pSetsContextMenu->popup(pEvent->button,pEvent->time);
	}
}

void MainWindow::on_setsNewSet_activated_event()
{
	m_pNewSetDialog->ClearInput();
	while( true ) {
		if( m_pNewSetDialog->m_pDialog->run() == Gtk::RESPONSE_OK ) {
			string new_setNum = m_pNewSetDialog->SetNumber();
			string new_description = m_pNewSetDialog->Description();
			int new_started = m_pNewSetDialog->Started();
			Gtk::TreeModel::Row newRow;
			bool alreadyExists = false;
			bool inserted = false;
			Gtk::TreeModel::Children setsRows = m_pSetsStore->children();
			for( Gtk::TreeModel::iterator iter = setsRows.begin(); iter!= setsRows.end(); ++iter ) {
				Gtk::TreeModel::Row setsRow = *iter;
				string setNum = setsRow[m_setsStore.m_setNumber];
				string description = setsRow[m_setsStore.m_description];
				int started = setsRow[m_setsStore.m_started];
				if( new_setNum == setNum ) {
					m_pNewSetDialog->ErrorLabel("Set number "+new_setNum+" already exists!");
					alreadyExists = true;
					break;
				} else if( new_started > started || (new_started == started && new_description < description) ) {
					newRow = *(m_pSetsStore->insert(iter));
					inserted = true;
					break;
				}
			}
			if( !alreadyExists ) {
				m_pNewSetDialog->ErrorLabel("");
				if( !inserted ) {
					newRow = *(m_pSetsStore->append());
				}
				stringstream sql;
				sql <<
					"INSERT INTO sets(num,description,started,ended) VALUES " <<
					"('" << m_pSql->Escape(new_setNum) << "','" <<
					m_pSql->Escape(m_pNewSetDialog->Description()) << "'," <<
					m_pNewSetDialog->Started() << "," <<
					m_pNewSetDialog->Ended() << ")";
				newRow[m_setsStore.m_rowId] = m_pSql->ExecInsert(&sql);
				newRow[m_setsStore.m_setNumber] = new_setNum;
				newRow[m_setsStore.m_description] = m_pNewSetDialog->Description();
				newRow[m_setsStore.m_started] = m_pNewSetDialog->Started();
				newRow[m_setsStore.m_ended] = m_pNewSetDialog->Ended();
				break;
			}
		} else {
			break;
		}
	}
	m_pNewSetDialog->m_pDialog->hide();
}

void MainWindow::on_setsDeleteSet_activated_event()
{
	Gtk::TreeModel::iterator iter = m_pSetsView->get_selection()->get_selected();
	if( iter ) {
		Gtk::MessageDialog areYouSure(*m_pWindow,"Are you sure?",false,Gtk::MESSAGE_QUESTION,Gtk::BUTTONS_OK_CANCEL);
		Gtk::TreeModel::Row row = *iter;
		string setNum = row[m_setsStore.m_setNumber];
		string setDescription = row[m_setsStore.m_description];
		areYouSure.set_secondary_text("Delete set # "+setNum+" ("+setDescription+")?");
		if( areYouSure.run() == Gtk::RESPONSE_OK ) {
			m_pSetsStore->erase(iter);
			stringstream sql;
			sql << "DELETE FROM set_parts WHERE set_num='" << m_pSql->Escape(setNum) << "'; DELETE FROM sets WHERE num='" << m_pSql->Escape(setNum) << "'";
			m_pSql->ExecNonQuery(&sql);
		}
	}
}

void MainWindow::on_sets_setNumber_clicked()
{
	m_pSetsView->set_search_column(m_setsStore.m_setNumber.index());
	m_pSetsView->grab_focus();
}

void MainWindow::on_sets_description_clicked()
{
	m_pSetsView->set_search_column(m_setsStore.m_description.index());
	m_pSetsView->grab_focus();
}

#endif
#ifndef _Pricelists_Routines
void MainWindow::PricelistsSetup()
{
	GET_OBJECT(m_refBuilder,"pricelistsStore",m_pPricelistsStore,ListStore)
	const char *cmd = 
		"SELECT pl.num,pl.description,c.name,c.code,c.rate FROM pricelists pl, currencies c WHERE pl.currency_code=c.code ORDER BY num";
	m_pSql->ExecQuery(cmd,PopulatePricelistsCallback);

	GET_WIDGET(m_refBuilder,"pricelistsView",m_pPricelistsView)
	
	Gtk::CellRendererToggle *pCellRenderer;
	GET_TOGGLE_RENDERER("pricelistsUseCellRendererToggle",pCellRenderer,m_pPricelistsView,m_pricelistsStore.m_use.index())
	pCellRenderer->signal_toggled().connect(sigc::mem_fun(*this,&MainWindow::on_pricelist_use_toggled));

	Gtk::CellRendererText *pTextCellRenderer = NULL;
	GET_TEXT_RENDERER("priceslistsDescriptionCellRendererText",pTextCellRenderer,m_pPricelistsView,m_pricelistsStore.m_description.index())
	pTextCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_pricelists_description_edited));

	GET_WIDGET(m_refBuilder,"pricelistsContextMenu",m_pPricelistsContextMenu)

	GET_WIDGET(m_refBuilder,"pricelistsImportPricelistMenuItem",m_pPricelistsImportPricelistMenuItem)
	m_pPricelistsImportPricelistMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_pricelistsImportPricelist_activated_event));

	GET_WIDGET(m_refBuilder,"pricelistsNewPricelistMenuItem",m_pPricelistsNewPricelistMenuItem)
	m_pPricelistsNewPricelistMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_pricelistsNewPricelist_activated_event));

	GET_WIDGET(m_refBuilder,"pricelistsDeletePricelistMenuItem",m_pPricelistsDeletePricelistMenuItem)
	m_pPricelistsDeletePricelistMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_pricelistsDeletePricelist_activated_event));

  m_pPricelistsView->signal_button_press_event().connect_notify(sigc::mem_fun(*this,&MainWindow::on_pricelists_button_pressed));

	m_pNewPricelistDialog = new NewPricelistDialog(m_refBuilder);
	if( !m_pNewPricelistDialog ) {
		throw "Could not get newPricelistDialog";
	}

	GET_WIDGET(m_refBuilder,"importFileChooserDialog",m_pPricelistImportCsvDialog);
	Gtk::FileFilter filter_csv;
	filter_csv.set_name("CSV files");
	filter_csv.add_mime_type("text/csv");
	m_pPricelistImportCsvDialog->add_filter(filter_csv);
	Gtk::FileFilter filter_any;
	filter_any.set_name("All files");
	filter_any.add_pattern("*");
	m_pPricelistImportCsvDialog->add_filter(filter_any);

	if( m_pPricelistsStore->children().size() > 0 ) {
		Gtk::TreeModel::Children rows = m_pPricelistsStore->children();
		Gtk::TreeModel::iterator r = rows.begin();
		(*r)[m_pricelistsStore.m_use] = true;
		m_pricelistNumber = (*r)[m_pricelistsStore.m_num];
		m_pricelistCurrencyRate = (*r)[m_pricelistsStore.m_currencyRate];
		m_pricelistCurrencyCode = (*r)[m_pricelistsStore.m_currencyCode];
	} else {
		throw "Oh no! No priceslists in database";
	}
	RefreshPriceLists();
}

void MainWindow::PopulatePricelists(gint64 num,string description,string currencyName,string code,double rate)
{
	Gtk::TreeModel::Row row = *(m_pPricelistsStore->append());
	row[m_pricelistsStore.m_use] = false;
	row[m_pricelistsStore.m_num] = num;
	row[m_pricelistsStore.m_description] = description;
	row[m_pricelistsStore.m_currencyName] = currencyName;
	row[m_pricelistsStore.m_currencyCode] = code;
	row[m_pricelistsStore.m_currencyRate] = rate;
}

void MainWindow::on_pricelists_description_edited(Glib::ustring pathStr, Glib::ustring text)
{
	Gtk::TreeIter iter = m_pPricelistsStore->get_iter(pathStr);
	if( !iter ) {
		throw "No iter for "+pathStr;
	}
	gint64 num = (*iter)[m_pricelistsStore.m_num];
	(*iter)[m_pricelistsStore.m_description] = text;
	stringstream sql;
	sql
		<< "UPDATE pricelists SET description='" << m_pSql->Escape(text)
		<< "' WHERE num=" << num;
	m_pSql->ExecNonQuery(&sql);
}

void MainWindow::on_pricelist_use_toggled(const Glib::ustring &pathStr)
{
	Gtk::TreeIter iter = m_pPricelistsStore->get_iter(pathStr);
	if( !iter ) {
		throw "No iter for "+pathStr;
	}
	m_pricelistNumber = (*iter)[m_pricelistsStore.m_num];
	m_pricelistCurrencyRate = (*iter)[m_pricelistsStore.m_currencyRate];
	m_pricelistCurrencyCode = (*iter)[m_pricelistsStore.m_currencyCode];
	Gtk::TreeModel::Children rows = m_pPricelistsStore->children();
	for( Gtk::TreeModel::iterator r = rows.begin(); r != rows.end(); ++r ) {
		Gtk::TreeModel::Row row = *r;
		gint64 pricelistNum = row[m_pricelistsStore.m_num];
		row[m_pricelistsStore.m_use] = (m_pricelistNumber == pricelistNum);
	}
	RefreshPriceLists();
}

void MainWindow::RefreshPriceLists()
{
	stringstream sql;
	sql
	 << "DROP VIEW IF EXISTS v_part_prices;CREATE VIEW v_part_prices AS "
	 << "SELECT pp.pricelist_num,pp.part_num,round(pp.price/c.rate*"
	 << m_baseCurrencyRate << ",2) price "
	 << "FROM part_prices pp,pricelists pl,currencies c WHERE "
	 << "pp.pricelist_num=pl.num AND pl.currency_code=c.code";
	m_pSql->ExecNonQuery(&sql);
	if( m_initialized ) {
		FillParts();
		FillCollection();
		FillToMake0();
	}
}

void MainWindow::on_pricelists_button_pressed(GdkEventButton *pEvent)
{
	if( pEvent->type==GDK_BUTTON_PRESS && pEvent->button==3 ) {
		Gtk::TreeModel::Path path;
		if( m_pPricelistsView->get_path_at_pos((int)pEvent->x,(int)pEvent->y,path) ) {
			m_pPricelistsView->set_cursor(path);
		}
		// only activate the delete pricelists function if there is a
		// selected pricelist and it's not the active one
		//
		// not allowing the active pricelist to be deleted means that
		// we will always have at least one pricelist available (assuming
		// the database starts off with one) and we never have to worry
		// about finding another pricelist when the active one was
		// deleted
		m_pPricelistsDeletePricelistMenuItem->set_sensitive(false);
		if( m_pPricelistsView->get_selection()->count_selected_rows() != 0 ) {
			Gtk::TreeModel::iterator iter = m_pPricelistsView->get_selection()->get_selected();
			gint64 selectedPricelistNum = (*iter)[m_pricelistsStore.m_num];
			m_pPricelistsDeletePricelistMenuItem->set_sensitive(selectedPricelistNum != m_pricelistNumber);
		}
		m_pPricelistsContextMenu->popup(pEvent->button,pEvent->time);
	}
}

void MainWindow::on_pricelistsImportPricelist_activated_event()
{
	int response = m_pPricelistImportCsvDialog->run();
	m_pPricelistImportCsvDialog->hide();
	if( response == Gtk::RESPONSE_OK ) {
		if( !m_pPricelistImportCsvDialog->get_filename().empty() ) {
			
		}
	}
}

void MainWindow::on_pricelistsNewPricelist_activated_event()
{
	m_pNewPricelistDialog->ClearInput();
	while( true ) {
		if( m_pNewPricelistDialog->m_pDialog->run() == Gtk::RESPONSE_OK ) {
			string new_description = m_pNewPricelistDialog->Description();
			Gtk::TreeModel::Row newRow;
			bool alreadyExists = false;
			bool inserted = false;
			Gtk::TreeModel::Children pricelistsRows = m_pPricelistsStore->children();
			for( Gtk::TreeModel::iterator iter = pricelistsRows.begin(); iter!= pricelistsRows.end(); ++iter ) {
				Gtk::TreeModel::Row pricelistsRow = *iter;
				string description = pricelistsRow[m_pricelistsStore.m_description];
				if( new_description == description ) {
					m_pNewSetDialog->ErrorLabel("Pricelist "+new_description+" already exists!");
					alreadyExists = true;
					break;
				} else if( new_description < description ) {
					newRow = *(m_pPricelistsStore->insert(iter));
					inserted = true;
					break;
				}
			}
			if( !alreadyExists ) {
				m_pNewPricelistDialog->ErrorLabel("");
				if( !inserted ) {
					newRow = *(m_pPricelistsStore->append());
				}
				stringstream sql;
				sql <<
					"INSERT INTO pricelists(description,currency_code) VALUES " <<
					"('" << m_pSql->Escape(new_description) << "','" <<
					m_pSql->Escape(m_pNewPricelistDialog->CurrencyCode()) << "')";
				newRow[m_pricelistsStore.m_num] = m_pSql->ExecInsert(&sql);
				newRow[m_pricelistsStore.m_use] = false;
				newRow[m_pricelistsStore.m_description] = new_description;
				newRow[m_pricelistsStore.m_currencyCode] = m_pNewPricelistDialog->CurrencyCode();
				newRow[m_pricelistsStore.m_currencyName] = m_pNewPricelistDialog->CurrencyName();
				newRow[m_pricelistsStore.m_currencyRate] = m_pNewPricelistDialog->CurrencyRate();
				break;
			}
		} else {
			break;
		}
	}
	m_pNewPricelistDialog->m_pDialog->hide();
}

void MainWindow::on_pricelistsDeletePricelist_activated_event()
{
	Gtk::TreeModel::iterator iter = m_pPricelistsView->get_selection()->get_selected();
	if( iter ) {
		Gtk::MessageDialog areYouSure(*m_pWindow,"Are you sure?",false,Gtk::MESSAGE_QUESTION,Gtk::BUTTONS_OK_CANCEL);
		Gtk::TreeModel::Row row = *iter;
		gint64 num = row[m_pricelistsStore.m_num];
		string description = row[m_pricelistsStore.m_description];
		areYouSure.set_secondary_text("Delete pricelist '"+description+"'?");
		if( areYouSure.run() == Gtk::RESPONSE_OK ) {
			m_pPricelistsStore->erase(iter);
			stringstream sql;
			sql << "DELETE FROM part_prices WHERE pricelist_num=" << num << "; DELETE FROM pricelists WHERE num=" << num;
			m_pSql->ExecNonQuery(&sql);
		}
	}
}

#endif
#ifndef _Currencies_Routines
void MainWindow::CurrenciesSetup()
{
	struct lconv *locale;
	locale=localeconv();
	string currSymbol = locale->int_curr_symbol;
	m_localeCurrencyCode = currSymbol.substr(0,3);

	GET_OBJECT(m_refBuilder,"currenciesStore",m_pCurrenciesStore,ListStore)
	const char *cmd =
		"SELECT code,name,rate FROM currencies ORDER BY code";
	m_pSql->ExecQuery(cmd,PopulateCurrenciesCallback);

	GET_WIDGET(m_refBuilder,"currenciesView",m_pCurrenciesView)
	int rateIndex = m_pCurrenciesView->append_column_numeric_editable("Rate",m_currenciesStore.m_rate,"%g")-1;

	GET_WIDGET(m_refBuilder,"currenciesContextMenu",m_pCurrenciesContextMenu)

	GET_WIDGET(m_refBuilder,"currenciesUpdateCurrencyMenuItem",m_pCurrenciesUpdateCurrencyMenuItem)
	m_pCurrenciesUpdateCurrencyMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_currenciesUpdateCurrency_activated_event));

	GET_WIDGET(m_refBuilder,"currenciesNewCurrencyMenuItem",m_pCurrenciesNewCurrencyMenuItem)
	m_pCurrenciesNewCurrencyMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_currenciesNewCurrency_activated_event));

	GET_WIDGET(m_refBuilder,"currenciesDeleteCurrencyMenuItem",m_pCurrenciesDeleteCurrencyMenuItem)
	m_pCurrenciesDeleteCurrencyMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_currenciesDeleteCurrency_activated_event));

	Gtk::CellRendererToggle *pToggleCellRenderer = NULL;
	GET_TOGGLE_RENDERER("currenciesUseCellRendererToggle",pToggleCellRenderer,m_pCurrenciesView,m_currenciesStore.m_use.index())
	pToggleCellRenderer->signal_toggled().connect(sigc::mem_fun(*this,&MainWindow::on_currency_use_toggled));

  m_pCurrenciesView->signal_button_press_event().connect_notify(sigc::mem_fun(*this,&MainWindow::on_currencies_button_pressed));

	Gtk::CellRendererText *pTextCellRenderer = NULL;
	GET_TEXT_RENDERER("currenciesNameCellRendererText",pTextCellRenderer,m_pCurrenciesView,m_currenciesStore.m_name.index())
	pTextCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_currencies_name_edited));

	GET_TEXT_RENDERER("currenciesRateCellRendererText",pTextCellRenderer,m_pCurrenciesView,rateIndex)
	pTextCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_currencies_rate_edited));

	m_pNewCurrencyDialog = new NewCurrencyDialog(m_refBuilder);
	if( !m_pNewCurrencyDialog ) {
		throw "Could not get newCurrencyDialog";
	}

	if( m_baseCurrencyCode.empty() ) {
		if( m_pCurrenciesStore->children().size() == 0 ) {
			throw "Oh no! No currencies in database!";
		}
		// no locale currency matched, just use the first one in the list
		Gtk::TreeModel::Children rows = m_pCurrenciesStore->children();
		Gtk::TreeModel::iterator r = rows.begin();
		m_baseCurrencyCode = (*r)[m_currenciesStore.m_code];
		m_baseCurrencyRate = (*r)[m_currenciesStore.m_rate];
	}
}

void MainWindow::on_currency_use_toggled(const Glib::ustring &pathStr)
{
	Gtk::TreeIter iter = m_pCurrenciesStore->get_iter(pathStr);
	if( !iter ) {
		throw "No iter for "+pathStr;
	}
	m_baseCurrencyCode = (*iter)[m_currenciesStore.m_code];
	m_baseCurrencyRate = (*iter)[m_currenciesStore.m_rate];
	m_pCollectionView->get_column(m_collectionViewPriceColumnIndex)->set_title(m_baseCurrencyCode);
	m_pPartsView->get_column(m_partsViewPriceColumnIndex)->set_title(m_baseCurrencyCode);
	m_pToMakeView->get_column(m_toMakeViewPriceColumnIndex)->set_title(m_baseCurrencyCode);
	
	Gtk::TreeModel::Children rows = m_pCurrenciesStore->children();
	for( Gtk::TreeModel::iterator r = rows.begin(); r != rows.end(); ++r ) {
		Gtk::TreeModel::Row row = *r;
		string currCode = row[m_currenciesStore.m_code];
		row[m_currenciesStore.m_use] = (m_baseCurrencyCode == currCode);
	}
	RefreshPriceLists();
}

void MainWindow::on_currencies_button_pressed(GdkEventButton *pEvent)
{
	if( pEvent->type==GDK_BUTTON_PRESS && pEvent->button==3 ) {
		Gtk::TreeModel::Path path;
		if( m_pCurrenciesView->get_path_at_pos((int)pEvent->x,(int)pEvent->y,path) ) {
			m_pCurrenciesView->set_cursor(path);
		}
		// only activate the delete currency function if there is a
		// selected currency and it's not the active one
		//
		// not allowing the active currency to be deleted means that
		// we will always have at least one currency available (assuming
		// the database starts off with one) and we never have to worry
		// about finding another currency when the active one was
		// deleted
		m_pCurrenciesDeleteCurrencyMenuItem->set_sensitive(false);
		if( m_pCurrenciesView->get_selection()->count_selected_rows() != 0 ) {
			Gtk::TreeModel::iterator iter = m_pCurrenciesView->get_selection()->get_selected();
			string selectedCurrencyCode = (*iter)[m_currenciesStore.m_code];
			m_pCurrenciesDeleteCurrencyMenuItem->set_sensitive(selectedCurrencyCode != m_baseCurrencyCode  && selectedCurrencyCode != m_pricelistCurrencyCode);
		}
		m_pCurrenciesContextMenu->popup(pEvent->button,pEvent->time);
	}
}

void MainWindow::PopulateCurrencies(string code,string name,double rate)
{
	Gtk::TreeModel::Row row = *(m_pCurrenciesStore->append());
	row[m_currenciesStore.m_use] = (code==m_localeCurrencyCode);
	row[m_currenciesStore.m_code] = code;
	row[m_currenciesStore.m_name] = name;
	row[m_currenciesStore.m_rate] = rate;
	if( code==m_localeCurrencyCode ) {
		row[m_currenciesStore.m_use] = true;
		m_baseCurrencyCode = code;
		m_baseCurrencyRate = rate;
	} else {
		row[m_currenciesStore.m_use] = false;
	}
}

void MainWindow::on_currenciesUpdateCurrency_activated_event()
{

	string destFilename = Glib::build_filename(Glib::build_filename(Glib::get_user_cache_dir(),Glib::get_prgname()),"eurofxref-daily.xml");
	string destDir = Glib::path_get_dirname(destFilename);
	g_mkdir_with_parents(destDir.c_str(), 0755);
	stringstream cmd;
	cmd << "wget -O \"" << destFilename << "\" http://www.ecb.europa.eu/stats/eurofxref/eurofxref-daily.xml";
	if( system(cmd.str().c_str()) == EXIT_SUCCESS ) {
		xmlDocPtr document;
		xmlXPathContextPtr xpath_ctx;
		xmlXPathObjectPtr xpath_obj;
		int i, len;

		xmlInitParser();
		document = xmlReadFile(destFilename.c_str(), NULL, 0);
		if (document == NULL) {
			cerr << "Couldn't parse data file" << endl;
			return;
		}

		xpath_ctx = xmlXPathNewContext(document);
		if (xpath_ctx == NULL) {
			xmlFreeDoc(document);
			cerr << "Couldn't create XPath context" << endl;
			return;
		}

		xmlXPathRegisterNs(xpath_ctx,BAD_CAST("xref"),BAD_CAST("http://www.ecb.int/vocabulary/2002-08-01/eurofxref"));
		xpath_obj = xmlXPathEvalExpression(BAD_CAST("//xref:Cube[@currency][@rate]"),xpath_ctx);

		if (xpath_obj == NULL) {
			xmlXPathFreeContext(xpath_ctx);
			xmlFreeDoc(document);
			cerr << "Couldn't create XPath object" << endl;
			return;
		}

		len = (xpath_obj->nodesetval) ? xpath_obj->nodesetval->nodeNr : 0;
		for (i = 0; i < len; i++) {
			if (xpath_obj->nodesetval->nodeTab[i]->type == XML_ELEMENT_NODE) {
				xmlAttrPtr attribute;
				xmlNodePtr node = xpath_obj->nodesetval->nodeTab[i];
				string currencyCode = "";
				double currencyRate = -1;
				for (attribute = node->properties; attribute; attribute = attribute->next) {
					if (strcmp((char *)attribute->name, "currency") == 0) {
						currencyCode = (char *)xmlNodeGetContent((xmlNodePtr) attribute);
					} else if (strcmp ((char *)attribute->name, "rate") == 0) {
						char *val = (char *)xmlNodeGetContent ((xmlNodePtr) attribute);
						currencyRate = atof(val);
						xmlFree (val);
					}
				}
				if( !currencyCode.empty() && currencyRate > 0 ) {
					stringstream sql;
					sql	<< "UPDATE currencies SET rate=" << currencyRate << " WHERE code='" << m_pSql->Escape(currencyCode) << "'";
					if( m_pSql->ExecUpdate(&sql) > 0 ) {
						Gtk::TreeModel::Children currencyRows = m_pCurrenciesStore->children();
						for( Gtk::TreeModel::iterator r = currencyRows.begin(); r!=currencyRows.end(); ++r ) {
							Gtk::TreeModel::Row currencyRow = *r;
							string lookupCode = currencyRow[m_currenciesStore.m_code];
							if( lookupCode == currencyCode ) {
								currencyRow[m_currenciesStore.m_rate] = currencyRate;
								if( currencyCode == m_baseCurrencyCode ) {
									m_baseCurrencyRate = currencyRate;
								}
								break;
							}
						}
					}
				}
			}
			// Avoid accessing removed elements
			if (xpath_obj->nodesetval->nodeTab[i]->type != XML_NAMESPACE_DECL) {
				xpath_obj->nodesetval->nodeTab[i] = NULL;
			}
		}

		xmlXPathFreeObject(xpath_obj);
		xmlXPathFreeContext(xpath_ctx);
		xmlFreeDoc(document);
		xmlCleanupParser();
		RefreshPriceLists();
	}
}

void MainWindow::on_currenciesNewCurrency_activated_event()
{
	m_pNewCurrencyDialog->ClearInput();
	while( true ) {
		if( m_pNewCurrencyDialog->m_pDialog->run() == Gtk::RESPONSE_OK ) {
			string new_description = m_pNewCurrencyDialog->Description();
			string new_currency = m_pNewCurrencyDialog->CurrencyCode();
			double new_rate = m_pNewCurrencyDialog->CurrencyRate();
			Gtk::TreeModel::Row newRow;
			bool alreadyExists = false;
			bool inserted = false;
			Gtk::TreeModel::Children currenciesRows = m_pCurrenciesStore->children();
			for( Gtk::TreeModel::iterator iter = currenciesRows.begin(); iter!= currenciesRows.end(); ++iter ) {
				Gtk::TreeModel::Row currenciesRow = *iter;
				string description = currenciesRow[m_currenciesStore.m_name];
				if( new_description == description ) {
					m_pNewSetDialog->ErrorLabel("Currency "+new_description+" already exists!");
					alreadyExists = true;
					break;
				} else if( new_description < description ) {
					newRow = *(m_pCurrenciesStore->insert(iter));
					inserted = true;
					break;
				}
			}
			if( !alreadyExists ) {
				m_pNewCurrencyDialog->ErrorLabel("");
				if( !inserted ) {
					newRow = *(m_pCurrenciesStore->append());
				}
				stringstream sql;
				sql
					<< "INSERT INTO currencies(name,code,rate) VALUES "
					<< "('" << m_pSql->Escape(new_description) << "','"
					<< m_pSql->Escape(new_currency) << "',"
					<< new_rate << ")";
				m_pSql->ExecInsert(&sql);
				newRow[m_currenciesStore.m_use] = false;
				newRow[m_currenciesStore.m_name] = new_description;
				newRow[m_currenciesStore.m_code] = new_currency;
				newRow[m_currenciesStore.m_rate] = new_rate;
				break;
			}
		} else {
			break;
		}
	}
	m_pNewCurrencyDialog->m_pDialog->hide();
}

void MainWindow::on_currenciesDeleteCurrency_activated_event()
{
	Gtk::TreeModel::iterator iter = m_pCurrenciesView->get_selection()->get_selected();
	if( iter ) {
		Gtk::MessageDialog areYouSure(*m_pWindow,"Are you sure?",false,Gtk::MESSAGE_QUESTION,Gtk::BUTTONS_OK_CANCEL);
		Gtk::TreeModel::Row row = *iter;
		string code = row[m_currenciesStore.m_code];
		string description = row[m_currenciesStore.m_name];
		areYouSure.set_secondary_text("Delete '"+description+"' currency? This will delete all pricelists that use this currency.");
		if( areYouSure.run() == Gtk::RESPONSE_OK ) {
			m_pCurrenciesStore->erase(iter);
			Gtk::TreeModel::Children rows = m_pPricelistsStore->children();
			for( Gtk::TreeModel::iterator iter2 = rows.begin(); iter2 != rows.end(); ) {
				Gtk::TreeModel::Row row2 = *iter2;
				string pricelistCurrencyCode = row2[m_pricelistsStore.m_currencyCode];
				if( pricelistCurrencyCode == code ) {
					iter2 = m_pPricelistsStore->erase(iter2);
				} else {
					++iter2;
				}
			}
			stringstream sql;
			sql << "PRAGMA foreign_keys = on; DELETE FROM currencies WHERE code='" << code << "'";
			m_pSql->ExecNonQuery(&sql);
			RefreshPriceLists();
		}
	}
}

void MainWindow::on_currencies_name_edited(Glib::ustring pathStr, Glib::ustring text)
{
	Gtk::TreeIter iter = m_pCurrenciesStore->get_iter(pathStr);
	if( !iter ) {
		throw "No iter for "+pathStr;
	}
	string code = (*iter)[m_currenciesStore.m_code];
	(*iter)[m_currenciesStore.m_name] = text;
	stringstream sql;
	sql
		<< "UPDATE currencies SET name='" << m_pSql->Escape(text)
		<< "' WHERE code='" << m_pSql->Escape(code) << "'";
	m_pSql->ExecNonQuery(&sql);
	RefreshPriceLists();
}

void MainWindow::on_currencies_rate_edited(Glib::ustring pathStr, Glib::ustring text)
{
	Gtk::TreeIter iter = m_pCurrenciesStore->get_iter(pathStr);
	if( !iter ) {
		throw "No iter for "+pathStr;
	}
	double rate = atof(text.c_str());
	string code = (*iter)[m_currenciesStore.m_code];
	(*iter)[m_currenciesStore.m_rate] = rate;
	// if we changed the rate of the base currency, update it
	if( m_baseCurrencyCode == code ) {
		m_baseCurrencyRate = rate;
	}
	stringstream sql;
	sql
		<< "UPDATE currencies SET rate=" << rate 
		<< " WHERE code='" << m_pSql->Escape(code) << "'";
	m_pSql->ExecUpdate(&sql);
	// if the current pricelist uses the currency code we just changed
	// the exchange rate on, update the rate in the store and in our
	// member variable
	if( code == m_pricelistCurrencyCode ) {
		Gtk::TreeModel::Children rows = m_pPricelistsStore->children();
		for( Gtk::TreeModel::iterator r = rows.begin(); r != rows.end(); ++r ) {
			Gtk::TreeModel::Row row = *r;
			gint64 pricelistNum = row[m_pricelistsStore.m_num];
			if( m_pricelistNumber == pricelistNum ) {
				m_pricelistCurrencyRate = rate;
				row[m_pricelistsStore.m_currencyRate] = rate;
				break;
			}
		}
	}
	RefreshPriceLists();
}
#endif
#ifndef _Static_Callbacks
static int PopulateCurrenciesCallback(void *wnd, int argc, char **argv, char **azColName)
{
	MainWindow *window = (MainWindow *)wnd;
	window->PopulateCurrencies(
		argv[0],				// code
		argv[1],				// name
		atof(argv[2]));	// rate
	return 0;
}

static int PopulatePricelistsCallback(void *wnd, int argc, char **argv, char **azColName)
{
	MainWindow *window = (MainWindow *)wnd;
	window->PopulatePricelists(
		atol(argv[0]),	// num
		argv[1],				// description
		argv[2],				// currency name
		argv[3],				// currency code
		atof(argv[4]));	// currency rate
	return 0;
}

static int PopulateToMakeCallback(void *wnd, int argc, char **argv, char **azColName)
{
	MainWindow *window = (MainWindow *)wnd;
	window->PopulateToMake(
		argv[0],	// w.part_num
		argv[1],	// w.part_description
		argv[2],	// w.size
		atof(argv[3]),	// w.price
		atoi(argv[4]),	// w.count-IFNULL(h.count,0)
		atof(argv[5]));	// w.price*(w.count-IFNULL(h.count,0))
	return 0;
}

// This is the callback function to display the select data in the table 
static int PopulatePartsCallback(void *wnd, int argc, char **argv, char **azColName)
{
	MainWindow *window = (MainWindow *)wnd;
	window->PopulateParts(atol(argv[0]),argv[1],argv[2],argv[3],atof(argv[4]),argv[5],argv[6],atoi(argv[7]),argv[8]);
	return 0;
}

// This is the callback function to display the select data in the table 
static int PopulateSetsCallback(void *wnd, int argc, char **argv, char **azColName)
{
	MainWindow *window = (MainWindow *)wnd;
	window->PopulateSets(argv[0],argv[1],atoi(argv[2]),atoi(argv[3]),atol(argv[4]));
	return 0;
}

// This is the callback function to display the select data in the table 
static int PopulateCollectionCallback(void *wnd, int argc, char **argv, char **azColName)
{
	MainWindow *window = (MainWindow *)wnd;
	window->PopulateCollection(atol(argv[0]),argv[1],argv[2],argv[3],atoi(argv[4]),atof(argv[5]),atof(argv[6]),argv[7],atoi(argv[8]),argv[9]);
	return 0;
}

static int AddSetPartCallback(void *wnd,int argc,char **argv, char **azColName)
{
	MainWindow *window = (MainWindow *)wnd;
	window->AddSetPartToPartsList(argv[0],atoi(argv[1]));
	return 0;
}
#endif
