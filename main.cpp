/*
 * main.cpp: the main() module and unhandled exception catcher for MecParts.
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
#include <gtkmm.h>
#include "mainWindow.h"

using namespace std;

void signal_exception_handler()
{
	try {
		throw;
	}
	catch( string ex ) {
		cerr << ex << endl;
	}
	catch( char const *ex ) {
		cerr << ex << endl;
	}
	catch( Glib::ustring ex ) {
		cerr << ex << endl;
	}
	catch( Glib::KeyFileError &ex ) {
		cerr << "Glib::KeyFileError: " << ex.what() << endl;
	}
	catch( Glib::Error &ex ) {
		cerr << "Glib::Error: " << ex.what() << endl;
	}
	catch( exception &ex ) {
		cerr << ex.what() << endl;
	}
	exit(EXIT_FAILURE);
}

int main(int argc,char *argv[])
{
	Gtk::Main kit(argc,argv);

	try {
		if( !Glib::thread_supported() ) {
			Glib::thread_init();
		}
		Glib::add_exception_handler(sigc::ptr_fun(signal_exception_handler));
		MainWindow mainWindow;
		kit.run(*mainWindow.m_pWindow);
	}
	catch( string ex ) {
		cerr << ex << endl;
	}
	catch( char const *ex ) {
		cerr << ex << endl;
	}
	catch( Glib::ustring ex ) {
		cerr << ex << endl;
	}
	catch( Glib::KeyFileError &ex ) {
		cerr << ex.what() << endl;
	}
	catch( exception &ex ) {
		cerr << ex.what() << endl;
	}
	return EXIT_SUCCESS;
}
