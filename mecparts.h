#ifndef _MECPARTS_H_
#define _MECPARTS_H_

#include <gtkmm.h>
#include <iostream>
#include <string>

#define GET_WIDGET(builder,glade_name,pointer) builder->get_widget(glade_name,pointer); if(!pointer) throw "Could not get " glade_name " widget";
#define CELL_RENDERER(rendererName,pointer,view,index) pointer = dynamic_cast<Gtk::CellRendererText *>(view->get_column_cell_renderer(index)); if( !pointer ) throw "Could not get " rendererName " object";

#endif
