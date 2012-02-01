/*
 * selectSetDialog.cpp
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
