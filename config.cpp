#include "config.h"

using namespace std;

// Constructor, initializes default settings.
Config::Config()
{
	m_mainWindow_posx = m_mainWindow_posy = -1;
	
	m_mainWindow_sizex = 610;
	m_mainWindow_sizey = 540;
	
	m_selectPartsDialog_sizex = 300;
	m_selectPartsDialog_sizey = 500;

	m_selectSetDialog_sizex = 300;
	m_selectSetDialog_sizey = 500;
}

// Singleton retreiver.  NOT THREAD SAFE.  Call this func early to create, then it's ok.
// @return  A pointer to the one config instance.
Config* Config::get_instance()
{
	// instance
	static Config* _instance;
	
	if( !_instance ) {
		_instance = new Config();
	}
	return _instance;
}

// Creates a clone of this Config instance. Used so the preferences dialog may manipulate the
// configuration without disturbing the real one.
Config* Config::clone()
{
	Config* retVal = new Config();
	
	retVal->m_mainWindow_posx = m_mainWindow_posx;
	retVal->m_mainWindow_posy = m_mainWindow_posy;
	retVal->m_mainWindow_sizex = m_mainWindow_sizex;
	retVal->m_mainWindow_sizey = m_mainWindow_sizey;
	
	retVal->m_selectPartsDialog_sizex = m_selectPartsDialog_sizex;
	retVal->m_selectPartsDialog_sizey = m_selectPartsDialog_sizey;
	
	retVal->m_selectSetDialog_sizex = m_selectSetDialog_sizex;
	retVal->m_selectSetDialog_sizey = m_selectSetDialog_sizey;
	
	return retVal;
}

/**
* Checks to make sure the config dir exists.
*
* @return  If it exists, or it was able to make it
*/
bool Config::check_dir()
{
	Glib::ustring dirname = Glib::build_filename(Glib::ustring(g_get_user_config_dir()), "mecparts");
	if ( !Glib::file_test(dirname, Glib::FILE_TEST_EXISTS ) ) {
		if ( g_mkdir_with_parents(dirname.c_str(), 0755) == -1 ) {
			return false;
		}
	}
	return true;
}

void Config::get_mainWindow_pos(int &x, int &y)
{
	x = m_mainWindow_posx;
	y = m_mainWindow_posy;
}

void Config::set_mainWindow_pos(int x, int y)
{
	m_mainWindow_posx = x;
	m_mainWindow_posy = y;
}

void Config::get_mainWindow_size(int &w, int &h)
{
	w = m_mainWindow_sizex;
	h = m_mainWindow_sizey;
}

void Config::set_mainWindow_size(int w, int h)
{
	m_mainWindow_sizex = w;
	m_mainWindow_sizey = h;
}

void Config::get_selectPartsDialog_size(int &w, int &h)
{
	w = m_selectPartsDialog_sizex;
	h = m_selectPartsDialog_sizey;
}

void Config::set_selectPartsDialog_size(int w, int h)
{
	m_selectPartsDialog_sizex = w;
	m_selectPartsDialog_sizey = h;
}

void Config::get_selectSetDialog_size(int &w, int &h)
{
	w = m_selectSetDialog_sizex;
	h = m_selectSetDialog_sizey;
}

void Config::set_selectSetDialog_size(int w, int h)
{
	m_selectSetDialog_sizex = w;
	m_selectSetDialog_sizey = h;
}

bool Config::save_cfg()
{
	if ( !check_dir() ) {
		return false;
	}

	Glib::ustring cfgfile = Glib::build_filename(Glib::ustring(g_get_user_config_dir()), Glib::ustring("mecparts"));
	cfgfile = Glib::build_filename(cfgfile, Glib::ustring("mecparts.cfg"));

	Glib::KeyFile kf;

	kf.set_integer("mainWindow_geometry", "posx", m_mainWindow_posx);
	kf.set_integer("mainWindow_geometry", "posy", m_mainWindow_posy);
	kf.set_integer("mainWindow_geometry", "sizex", m_mainWindow_sizex);
	kf.set_integer("mainWindow_geometry", "sizey", m_mainWindow_sizey);

	kf.set_integer("selectPartsDialog_geometry", "sizex", m_selectPartsDialog_sizex);
	kf.set_integer("selectPartsDialog_geometry", "sizey", m_selectPartsDialog_sizey);

	kf.set_integer("selectSetDialog_geometry", "sizex", m_selectSetDialog_sizex);
	kf.set_integer("selectSetDialog_geometry", "sizey", m_selectSetDialog_sizey);

	if( g_file_set_contents(cfgfile.c_str(), kf.to_data().c_str(), -1, NULL) == FALSE ) {
		return false;
	}
	return true;
}

bool Config::load_cfg()
{
	if( !check_dir() ) {
		return false;
	}
	
	Glib::ustring cfgfile = Glib::build_filename(Glib::ustring(g_get_user_config_dir()), Glib::ustring("mecparts"));
	cfgfile = Glib::build_filename(cfgfile, Glib::ustring("mecparts.cfg"));
	
	if( !Glib::file_test(cfgfile, Glib::FILE_TEST_EXISTS) ) {
		return false;
	}
	
	Glib::KeyFile kf;
	if( !kf.load_from_file(cfgfile) ) {
		return false;
	}
	if( kf.has_group("mainWindow_geometry") ) {
		if( kf.has_key("mainWindow_geometry", "posx") ) {
			m_mainWindow_posx = kf.get_integer("mainWindow_geometry", "posx");
		}
		if( kf.has_key("mainWindow_geometry", "posy") ) {
			m_mainWindow_posy = kf.get_integer("mainWindow_geometry", "posy");
		}
		if( kf.has_key("mainWindow_geometry", "sizex") ) {
			m_mainWindow_sizex = kf.get_integer("mainWindow_geometry", "sizex");
		}
		if( kf.has_key("mainWindow_geometry", "sizey") ) {
			m_mainWindow_sizey = kf.get_integer("mainWindow_geometry", "sizey");
		}
	}
	if( kf.has_group("selectPartsDialog_geometry") ) {
		if( kf.has_key("selectPartsDialog_geometry", "sizex") ) {
			m_selectPartsDialog_sizex = kf.get_integer("selectPartsDialog_geometry", "sizex");
		}
		if( kf.has_key("selectPartsDialog_geometry", "sizey") ) {
			m_selectPartsDialog_sizey = kf.get_integer("selectPartsDialog_geometry", "sizey");
		}
	}
	if( kf.has_group("selectSetDialog_geometry") ) {
		if( kf.has_key("selectSetDialog_geometry", "sizex") ) {
			m_selectSetDialog_sizex = kf.get_integer("selectSetDialog_geometry", "sizex");
		}
		if( kf.has_key("selectSetDialog_geometry", "sizey") ) {
			m_selectSetDialog_sizey = kf.get_integer("selectSetDialog_geometry", "sizey");
		}
	}
	return true;
}
