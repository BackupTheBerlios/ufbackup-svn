/* multipleoptionview.cpp
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


#include <multipleoptionview.hpp>

using namespace Glib;
using namespace Gtk;

MultipleOptionView::MultipleOptionView()
{
	// add scrollable window to box
	ScrolledWindow* pscrolled = manage(new ScrolledWindow());
	{
		pscrolled->set_border_width(2);
		pscrolled->set_shadow_type(SHADOW_IN);
		pscrolled->set_policy(POLICY_AUTOMATIC, POLICY_AUTOMATIC);
		
		pack_start(*pscrolled, PACK_EXPAND_WIDGET);
	}
	
	
	// Init view and add to scrollable window
	{
		mView.set_headers_visible(false);
		pscrolled->add(mView);
	}

	// setup "add/remove" buttons and add to box
	Button* pRemoveButton =  manage(new Button(Stock::REMOVE));
	pRemoveButton->signal_clicked().connect(
			sigc::mem_fun(*this, &MultipleOptionView::on_remove_button));
	mButtons.pack_end(*pRemoveButton);
	mButtons.set_border_width(3);
	pack_start(mButtons, PACK_SHRINK);

	show_all_children();
}


void MultipleOptionView::reload()
{
	mrListData = ListStore::create(mColumnRecord);
	mView.set_model(mrListData);
	mView.append_column("", mColumnRecord.mEntry);
	mrListData->set_sort_column_id(0, SORT_ASCENDING);

	// add entrys to list 
	tValueList entries = mpConfig->get_multiple_option(mSection, mName.c_str());

	TreeModel::Row row;
	for(tValueList::const_iterator iter = entries.begin();
			iter != entries.end();	++iter)
	{
		row = *(mrListData->append());
		row[mColumnRecord.mEntry] = *iter;
	}
}


void MultipleOptionView::save_to_config()
{
	mpConfig->remove_multiple_option_entries(mSection, mName.c_str());

	for(ListStore::const_iterator iter =
			mrListData->children().begin();
			iter !=
			mrListData->children().end();
			++iter)
		mpConfig->add_multiple_option(mSection, mName.c_str(),
				(*iter)[mColumnRecord.mEntry]);
}


void MultipleOptionView::add_entry(Glib::ustring entry)
{
	TreeModel::Row row;
	row = *(mrListData->append());
	row[mColumnRecord.mEntry] = entry;
}

void MultipleOptionView::on_remove_button()
{
	// remove selected entry from list
	RefPtr<TreeSelection> refTreeSelection = mView.get_selection();
	TreeModel::const_iterator iter = refTreeSelection->get_selected();
	if(!iter)
		return;
	mrListData->erase(iter);
}

void MultipleOptionView::on_add_filter_button()
{
	Dialog dialog("Add filter", get_toplevel());
	dialog.add_button(Stock::OK, 1);
	dialog.add_button(Stock::CANCEL, 2);
	Entry* pentry = manage(new Entry());

	// setup dialog
	{
		VBox* mainbox = dialog.get_vbox(); 
		Label* plabel = manage(new Label("Please enter your filter string (Wildcards supported):"));
		mainbox->pack_start(*plabel, PACK_SHRINK);
		mainbox->pack_start(*pentry, PACK_SHRINK);
	}
	dialog.show_all_children();
	
	if(dialog.run() == 1 && pentry->get_text() != "")
		add_entry(pentry->get_text());
}


MultipleDirOptionView::MultipleDirOptionView()
{
	Button* pAddButton = manage(new Button(Stock::ADD));
	pAddButton->signal_clicked().connect(
			sigc::mem_fun(*this, &MultipleDirOptionView::on_add_button));

	mButtons.pack_end(*pAddButton);
	mButtons.reorder_child(*pAddButton, 0);
}

void MultipleDirOptionView::add_filter_button()
{
	Button* pAddButton = manage(new Button("Add filter"));
	pAddButton->signal_clicked().connect(
			sigc::mem_fun(*this, &MultipleOptionView::on_add_filter_button));

	mButtons.pack_end(*pAddButton);
	mButtons.reorder_child(*pAddButton, 1);
}

MultipleFilterOptionView::MultipleFilterOptionView()
{
	Button* pAddButton = manage(new Button("Add filter"));
	pAddButton->signal_clicked().connect(
			sigc::mem_fun(*this, &MultipleOptionView::on_add_filter_button));

	mButtons.pack_end(*pAddButton);
	mButtons.reorder_child(*pAddButton, 0);
}


void MultipleDirOptionView::on_add_button()
{

	// open dialog to get file/dir

	FileChooserDialog dialog("Please choose folders",
			FILE_CHOOSER_ACTION_SELECT_FOLDER);

	dialog.set_current_folder(mRoot);
	dialog.add_button("Select", Gtk::RESPONSE_OK);
	dialog.set_select_multiple();


	if(dialog.run() == RESPONSE_OK)
	{
		Glib::SListHandle<Glib::ustring> list = dialog.get_filenames();
		Glib::SListHandle<Glib::ustring>::const_iterator iter = list.begin();
		bool legal = true;
		for(iter = list.begin(); iter != list.end(); ++iter) {
			Glib::ustring selected = *iter; 
			// abort if selected file/dir is not below root dir)
			if(selected.substr(0, mRoot.size()) != mRoot) {
				legal = false;
			}else
			{	
				// strip off root path
				selected = selected.substr(mRoot.size(), selected.size() - mRoot.size());
				if(selected[0] == '/')
					selected = selected.substr(1, selected.size());

				// add to DirList
				add_entry(selected);
			}
		}
		if(not legal) {
			Gtk::MessageDialog errdialog(
					*((Gtk::Window*) get_toplevel()),
					"Some or all of the selected folders have been skipped because they are not below the root directory",
					Gtk::MESSAGE_ERROR);
			errdialog.run();
		}
	}
}
