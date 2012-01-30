#ifndef SETSSTORE_H
#define SETSSTORE_H

#include "mecparts.h"

using namespace std;

//Tree model columns:
class SetsStore : public Gtk::TreeModel::ColumnRecord
{
	public:

		Gtk::TreeModelColumn<string> m_setNumber;
		Gtk::TreeModelColumn<string> m_description;
		Gtk::TreeModelColumn<guint> m_started;
		Gtk::TreeModelColumn<guint> m_ended;
		Gtk::TreeModelColumn<gint64> m_rowId;
		
		SetsStore()
		{
			add(m_setNumber);
			add(m_description);
			add(m_started);
			add(m_ended);
			add(m_rowId);
		}
};
#endif
