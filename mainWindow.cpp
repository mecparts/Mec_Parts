#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <assert.h>
#include <boost/regex.hpp>
#include "mainWindow.h"

static int PopulatePricelistsCallback(void *wnd, int argc, char **argv, char **azColName);
static int PopulatePartsCallback(void *wnd, int argc, char **argv, char **azColName);
static int PopulateSetsCallback(void *wnd, int argc, char **argv, char **azColName);
static int PopulateCollectionCallback(void *wnd, int argc, char **argv, char **azColName);
static int AddSetPartCallback(void *wnd,int argc,char **argv, char **azColName);
static int PopulateNeededCallback(void *wnd, int argc, char **argv, char **azColName);

using namespace std;

#ifndef _Constructor_Destructor
MainWindow::MainWindow() :
	m_pWindow(NULL),
  m_initialized(false),
	m_refBuilder(NULL),
	m_pSql(NULL),
	m_bSetsFiltered(false),
	m_pNewDatabaseMenuItem(NULL),
	m_pOpenDatabaseMenuItem(NULL),
	m_pProgramQuitMenuItem(NULL),
	m_pImportPricelistMenuItem(NULL),
	m_pAboutMenuItem(NULL),
	m_pPricelistMenu(NULL),
	m_pPricelistStore(NULL),
	m_pricelistNumber(0),
	m_pFirstPricelistMenuItem(NULL),
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
	m_pSetsView(NULL),
	m_pSetsStore(NULL),
	m_pSetsContextMenu(NULL),
	m_pSetsNewSetMenuItem(NULL),
	m_pSetsDeleteSetMenuItem(NULL),
	m_pNeededView(NULL),
	m_pNeededStore(NULL),
	m_pNeededCost(NULL),
	m_pNeededHaveComboBox(NULL),
	m_pNeededWantComboBox(NULL),
	m_pNeededContextMenu(NULL),
	m_pNeededViewPartMenuItem(NULL),
	m_neededPartsCount(0),
	m_neededCost(0),
	m_pSelectPartsDialog(NULL),
	m_pSelectSetDialog(NULL),
	m_pNewPartDialog(NULL),
	m_pNewSetDialog(NULL)
{
	m_pSql = new Sql(this,"meccano.sqlite");

	// Load the GtkBuilder file and instantiate its widgets
	m_refBuilder = Gtk::Builder::create();
	if( !m_refBuilder ) {
		throw "null m_refBuilder";
	}
	try {
		m_refBuilder->add_from_file("mecparts.glade");
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

	GET_WIDGET(m_refBuilder,"newDatabaseMenuItem",m_pNewDatabaseMenuItem);
	m_pNewDatabaseMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_new_database));

	GET_WIDGET(m_refBuilder,"openDatabaseMenuItem",m_pOpenDatabaseMenuItem);
	m_pOpenDatabaseMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_open_database));

	GET_WIDGET(m_refBuilder,"programQuitMenuItem",m_pProgramQuitMenuItem);
	m_pProgramQuitMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_program_quit));

	GET_WIDGET(m_refBuilder,"importPricelistMenuItem",m_pImportPricelistMenuItem);
	m_pImportPricelistMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_import_pricelist));

	GET_WIDGET(m_refBuilder,"aboutMenuItem",m_pAboutMenuItem);
	m_pAboutMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_about));

	m_cfg.load_cfg();
	int w,h;
	m_cfg.get_mainWindow_size(w,h);
	m_pWindow->set_default_size(w,h);

	int x,y;
	m_cfg.get_mainWindow_pos(x,y);
	if( x != -1 && x != -1 ) {
		m_pWindow->move(x,y);
	}

	PricelistsSetup();
	PartsSetup();
	SetsSetup();
	CollectionSetup();
	NeededSetup();

	m_initialized = true;
	if( m_pPricelistStore->children().size() > 0 ) {
		m_pFirstPricelistMenuItem->toggled();
	}
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

void MainWindow::on_new_database()
{
	// new
}

void MainWindow::on_open_database()
{
	// open
}

