#ifndef PARTSSTORE_H
#define PARTSSTORE_H

#include "mecparts.h"

using namespace std;

//Tree model columns:
class PartsStore : public Gtk::TreeModel::ColumnRecord
{
	public:

		Gtk::TreeModelColumn<string> m_partNumber;
		Gtk::TreeModelColumn<string> m_description;
		Gtk::TreeModelColumn<string> m_size;
		Gtk::TreeModelColumn<gdouble> m_price;
		Gtk::TreeModelColumn<string> m_notes;
		Gtk::TreeModelColumn<gint64> m_rowId;
		Gtk::TreeModelColumn<string> m_pnPrefix;
		Gtk::TreeModelColumn<guint> m_pnDigits;
		Gtk::TreeModelColumn<string> m_pnSuffix;

		PartsStore()
		{
			add(m_partNumber);
			add(m_description);
			add(m_size);
			add(m_price);
			add(m_notes);
			add(m_rowId);
			add(m_pnPrefix);
			add(m_pnDigits);
			add(m_pnSuffix);
		}
};
#endif
