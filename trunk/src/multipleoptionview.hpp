/* multipleoptionview.hpp
 *
 * Copyright (C) 2002 Christian Neumann
 *
 * ufBackup is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * ufBackup is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with ufBackup; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef multipleoptionview_hpp
#define multipleoptionview_hpp

#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/box.h>
#include <gtkmm/dialog.h>
#include <gtkmm/stock.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/messagedialog.h>
#include <config.hpp>

class MultipleOptionView : public Gtk::HBox
{
	class ColumnRecord : public Gtk::TreeModel::ColumnRecord
	{
		public:
			ColumnRecord() { add(mEntry); }
			Gtk::TreeModelColumn<Glib::ustring> mEntry;
	};
	
	public:
	MultipleOptionView();
	void set_config(Config* config) { mpConfig = config; }
	void set_section(tSectionID section) { mSection = section; }
	void set_name(Glib::ustring name) { mName = name; }
	void save_to_config();
	void reload();
	void add_entry(Glib::ustring);

	protected:
	Gtk::VButtonBox mButtons;
	void on_add_filter_button();

	private:
	void on_remove_button();

	Config* mpConfig;
	tSectionID mSection;
	Glib::ustring mName;

	Gtk::TreeView mView;
	ColumnRecord mColumnRecord;
	Glib::RefPtr<Gtk::ListStore> mrListData;
};


class MultipleFilterOptionView : public MultipleOptionView
{
	public:
		MultipleFilterOptionView();
};

class MultipleDirOptionView : public MultipleOptionView
{
	public:
		MultipleDirOptionView();
		void set_root(const Glib::ustring& root) {
			mRoot = root; }
	protected:
		void on_add_button();
		Glib::ustring mRoot;
};


#endif