void MainWindow::on_import_pricelist()
{
	Gtk::FileChooserDialog *pImport = NULL;
	GET_WIDGET(m_refBuilder,"importFileChooserDialog",pImport);
	pImport->run();
	pImport->hide();
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
#ifndef _Pricelists_Routines
void MainWindow::PricelistsSetup()
{
	GET_WIDGET(m_refBuilder,"pricelistMenu",m_pPricelistMenu);
	
	m_pPricelistStore = Glib::RefPtr<Gtk::ListStore>::cast_static(m_refBuilder->get_object("pricelistsStore"));
	if( !m_pPricelistStore ) {
		throw "Could not get pricelistStore object";
	}

	const char *cmd = 
		"SELECT num,description FROM pricelists ORDER BY num";
	m_pSql->ExecQuery(cmd,PopulatePricelistsCallback);

	Gtk::TreeModel::Children rows = m_pPricelistStore->children();
	Gtk::RadioMenuItem::Group group;
	bool selected = true;
	for( Gtk::TreeModel::iterator r = rows.begin(); r != rows.end(); ++r ) {
		Gtk::TreeModel::Row row = *r;
		gint64 num = row[m_pricelistStore.m_num];
		string description = row[m_pricelistStore.m_description];
		Gtk::RadioMenuItem *pMenuItem = new Gtk::RadioMenuItem(group,description);
		m_pPricelistMenu->append(*Gtk::manage(pMenuItem));
		pMenuItem->show();
		pMenuItem->signal_toggled().connect( sigc::bind<Gtk::RadioMenuItem *,gint64>(sigc::mem_fun(*this,&MainWindow::on_pricelist_toggled_event),pMenuItem,num));
		if( selected ) {
			m_pFirstPricelistMenuItem = pMenuItem;
			selected = false;
			group = pMenuItem->get_group();
		}
	}
}

void MainWindow::PopulatePricelists(gint64 num,string description)
{
	Gtk::TreeModel::Row row = *(m_pPricelistStore->append());
	row[m_pricelistStore.m_num] = num;
	row[m_pricelistStore.m_description] = description;
}

void MainWindow::on_pricelist_toggled_event(Gtk::RadioMenuItem *pMenuItem,gint64 rowId)
{
	if( pMenuItem->get_active() ) {
		m_pricelistNumber = rowId;
		if( m_initialized ) {
			FillCollection();
			FillParts();
			on_neededComboBox_changed_event();
		}
	}
}
#endif
#ifndef _Parts_Routines
void MainWindow::PartsSetup()
{
	m_pPartsStore = Glib::RefPtr<Gtk::ListStore>::cast_static(m_refBuilder->get_object("partsStore"));
	if( !m_pPartsStore ) {
		throw "Could not get partsStore object";
	}

	GET_WIDGET(m_refBuilder,"partsView",m_pPartsView);
	m_pPartsView->append_column_numeric_editable("Price",m_partsStore.m_price,"%.2lf");
	m_pPartsView->append_column_editable("Notes",m_partsStore.m_notes);

	Gtk::CellRendererText *pCellRenderer;
	CELL_RENDERER("partsDescriptionCellRenderer",pCellRenderer,m_pPartsView,m_partsStore.m_description.index())
	pCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_parts_description_edited));

	CELL_RENDERER("partsSizeCellRenderer",pCellRenderer,m_pPartsView,m_partsStore.m_size.index())
	pCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_parts_size_edited));

	CELL_RENDERER("partsPriceCellRenderer",pCellRenderer,m_pPartsView,m_partsStore.m_price.index())
	pCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_parts_price_edited));

	CELL_RENDERER("partsNotesCellRenderer",pCellRenderer,m_pPartsView,m_partsStore.m_notes.index())
	pCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_parts_notes_edited));

  m_pPartsView->get_selection()->signal_changed().connect(sigc::mem_fun(*this,&MainWindow::on_parts_selection_changed_event));
   m_pPartsView->signal_button_press_event().connect_notify(sigc::mem_fun(*this,&MainWindow::on_parts_button_pressed));
		
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

	m_pNewSetDialog = new NewSetDialog(m_refBuilder);
	if( !m_pNewSetDialog ) {
		throw "Could not get newSetDialog";
	}

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

