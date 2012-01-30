#ifndef PRICELISTSSTORE_H
#define PRICELISTSSTORE_H

#include "mecparts.h"

using namespace std;

//Tree model columns:
class PricelistsStore : public Gtk::TreeModel::ColumnRecord
{
	public:

		Gtk::TreeModelColumn<string> m_description;
		Gtk::TreeModelColumn<gint64> m_num;
		
		PricelistsStore()
		{
			add(m_description);
			add(m_num);
		}
};
#endif
