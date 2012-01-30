#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "mecparts.h"

// Static class that interfaces with the configuration. 
	class Config {
		private:
		
			bool check_dir();
			int m_mainWindow_posx, m_mainWindow_posy;
			int m_mainWindow_sizex, m_mainWindow_sizey;
			int m_selectPartsDialog_sizex, m_selectPartsDialog_sizey;
			int m_selectSetDialog_sizex, m_selectSetDialog_sizey;
			
		public:
			static Config* get_instance();	// instance getter
			Config();
			Config* clone();

			void get_mainWindow_pos(int &x, int &y);
			void set_mainWindow_pos(int x, int y);
			void get_mainWindow_size(int &w, int &h);
			void set_mainWindow_size(int w, int h);

			void get_selectPartsDialog_size(int &w, int &h);
			void set_selectPartsDialog_size(int w, int h);

			void get_selectSetDialog_size(int &w, int &h);
			void set_selectSetDialog_size(int w, int h);
			
			bool save_cfg();            // save non bg related cfg info
			bool load_cfg();
	};
#endif
