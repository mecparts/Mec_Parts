/*
 * mainWindow.cpp: the beating heart of MecParts (a Meccano parts inventory
 * program). You can define parts, sets, prices, currencies, quantities, and
 * even pictures. One feature I've found to be really handy is the ability
 * to say "if I have this set of parts and want to build this model, what
 * parts do I need that I don't have?"
 * 
 * Copyright 2012 Wayne Hortensius <whortens@shaw.ca>
 * 
 * BTW: I'm using #ifndef _region_name to ape .NET's #region directive.
 * Geany's (the lightweight IDE I used for development) code folding
 * handles it nicely and allows me to concentrate on just the section of
 * code that I'm working on.
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
#include <time.h>
#include <boost/regex.hpp>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include "mainWindow.h"
#include "csvReader.h"

using namespace std;

#ifndef _Constructor_Destructor
MainWindow::MainWindow()
{
	m_pWindow = NULL;
	m_initialized = false;
	m_pSql= NULL;
	m_bSetsFiltered = false;
	m_pCurrenciesView = NULL;
	m_localeCurrencyCode = "";
	m_baseCurrencyCode = "";
	m_baseCurrencyRate = 0;
	m_pCurrenciesContextMenu = NULL;
	m_pCurrenciesUpdateCurrencyMenuItem = NULL;
	m_pCurrenciesNewCurrencyMenuItem = NULL;
	m_pCurrenciesDeleteCurrencyMenuItem = NULL;
	m_pPricelistsView = NULL;
	m_pricelistNumber = 0;
	m_pricelistCurrencyRate = 0;
	m_pricelistCurrencyCode = "";
	m_pPricelistsContextMenu = NULL;
	m_pPricelistsImportPricesMenuItem = NULL;
	m_pPricelistsNewPricelistMenuItem = NULL;
	m_pPricelistsDeletePricelistMenuItem = NULL;
	m_pPricelistImportCsvDialog = NULL;
	m_pImportPricesResultDialog = NULL;
	m_pCollectionView = NULL;
	m_pCollectionCountCost = NULL;
	m_pCollectionSetComboBox = NULL;
	m_pCollectionContextMenu = NULL;
	m_pCollectionViewPartMenuItem = NULL;
	m_pCollectionAddPartMenuItem = NULL;
	m_pCollectionDeletePartMenuItem = NULL;
	m_pCollectionAddSetMenuItem = NULL;
	m_collectionNumber = "";
	m_collectionDescription = "";
	m_collectionPartsCount = 0;
	m_collectionCost = 0;
	m_collectionViewPriceColumnIndex = 0;
	m_pPartsView = NULL;
	m_pPartsContextMenu = NULL;
	m_pPartsViewPartMenuItem = NULL;
	m_pPartsNewPartMenuItem = NULL;
	m_pPartsDeletePartMenuItem = NULL;
	m_pPartsFilterSetsMenuItem = NULL;
	m_pPartsUnfilterSetsMenuItem = NULL;
	m_partsViewPriceColumnIndex = 0;
	m_pSetsView = NULL;
	m_pSetsContextMenu = NULL;
	m_pSetsNewSetMenuItem = NULL;
	m_pSetsDeleteSetMenuItem = NULL;
	m_pToMakeView = NULL;
	m_pToMakeCost = NULL;
	m_pToMakeHaveComboBox = NULL;
	m_pToMakeWantComboBox = NULL;
	m_pToMakeContextMenu = NULL;
	m_pToMakeViewPartMenuItem = NULL;
	m_toMakePartsCount = 0;
	m_toMakeCost = 0;
	m_toMakeViewPriceColumnIndex = 0;
	m_pSelectPartsDialog = NULL;
	m_pSelectSetDialog = NULL;
	m_pNewPartDialog = NULL;
	m_pNewSetDialog = NULL;
	m_pNewPricelistDialog = NULL;
	m_pNewCurrencyDialog = NULL;

	// hook up to the database
	m_pSql = new Sql(this,"Database/meccano.db");

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
	
	// grab the mainWindow widget and hook up the window delete handler
	GET_WIDGET(m_refBuilder,"mainWindow",m_pWindow)
	m_pWindow->signal_delete_event().connect(sigc::mem_fun(*this,&MainWindow::on_delete_event));

	// load the window size and position from the last run of the program
	m_cfg.load_cfg();
	int w,h;
	m_cfg.get_mainWindow_size(w,h);
	m_pWindow->set_default_size(w,h);

	int x,y;
	m_cfg.get_mainWindow_pos(x,y);
	if( x != -1 && x != -1 ) {
		m_pWindow->move(x,y);
	}

	// set up the contents of each tab in the main window
	CurrenciesSetup();
	PricelistsSetup();
	PartsSetup();
	SetsSetup();
	CollectionSetup();
	ToMakeSetup();

	// let all refresh/refill routines know that from now on they should run
	// when called
	m_initialized = true;
}

MainWindow::~MainWindow()
{
	delete m_pWindow;
}
#endif
#ifndef _General_Routines
bool MainWindow::on_delete_event(GdkEventAny *e)
{
	// save the main window position and size for the next program run
	int width,height,x,y;
	m_pWindow->get_size(width,height);
	m_cfg.set_mainWindow_size(width,height);
	m_pWindow->get_position(x,y);
	m_cfg.set_mainWindow_pos(x,y);
	m_cfg.save_cfg();
	return false;
}

// turn the wait cursor on or off
void MainWindow::WaitCursor(bool on)
{
	Glib::RefPtr<Gdk::Window> refWindow = m_pWindow->get_window();
	if( refWindow ) {
		if( on ) {
		Glib::RefPtr<Gdk::Cursor> waitCursor = Gdk::Cursor::create(Gdk::WATCH);
		refWindow->set_cursor(waitCursor);
		} else {
			refWindow->set_cursor();
		}
		Gdk::flush();
		while(Gtk::Main::events_pending()) {
			Gtk::Main::iteration();
		}
	}
}

// Display a part. Look in the Pictures subdirectory (relative to where the executable is
// running from). Look for an image file named for the part with any of the following
// extensions: jpg, gif, png, bmp, jpeg, pxc, tif or tiff.
//
// If we find an image, it's scaled to fit into a 300x300 square.
void MainWindow::DisplayPicture(string partNumber,string description,string size,string notes)
{
	static string imageExtensions[] = {".jpg",".gif",".png",".bmp",".jpeg",".pcx",".tif",".tiff"};
	static int numImageExtensions = sizeof(imageExtensions)/sizeof(string);

	Gtk::MessageDialog pictureDialog(*m_pWindow,partNumber,false,Gtk::MESSAGE_INFO,Gtk::BUTTONS_OK);
	pictureDialog.set_secondary_text(description+", "+size+"\n"+notes);
	Gtk::Image *pImage = NULL;
	bool foundFile = false;
	for( int i=0; i<numImageExtensions; ++i ) {
		string fileName = "Pictures/"+partNumber+imageExtensions[i];
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
#endif
#ifndef _Collection_Routines
// Collection tab setup: the Collection tab shows the contents of a set (ie a
// list of parts and the # of each part in the set). You can think of a collection
// as 'set parts' if it makes more sense to you. I just needed a name that
// differeniated a 'set' from the 'colleciton of parts in a set'.
//
// Possible future enhancements: printing the collection or exporting it to
// a csv file.
void MainWindow::CollectionSetup()
{
	// set up a context menu on the collection view that pops up when the right mouse button is pressed
	// this will be used to add parts, add sets or delete parts to/from the collection
	GET_WIDGET(m_refBuilder,"collectionView",m_pCollectionView)
	m_pCollectionView->signal_button_press_event().connect_notify(sigc::mem_fun(*this,&MainWindow::on_collection_button_pressed));

	// total value of the parts in the collection
	GET_WIDGET(m_refBuilder,"collectionCountCost",m_pCollectionCountCost);

	// the backing data store for the collection tree view
	GET_OBJECT(m_refBuilder,"collectionStore",m_pCollectionStore,ListStore);
	
	// add a price column to the view. Glade doesn't have a nicely formatted
	// and editable numeric column so we add it manually.
	m_collectionViewPriceColumnIndex = m_pCollectionView->append_column_numeric("Price",m_collectionStore.m_price,"%.2lf")-1;
	m_pCollectionView->append_column_numeric("Total",m_collectionStore.m_total,"%.2lf");

	// the number of each part present in the collection (editable)
	Gtk::CellRendererText *pCellRenderer;
	GET_TEXT_RENDERER("collectionCountCellRenderer",pCellRenderer,m_pCollectionView,m_collectionStore.m_count.index())
	pCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_collection_count_edited));

	// the price of an individual part (non-editable in the collection view)
	GET_TEXT_RENDERER("collectionPriceCellRenderer",pCellRenderer,m_pCollectionView,m_collectionStore.m_price.index())
	string propertyName = "background-gdk"; 
  Gdk::Color bkColour= Gdk::Color(string("grey88"));
	pCellRenderer->set_property(propertyName,bkColour);

	// the 'total value' of each part in the collection (cost of an individual
	// part timex the count of this part in the collection)
	GET_TEXT_RENDERER("collectionTotalCellRenderer",pCellRenderer,m_pCollectionView,m_collectionStore.m_total.index())
	pCellRenderer->set_property(propertyName,bkColour);
	
	// the collection combobox selector. Use the set description out of the
	// sets data store in the drop down list and hook up an event on the
	// selection changed event to list the newly chosen collection.
	GET_WIDGET(m_refBuilder,"collectionSetComboBox",m_pCollectionSetComboBox);
	m_pCollectionSetComboBox->pack_start(m_setsStore.m_description);
	m_pCollectionSetComboBox->signal_changed().connect(sigc::mem_fun(*this,&MainWindow::on_collection_set_combobox_changed_event));

	// the view part/add part/add set/delete part popup menu for the collection
	GET_WIDGET(m_refBuilder,"collectionContextMenu",m_pCollectionContextMenu)
		
	// the view part popup menu item
	GET_WIDGET(m_refBuilder,"collectionViewPartMenuItem",m_pCollectionViewPartMenuItem)
	m_pCollectionViewPartMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_collectionViewPart_activated_event));

	// the add part popup menu item
	GET_WIDGET(m_refBuilder,"collectionAddPartMenuItem",m_pCollectionAddPartMenuItem)
	m_pCollectionAddPartMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_collectionAddPart_activated_event));

	// the delete part popup menu item
	GET_WIDGET(m_refBuilder,"collectionDeletePartMenuItem",m_pCollectionDeletePartMenuItem)
	m_pCollectionDeletePartMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_collectionDeletePart_activated_event));

	// the add set contents popup menu item
	GET_WIDGET(m_refBuilder,"collectionAddSetMenuItem",m_pCollectionAddSetMenuItem)
	m_pCollectionAddSetMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_collectionAddSet_activated_event));

	// handle a click on the part number or part description column header
	// (allows searching the collection on either the part number or description)
	m_pCollectionView->get_column(m_collectionStore.m_partNumber.index())->signal_clicked().connect_notify(sigc::mem_fun(*this,&MainWindow::on_collection_partNumber_clicked));
	m_pCollectionView->get_column(m_collectionStore.m_description.index())->signal_clicked().connect_notify(sigc::mem_fun(*this,&MainWindow::on_collection_description_clicked));

	// select parts to add dialog
	m_pSelectPartsDialog = new SelectPartsDialog(m_refBuilder,&m_cfg);
	if( !m_pSelectPartsDialog ) {
		throw "Could not get selectPartsDialog";
	}

	// select set to add dialog
	m_pSelectSetDialog = new SelectSetDialog(m_refBuilder,&m_cfg);
	if( !m_pSelectSetDialog ) {
		throw "Could not get selectSetDialog";
	}

	// set the title of the part price column to the currently selected
	// currency's code (which defaults to the current locale)
	m_pCollectionView->get_column(m_collectionViewPriceColumnIndex)->set_title(m_baseCurrencyCode);

	// select the first set to display
	if( m_pSetsStore->children().size() > 0 ) {
		m_pCollectionSetComboBox->set_active(0);
	}
}

// collection view context menu popup handler
void MainWindow::on_collection_button_pressed(GdkEventButton *pEvent)
{
	// if right mouse button pressed
	if( pEvent->type==GDK_BUTTON_PRESS && pEvent->button==GDK_BUTTON_SECONDARY ) {
		// select the part (if any) under the cursor
		Gtk::TreeModel::Path path;
		if( m_pCollectionView->get_path_at_pos((int)pEvent->x,(int)pEvent->y,path) ) {
			m_pCollectionView->set_cursor(path);
		}
		// view & delete are only active if there is a selected part
		bool partSelected = m_pCollectionView->get_selection()->count_selected_rows() != 0;
		m_pCollectionViewPartMenuItem->set_sensitive(partSelected);
		m_pCollectionDeletePartMenuItem->set_sensitive(partSelected);
		// show the popup menu
		m_pCollectionContextMenu->popup(pEvent->button,pEvent->time);
	}
}

// display a new collection
void MainWindow::on_collection_set_combobox_changed_event()
{
	m_pCollectionStore->clear();  // empty the collection datastore of any previous list of parts
	m_collectionPartsCount = 0;   // zero parts and cost
	m_collectionCost = 0;
	m_pCollectionCountCost->set_text("");
	// grab newly selected set
	Gtk::TreeModel::iterator iter = m_pCollectionSetComboBox->get_active();
	if(iter) {
		Gtk::TreeModel::Row row = *iter;
		if(row) {
			// save collection # and description and fill the datastore
			m_collectionNumber = row[m_setsStore.m_setNumber];
			m_collectionDescription = row[m_setsStore.m_description];
			FillCollection();
		}
	}
}

// display the currently selected part
void MainWindow::on_collectionViewPart_activated_event()
{
	// fin the currently selected part
	Gtk::TreeModel::iterator iter = m_pCollectionView->get_selection()->get_selected();
	if( iter ) {
		// grab its information
		Gtk::TreeModel::Row row = *iter;
		string partNumber = row[m_collectionStore.m_partNumber];
		string description = row[m_collectionStore.m_description];
		string size = row[m_collectionStore.m_size];
		string notes = row[m_collectionStore.m_notes];
		// and display it
		DisplayPicture(partNumber,description,size,notes);
	}
}

// add a part (or parts) to the current collection
void MainWindow::on_collectionAddPart_activated_event()
{
	if( m_pSelectPartsDialog->Run() == Gtk::RESPONSE_OK ) {
		// grab a list of the selected parts
		Glib::RefPtr<Gtk::TreeSelection> pSelected = m_pSelectPartsDialog->Selected();
		if( m_pSelectPartsDialog->SelectedCount() > 0 ) {
			// add each selected part to the collection
			pSelected->selected_foreach_iter(sigc::mem_fun(*this,&MainWindow::AddPartCallback));
		}
	}
	// all done, hide the dialog
	m_pSelectPartsDialog->Hide();
}

// callback routine to add a part to the collection
void MainWindow::AddPartCallback(const Gtk::TreeModel::iterator &iter)
{
	Gtk::TreeModel::Row partRow = *iter;
	AddPartToCollection(partRow,0,false);
}

// delete a part from the collection
void MainWindow::on_collectionDeletePart_activated_event()
{
	// find the currently selected part
	Gtk::TreeModel::iterator iter = m_pCollectionView->get_selection()->get_selected();
	if( iter ) {
		// make sure!
		Gtk::MessageDialog areYouSure(*m_pWindow,"Are you sure?",false,Gtk::MESSAGE_QUESTION,Gtk::BUTTONS_OK_CANCEL);
		Gtk::TreeModel::Row row = *iter;
		string partNum = row[m_collectionStore.m_partNumber];
		string partDescription = row[m_collectionStore.m_description];
		areYouSure.set_secondary_text("Delete part # "+partNum+" ("+partDescription+") from collection \""+m_collectionDescription+"\"?");
		if( areYouSure.run() == Gtk::RESPONSE_OK ) {
			// remove from the datastore
			m_pCollectionStore->erase(iter);
			// and remove from the database
			stringstream sql;
			sql
				<< "DELETE FROM set_parts WHERE part_num='" << m_pSql->Escape(partNum) 
				<< "' AND set_num='" << m_pSql->Escape(m_collectionNumber) << "'";
			m_pSql->ExecNonQuery(&sql);
			// update the collection totals and refresh the to make contents
			CalculateCollectionTotals();
			FillToMake();
		}
	}
}

// add the entire contents of another set(s) to the current collection
void MainWindow::on_collectionAddSet_activated_event()
{
	if( m_pSelectSetDialog->Run() == Gtk::RESPONSE_OK ) {
		// they've approved the addition, go to it
		m_pSelectSetDialog->Hide();
		WaitCursor(true);
		// get the selected set
		Glib::RefPtr<Gtk::TreeSelection> pSelected = m_pSelectSetDialog->Selected();
		// and add each part from it to the current collection
		if( m_pSelectSetDialog->SelectedCount() > 0 ) {
			pSelected->selected_foreach_iter(sigc::mem_fun(*this,&MainWindow::AddSetPartsCallback));
		}
		// update the parts count and total cost
		CalculateCollectionTotals();
		FillToMake();
		WaitCursor(false);
	} else {
		m_pSelectSetDialog->Hide();
	}
}

// called for each set to be added
void MainWindow::AddSetPartsCallback(const Gtk::TreeModel::iterator &iter)
{
	// get the set number to add
	Gtk::TreeModel::Row setRow = *iter;
	string setNumber = setRow[m_setsStore.m_setNumber];
	// clear out the set's part list map
	m_partsList.clear();
	// select the part # for part count list
	stringstream sql;
	sql << "SELECT part_num,count FROM set_parts WHERE set_num='" << m_pSql->Escape(setNumber) << "'";
	m_pSql->ExecQuery(&sql,MainWindow::AddSetPartCallback);
	// for each part in the set
	for( map<string,int>::iterator iter=m_partsList.begin(); iter!=m_partsList.end(); ++iter ) {
		string partNumber = (*iter).first;
		int count = (*iter).second;
		// search for the part in the parts datastore
		Gtk::TreeModel::Children partRows = m_pPartsStore->children();
		for( Gtk::TreeModel::iterator r = partRows.begin(); r!=partRows.end(); ++r ) {
			Gtk::TreeModel::Row partRow = *r;
			string partRowNumber = partRow[m_partsStore.m_partNumber];
			if( partRowNumber == partNumber ) {
				// found the part, add it to the collection, and allow
				// for the counts of existing parts in the collection
				// to be updated
				AddPartToCollection(partRow,count,true);
				break;
			}
		}
	}
}

int MainWindow::AddSetPartCallback(void *wnd,int argc,char **argv, char **azColName)
{
	MainWindow *pMainWindow = (MainWindow *)wnd;
	pMainWindow->m_partsList[argv[0]] = atoi(argv[1]);
	return 0;
}

// Add a part to the current collection.
// partRow: a row from the parts datastore
// count: how many of the part to add
// updateExistingRow: true if we're expecting a corresponding part row to already
//                    exist in the collection, false otherwise
void MainWindow::AddPartToCollection(Gtk::TreeModel::Row partRow,int count,bool updateExistingRow)
{
	// grab the relevant info from the selected part
	gint64 partRowId = partRow[m_partsStore.m_rowId];
	string partRowPrefix = partRow[m_partsStore.m_pnPrefix];
	int partRowDigits = partRow[m_partsStore.m_pnDigits];
	string partRowSuffix = partRow[m_partsStore.m_pnSuffix];
	// prepare to search the collection datastore
	Gtk::TreeModel::Children rows = m_pCollectionStore->children();
	// assume a new row was not inserted in the middle of the collection
	bool inserted = false;
	Gtk::TreeModel::Row newRow;
	// look for an existing row or find the proper insertion point
	for( Gtk::TreeModel::iterator r = rows.begin(); r != rows.end(); ++r ) {
		Gtk::TreeModel::Row row = *r;
		gint64 collectionRowId = row[m_collectionStore.m_rowId];
		if( partRowId == collectionRowId ) {
			// found the part in the collection
			if( updateExistingRow ) {
				// we expected to find an existing row and did
				// so add the count and update the total price of the parts in the collection
				row[m_collectionStore.m_count] = row[m_collectionStore.m_count]+count;
				row[m_collectionStore.m_total] = row[m_collectionStore.m_count]*row[m_collectionStore.m_price];
				stringstream sql;
				string partNumber = partRow[m_partsStore.m_partNumber];
				// update the database
				sql << "UPDATE set_parts SET count=count+" << count << " WHERE set_num='" << m_pSql->Escape(m_collectionNumber) << "' AND part_num='" << m_pSql->Escape(partNumber) << "'";
				m_pSql->ExecNonQuery(&sql);
			} else {
				// if we weren't expecting to find an existing part (ie when we're
				// adding a part from the 'Add Part' menu), do nothing. This prevents
				// part duplication in the collection due to chair-keyboard interface
				// issues.
			}
			return;
		} else {
			// see if we're at the right point to insert the new part
			string collectionRowPrefix = row[m_collectionStore.m_pnPrefix];
			int collectionRowDigits = row[m_collectionStore.m_pnDigits];
			string collectionRowSuffix = row[m_collectionStore.m_pnSuffix];
			if( partRowPrefix < collectionRowPrefix || (partRowPrefix == collectionRowPrefix && partRowDigits < collectionRowDigits) || (partRowPrefix==collectionRowPrefix && partRowDigits==collectionRowDigits && partRowSuffix < collectionRowSuffix) ) {
				// create a new row at the insertion and break out of the search loop
				newRow = *(m_pCollectionStore->insert(r));
				inserted = true;
				break;
			}
		}
	}
	// if we didn't find the insertion point, add the new part at the end of the
	// collection
	if( !inserted ) {
		newRow = *(m_pCollectionStore->append());
	}
	// populate the part data in the collection datastore
	newRow[m_collectionStore.m_rowId] = partRowId;
	// use a temp string variable to assign datastore values to other
	// datastore values. Trying to do it direct just doesn't work.
	string temp;
	newRow[m_collectionStore.m_partNumber] = temp = partRow[m_partsStore.m_partNumber];
	newRow[m_collectionStore.m_description] = temp = partRow[m_partsStore.m_description];
	newRow[m_collectionStore.m_size] = temp = partRow[m_partsStore.m_size];
	newRow[m_collectionStore.m_count] = count;
	// use a temp numeric variable to assign datastore values to other
	// datastore values. Trying to do it direct just doesn't work.
	double price;
	newRow[m_collectionStore.m_price] = price = partRow[m_partsStore.m_price];
	newRow[m_collectionStore.m_total] = price * count;
	newRow[m_collectionStore.m_pnPrefix] = partRowPrefix;
	newRow[m_collectionStore.m_pnDigits] = partRowDigits;
	newRow[m_collectionStore.m_pnSuffix] = partRowSuffix;
	string partNumber = partRow[m_partsStore.m_partNumber];
	// insert the new part into the set in the database
	stringstream sql;
	sql 
		<< "INSERT INTO set_parts(set_num,part_num,count) VALUES ('" 
		<< m_pSql->Escape(m_collectionNumber) << "','" 
		<< m_pSql->Escape(partNumber) << "'," << count << ")";
	m_pSql->ExecNonQuery(&sql);
}

// fill the collection datastore with the parts in the current collection
// and show the parts cost using the current pricelist
void MainWindow::FillCollection()
{
	// clear out the parts in the previously selected collection
	if( m_pCollectionStore->children().size() > 0 ) {
		m_pCollectionStore->clear();
		m_collectionPartsCount = 0;
		m_collectionCost = 0;
		m_pCollectionCountCost->set_text("");
	}
	// select the parts from the current collection
	stringstream cmd;
	cmd 
		<< "SELECT p.rowid,p.num,p.description,p.size,sp.count,IFNULL(pp.price,0) price,"
		<< "IFNULL(pp.price,0)*sp.count total,p.pnPrefix a,p.pnDigits b,p.pnSuffix c,p.notes "
		<< "FROM parts p JOIN set_parts sp ON p.num=sp.part_num AND sp.set_num='" << m_collectionNumber <<"' "
		<< "LEFT OUTER JOIN v_part_prices pp ON p.num=pp.part_num AND pp.pricelist_num=" << m_pricelistNumber
		<< " ORDER BY p.pnPrefix,p.pnDigits,p.pnSuffix";
	m_pSql->ExecQuery(&cmd, MainWindow::PopulateCollectionCallback);
	
	// update the total # of parts of cost of parts in the collection
	stringstream temp;
	temp << "# Parts: " << m_collectionPartsCount << "  Total Cost: " << setiosflags(ios::fixed) << setprecision(2) << m_collectionCost;
	m_pCollectionCountCost->set_text(temp.str());
}

// Append a row to the collection and populate it. The sqlite3 library doesn't like calling a member
// function so this is actually a static function in MainWindow. Hence the necessity of grabbing
// a pointer to the mainWindow instance and referencing all member variables through it.
int MainWindow::PopulateCollectionCallback(void *wnd, int argc, char **argv, char **azColName)
{
	MainWindow *pMainWindow = (MainWindow *)wnd;
	gint64 rowId = atol(argv[0]);
	string partNumber = argv[1];
	string description = argv[2];
	string size = argv[3];
	guint count = atoi(argv[4]);
	gdouble price = atof(argv[5]);
	gdouble total = atof(argv[6]);
	string pnPrefix = argv[7];
	int pnDigits = atoi(argv[8]);
	string pnSuffix = argv[9];
	string notes = argv[10];
	Gtk::TreeModel::Row row = *(pMainWindow->m_pCollectionStore->append());
	row[pMainWindow->m_collectionStore.m_rowId] = rowId;
	row[pMainWindow->m_collectionStore.m_partNumber] = partNumber;
	row[pMainWindow->m_collectionStore.m_description] = description;
	row[pMainWindow->m_collectionStore.m_size] = size;
	row[pMainWindow->m_collectionStore.m_count] = count;
	row[pMainWindow->m_collectionStore.m_price] = price;
	row[pMainWindow->m_collectionStore.m_total] = total;
	row[pMainWindow->m_collectionStore.m_pnPrefix] = pnPrefix;
	row[pMainWindow->m_collectionStore.m_pnDigits] = pnDigits;
	row[pMainWindow->m_collectionStore.m_pnSuffix] = pnSuffix;
	row[pMainWindow->m_collectionStore.m_notes] = notes;
	// update the collection parts count and cost
	pMainWindow->m_collectionPartsCount += count;
	pMainWindow->m_collectionCost += total;
	return 0;
}

// set up searching on the part # column when the part # column header is clicked
void MainWindow::on_collection_partNumber_clicked()
{
	m_pCollectionView->set_search_column(m_collectionStore.m_partNumber.index());
	m_pCollectionView->grab_focus();
}

// set up searching on the part description column when the part description column header is clicked
void MainWindow::on_collection_description_clicked()
{
	m_pCollectionView->set_search_column(m_collectionStore.m_description.index());
	m_pCollectionView->grab_focus();
}

// When a part's count is edited, update the count in the datastore and calculate
// the new total and update that in the datastore as well. Then update the
// parts count in the database and recalculate the collection totals. Finally,
// update the "To Make" tab info on the chance that the part we've updated is
// in the "to make" set.
void MainWindow::on_collection_count_edited(Glib::ustring pathStr, Glib::ustring text)
{
	GET_ITER(iter,m_pCollectionStore,pathStr)
	// grab the new count
	int count = atoi(text.c_str());
	// grab the part price and number
	double price = (*iter)[m_collectionStore.m_price];
	string num = (*iter)[m_collectionStore.m_partNumber];
	// update the count and total in the datastore
	(*iter)[m_collectionStore.m_count] = count;
	(*iter)[m_collectionStore.m_total] = count * price;
	// update the count in the database
	stringstream sql;
	sql
		<< "UPDATE set_parts SET count=" << count 
		<< " WHERE part_num='" << m_pSql->Escape(num) 
		<< "' AND set_num='" << m_pSql->Escape(m_collectionNumber) << "'";
	m_pSql->ExecNonQuery(&sql);
	// recalculate the collection parts count and cost
	CalculateCollectionTotals();
	// update the to make tab info
	FillToMake();
}

// iterate through the parts in the collection datastore to update the
// parts count and cost
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
// Parts tab setup: the Parts tab shows the parts list.
void MainWindow::PartsSetup()
{
	// parts datastore
	GET_OBJECT(m_refBuilder,"partsStore",m_pPartsStore,ListStore)

	// parts treeview. Append an editable numeric column for the part price
	// and an editable text column for the notes. Oh, c'mon Glade! :(
	GET_WIDGET(m_refBuilder,"partsView",m_pPartsView)
	m_partsViewPriceColumnIndex = m_pPartsView->append_column_numeric_editable("Price",m_partsStore.m_price,"%.2lf")-1;
	m_pPartsView->append_column_editable("Notes",m_partsStore.m_notes);

	// part description
	Gtk::CellRendererText *pCellRenderer;
	GET_TEXT_RENDERER("partsDescriptionCellRenderer",pCellRenderer,m_pPartsView,m_partsStore.m_description.index())
	pCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_parts_description_edited));

	// parts size. Essentially more description
	GET_TEXT_RENDERER("partsSizeCellRenderer",pCellRenderer,m_pPartsView,m_partsStore.m_size.index())
	pCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_parts_size_edited));

	// parts price
	GET_TEXT_RENDERER("partsPriceCellRenderer",m_pPartsPriceCellRenderer,m_pPartsView,m_partsStore.m_price.index())
	m_pPartsPriceCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_parts_price_edited));
  // zero prices will be shown in red
  m_defaultPriceCellForeColor = m_pPartsPriceCellRenderer->property_foreground_gdk();
  m_missingPriceCellForeColor = Gdk::Color(string("red"));
	m_pPartsView->get_column(m_partsViewPriceColumnIndex)->set_cell_data_func(*m_pPartsPriceCellRenderer,sigc::mem_fun(*this,&MainWindow::on_price_column_drawn));
	// set the title of the part price column to the currently selected
	// currency's code (which defaults to the current locale)
	m_pPartsView->get_column(m_partsViewPriceColumnIndex)->set_title(m_baseCurrencyCode);

	// parts notes
	GET_TEXT_RENDERER("partsNotesCellRenderer",pCellRenderer,m_pPartsView,m_partsStore.m_notes.index())
	pCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_parts_notes_edited));

	// parts view popup context menu
	m_pPartsView->signal_button_press_event().connect_notify(sigc::mem_fun(*this,&MainWindow::on_parts_button_pressed));

	// allow searching on part number or part description
	m_pPartsView->get_column(m_partsStore.m_partNumber.index())->signal_clicked().connect_notify(sigc::mem_fun(*this,&MainWindow::on_parts_partNumber_clicked));
	m_pPartsView->get_column(m_partsStore.m_description.index())->signal_clicked().connect_notify(sigc::mem_fun(*this,&MainWindow::on_parts_description_clicked));

	// parts context menu
	GET_WIDGET(m_refBuilder,"partsContextMenu",m_pPartsContextMenu)
	GET_WIDGET(m_refBuilder,"partsViewPartMenuItem",m_pPartsViewPartMenuItem)
	m_pPartsViewPartMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_partsViewPart_activated_event));

	// create new part menu item
	GET_WIDGET(m_refBuilder,"partsNewPartMenuItem",m_pPartsNewPartMenuItem)
	m_pPartsNewPartMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_partsNewPart_activated_event));

	// delete part menu item
	GET_WIDGET(m_refBuilder,"partsDeletePartMenuItem",m_pPartsDeletePartMenuItem)
	m_pPartsDeletePartMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_partsDeletePart_activated_event));

	// filter sets on part (only show sets containing this part)
	GET_WIDGET(m_refBuilder,"partsFilterSetsMenuItem",m_pPartsFilterSetsMenuItem)
	m_pPartsFilterSetsMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_partsFilterSets_activated_event));

	// remove part filter
	GET_WIDGET(m_refBuilder,"partsUnfilterSetsMenuItem",m_pPartsUnfilterSetsMenuItem)
	m_pPartsUnfilterSetsMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_partsUnfilterSets_activated_event));

	// create a new part dialog
	m_pNewPartDialog = new NewPartDialog(m_refBuilder);
	if( !m_pNewPartDialog ) {
		throw "Could not get newPartDialog";
	}

	FillParts();
}

// show zero (ie not available or missing) prices in red
//
// Setting the text colour here revealed an odd quirk (bug!) in how the text is
// drawn. Even though the default text colour I'm using is taken from the
// widget itself, when I set it here, it comes out differently. Left to its own
// devices, the TreeView text displays as a darkish grey *even though* when
// queried it comes back as black. When I set the foreground to what the widget
// told me it was, it comes out black. Very odd!
void MainWindow::on_price_column_drawn(Gtk::CellRenderer *r,const Gtk::TreeModel::iterator &iter)
{
  Gtk::TreeModel::Row row = *iter;
  double price = row[m_partsStore.m_price];
  stringstream temp;
  temp << fixed << setprecision(2) << price;
  m_pPartsPriceCellRenderer->property_text() = temp.str();
  if( price == 0.0 ) {
    m_pPartsPriceCellRenderer->property_foreground_gdk() = m_missingPriceCellForeColor;
  } else {
    m_pPartsPriceCellRenderer->property_foreground_gdk() = m_defaultPriceCellForeColor;
  }
}

// part number header clicked on: allow search by part number
void MainWindow::on_parts_partNumber_clicked()
{
	m_pPartsView->set_search_column(m_partsStore.m_partNumber.index());
	m_pPartsView->grab_focus();
}

// part description header clicked on: allow search by part description
void MainWindow::on_parts_description_clicked()
{
	m_pPartsView->set_search_column(m_partsStore.m_description.index());
	m_pPartsView->grab_focus();
}

// fill the parts datastore
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
	m_pSql->ExecQuery(&cmd, MainWindow::PopulatePartsCallback);
}

// parts datastore fill callback. Note that this is a static function!
int MainWindow::PopulatePartsCallback(void *wnd, int argc, char **argv, char **azColName)
{
	MainWindow *pMainWindow = (MainWindow *)wnd;
	gint64 rowId = atol(argv[0]);
	string partNumber = argv[1];
	string description = argv[2];
	string size = argv[3];
	gdouble price = atof(argv[4]);
	string notes = argv[5];
	string pnPrefix = argv[6];
	int pnDigits = atoi(argv[7]);
	string pnSuffix = argv[8];
	Gtk::TreeModel::Row row = *(pMainWindow->m_pPartsStore->append());
	row[pMainWindow->m_partsStore.m_rowId] = rowId;
	row[pMainWindow->m_partsStore.m_partNumber] = partNumber;
	row[pMainWindow->m_partsStore.m_description] = description;
	row[pMainWindow->m_partsStore.m_size] = size;
	row[pMainWindow->m_partsStore.m_price] = price;
	row[pMainWindow->m_partsStore.m_notes] = notes;
	row[pMainWindow->m_partsStore.m_pnPrefix] = pnPrefix;
	row[pMainWindow->m_partsStore.m_pnDigits] = pnDigits;
	row[pMainWindow->m_partsStore.m_pnSuffix] = pnSuffix;
	return 0;
}

// test to see if a specific part number actually exists in the datastore
// used during price list price data import
bool MainWindow::PartExists(string partNumber)
{
	bool exists = false;
	// search the parts datastore
	Gtk::TreeModel::Children partRows = m_pPartsStore->children();
	for( Gtk::TreeModel::iterator r = partRows.begin(); r!=partRows.end(); ++r ) {
		Gtk::TreeModel::Row partRow = *r;
		string partRowNumber = partRow[m_partsStore.m_partNumber];
		if( partRowNumber == partNumber ) {
			// found the part number, all done
			exists = true;
			break;
		}
	}
	return exists;
}

#ifndef _Parts_Fields_Edited
void MainWindow::on_parts_description_edited(Glib::ustring pathStr, Glib::ustring text)
{
	GET_ITER(iter,m_pPartsStore,pathStr)
	string num = (*iter)[m_partsStore.m_partNumber];
	(*iter)[m_partsStore.m_description] = text;
	stringstream sql;
	sql
		<< "UPDATE parts SET description='" << m_pSql->Escape(text)
		<< "' WHERE num='" << m_pSql->Escape(num) << "'";
	m_pSql->ExecNonQuery(&sql);
	FillCollection();
	FillToMake();
}

void MainWindow::on_parts_size_edited(Glib::ustring pathStr, Glib::ustring text)
{
	GET_ITER(iter,m_pPartsStore,pathStr)
	string num = (*iter)[m_partsStore.m_partNumber];
	(*iter)[m_partsStore.m_size] = text;
	stringstream sql;
	sql
		<< "UPDATE parts SET size='" << m_pSql->Escape(text)
		<< "' WHERE num='" << m_pSql->Escape(num) << "'";
	m_pSql->ExecNonQuery(&sql);
	FillCollection();
	FillToMake();
}

void MainWindow::on_parts_price_edited(Glib::ustring pathStr, Glib::ustring text)
{
	GET_ITER(iter,m_pPartsStore,pathStr)
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
	FillToMake();
}

void MainWindow::on_parts_notes_edited(Glib::ustring pathStr, Glib::ustring text)
{
	GET_ITER(iter,m_pPartsStore,pathStr)
	string num = (*iter)[m_partsStore.m_partNumber];
	(*iter)[m_partsStore.m_notes] = text;
	stringstream sql;
	sql
		<< "UPDATE parts SET notes='" << m_pSql->Escape(text)
		<< "' WHERE num='" << m_pSql->Escape(num) << "'";
	m_pSql->ExecNonQuery(&sql);
	FillCollection();
	FillToMake();
}
#endif
// parts view context menut handler
void MainWindow::on_parts_button_pressed(GdkEventButton *pEvent)
{
	// right mouse button pressed?
	if( pEvent->type==GDK_BUTTON_PRESS && pEvent->button==GDK_BUTTON_SECONDARY ) {
		Gtk::TreeModel::Path path;
		// assume the cursor wasn't not on a part until proven otherwise
		m_pPartsViewPartMenuItem->set_sensitive(false);
		m_pPartsDeletePartMenuItem->set_sensitive(false);
		m_pPartsFilterSetsMenuItem->set_sensitive(false);
		// if we're currently filtering sets, allow them to be unfiltered
		m_pPartsUnfilterSetsMenuItem->set_sensitive(m_bSetsFiltered);
		if( m_pPartsView->get_path_at_pos((int)pEvent->x,(int)pEvent->y,path) ) {
			// make sure the current part is selected
			m_pPartsView->set_cursor(path);
			// since we're on a part, we can view it, delete it or filter sets by it
			m_pPartsViewPartMenuItem->set_sensitive(true);
			m_pPartsDeletePartMenuItem->set_sensitive(true);
			m_pPartsFilterSetsMenuItem->set_sensitive(true);
		}
		// show the popup menu
		m_pPartsContextMenu->popup(pEvent->button,pEvent->time);
	}
}

// display part info, including a picture if we've got one
void MainWindow::on_partsViewPart_activated_event()
{
	Gtk::TreeModel::iterator iter = m_pPartsView->get_selection()->get_selected();
	if( iter ) {
		Gtk::TreeModel::Row row = *iter;
		string partNumber = row[m_partsStore.m_partNumber];
		string description = row[m_partsStore.m_description];
		string size = row[m_partsStore.m_size];
		string notes = row[m_partsStore.m_notes];
		DisplayPicture(partNumber,description,size,notes);
	}
}

// add a new part
void MainWindow::on_partsNewPart_activated_event()
{
	string errorLabel = "";
	while( true ) {
		if( m_pNewPartDialog->Run(errorLabel,m_pricelistCurrencyCode) == Gtk::RESPONSE_OK ) {
			// separate the part number into an (optional) alphabetic prefix,
			// a (required) numeric part number and an (optional) alphanumeric suffix
			boost::regex pnRegex("([A-Za-z]*)([0-9]+)([A-Za-z0-9]*)");
			boost::smatch matches;
			string new_partNum = m_pNewPartDialog->PartNumber();
			if( !boost::regex_match(new_partNum,matches,pnRegex) || matches.size()!=4) {
				throw "Regex failed";
			}
			string new_pnPrefix = matches[1];
			int new_pnDigits = atoi(matches[2].str().c_str());
			string new_pnSuffix = matches[3];
			// insert the new part into the appropriate point in the parts datastore
			// as long as it doesn't already exist
			Gtk::TreeModel::Row newRow;
			bool alreadyExists = false;
			bool inserted = false;
			// search the parts datastore
			Gtk::TreeModel::Children partsRows = m_pPartsStore->children();
			for( Gtk::TreeModel::iterator iter = partsRows.begin(); iter!= partsRows.end(); ++iter ) {
				Gtk::TreeModel::Row partsRow = *iter;
				string partNum = partsRow[m_partsStore.m_partNumber];
				string pnPrefix = partsRow[m_partsStore.m_pnPrefix];
				int pnDigits = partsRow[m_partsStore.m_pnDigits];
				string pnSuffix = partsRow[m_partsStore.m_pnSuffix];
				if( new_partNum == partNum ) {
					// oops, it's already in there
					errorLabel = "Part number "+m_pNewPartDialog->PartNumber()+" already exists!";
					alreadyExists = true;
					break;
				} else if( new_pnPrefix < pnPrefix || (new_pnPrefix == pnPrefix && new_pnDigits < pnDigits) || (new_pnPrefix==pnPrefix && new_pnDigits==pnDigits && new_pnSuffix <= pnSuffix) ) {
					// found the insertion point
					newRow = *(m_pPartsStore->insert(iter));
					inserted = true;
					break;
				}
			}
			if( !alreadyExists ) {
				// okay to add
				errorLabel = "";
				if( !inserted ) {
					// the new part number needs to be added to the END of the datastore
					newRow = *(m_pPartsStore->append());
				}
				// set up the SQL to add it to the parts table in the database
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
				// ExecInsert returns the new row id #
				newRow[m_partsStore.m_rowId] = m_pSql->ExecInsert(&sql);
				newRow[m_partsStore.m_partNumber] = new_partNum;
				newRow[m_partsStore.m_pnPrefix] = new_pnPrefix;
				newRow[m_partsStore.m_pnDigits] = new_pnDigits;
				newRow[m_partsStore.m_pnSuffix] = new_pnSuffix;
				newRow[m_partsStore.m_description] = m_pNewPartDialog->Description();
				newRow[m_partsStore.m_size] = m_pNewPartDialog->Size();
				newRow[m_partsStore.m_notes] = m_pNewPartDialog->Notes();
				newRow[m_partsStore.m_price] = m_pNewPartDialog->Price()/m_pricelistCurrencyRate*m_baseCurrencyRate;
				// add the part price to the current pricelist
				// note that the price is assumed to be in the currency
				// of the pricelist, not the currently selected currency
				if( m_pNewPartDialog->Price() != 0.0 ) {
					sql.str("");
					sql
						<< "INSERT INTO part_prices(pricelist_num,part_num,price) VALUES ("
						<< m_pricelistNumber << ","
						<< "'" << m_pSql->Escape(new_partNum) << "',"
						<< m_pNewPartDialog->Price() << ")";
					m_pSql->ExecInsert(&sql);
				}
				break;
			}
		} else {
			break;
		}
	}
	m_pNewPartDialog->Hide();
}

// delete a part. This deletes the part from all priceslists and sets as well.
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
			FillToMake();
		}
	}
}

// I think this is probably one of those functions that you're either love or
// ignore. It's the ability to filter the sets list to *just* those sets that
// contain a particular part. I found it handy when I was looking for a
// particular part and wondered if there was an available set I could buy that
// contained it. Nowadays I'd just go to eBay or Dave Taylor! ;)
void MainWindow::on_partsFilterSets_activated_event()
{
	Gtk::TreeModel::iterator iter = m_pPartsView->get_selection()->get_selected();
	if( iter ) {
		m_bSetsFiltered = true;
		Gtk::TreeModel::Row row = *iter;
		string partNumber = row[m_partsStore.m_partNumber];
		stringstream sql;
		sql << 
			"SELECT s.num,s.description,IFNULL(s.started,9999),IFNULL(s.ended,9999),s.rowid " <<
			"FROM sets s,set_parts sp " <<
			"WHERE s.num=sp.set_num AND sp.part_num='" << partNumber << "' " <<
			"ORDER BY s.started,s.description DESC";
		m_pSetsStore->clear();
		m_pSql->ExecQuery(&sql,MainWindow::PopulateSetsCallback);
	}
}

// And of course, the ability to remove the filtering.
void MainWindow::on_partsUnfilterSets_activated_event()
{
	m_bSetsFiltered = false;
	m_pSetsStore->clear();
	const char *cmd = 
		"SELECT num,description,IFNULL(started,9999),IFNULL(ended,9999),rowid "\
		"FROM sets "\
		"ORDER BY started,description DESC";
	m_pSql->ExecQuery(cmd,MainWindow::PopulateSetsCallback);
}
#endif
#ifndef _ToMake_Routines
// To Make: a 'set' can be an actual Meccano set, an arbitrary collection of
// parts, an order you've made to a supplier, or the parts list of a model.
// The latter case is where this comes in handy. You 'have' your collection of
// parts and 'want' to build a particular model. What parts are you short? This
// tab makes answering that question a breeze.
void MainWindow::ToMakeSetup()
{
	// to make treeview and context menu handler
	GET_WIDGET(m_refBuilder,"toMakeView",m_pToMakeView)
	 m_pToMakeView->signal_button_press_event().connect_notify(sigc::mem_fun(*this,&MainWindow::on_toMake_button_pressed));

	// cost of parts 'to make' what I 'want' from what I 'have'
	GET_WIDGET(m_refBuilder,"toMakeCost",m_pToMakeCost);
	
	// to make datastore
	GET_OBJECT(m_refBuilder,"toMakeStore",m_pToMakeStore,ListStore)
	
	// once again, Glade doesn't have the concept of a numeric formatted column...
	// or at least not that I've found!
	m_toMakeViewPriceColumnIndex = m_pToMakeView->append_column_numeric("Price",m_toMakeStore.m_price,"%.2lf")-1;
	m_pToMakeView->append_column_numeric("Total",m_toMakeStore.m_total,"%.2lf");

	// read only columns are shown with a grey background, editable columns have a white one
	// no columns in the to make view are editable
	Gtk::CellRendererText *pCellRenderer = NULL;
	GET_TEXT_RENDERER("toMakePriceCellRenderer",pCellRenderer,m_pToMakeView,m_toMakeStore.m_price.index())
	string propertyName = "background-gdk"; 
  Gdk::Color bkColour= Gdk::Color(string("grey88"));
	pCellRenderer->set_property(propertyName,bkColour);

	GET_TEXT_RENDERER("toMakeTotalCellRenderer",pCellRenderer,m_pToMakeView,m_toMakeStore.m_total.index())
	pCellRenderer->set_property(propertyName,bkColour);
	
	// 'have' combo box
	GET_WIDGET(m_refBuilder,"toMakeHaveComboBox",m_pToMakeHaveComboBox);
	m_pToMakeHaveComboBox->pack_start(m_setsStore.m_description);
	m_pToMakeHaveComboBox->signal_changed().connect(sigc::mem_fun(*this,&MainWindow::FillToMake));

	// 'want' combo box
	GET_WIDGET(m_refBuilder,"toMakeWantComboBox",m_pToMakeWantComboBox);
	m_pToMakeWantComboBox->pack_start(m_setsStore.m_description);
	m_pToMakeWantComboBox->signal_changed().connect(sigc::mem_fun(*this,&MainWindow::FillToMake));

	// pop up context menu
	GET_WIDGET(m_refBuilder,"toMakeContextMenu",m_pToMakeContextMenu)
	
	// view part menu item
	GET_WIDGET(m_refBuilder,"toMakeViewPartMenuItem",m_pToMakeViewPartMenuItem)
	m_pToMakeViewPartMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_toMakeViewPart_activated_event));

	// searchable on part number or part description
	m_pToMakeView->get_column(m_toMakeStore.m_partNumber.index())->signal_clicked().connect_notify(sigc::mem_fun(*this,&MainWindow::on_toMake_partNumber_clicked));
	m_pToMakeView->get_column(m_toMakeStore.m_description.index())->signal_clicked().connect_notify(sigc::mem_fun(*this,&MainWindow::on_toMake_description_clicked));

	// currency code in use
	m_pToMakeView->get_column(m_toMakeViewPriceColumnIndex)->set_title(m_baseCurrencyCode);
}

// popup menu handler
void MainWindow::on_toMake_button_pressed(GdkEventButton *pEvent)
{
	// right button press?
	if( pEvent->type==GDK_BUTTON_PRESS && pEvent->button==GDK_BUTTON_SECONDARY ) {
		Gtk::TreeModel::Path path;
		// hovering over a part?
		if( m_pToMakeView->get_path_at_pos((int)pEvent->x,(int)pEvent->y,path) ) {
			// make sure the part is selected
			m_pToMakeView->set_cursor(path);
			// and pop up the menu
			m_pToMakeContextMenu->popup(pEvent->button,pEvent->time);
		}
	}
}

// display part information
void MainWindow::on_toMakeViewPart_activated_event()
{
	Gtk::TreeModel::iterator iter = m_pToMakeView->get_selection()->get_selected();
	if( iter ) {
		Gtk::TreeModel::Row row = *iter;
		string partNumber = row[m_toMakeStore.m_partNumber];
		string description = row[m_toMakeStore.m_description];
		string size = row[m_toMakeStore.m_size];
		string notes = row[m_toMakeStore.m_notes];
		DisplayPicture(partNumber,description,size,notes);
	}
}

// search on part number
void MainWindow::on_toMake_partNumber_clicked()
{
	m_pToMakeView->set_search_column(m_toMakeStore.m_partNumber.index());
	m_pToMakeView->grab_focus();
}

// search on part description
void MainWindow::on_toMake_description_clicked()
{
	m_pToMakeView->set_search_column(m_toMakeStore.m_description.index());
	m_pToMakeView->grab_focus();
}

// fill the to make tab view
void MainWindow::FillToMake()
{
	// clear out the to make datastore
	m_pToMakeStore->clear();
	// zero the parts count and costs
	m_toMakePartsCount = 0;
	m_toMakeCost = 0;
	m_pToMakeCost->set_text("");
	// get the active entries from both combo boxes
	Gtk::TreeModel::iterator iterHave = m_pToMakeHaveComboBox->get_active();
	Gtk::TreeModel::iterator iterWant = m_pToMakeWantComboBox->get_active();
	// we have active entries from both
	if(iterHave && iterWant)
	{
		// get the have and want rows from the sets datastore
		Gtk::TreeModel::Row haveRow = *iterHave;
		Gtk::TreeModel::Row wantRow = *iterWant;
		if(haveRow && wantRow)
		{
			// get the have and want set numbers (escaped for sql)
			string haveNum = m_pSql->Escape(haveRow[m_setsStore.m_setNumber]);
			string wantNum = m_pSql->Escape(wantRow[m_setsStore.m_setNumber]);
			// Here's the slightly tricky bit. A temporary view joins the set_parts,
			// parts, and pricelists table (using the current pricelist). This temp
			// view is then used to create a couple of temporary tables, one for
			// 'what I have' and the second for 'what I want'. These two temp tables
			// are used in a select that calculates the difference between the 'what
			// I have' and 'what I want' -- giving me 'what I need'!
			stringstream cmd;
			cmd <<
				"DROP VIEW IF EXISTS v_set_parts; " <<
				"CREATE TEMP VIEW v_set_parts AS " <<
					"SELECT " <<
						"s.num set_num,s.description set_description,p.num part_num," <<
						"p.description part_description,p.size,sp.count,IFNULL(pp.price,0) price," <<
						"sp.count*IFNULL(pp.price,0) total,p.notes " <<
					"FROM " <<
						"sets s INNER join set_parts sp ON s.num=sp.set_num " << 
						"JOIN parts p ON p.num=sp.part_num " <<
						"LEFT OUTER JOIN v_part_prices pp ON p.num=pp.part_num AND pp.pricelist_num=" << m_pricelistNumber << ' ' <<
				"ORDER BY " <<
					"p.pnPrefix,p.pnDigits,p.pnSuffix; " <<
				"DROP TABLE IF EXISTS have; " <<
				"DROP TABLE IF EXISTS want; " <<
				"CREATE TEMP TABLE have AS SELECT * FROM v_set_parts WHERE set_num='" << haveNum << "'; " <<
				"CREATE TEMP TABLE want AS SELECT * FROM v_set_parts WHERE set_num='" << wantNum << "'; " <<
				"SELECT " <<
					"w.part_num,w.part_description,w.size,w.price,w.count-IFNULL(h.count,0)," <<
					"w.price*(w.count-IFNULL(h.count,0)),w.notes " <<
				"FROM " <<
					"want w LEFT OUTER JOIN have h ON w.part_num=h.part_num "
				"WHERE " <<
					"w.count-IFNULL(h.count,0)>0";
			m_pSql->ExecQuery(&cmd, MainWindow::PopulateToMakeCallback);

			// The PopulateToMake routine calculates the parts count and costs
			stringstream temp;
			temp << setiosflags(ios::fixed) << setprecision(2) << m_toMakeCost;
			m_pToMakeCost->set_text(temp.str());
		}
	}
}

// update the to make datastore
int MainWindow::PopulateToMakeCallback(void *wnd, int argc, char **argv, char **azColName)
{
	MainWindow *pMainWindow = (MainWindow *)wnd;
	string partNumber = argv[0];        // w.part_num
	string description = argv[1];       // w.part_description
	string size = argv[2];              // w.size
	gdouble price = atof(argv[3]);      // w.price
	gint count = atoi(argv[4]);         // w.count-IFNULL(h.count,0)
	gdouble total = atof(argv[5]);      // w.price*(w.count-IFNULL(h.count,0))
	string notes = argv[6];             // w.notes
	Gtk::TreeModel::Row row = *(pMainWindow->m_pToMakeStore->append());
	row[pMainWindow->m_toMakeStore.m_partNumber] = partNumber;
	row[pMainWindow->m_toMakeStore.m_description] = description;
	row[pMainWindow->m_toMakeStore.m_size] = size;
	row[pMainWindow->m_toMakeStore.m_count] = count;
	row[pMainWindow->m_toMakeStore.m_price] = price;
	row[pMainWindow->m_toMakeStore.m_total] = total;
	row[pMainWindow->m_toMakeStore.m_notes] = notes;
	pMainWindow->m_toMakePartsCount += count;
	pMainWindow->m_toMakeCost += total;
	return 0;
}

#endif
#ifndef _Sets_Routines
// sets: probably the simplest basic entity in MecParts. A set is the name of
// a collection of parts. It isn't the collection, just the name. There's a
// description and a beginning and end date (neither of which I do much with)
// and that's about it.
void MainWindow::SetsSetup()
{
	// the sets datastore
	GET_OBJECT(m_refBuilder,"setsStore",m_pSetsStore,ListStore)

	// the sets treeview with a couple of numeric columns added for the dates
	GET_WIDGET(m_refBuilder,"setsView",m_pSetsView)
	m_pSetsView->append_column_numeric_editable("Started",m_setsStore.m_started,"%d");
	m_pSetsView->append_column_numeric_editable("Ended",m_setsStore.m_ended,"%d");
	
	// description edited handler
	Gtk::CellRendererText *pCellRenderer;
	GET_TEXT_RENDERER("setsDescriptionCellRenderer",pCellRenderer,m_pSetsView,m_setsStore.m_description.index())
	pCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_sets_description_edited));

	// started date edited handler
	GET_TEXT_RENDERER("setsStartedCellRenderer",pCellRenderer,m_pSetsView,m_setsStore.m_started.index())
	pCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_sets_started_edited));

	// ended date edited handler
	GET_TEXT_RENDERER("setsEndedCellRenderer",pCellRenderer,m_pSetsView,m_setsStore.m_ended.index())
	pCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_sets_ended_edited));

	// pop up menu handler
	m_pSetsView->signal_button_press_event().connect_notify(sigc::mem_fun(*this,&MainWindow::on_sets_button_pressed));

	// pop up menu
	GET_WIDGET(m_refBuilder,"setsContextMenu",m_pSetsContextMenu)
	// new set menu item
	GET_WIDGET(m_refBuilder,"setsNewSetMenuItem",m_pSetsNewSetMenuItem)
	m_pSetsNewSetMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_setsNewSet_activated_event));
	// delete set menu item
	GET_WIDGET(m_refBuilder,"setsDeleteSetMenuItem",m_pSetsDeleteSetMenuItem)
	m_pSetsDeleteSetMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_setsDeleteSet_activated_event));

	// search sets treeview on set number or description
	m_pSetsView->get_column(m_setsStore.m_setNumber.index())->signal_clicked().connect_notify(sigc::mem_fun(*this,&MainWindow::on_sets_setNumber_clicked));
	m_pSetsView->get_column(m_setsStore.m_description.index())->signal_clicked().connect_notify(sigc::mem_fun(*this,&MainWindow::on_sets_description_clicked));

	// new set dialog
	m_pNewSetDialog = new NewSetDialog(m_refBuilder);
	if( !m_pNewSetDialog ) {
		throw "Could not get newSetDialog";
	}

	// fill the sets datastore
	// notice that we do it backwards so that we can prepend rather than
	// append to the sets datastore. It's about 1/3 faster to do a prepend
	// than it is to do an append.
	const char *cmd = 
		"SELECT num,description,IFNULL(started,9999),IFNULL(ended,9999),rowid "\
		"FROM sets "\
		"ORDER BY started,description DESC";
	m_pSql->ExecQuery(cmd,MainWindow::PopulateSetsCallback);
}

// fill sets datastore
// notice that we do it backwards so that we can prepend rather than
// append to the sets datastore. It's about 1/3 faster to do a prepend
// than it is to do an append.
int MainWindow::PopulateSetsCallback(void *wnd, int argc, char **argv, char **azColName)
{
	MainWindow *pMainWindow = (MainWindow *)wnd;
	string setNumber = argv[0];
	string description = argv[1];
	int started = atoi(argv[2]);
	int ended = atoi(argv[3]);
	gint64 rowid = atol(argv[4]);
	Gtk::TreeModel::Row row = *(pMainWindow->m_pSetsStore->prepend());
	row[pMainWindow->m_setsStore.m_rowId] = rowid;
	row[pMainWindow->m_setsStore.m_setNumber] = setNumber;
	row[pMainWindow->m_setsStore.m_description] = description;
	row[pMainWindow->m_setsStore.m_started] = started;
	row[pMainWindow->m_setsStore.m_ended] = ended;
	return 0;
}

#ifndef _Sets_Fields_Edited
// set description edited
void MainWindow::on_sets_description_edited(Glib::ustring pathStr, Glib::ustring text)
{
	GET_ITER(iter,m_pSetsStore,pathStr)
	string num = (*iter)[m_setsStore.m_setNumber];
	(*iter)[m_setsStore.m_description] = text;
	stringstream sql;
	sql
		<< "UPDATE sets SET description='" << m_pSql->Escape(text)
		<< "' WHERE num='" << m_pSql->Escape(num) << "'";
	m_pSql->ExecNonQuery(&sql);
}

// set starting date edited
void MainWindow::on_sets_started_edited(Glib::ustring pathStr, Glib::ustring text)
{
	GET_ITER(iter,m_pSetsStore,pathStr)
	string num = (*iter)[m_setsStore.m_setNumber];
	(*iter)[m_setsStore.m_started] = atoi(text.c_str());
	stringstream sql;
	sql
		<< "UPDATE sets SET started=" << atoi(text.c_str())
		<< " WHERE num='" << m_pSql->Escape(num) << "'";
	m_pSql->ExecNonQuery(&sql);
}

// set ending date edited
void MainWindow::on_sets_ended_edited(Glib::ustring pathStr, Glib::ustring text)
{
	GET_ITER(iter,m_pSetsStore,pathStr)
	string num = (*iter)[m_setsStore.m_setNumber];
	(*iter)[m_setsStore.m_ended] = atoi(text.c_str());
	stringstream sql;
	sql
		<< "UPDATE sets SET ended=" << atoi(text.c_str())
		<< " WHERE num='" << m_pSql->Escape(num) << "'";
	m_pSql->ExecNonQuery(&sql);
}
#endif

// set popup context menu
void MainWindow::on_sets_button_pressed(GdkEventButton *pEvent)
{
	// right mouse button?
	if( pEvent->type==GDK_BUTTON_PRESS && pEvent->button==GDK_BUTTON_SECONDARY ) {
		Gtk::TreeModel::Path path;
		// hovering on a set?
		if( m_pSetsView->get_path_at_pos((int)pEvent->x,(int)pEvent->y,path) ) {
			// make sure it's selected
			m_pSetsView->set_cursor(path);
		}
		// only enable deletion if a set is selected
		m_pSetsDeleteSetMenuItem->set_sensitive(m_pSetsView->get_selection()->count_selected_rows() != 0);
		m_pSetsContextMenu->popup(pEvent->button,pEvent->time);
	}
}

// add a new set
void MainWindow::on_setsNewSet_activated_event()
{
	string errorLabel = "";
	while( true ) {
		// get the new set info
		if( m_pNewSetDialog->Run(errorLabel) == Gtk::RESPONSE_OK ) {
			// going ahead with it, grab info
			string new_setNum = m_pNewSetDialog->SetNumber();
			string new_description = m_pNewSetDialog->Description();
			int new_started = m_pNewSetDialog->Started();
			// find the appropriate insertion point in the sets datastore
			Gtk::TreeModel::Row newRow;
			bool alreadyExists = false;
			bool inserted = false;
			Gtk::TreeModel::Children setsRows = m_pSetsStore->children();
			for( Gtk::TreeModel::iterator iter = setsRows.begin(); iter!= setsRows.end(); ++iter ) {
				Gtk::TreeModel::Row setsRow = *iter;
				string setNum = setsRow[m_setsStore.m_setNumber];
				string description = setsRow[m_setsStore.m_description];
				int started = setsRow[m_setsStore.m_started];
				// does it already exist?
				if( new_setNum == setNum ) {
					errorLabel = "Set number "+new_setNum+" already exists!";
					alreadyExists = true;
					break;
				} else if( new_started > started || (new_started == started && new_description < description) ) {
					// insert here
					newRow = *(m_pSetsStore->insert(iter));
					inserted = true;
					break;
				}
			}
			if( !alreadyExists ) {
				errorLabel = "";
				// does it need to be added to the end of the sets list?
				if( !inserted ) {
					newRow = *(m_pSetsStore->append());
				}
				// add to database
				stringstream sql;
				sql <<
					"INSERT INTO sets(num,description,started,ended) VALUES " <<
					"('" << m_pSql->Escape(new_setNum) << "','" <<
					m_pSql->Escape(m_pNewSetDialog->Description()) << "'," <<
					m_pNewSetDialog->Started() << "," <<
					m_pNewSetDialog->Ended() << ")";
				// ExecInsert returns the new rowId
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
	m_pNewSetDialog->Hide();
}

// delete an existing set (including all parts that belong to the set)
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

// search on set number
void MainWindow::on_sets_setNumber_clicked()
{
	m_pSetsView->set_search_column(m_setsStore.m_setNumber.index());
	m_pSetsView->grab_focus();
}

// search on set description
void MainWindow::on_sets_description_clicked()
{
	m_pSetsView->set_search_column(m_setsStore.m_description.index());
	m_pSetsView->grab_focus();
}

#endif
#ifndef _Pricelists_Routines
// Pricelists: this was the part that really held things up. Specifically,
// importing price list prices. I'm still not entirely happy with the whole
// csv import bit but it *DOES* work. It's just very basic is all. No selecting
// columns, no looking at headers, no including pricelist info in the file,
// blah blah blah.
//
// But hey, priceslists! Each pricelist has a currency. And each pricelist has
// a list of parts attached to it. Not necessarily all the parts, as no one
// vendor may offer all the parts in the parts list.
void MainWindow::PricelistsSetup()
{
	// pricelists datastore
	GET_OBJECT(m_refBuilder,"pricelistsStore",m_pPricelistsStore,ListStore)
	
	// fill the datastore
	const char *cmd = 
		"SELECT pl.num,pl.description,c.name,c.code,c.rate FROM pricelists pl, currencies c WHERE pl.currency_code=c.code ORDER BY num";
	m_pSql->ExecQuery(cmd,MainWindow::PopulatePricelistsCallback);

	// pricelists view
	GET_WIDGET(m_refBuilder,"pricelistsView",m_pPricelistsView)
	
	// use pricelist toggled handler
	Gtk::CellRendererToggle *pCellRenderer;
	GET_TOGGLE_RENDERER("pricelistsUseCellRendererToggle",pCellRenderer,m_pPricelistsView,m_pricelistsStore.m_use.index())
	pCellRenderer->signal_toggled().connect(sigc::mem_fun(*this,&MainWindow::on_pricelist_use_toggled));

	// pricelist description edited handler
	Gtk::CellRendererText *pTextCellRenderer = NULL;
	GET_TEXT_RENDERER("priceslistsDescriptionCellRendererText",pTextCellRenderer,m_pPricelistsView,m_pricelistsStore.m_description.index())
	pTextCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_pricelists_description_edited));

	// pricelist currency changed handler
	Gtk::CellRendererCombo *pComboCellRenderer = NULL;
	GET_COMBO_RENDERER("pricelistsCurrencyCellRendererCombo",pComboCellRenderer,m_pPricelistsView,m_pricelistsStore.m_currencyName.index())
	pComboCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_pricelists_currency_edited));
	
	// pricelists context popup menu
	GET_WIDGET(m_refBuilder,"pricelistsContextMenu",m_pPricelistsContextMenu)

	// import prices menu item
	GET_WIDGET(m_refBuilder,"pricelistsImportPricesMenuItem",m_pPricelistsImportPricesMenuItem)
	m_pPricelistsImportPricesMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_pricelistsImportPrices_activated_event));

	// new pricelist menu item
	GET_WIDGET(m_refBuilder,"pricelistsNewPricelistMenuItem",m_pPricelistsNewPricelistMenuItem)
	m_pPricelistsNewPricelistMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_pricelistsNewPricelist_activated_event));

	// delete pricelist menu item
	GET_WIDGET(m_refBuilder,"pricelistsDeletePricelistMenuItem",m_pPricelistsDeletePricelistMenuItem)
	m_pPricelistsDeletePricelistMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_pricelistsDeletePricelist_activated_event));

	// popup context menu handler
	m_pPricelistsView->signal_button_press_event().connect_notify(sigc::mem_fun(*this,&MainWindow::on_pricelists_button_pressed));

	// new pricelist dialog
	m_pNewPricelistDialog = new NewPricelistDialog(m_refBuilder);
	if( !m_pNewPricelistDialog ) {
		throw "Could not get newPricelistDialog";
	}
  
	// import csv dialog
	GET_WIDGET(m_refBuilder,"importFileChooserDialog",m_pPricelistImportCsvDialog);
	// file list filer (CSV and All)
	Glib::RefPtr<Gtk::FileFilter> filter_csv = Gtk::FileFilter::create();
	filter_csv->set_name("CSV files");
	filter_csv->add_mime_type("text/csv");
	m_pPricelistImportCsvDialog->add_filter(filter_csv);
	Glib::RefPtr<Gtk::FileFilter> filter_any = Gtk::FileFilter::create();
	filter_any->set_name("All files");
	filter_any->add_pattern("*");
	m_pPricelistImportCsvDialog->add_filter(filter_any);

	// start up with the first pricelist selected
	// I probably should store the selected pricelist in the config file...
	if( m_pPricelistsStore->children().size() > 0 ) {
		Gtk::TreeModel::Children rows = m_pPricelistsStore->children();
		Gtk::TreeModel::iterator r = rows.begin();
		// mark the first pricelist as in use and store the currency info
		(*r)[m_pricelistsStore.m_use] = true;
		m_pricelistNumber = (*r)[m_pricelistsStore.m_num];
		m_pricelistCurrencyRate = (*r)[m_pricelistsStore.m_currencyRate];
		m_pricelistCurrencyCode = (*r)[m_pricelistsStore.m_currencyCode];
	} else {
		throw "Oh no! No priceslists in database";
	}
	m_pImportPricesResultDialog = new ImportPricesResultDialog(m_refBuilder);
	if( !m_pImportPricesResultDialog ) {
		throw "Could not get importPricesResultDialog";
	}
	// refresh the prices in the chosen currency
	RefreshPrices();
}

// fill the pricelists datastore
int MainWindow::PopulatePricelistsCallback(void *wnd, int argc, char **argv, char **azColName)
{
	MainWindow *pMainWindow = (MainWindow *)wnd;
	long num = atol(argv[0]);
	string description = argv[1];
	string currencyName = argv[2];
	string currencyCode = argv[3];
	double currencyRate = atof(argv[4]);
	Gtk::TreeModel::Row row = *(pMainWindow->m_pPricelistsStore->append());
	// the in use indicator will be filled in later on
	row[pMainWindow->m_pricelistsStore.m_use] = false;
	row[pMainWindow->m_pricelistsStore.m_num] = num;
	row[pMainWindow->m_pricelistsStore.m_description] = description;
	row[pMainWindow->m_pricelistsStore.m_currencyName] = currencyName;
	row[pMainWindow->m_pricelistsStore.m_currencyCode] = currencyCode;
	row[pMainWindow->m_pricelistsStore.m_currencyRate] = currencyRate;
	return 0;
}

// pricelist description edited
void MainWindow::on_pricelists_description_edited(Glib::ustring pathStr, Glib::ustring text)
{
	GET_ITER(iter,m_pPricelistsStore,pathStr)
	gint64 num = (*iter)[m_pricelistsStore.m_num];
	(*iter)[m_pricelistsStore.m_description] = text;
	stringstream sql;
	sql
		<< "UPDATE pricelists SET description='" << m_pSql->Escape(text)
		<< "' WHERE num=" << num;
	m_pSql->ExecNonQuery(&sql);
}

// pricelist currency edited: this one's a bit trickier than usual, since the currency
// column is a combo box. Gtk gives me the combo box text but there doesn't seem to
// be any way to get the associated currency code without manually searching the
// currencies datastore for the matching currency name.
void MainWindow::on_pricelists_currency_edited(Glib::ustring pathStr, Glib::ustring text)
{
	// get the pricelist number
	GET_ITER(iter,m_pPricelistsStore,pathStr)
	gint64 num = (*iter)[m_pricelistsStore.m_num];
	// there MUST be a better way of getting the active row
	// from a CellRendererCombo!
	Gtk::TreeModel::Children rows = m_pCurrenciesStore->children();
	string code;
	// search all currencies for a matching currency name
	for( Gtk::TreeModel::iterator r = rows.begin(); r != rows.end(); ++r ) {
		Gtk::TreeModel::Row row = *r;
		string currencyName = row[m_currenciesStore.m_name];
		if( currencyName == text ) {
			// found the currency, update the dasta store
			(*iter)[m_pricelistsStore.m_currencyCode] = code = row[m_currenciesStore.m_code];
			(*iter)[m_pricelistsStore.m_currencyName] = text;
			// if we've changed the currency type of the in use pricelist
			// then we'll need to change the in use currency rate
			bool inUse = (*iter)[m_pricelistsStore.m_use];
			if( inUse ) {
				m_pricelistCurrencyRate = (*iter)[m_pricelistsStore.m_currencyRate];
				m_pricelistCurrencyCode = (*iter)[m_pricelistsStore.m_currencyCode];
				RefreshPrices();
			}
			break;
		}
	}
	// we *should* always find the new currency, but just in case test for it
	if( !code.empty() ) {
		stringstream sql;
		sql
			<< "UPDATE pricelists SET currency_code='" << m_pSql->Escape(code)
			<< "' WHERE num=" << num;
		m_pSql->ExecNonQuery(&sql);
	}
}

// change the pricelist in use
void MainWindow::on_pricelist_use_toggled(const Glib::ustring &pathStr)
{
	// update the pricelist number and currency info
	GET_ITER(iter,m_pPricelistsStore,pathStr)
	m_pricelistNumber = (*iter)[m_pricelistsStore.m_num];
	m_pricelistCurrencyRate = (*iter)[m_pricelistsStore.m_currencyRate];
	m_pricelistCurrencyCode = (*iter)[m_pricelistsStore.m_currencyCode];
	// update the in use indicator in the datstore
	Gtk::TreeModel::Children rows = m_pPricelistsStore->children();
	for( Gtk::TreeModel::iterator r = rows.begin(); r != rows.end(); ++r ) {
		Gtk::TreeModel::Row row = *r;
		gint64 pricelistNum = row[m_pricelistsStore.m_num];
		row[m_pricelistsStore.m_use] = (m_pricelistNumber == pricelistNum);
	}
	// refresh the prices with the new currency rate
	RefreshPrices();
}

// when a new currency rate is used, the prices for the parts displayed
// have to be updated
void MainWindow::RefreshPrices()
{
	// Create a view of the part prices table in the currently chosen currency.
	// The part prices are stored in the pricelist's currency.
	stringstream sql;
	sql <<
		"DROP VIEW IF EXISTS v_part_prices;" <<
		"CREATE VIEW " <<
			"v_part_prices AS " <<
		"SELECT " <<
			"pp.pricelist_num," <<
			"pp.part_num," <<
			"round(pp.price/c.rate*" << m_baseCurrencyRate << ",2) price " <<
		"FROM " <<
			"part_prices pp," <<
			"pricelists pl," <<
			"currencies c " <<
		"WHERE " <<
			"pp.pricelist_num=pl.num AND pl.currency_code=c.code";
	m_pSql->ExecNonQuery(&sql);
	// this routine can be called during the program initialization
	// but the fill routines shouldn't be called until afterwards.
	if( m_initialized ) {
		FillParts();
		FillCollection();
		FillToMake();
	}
}

// pricelists context menu handler
void MainWindow::on_pricelists_button_pressed(GdkEventButton *pEvent)
{
	if( pEvent->type==GDK_BUTTON_PRESS && pEvent->button==GDK_BUTTON_SECONDARY ) {
		Gtk::TreeModel::Path path;
		if( m_pPricelistsView->get_path_at_pos((int)pEvent->x,(int)pEvent->y,path) ) {
			m_pPricelistsView->set_cursor(path);
		}
		// only activate the import prices function if there is a selected
		// pricelist
		//
		// only activate the delete pricelists function if there is a
		// selected pricelist and it's not the active one
		//
		// not allowing the active pricelist to be deleted means that
		// we will always have at least one pricelist available (assuming
		// the database starts off with one) and we never have to worry
		// about finding another pricelist when the active one was
		// deleted
		m_pPricelistsDeletePricelistMenuItem->set_sensitive(false);
		m_pPricelistsImportPricesMenuItem->set_sensitive(false);
		if( m_pPricelistsView->get_selection()->count_selected_rows() != 0 ) {
			m_pPricelistsImportPricesMenuItem->set_sensitive(true);
			Gtk::TreeModel::iterator iter = m_pPricelistsView->get_selection()->get_selected();
			gint64 selectedPricelistNum = (*iter)[m_pricelistsStore.m_num];
			m_pPricelistsDeletePricelistMenuItem->set_sensitive(selectedPricelistNum != m_pricelistNumber);
		}
		m_pPricelistsContextMenu->popup(pEvent->button,pEvent->time);
	}
}

// Import prices to pricelist. This is more basic than I would like, but it
// does the job. The first column should be the part number, and the second
// the part price in the currency of the pricelist.
void MainWindow::on_pricelistsImportPrices_activated_event()
{
	// get a csv file
	int response = m_pPricelistImportCsvDialog->run();
	m_pPricelistImportCsvDialog->hide();
	if( response == Gtk::RESPONSE_OK ) {
		// grab the csv file name
		if( !m_pPricelistImportCsvDialog->get_filename().empty() ) {
			Gtk::TreeModel::iterator iter = m_pPricelistsView->get_selection()->get_selected();
			// selected pricelist number
			gint64 selectedPricelistNum = (*iter)[m_pricelistsStore.m_num];
			// set up to read the csv file
			CsvReader reader(m_pPricelistImportCsvDialog->get_filename());
			vector<string> fields;

			WaitCursor(true);
			int numUpdated = 0;                 // # of part prices updated
			int numInserted = 0;                // # of part prices inserted
			vector<string> unknownPartNumbers;  // part prices not found in database
			while( reader.Parse(fields) ) {
				// some basic data checking: 2, and only 2, non empty fields
				if( fields.size() == 2 && !fields[0].empty() && !fields[1].empty() ) {
					double price = atof(fields[1].c_str());
					string num = fields[0];
					// trying updating an existing part price
					stringstream sql;
					sql
						<< "UPDATE part_prices SET price=" << price 
						<< " WHERE part_num='" << m_pSql->Escape(num) << "' AND pricelist_num=" << selectedPricelistNum;
					// ExecUpdate returns the # of rows updated. 0 rows mean the part
					// didn't already have a price
					if( m_pSql->ExecUpdate(&sql) == 0 ) {
						// This part didn't already exist in the currently selected pricelist.
						// There are 2 possibilities. The 1st is that the part itself exists,
						// but has never been added to this pricelist. That's fine; we'll go
						// ahead and add it. But it's also possible that the part itself
						// doesn't even exist, in which case we want to skip adding it.
						//
						// It should really be reported at some point too.
						if( PartExists(num) ) {
							sql.str("");
							sql
								<< "INSERT INTO part_prices(pricelist_num,part_num,price) VALUES ("
								<< selectedPricelistNum
								<< ",'" << m_pSql->Escape(num) << "',"
								<< price << ")";
							m_pSql->ExecInsert(&sql);
							++numInserted;
						} else {
							unknownPartNumbers.push_back(num);
						}
					} else {
						++numUpdated;
					}
				}
			}
			WaitCursor(false);
			FillCollection();
			FillToMake();
			// show the import results
			m_pImportPricesResultDialog->Run(numInserted,numUpdated,unknownPartNumbers);
			m_pImportPricesResultDialog->Hide();
		}
	}
}

// create a new pricelist
void MainWindow::on_pricelistsNewPricelist_activated_event()
{
	string errorLabel = "";
	while( true ) {
		if( m_pNewPricelistDialog->Run(errorLabel) == Gtk::RESPONSE_OK ) {
			// insert the new pricelist doesn't already exist
			string new_description = m_pNewPricelistDialog->Description();
			Gtk::TreeModel::Row newRow;
			bool alreadyExists = false;
			bool inserted = false;
			Gtk::TreeModel::Children pricelistsRows = m_pPricelistsStore->children();
			for( Gtk::TreeModel::iterator iter = pricelistsRows.begin(); iter!= pricelistsRows.end(); ++iter ) {
				Gtk::TreeModel::Row pricelistsRow = *iter;
				string description = pricelistsRow[m_pricelistsStore.m_description];
				if( new_description == description ) {
					// would duplicate an existing list
					errorLabel = "Pricelist "+new_description+" already exists!";
					alreadyExists = true;
					break;
				} else if( new_description < description ) {
					// found the insertion point
					newRow = *(m_pPricelistsStore->insert(iter));
					inserted = true;
					break;
				}
			}
			if( !alreadyExists ) {
				errorLabel = "";
				if( !inserted ) {
					// add pricelist to end of datastore
					newRow = *(m_pPricelistsStore->append());
				}
				// add to database
				stringstream sql;
				sql <<
					"INSERT INTO pricelists(description,currency_code) VALUES " <<
					"('" << m_pSql->Escape(new_description) << "','" <<
					m_pSql->Escape(m_pNewPricelistDialog->CurrencyCode()) << "')";
				// ExecInsert returns the new row id
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
	m_pNewPricelistDialog->Hide();
}

// delete a pricelist: this function is only enabled when the selected pricelist
// isn't the one in use (avoids a whole raft of issues)
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
// Currencies: each pricelist has a currency type and rate.
// Currency exchange rate can be updated from the European Central Bank, which
// very kindly provides an XML file with exchange rate info.
void MainWindow::CurrenciesSetup()
{
	// get the user's locale (used as the default currency rate)
	struct lconv *locale;
	locale=localeconv();
	string currSymbol = locale->int_curr_symbol;
	m_localeCurrencyCode = currSymbol.substr(0,3);

	// currency datastore
	GET_OBJECT(m_refBuilder,"currenciesStore",m_pCurrenciesStore,ListStore)
	// fill the currency datastore
	const char *cmd =
		"SELECT code,name,rate FROM currencies ORDER BY code";
	m_pSql->ExecQuery(cmd,MainWindow::PopulateCurrenciesCallback);

	// currency treeview
	GET_WIDGET(m_refBuilder,"currenciesView",m_pCurrenciesView)
	int rateIndex = m_pCurrenciesView->append_column_numeric_editable("Rate",m_currenciesStore.m_rate,"%g")-1;

	// currency popup context menu
	GET_WIDGET(m_refBuilder,"currenciesContextMenu",m_pCurrenciesContextMenu)

	// update currency rates menu item
	GET_WIDGET(m_refBuilder,"currenciesUpdateCurrencyMenuItem",m_pCurrenciesUpdateCurrencyMenuItem)
	m_pCurrenciesUpdateCurrencyMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_currenciesUpdateCurrency_activated_event));

	// add new currency menu item
	GET_WIDGET(m_refBuilder,"currenciesNewCurrencyMenuItem",m_pCurrenciesNewCurrencyMenuItem)
	m_pCurrenciesNewCurrencyMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_currenciesNewCurrency_activated_event));

	// delete currency menu item
	GET_WIDGET(m_refBuilder,"currenciesDeleteCurrencyMenuItem",m_pCurrenciesDeleteCurrencyMenuItem)
	m_pCurrenciesDeleteCurrencyMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_currenciesDeleteCurrency_activated_event));

	// currency in used toggled handler
	Gtk::CellRendererToggle *pToggleCellRenderer = NULL;
	GET_TOGGLE_RENDERER("currenciesUseCellRendererToggle",pToggleCellRenderer,m_pCurrenciesView,m_currenciesStore.m_use.index())
	pToggleCellRenderer->signal_toggled().connect(sigc::mem_fun(*this,&MainWindow::on_currency_use_toggled));

	// currency context popup menu
	m_pCurrenciesView->signal_button_press_event().connect_notify(sigc::mem_fun(*this,&MainWindow::on_currencies_button_pressed));

	// currency name edited handler
	Gtk::CellRendererText *pTextCellRenderer = NULL;
	GET_TEXT_RENDERER("currenciesNameCellRendererText",pTextCellRenderer,m_pCurrenciesView,m_currenciesStore.m_name.index())
	pTextCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_currencies_name_edited));

	// currency rate edited handler
	GET_TEXT_RENDERER("currenciesRateCellRendererText",pTextCellRenderer,m_pCurrenciesView,rateIndex)
	pTextCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_currencies_rate_edited));

	m_pNewCurrencyDialog = new NewCurrencyDialog(m_refBuilder);
	if( !m_pNewCurrencyDialog ) {
		throw "Could not get newCurrencyDialog";
	}

	// set the currency in use to the user's locale currency
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
	GET_ITER(iter,m_pCurrenciesStore,pathStr)
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
	RefreshPrices();
}

void MainWindow::on_currencies_button_pressed(GdkEventButton *pEvent)
{
	if( pEvent->type==GDK_BUTTON_PRESS && pEvent->button==GDK_BUTTON_SECONDARY ) {
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

int MainWindow::PopulateCurrenciesCallback(void *wnd, int argc, char **argv, char **azColName)
{
	MainWindow *pMainWindow = (MainWindow *)wnd;
	string code = argv[0];
	string name = argv[1];
	double rate = atof(argv[2]);
	Gtk::TreeModel::Row row = *(pMainWindow->m_pCurrenciesStore->append());
	row[pMainWindow->m_currenciesStore.m_use] = (code==pMainWindow->m_localeCurrencyCode);
	row[pMainWindow->m_currenciesStore.m_code] = code;
	row[pMainWindow->m_currenciesStore.m_name] = name;
	row[pMainWindow->m_currenciesStore.m_rate] = rate;
	if( code==pMainWindow->m_localeCurrencyCode ) {
		row[pMainWindow->m_currenciesStore.m_use] = true;
		pMainWindow->m_baseCurrencyCode = code;
		pMainWindow->m_baseCurrencyRate = rate;
	} else {
		row[pMainWindow->m_currenciesStore.m_use] = false;
	}
	return 0;
}

// Update the currency exchange rates. The European Central Bank very kindly
// provides an XML file with exchange rates against the Euro.
void MainWindow::on_currenciesUpdateCurrency_activated_event()
{
	// a temporary storage place to download the XML exchange rate file to
	string destFilename = Glib::build_filename(Glib::build_filename(Glib::get_user_cache_dir(),Glib::get_prgname()),"eurofxref-daily.xml");
	string destDir = Glib::path_get_dirname(destFilename);
	// make sure the directory actually exists
	g_mkdir_with_parents(destDir.c_str(), 0755);
	// Use wget to fetch the file. There's a Gtk way to do this, but I could never
	// quite figure it out.
	stringstream cmd;
	cmd << "wget -O \"" << destFilename << "\" http://www.ecb.europa.eu/stats/eurofxref/eurofxref-daily.xml";
	if( system(cmd.str().c_str()) == EXIT_SUCCESS ) {
		// I don't even remember where I cribbed this code from!
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
				// Got a currency code and rate. Try updating the currencies table.
				// If the currency code isn't in there, it won't do anything.
				if( !currencyCode.empty() && currencyRate > 0 ) {
					stringstream sql;
					sql << "UPDATE currencies SET rate=" << currencyRate << " WHERE code='" << m_pSql->Escape(currencyCode) << "'";
					if( m_pSql->ExecUpdate(&sql) > 0 ) {
						// update the currencies datastore
						Gtk::TreeModel::Children currencyRows = m_pCurrenciesStore->children();
						for( Gtk::TreeModel::iterator r = currencyRows.begin(); r!=currencyRows.end(); ++r ) {
							Gtk::TreeModel::Row currencyRow = *r;
							string lookupCode = currencyRow[m_currenciesStore.m_code];
							if( lookupCode == currencyCode ) {
								// found the currency code, update it
								currencyRow[m_currenciesStore.m_rate] = currencyRate;
								if( currencyCode == m_baseCurrencyCode ) {
									// update the base currency rate
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
		// and now we're back to my code
		RefreshPrices();
	}
}

// add a new currency
void MainWindow::on_currenciesNewCurrency_activated_event()
{
	string errorLabel = "";
	while( true ) {
		if( m_pNewCurrencyDialog->Run(errorLabel) == Gtk::RESPONSE_OK ) {
			// get the user info
			string new_description = m_pNewCurrencyDialog->Description();
			string new_currency = m_pNewCurrencyDialog->CurrencyCode();
			double new_rate = m_pNewCurrencyDialog->CurrencyRate();
			Gtk::TreeModel::Row newRow;
			// insert the new currency into the datastore if it doesn't already exist
			bool alreadyExists = false;
			bool inserted = false;
			Gtk::TreeModel::Children currenciesRows = m_pCurrenciesStore->children();
			for( Gtk::TreeModel::iterator iter = currenciesRows.begin(); iter!= currenciesRows.end(); ++iter ) {
				Gtk::TreeModel::Row currenciesRow = *iter;
				string description = currenciesRow[m_currenciesStore.m_name];
				if( new_description == description ) {
					// no duplicates please!
					errorLabel = "Currency "+new_description+" already exists!";
					alreadyExists = true;
					break;
				} else if( new_description < description ) {
					// found the insertion point
					newRow = *(m_pCurrenciesStore->insert(iter));
					inserted = true;
					break;
				}
			}
			if( !alreadyExists ) {
				errorLabel = "";
				if( !inserted ) {
					// not inserted, add to add of datastore
					newRow = *(m_pCurrenciesStore->append());
				}
				// add to database
				stringstream sql;
				sql
					<< "INSERT INTO currencies(name,code,rate) VALUES "
					<< "('" << m_pSql->Escape(new_description) << "','"
					<< m_pSql->Escape(new_currency) << "',"
					<< new_rate << ")";
				m_pSql->ExecInsert(&sql);
				// add to datastore
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
	m_pNewCurrencyDialog->Hide();
}

// delete a currency (and any pricelists that use it)
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
			// delete the currency from the datastore
			m_pCurrenciesStore->erase(iter);
			// and delete any priceslists that use it
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
			// delete the currency from the database.
			// the PRAMGA foreign_keys = on; performs a cascade delete and gets rid
			// of any pricelists and part prices associated with the currency
			stringstream sql;
			sql << "PRAGMA foreign_keys = on; DELETE FROM currencies WHERE code='" << code << "'";
			m_pSql->ExecNonQuery(&sql);
			RefreshPrices();
		}
	}
}

// currency name edited handler
void MainWindow::on_currencies_name_edited(Glib::ustring pathStr, Glib::ustring text)
{
	GET_ITER(iter,m_pCurrenciesStore,pathStr)
	string code = (*iter)[m_currenciesStore.m_code];
	(*iter)[m_currenciesStore.m_name] = text;
	stringstream sql;
	sql
		<< "UPDATE currencies SET name='" << m_pSql->Escape(text)
		<< "' WHERE code='" << m_pSql->Escape(code) << "'";
	m_pSql->ExecNonQuery(&sql);
	RefreshPrices();
}

// currency rate edited handler
void MainWindow::on_currencies_rate_edited(Glib::ustring pathStr, Glib::ustring text)
{
	GET_ITER(iter,m_pCurrenciesStore,pathStr)
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
	RefreshPrices();
}
#endif
