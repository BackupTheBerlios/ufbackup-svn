/* listingdialog.hpp
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

#ifndef listingdialog_hpp
#define listingdialog_hpp

#include <gtkmm/frame.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>
#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/dialog.h>
#include <gtkmm/stock.h>

#include <daraction.hpp>

#include <config.hpp>
#include <string>
#include <memory>

#ifdef DEBUG
#include <iostream>
#endif

class ListingDialog : public Gtk::Dialog, public DARAction
{
	class ColumnRecord : public Gtk::TreeModel::ColumnRecord
	{
		public:
			ColumnRecord() { add(mFlag); add(mPermissions);
				add(mUID); add(mGID); add(mSize); add(mDate); 
				add(mFilename); }

			Gtk::TreeModelColumn<Glib::ustring> mFlag, mPermissions,
				mUID, mGID, mDate, mFilename;
			Gtk::TreeModelColumn<unsigned long> mSize;
	};

	public:
	ListingDialog(Gtk::Window&, Config*, tSectionID);
	
	private:

	void reload_archive(unsigned int = 1);
	
	void listing(const std::string &, const std::string &, const std::string &, const std::string &, const std::string &, const std::string &, const std::string &); 

	void warning(const std::string&);
	void pause(const std::string&);

	void list_path(const string& dir);
	
	Config* mpConfig;
	tSectionID mSection;
	Gtk::TreeModel::Row* mpParentRow;
	Glib::ustring mParentDir;

	std::auto_ptr<libdar::archive> mpArchive;

	Gtk::TreeView mView;
	ColumnRecord mColumnRecord;
	Glib::RefPtr<Gtk::TreeStore> mrData;
};

#endif