void MainWindow::FillParts()
{
	if( m_pPartsStore->children().size() > 0 ) {
		m_pPartsStore->clear();
	}
	stringstream cmd;
	cmd 
		<< "SELECT p.rowid,p.num,p.description,p.size,IFNULL(pp.price,0) price,"
		<< "p.notes,p.pnPrefix a,p.pnDigits b,p.pnSuffix c "
		<< "FROM parts p LEFT OUTER JOIN part_prices pp ON p.num=pp.part_num AND pp.pricelist_num=" << m_pricelistNumber
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
	on_collection_set_combobox_changed_event();
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
	on_collection_set_combobox_changed_event();
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
		<< "UPDATE part_prices SET price=" << price 
		<< " WHERE part_num='" << m_pSql->Escape(num) << "' AND pricelist_num=" << m_pricelistNumber;
	if( m_pSql->ExecUpdate(&sql) == 0 ) {
		sql.str("");
		sql
			<< "INSERT INTO part_prices(pricelist_num,part_num,price) VALUES ("
			<< m_pricelistNumber
			<< ",'" << m_pSql->Escape(num) << "',"
			<< price << ")";
		m_pSql->ExecInsert(&sql);
	}
	on_collection_set_combobox_changed_event();
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
	on_collection_set_combobox_changed_event();
}
#endif
void MainWindow::on_parts_selection_changed_event()
{
	bool partSelected = m_pPartsView->get_selection()->count_selected_rows() != 0;
	m_pPartsViewPartMenuItem->set_sensitive(partSelected);
	m_pPartsDeletePartMenuItem->set_sensitive(partSelected);
	m_pPartsFilterSetsMenuItem->set_sensitive(partSelected);
	m_pPartsUnfilterSetsMenuItem->set_sensitive(m_bSetsFiltered);
	m_partsPartNumber = m_partsPartDescription = "";
	Gtk::TreeModel::iterator iter = m_pPartsView->get_selection()->get_selected();
	if( iter ) {
		Gtk::TreeModel::Row row = *iter;
		m_partsPartNumber = row[m_partsStore.m_partNumber];
		m_partsPartDescription = row[m_partsStore.m_description];
	}
}

void MainWindow::on_parts_button_pressed(GdkEventButton *pEvent)
{
	if( pEvent->type==GDK_BUTTON_PRESS && pEvent->button==3 ) {
		Gtk::TreeModel::Path path;
		if( m_pPartsView->get_path_at_pos((int)pEvent->x,(int)pEvent->y,path) ) {
			m_pPartsView->set_cursor(path);
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
			boost::regex pnRegex("([A-Za-z]*?)([0-9]*?)([A-Za-z0-9]*?)");
			boost::smatch matches;
			string new_partNum = m_pNewPartDialog->PartNumber();
			if( !boost::regex_match(new_partNum,matches,pnRegex) || matches.size()!=4) {
				throw "Regex failed";
			}
			string new_pnPrefix = matches[1];
			int new_pnDigits = atoi(matches[2].str().c_str());
			string new_pnSuffix = matches[3];
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
					"INSERT INTO parts(pnPrefix,pnDigits,pnSuffix,num,description,size,notes,price) VALUES ("
					<< "'" << m_pSql->Escape(new_pnPrefix) << "'," << new_pnDigits << ",'" << m_pSql->Escape(new_pnSuffix) << "','" << 
					new_partNum << "','" <<
					m_pNewPartDialog->Description() << "','" <<
					m_pNewPartDialog->Size() << "','" <<
					m_pNewPartDialog->Notes() << "'," <<
					m_pNewPartDialog->Price() << ")";
				newRow[m_partsStore.m_rowId] = m_pSql->ExecInsert(&sql);
				newRow[m_partsStore.m_partNumber] = new_partNum;
				newRow[m_partsStore.m_pnPrefix] = new_pnPrefix;
				newRow[m_partsStore.m_pnDigits] = new_pnDigits;
				newRow[m_partsStore.m_pnSuffix] = new_pnSuffix;
				newRow[m_partsStore.m_description] = m_pNewPartDialog->Description();
				newRow[m_partsStore.m_size] = m_pNewPartDialog->Size();
				newRow[m_partsStore.m_notes] = m_pNewPartDialog->Notes();
				newRow[m_partsStore.m_price] = m_pNewPartDialog->Price();
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
			Gtk::TreeModel::Children collectionRows = m_pCollectionStore->children();
			for( Gtk::TreeModel::iterator r = collectionRows.begin(); r!=collectionRows.end(); ++r ) {
				Gtk::TreeModel::Row collectionRow = *r;
				string collectionPartNumber = collectionRow[m_collectionStore.m_partNumber];
				if( collectionPartNumber == partNum ) {
					m_pCollectionStore->erase(r);
					CalculateCollectionTotals();
					break;
				}
			}
		}
		stringstream sql;
		sql << "DELETE FROM set_parts WHERE part_num='" << m_pSql->Escape(partNum) << "'; DELETE FROM parts WHERE num='" << m_pSql->Escape(partNum) << "'";
		m_pSql->ExecNonQuery(&sql);
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
#ifndef _Sets_Routines
void MainWindow::SetsSetup()
{
		m_pSetsStore = Glib::RefPtr<Gtk::ListStore>::cast_static(m_refBuilder->get_object("setsStore"));
	  if( !m_pSetsStore ) {
			throw "Could not get setsStore object";
		}
		
	  GET_WIDGET(m_refBuilder,"setsView",m_pSetsView)
		m_pSetsView->append_column_numeric_editable("Started",m_setsStore.m_started,"%d");
		m_pSetsView->append_column_numeric_editable("Ended",m_setsStore.m_ended,"%d");
		
		Gtk::CellRendererText *pCellRenderer;
		CELL_RENDERER("setsDescriptionCellRenderer",pCellRenderer,m_pSetsView,m_setsStore.m_description.index())
		pCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_sets_description_edited));

		CELL_RENDERER("setsStartedCellRenderer",pCellRenderer,m_pSetsView,m_setsStore.m_started.index())
		pCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_sets_started_edited));

		CELL_RENDERER("setsEndedCellRenderer",pCellRenderer,m_pSetsView,m_setsStore.m_ended.index())
		pCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_sets_ended_edited));

    m_pSetsView->signal_button_press_event().connect_notify(sigc::mem_fun(*this,&MainWindow::on_sets_button_pressed));
	  m_pSetsView->get_selection()->signal_changed().connect(sigc::mem_fun(*this,&MainWindow::on_sets_selection_changed_event));

		GET_WIDGET(m_refBuilder,"setsContextMenu",m_pSetsContextMenu)
		GET_WIDGET(m_refBuilder,"setsNewSetMenuItem",m_pSetsNewSetMenuItem)
		m_pSetsNewSetMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_setsNewSet_activated_event));

		GET_WIDGET(m_refBuilder,"setsDeleteSetMenuItem",m_pSetsDeleteSetMenuItem)
		m_pSetsDeleteSetMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_setsDeleteSet_activated_event));

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

