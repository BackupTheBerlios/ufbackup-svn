/* rootwindow.hpp
 *
 * Copyright (C) 2004 Christian Neumann
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

#ifndef rootwindow_hpp
#define rootwindow_hpp

#include <gtkmm/window.h>
#include <gtkmm/frame.h>
#include <gtkmm/box.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/separatortoolitem.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/tooltips.h>
#include <gtkmm/menubar.h>
#include <gtkmm/menu.h>
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>
#include <gtkmm/stock.h>

#include <preferencesdialog.hpp>
#include <backupdialog.hpp>
#include <propertiesdialog.hpp>
#include <aboutdialog.hpp>
#include <listingdialog.hpp>

#include <config.hpp>


class RootWindow : public Gtk::Window
{
	class JobListColumnRecord : public Gtk::TreeModel::ColumnRecord
	{
		public:
			JobListColumnRecord() { add(mShort); add(mLong); add(mSectionID); }
			Gtk::TreeModelColumn<Glib::ustring> mShort;
			Gtk::TreeModelColumn<Glib::ustring> mLong;
			Gtk::TreeModelColumn<tSectionID> mSectionID;
	};
	
	public:
		RootWindow(Config* config);
	private:
		Config* mpConfig;
	
		void on_button_preferences();
		void on_button_help() {};
		void on_button_about();
		void on_button_add_job();
		void on_button_remove_job();
		void on_button_job_properties();
		void on_button_start_backup();
		void on_button_restore_backup() {};
		void on_button_listing();
		void refresh_joblist();
		void on_jobname_change(const Gtk::TreeModel::Path&, const Gtk::TreeModel::iterator&); 
		tSectionID get_selected_job_id();

		Gtk::TreeView mJobList;
		JobListColumnRecord mJobListColumnRecord;
		Glib::RefPtr<Gtk::ListStore> mrJobListData;
};

#endif
