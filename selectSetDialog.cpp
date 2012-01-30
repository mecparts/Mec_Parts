#include <iostream>
#include "selectSetDialog.h"

using namespace std;

SelectSetDialog::SelectSetDialog(Glib::RefPtr<Gtk::Builder> pRefBuilder, Config *pCfg) :
	m_pDialog(NULL),
	m_pCfg(NULL),
	m_pSelectSetView(NULL)
{
	m_pCfg = pCfg;
	GET_WIDGET(pRefBuilder,"selectSetDialog",m_pDialog)
	m_pDialog->signal_show().connect(sigc::mem_fun(*this,&SelectSetDialog::on_show_event));
	m_pDialog->signal_hide().connect(sigc::mem_fun(*this,&SelectSetDialog::on_hide_event));
	m_pDialog->signal_delete_event().connect(sigc::mem_fun(*this,&SelectSetDialog::on_delete_event));

	GET_WIDGET(pRefBuilder,"selectSetView",m_pSelectSetView)
	Selected()->set_mode(Gtk::SELECTION_MULTIPLE);

	int width,height;
	m_pCfg->get_selectSetDialog_size(width,height);
	m_pDialog->set_default_size(width,height);
}

SelectSetDialog::~SelectSetDialog()
{
	delete m_pDialog;
}

void SelectSetDialog::on_show_event()
{
	int width,height;
	m_pCfg->get_selectSetDialog_size(width,height);
	m_pDialog->set_default_size(width,height);
}

void SelectSetDialog::on_hide_event()
{
	int width,height;
	m_pDialog->get_size(width,height);
	m_pCfg->set_selectSetDialog_size(width,height);
}

bool SelectSetDialog::on_delete_event(GdkEventAny *e)
{
  return false;
}