void MainWindow::on_sets_selection_changed_event()
{
	m_pSetsDeleteSetMenuItem->set_sensitive(m_pSetsView->get_selection()->count_selected_rows() != 0);
}

void MainWindow::on_sets_button_pressed(GdkEventButton *pEvent)
{
	if( pEvent->type==GDK_BUTTON_PRESS && pEvent->button==3 ) {
		Gtk::TreeModel::Path path;
		if( m_pSetsView->get_path_at_pos((int)pEvent->x,(int)pEvent->y,path) ) {
			m_pSetsView->set_cursor(path);
		}
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
		}
		stringstream sql;
		sql << "DELETE FROM set_parts WHERE set_num='" << m_pSql->Escape(setNum) << "'; DELETE FROM sets WHERE num='" << m_pSql->Escape(setNum) << "'";
		m_pSql->ExecNonQuery(&sql);
	}
}
#endif
#ifndef _Collection_Routines
void MainWindow::CollectionSetup()
{
  GET_WIDGET(m_refBuilder,"collectionView",m_pCollectionView)
  m_pCollectionView->get_selection()->signal_changed().connect(sigc::mem_fun(*this,&MainWindow::on_collection_selection_changed_event));
   m_pCollectionView->signal_button_press_event().connect_notify(sigc::mem_fun(*this,&MainWindow::on_collection_button_pressed));

	GET_WIDGET(m_refBuilder,"collectionCountCost",m_pCollectionCountCost);
		
  GET_WIDGET(m_refBuilder,"collectionView",m_pCollectionView)
	
	m_pCollectionStore = Glib::RefPtr<Gtk::ListStore>::cast_static(m_refBuilder->get_object("collectionStore"));
  if( !m_pCollectionStore ) {
		throw "Could not get collectionStore object";
	}
		
	m_pCollectionView->append_column_numeric("Price",m_collectionStore.m_price,"%.2lf");

	Gtk::CellRendererText *pCellRenderer;
	CELL_RENDERER("collectionCountCellRenderer",pCellRenderer,m_pCollectionView,m_collectionStore.m_count.index())
	pCellRenderer->signal_edited().connect(sigc::mem_fun(*this,&MainWindow::on_collection_count_edited));
		
	m_pCollectionView->append_column_numeric("Total",m_collectionStore.m_total,"%.2lf");
	
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
}

void MainWindow::on_collection_selection_changed_event()
{
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
}

