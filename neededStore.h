#ifndef _NEEDEDSTORE_H
#define _NEEDEDSTORE_H

#include "mecparts.h"

using namespace std;

//Tree model columns:
class NeededStore : public Gtk::TreeModel::ColumnRecord
{
	public:

		Gtk::TreeModelColumn<string> m_partNumber;
		Gtk::TreeModelColumn<string> m_description;
		Gtk::TreeModelColumn<string> m_size;
		Gtk::TreeModelColumn<guint> m_count;
		Gtk::TreeModelColumn<gdouble> m_price;
		Gtk::TreeModelColumn<gdouble> m_total;

		NeededStore()
		{
			add(m_partNumber);
			add(m_description);
			add(m_size);
			add(m_count);
			add(m_price);
			add(m_total);
		}
};
#endif
