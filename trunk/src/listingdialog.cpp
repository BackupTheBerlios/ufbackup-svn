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
		mpConfig(config), mSection(section), mpParentRow(NULL)
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
		list_path("");
	} catch(Egeneric& e) {
		warning(e.get_message().c_str());
	}
}

void ListingDialog::listing(const string & flag,
		const string & perm,
		     const string & uid,
		     const string & gid,
		     const string & size,
		     const string & date,
		     const string & filename)
{
	
	TreeModel::Row* prow, *poldrow;

	// remember the current parent, because we have to reset it after
	// processing this row's chilren (and thus making this row the parent)
	poldrow = mpParentRow;
	ustring lastdir = mParentDir;

	// check if this is a child of another row
	if(mpParentRow != NULL) {
		prow = new TreeModel::Row(*(mrData->append(mpParentRow->children())));
	}
	else
		prow = new TreeModel::Row(*(mrData->append()));
	
	// set data of row
	(*prow)[mColumnRecord.mFlag] = flag;
	(*prow)[mColumnRecord.mPermissions] = perm;
	(*prow)[mColumnRecord.mUID] = uid;
	(*prow)[mColumnRecord.mGID] = gid;
	stringstream filesize;
	filesize << size;
	unsigned long numfilesize;
	filesize >> numfilesize;
	(*prow)[mColumnRecord.mSize] = numfilesize;
	(*prow)[mColumnRecord.mDate] = date;
	(*prow)[mColumnRecord.mFilename] = filename;
	

	// we are going to process the children of this row.
	// before processing the children: make this row the new parent
	mpParentRow = prow;
	if(mParentDir != "")
		mParentDir += '/';
	mParentDir += filename;
	
	// process children
	list_path(mParentDir);

	// processing done.
	// this row is no longer parent. reset to old parent
	delete mpParentRow;
	mpParentRow = poldrow; 
	mParentDir = lastdir;

	// add the size of this row to the parent row.
	// directories' sizes in the listing will thus be the summed
	// sizes of all its children
	if(mpParentRow != NULL)
		(*mpParentRow)[mColumnRecord.mSize] = (*prow)[mColumnRecord.mSize]
			+ (*mpParentRow)[mColumnRecord.mSize];
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

void ListingDialog::list_path(const string& dir)
{
	if(not get_children_of(*this, mpArchive.get(), dir)) {
		
		mpParentRow = new TreeModel::Row(*(mpParentRow->parent()));
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
