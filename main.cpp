#include <iostream>
#include <gtkmm.h>
#include "mainWindow.h"

using namespace std;

int main(int argc,char *argv[])
{
	Gtk::Main kit(argc,argv);

	try {
		if( !Glib::thread_supported() ) {
			Glib::thread_init();
		}
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
	catch( Glib::KeyFileError ex ) {
		cerr << ex.what() << endl;
	}
	catch( exception &ex ) {
		cerr << ex.what() << endl;
	}
	return EXIT_SUCCESS;
}
