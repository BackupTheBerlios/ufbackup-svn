/* listingdialog.cpp
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

#include <listingdialog.hpp>

using namespace Glib;
using namespace Gtk;
using namespace std;
using namespace libdar;

ListingDialog::ListingDialog(Window& window, Config* config,
		tSectionID section) : Dialog("Listing", window),
		mpConfig(config), mSection(section)
{
	set_use_listing(true);
	set_default_size(650,550);
	add_button(Stock::CLOSE, 1);

	VBox* mainbox = get_vbox(); 

	// setup treemodel
	{
		mrData = TreeStore::create(mColumnRecord);
	}
	
	// setup treeview
	{
		mView.set_model(mrData);
		mView.set_reorderable();
		mView.append_column("Filename",mColumnRecord.mFilename);
		mView.append_column("Size",mColumnRecord.mSize);
//		mView.append_column("Flag",mColumnRecord.mFlag);
		mView.append_column("User",mColumnRecord.mUID);
		mView.append_column("Group",mColumnRecord.mGID);
		mView.append_column("Permissions",mColumnRecord.mPermissions);
		mView.append_column("Date",mColumnRecord.mDate);
		mView.signal_row_expanded().connect(
				sigc::mem_fun(*this, &ListingDialog::on_row_expand));
	}
	// put view in scrolled window and add it to mainbox
	{
		ScrolledWindow* pscrolled = manage(new ScrolledWindow());
		pscrolled->add(mView);
		pscrolled->set_border_width(3);
		pscrolled->set_shadow_type(SHADOW_IN);
		pscrolled->set_policy(POLICY_AUTOMATIC, POLICY_AUTOMATIC);
		mainbox->pack_start(*pscrolled, PACK_EXPAND_WIDGET);
	}
		
	show_all_children();	

	try {
		reload_archive();
		add_rows("", NULL);
	} catch(Egeneric& e) {
		warning(e.get_message().c_str());
	}
}

void ListingDialog::on_row_expand(const TreeModel::iterator& row, const TreeModel::Path& path)
{
	if(row->children().size() == 1 && (*(row->children().begin()))[mColumnRecord.mFilename] == "") {
		mrData->erase(row->children().begin());
		cerr << "Expanding " << (*row)[mColumnRecord.mFilename] << endl;
		TreeIter thisrow(row);
		add_rows(get_full_path(row), &thisrow);
	}
	mView.expand_row(path, false);
}

ustring ListingDialog::get_full_path(TreeModel::iterator row)
{
	ustring path;
	while((*row)[mColumnRecord.mFilename] != "")
	{
		cerr << "path snipped: " << (*row)[mColumnRecord.mFilename] << endl;
		ustring snippet = (*row)[mColumnRecord.mFilename];
		snippet += '/';
		path = snippet + path;
		row = row->parent();
	}
	cerr << "Generated path: " << path << endl;
	return path;
}

void ListingDialog::listing(const string & flag,
		const string & perm,
		     const string & uid,
		     const string & gid,
		     const string & size,
		     const string & date,
		     const string & filename,
			  bool isDir,
			  bool has_children )
{
	TreeIter newRow;
	
	// check if this is a child of another row
	if(mpParent != NULL) {
		newRow = mrData->append((**mpParent)->children());
	}
	else
		newRow = mrData->append();
	
	// set data of row
	(*newRow)[mColumnRecord.mFlag] = flag;
	(*newRow)[mColumnRecord.mPermissions] = perm;
	(*newRow)[mColumnRecord.mUID] = uid;
	(*newRow)[mColumnRecord.mGID] = gid;
	stringstream filesize;
	filesize << size;
	unsigned long numfilesize;
	filesize >> numfilesize;
	(*newRow)[mColumnRecord.mSize] = numfilesize;
	(*newRow)[mColumnRecord.mDate] = date;
	(*newRow)[mColumnRecord.mFilename] = filename;

	// Dirty Hack (any other way to accomplish this?):
	// If this is an directory which has children,
	// add an empty child row to show the user that
	// this row has children (the GUI draws an
	// arrow). The children will be added when the
	// user opens the row.
	if(has_children)
		newRow = mrData->append((*newRow).children());	
}


void ListingDialog::reload_archive(unsigned int level)
{
	// get filename of archive catalogue
	stringstream filename;
	filename << mpConfig->get_option(mSection, "name");
	if(mpConfig->get_option(mSection, "type") == "incremental")
		filename << '.' << (level);

	// open archive
	mpArchive = auto_ptr<archive>(new archive(
					*this,
					mpConfig->get_option(0, "home").c_str(),
					filename.str(),
					"dar",
					crypto_none,
					"", // pass
					"", // in pipe
					"", // out pipe
					"", // execute
					mpConfig->get_bool_option(0, "verbose")					
					));
}

void ListingDialog::add_rows(const string& dir, TreeIter* parent)
{
	// set current parent, so that the listing(...) function knows where
	// to add new rows
	mpParent = parent;

	try {
		get_children_of(*this, mpArchive.get(), dir);
	} catch(Egeneric& e) {
		warning(e.get_message().c_str());
	}
}

void ListingDialog::warning(const string& msg)
{
	cerr << "Warning: " << msg << endl;
}

void ListingDialog::pause(const string& msg)
{
	cerr << "Pause: " << msg << endl;
}