void MainWindow::on_collection_button_pressed(GdkEventButton *pEvent)
{
	if( pEvent->type==GDK_BUTTON_PRESS && pEvent->button==3 ) {
		Gtk::TreeModel::Path path;
		if( m_pCollectionView->get_path_at_pos((int)pEvent->x,(int)pEvent->y,path) ) {
			m_pCollectionView->set_cursor(path);
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
  if(iter)
  {
    Gtk::TreeModel::Row row = *iter;
    if(row)
    {
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
	sql << "INSERT INTO set_parts(set_num,part_num,count) VALUES ('" << m_pSql->Escape(m_collectionNumber) << "','" << m_pSql->Escape(partNumber) << "'," << count << ")";
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
		<< "LEFT OUTER JOIN part_prices pp ON p.num=pp.part_num AND pp.pricelist_num=" << m_pricelistNumber
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
#ifndef _Needed_Routines
void MainWindow::NeededSetup()
{
	  GET_WIDGET(m_refBuilder,"neededView",m_pNeededView)
    m_pNeededView->signal_button_press_event().connect_notify(sigc::mem_fun(*this,&MainWindow::on_needed_button_pressed));

		GET_WIDGET(m_refBuilder,"neededCost",m_pNeededCost);
	
		m_pNeededStore = Glib::RefPtr<Gtk::ListStore>::cast_static(m_refBuilder->get_object("neededStore"));
	  if( !m_pNeededStore ) {
			throw "Could not get neededStore object";
		}
		
		m_pNeededView->append_column_numeric("Price",m_neededStore.m_price,"%.2lf");

		m_pNeededView->append_column_numeric("Total",m_neededStore.m_total,"%.2lf");
	
		GET_WIDGET(m_refBuilder,"neededHaveComboBox",m_pNeededHaveComboBox);
		m_pNeededHaveComboBox->pack_start(m_setsStore.m_description);
		m_pNeededHaveComboBox->signal_changed().connect(sigc::mem_fun(*this,&MainWindow::on_neededComboBox_changed_event));

		GET_WIDGET(m_refBuilder,"neededWantComboBox",m_pNeededWantComboBox);
		m_pNeededWantComboBox->pack_start(m_setsStore.m_description);
		m_pNeededWantComboBox->signal_changed().connect(sigc::mem_fun(*this,&MainWindow::on_neededComboBox_changed_event));

		GET_WIDGET(m_refBuilder,"neededContextMenu",m_pNeededContextMenu)
		
		GET_WIDGET(m_refBuilder,"neededViewPartMenuItem",m_pNeededViewPartMenuItem)
		m_pNeededViewPartMenuItem->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::on_neededViewPart_activated_event));

}

void MainWindow::on_needed_button_pressed(GdkEventButton *pEvent)
{
	if( pEvent->type==GDK_BUTTON_PRESS && pEvent->button==3 ) {
		Gtk::TreeModel::Path path;
		if( m_pNeededView->get_path_at_pos((int)pEvent->x,(int)pEvent->y,path) ) {
			m_pNeededView->set_cursor(path);
			m_pNeededContextMenu->popup(pEvent->button,pEvent->time);
		}
	}
}

void MainWindow::on_neededViewPart_activated_event()
{
	Gtk::TreeModel::iterator iter = m_pNeededView->get_selection()->get_selected();
	if( iter ) {
		Gtk::TreeModel::Row row = *iter;
		string partNumber = row[m_neededStore.m_partNumber];
		string description = row[m_neededStore.m_description];
		string size = row[m_neededStore.m_size];
		DisplayPicture(partNumber,description,size);
	}
}

void MainWindow::on_neededComboBox_changed_event()
{
	m_pNeededStore->clear();
	m_neededPartsCount = 0;
	m_neededCost = 0;
	m_pNeededCost->set_text("");
  Gtk::TreeModel::iterator iterHave = m_pNeededHaveComboBox->get_active();
	Gtk::TreeModel::iterator iterWant = m_pNeededWantComboBox->get_active();
  if(iterHave && iterWant)
  {
    Gtk::TreeModel::Row haveRow = *iterHave;
    Gtk::TreeModel::Row wantRow = *iterWant;
    if(haveRow && wantRow)
    {
			string haveNum = haveRow[m_setsStore.m_setNumber];
			string wantNum = wantRow[m_setsStore.m_setNumber];
			FillNeeded(haveNum,wantNum);
		}
	}
}

void MainWindow::FillNeeded(string haveNum,string wantNum)
{
	if( m_pNeededStore->children().size() > 0 ) {
		m_pNeededStore->clear();
		m_neededPartsCount = 0;
		m_neededCost = 0;
		m_pNeededCost->set_text("");
	}
	stringstream cmd;
	cmd
	  << "DROP VIEW IF EXISTS v_set_parts; CREATE TEMP VIEW v_set_parts AS SELECT "
	  << "s.num set_num,s.description set_description,p.num part_num,p.description part_description,p.size,"
	  << "sp.count,IFNULL(pp.price,0) price,sp.count*IFNULL(pp.price,0) total " 
		<< "FROM sets s INNER join set_parts sp ON s.num=sp.set_num JOIN parts p ON p.num=sp.part_num "
		<< "LEFT OUTER JOIN part_prices pp ON p.num=pp.part_num AND pp.pricelist_num=" << m_pricelistNumber
		<< " ORDER BY p.pnPrefix,p.pnDigits,p.pnSuffix; "
		<< "DROP TABLE IF EXISTS have; DROP TABLE IF EXISTS want; "
		<< "CREATE TEMP TABLE have AS SELECT * FROM v_set_parts WHERE set_num='" << m_pSql->Escape(haveNum) << "'; "
		<< "CREATE TEMP TABLE want AS SELECT * FROM v_set_parts WHERE set_num='" << m_pSql->Escape(wantNum) << "'; "
		<< "SELECT w.part_num,w.part_description,w.size,w.price,w.count-IFNULL(h.count,0),w.price*(w.count-IFNULL(h.count,0)) "
		<< "FROM want w LEFT OUTER JOIN have h ON w.part_num=h.part_num WHERE w.count-IFNULL(h.count,0)>0";
	m_pSql->ExecQuery(&cmd, PopulateNeededCallback);
	
	stringstream temp;
	temp << setiosflags(ios::fixed) << setprecision(2) << m_neededCost;
	m_pNeededCost->set_text(temp.str());
}

void MainWindow::PopulateNeeded(string partNumber,string description,string size,double price,int count,double total)
{
	Gtk::TreeModel::Row row = *(m_pNeededStore->append());
	row[m_neededStore.m_partNumber] = partNumber;
	row[m_neededStore.m_description] = description;
	row[m_neededStore.m_size] = size;
	row[m_neededStore.m_count] = count;
	row[m_neededStore.m_price] = price;
	row[m_neededStore.m_total] = total;
	m_neededPartsCount += count;
	m_neededCost += total;
}

#endif
#ifndef _Static_Callbacks
static int PopulatePricelistsCallback(void *wnd, int argc, char **argv, char **azColName)
{
	MainWindow *window = (MainWindow *)wnd;
	window->PopulatePricelists(
		atol(argv[0]),	// num
		argv[1]);				// description
	return 0;
}

static int PopulateNeededCallback(void *wnd, int argc, char **argv, char **azColName)
{
	MainWindow *window = (MainWindow *)wnd;
	window->PopulateNeeded(
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
	//cout << "parts\t";
	//for( int i=0; i<argc; ++i ) {
		//cout << argv[i] << '\t';
	//}
	//cout << endl;
	return 0;
}

// This is the callback function to display the select data in the table 
static int PopulateSetsCallback(void *wnd, int argc, char **argv, char **azColName)
{
	MainWindow *window = (MainWindow *)wnd;
	window->PopulateSets(argv[0],argv[1],atoi(argv[2]),atoi(argv[3]),atol(argv[4]));
	//cout << "sets\t";
	//for( int i=0; i<argc; ++i ) {
		//cout << argv[i] << '\t';
	//}
	//cout << endl;
	return 0;
}

// This is the callback function to display the select data in the table 
static int PopulateCollectionCallback(void *wnd, int argc, char **argv, char **azColName)
{
	MainWindow *window = (MainWindow *)wnd;
	window->PopulateCollection(atol(argv[0]),argv[1],argv[2],argv[3],atoi(argv[4]),atof(argv[5]),atof(argv[6]),argv[7],atoi(argv[8]),argv[9]);
	//cout << "collection\t";
	//for( int i=0; i<argc; ++i ) {
		//cout << argv[i] << '\t';
	//}
	//cout << endl;
	return 0;
}

static int AddSetPartCallback(void *wnd,int argc,char **argv, char **azColName)
{
	MainWindow *window = (MainWindow *)wnd;
	window->AddSetPartToPartsList(argv[0],atoi(argv[1]));
	//for( int i=0; i<argc; ++i ) {
		//cout << argv[i] << '\t';
	//}
	//cout << endl;
	return 0;
}
#endif
