/*
 * metparts.h
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

#ifndef _MECPARTS_H_
#define _MECPARTS_H_

#include <gtkmm.h>
#include <iostream>
#include <string>

#define GET_WIDGET(builder,glade_name,pointer) builder->get_widget(glade_name,pointer); if(!pointer) throw "Could not get " glade_name " widget";
#define GET_OBJECT(builder,glade_name,pointer,ptr_cast) pointer = Glib::RefPtr<Gtk::ptr_cast>::cast_static(builder->get_object(glade_name)); if( !pointer) throw "Could not get " glade_name " object";
#define GET_TEXT_RENDERER(rendererName,pointer,view,index) pointer = dynamic_cast<Gtk::CellRendererText *>(view->get_column_cell_renderer(index)); if( !pointer ) throw "Could not get " rendererName " object";
#define GET_TOGGLE_RENDERER(rendererName,pointer,view,index) pointer = dynamic_cast<Gtk::CellRendererToggle *>(view->get_column_cell_renderer(index)); if( !pointer ) throw "Could not get " rendererName " object";
#define GET_COMBO_RENDERER(rendererName,pointer,view,index) pointer = dynamic_cast<Gtk::CellRendererCombo *>(view->get_column_cell_renderer(index)); if( !pointer ) throw "Could not get " rendererName " object";
#define GET_ITER(iter,dataStore,pathStr) Gtk::TreeIter iter = dataStore->get_iter(pathStr); if( !iter ) { throw "No iter for "+pathStr; }

#endif
