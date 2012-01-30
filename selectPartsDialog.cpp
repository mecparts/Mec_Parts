#include <iostream>
#include "selectPartsDialog.h"

using namespace std;

SelectPartsDialog::SelectPartsDialog(Glib::RefPtr<Gtk::Builder> pRefBuilder, Config *pCfg) :
	m_pDialog(NULL),
	m_pCfg(NULL),
	m_pSelectPartsView(NULL)
{
	m_pCfg = pCfg;
	GET_WIDGET(pRefBuilder,"selectPartsDialog",m_pDialog)
	m_pDialog->signal_show().connect(sigc::mem_fun(*this,&SelectPartsDialog::on_show_event));
	m_pDialog->signal_hide().connect(sigc::mem_fun(*this,&SelectPartsDialog::on_hide_event));
	m_pDialog->signal_delete_event().connect(sigc::mem_fun(*this,&SelectPartsDialog::on_delete_event));

	GET_WIDGET(pRefBuilder,"selectPartsView",m_pSelectPartsView)
	Selected()->set_mode(Gtk::SELECTION_MULTIPLE);

	int width,height;
	m_pCfg->get_selectPartsDialog_size(width,height);
	m_pDialog->set_default_size(width,height);
}

SelectPartsDialog::~SelectPartsDialog()
{
	delete m_pDialog;
}

void SelectPartsDialog::on_show_event()
{
	int width,height;
	m_pCfg->get_selectPartsDialog_size(width,height);
	m_pDialog->set_default_size(width,height);
}

void SelectPartsDialog::on_hide_event()
{
	int width,height;
	m_pDialog->get_size(width,height);
	m_pCfg->set_selectPartsDialog_size(width,height);
}

bool SelectPartsDialog::on_delete_event(GdkEventAny *e)
{
  return false;
}
