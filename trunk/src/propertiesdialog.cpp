/* propertiesdialog.cpp
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

#include <propertiesdialog.hpp>

using namespace Gtk;

#ifdef DEBUG
using namespace std;
#endif


PropertiesDialog::PropertiesDialog(Window& parent, Config* config, tSectionID sectionid)
	: Dialog("Job properties", parent), mpConfig(config), mSection(sectionid)
{
	set_default_size(650,550);

	// add ok button
	add_button(Stock::SAVE, 1);
	add_button(Stock::CANCEL, 0);

	VBox* box = get_vbox(); 

	// add options frame
	{
		Frame* pFrame = manage(new Frame());
		pFrame->set_border_width(3);
		pFrame->set_label("Options");
		box->pack_start(*pFrame, false, false);

		// add table to frame
		Table* pTable = manage(new Table(3,6));
		pFrame->add(*pTable);				

		// add options to table
		Label* label;

		// setup type menu
		{
			Menu* pmenu = manage(new Menu());

			pmenu->append(*manage(new MenuItem("Full")));
			pmenu->append(*manage(new MenuItem("Incremental")));

			Glib::ustring selected = mpConfig->get_option(mSection, "type");

			if(selected == "full")
				pmenu->set_active(0);
			else
				pmenu->set_active(1);

			mOptionType.set_menu(*pmenu);
		}

		label = manage(new Label("Backup type:"));
		label->set_alignment(1,0.5);
		pTable->attach(*label, 0, 1, 0, 1, FILL, FILL, 5, 0);
		pTable->attach(mOptionType, 1, 3, 0, 1, FILL, FILL, 5, 0);

		Button *pbutton;
		label = manage(new Label("Root directory:"));
		label->set_alignment(1,0.5);
		pTable->attach(*label, 0, 1, 1, 2, FILL, FILL, 5, 0);
		mOptionRoot.set_text(mpConfig->get_option(mSection, "root"));
		pTable->attach(mOptionRoot, 1, 2, 1, 2, EXPAND | FILL, EXPAND | FILL, 5, 0);
		mOptionRoot.set_alignment(0,0.5);
		pbutton = manage(new Button(Stock::OPEN));
		pbutton->signal_clicked().connect(
				sigc::mem_fun(*this, &PropertiesDialog::on_root_choose_button));
		pTable->attach(*pbutton, 2, 3, 1, 2, Gtk::SHRINK, Gtk::SHRINK, 5, 0);

		label = manage(new Label("Target directory:"));
		label->set_alignment(1,0.5);
		pTable->attach(*label, 0, 1, 2, 3,  FILL,FILL, 5, 0);
		mOptionTarget.set_text(mpConfig->get_option(mSection, "target"));
		pTable->attach(mOptionTarget, 1, 2, 2, 3, EXPAND | FILL, EXPAND | FILL, 5, 0);
		mOptionTarget.set_alignment(0,0.5);
		pbutton = manage(new Button(Stock::OPEN));
		pbutton->signal_clicked().connect(
				sigc::mem_fun(*this, &PropertiesDialog::on_targetdir_choose_button));
		pTable->attach(*pbutton, 2, 3, 2, 3, Gtk::SHRINK, Gtk::SHRINK, 5, 0);
	}

	// setup directories frame and add to main box 
	Frame* fileFrame = manage(new Frame());
	{
		fileFrame->set_label("Directories");
		fileFrame->set_border_width(3);
	}

	// setup dir box and add to frame
	VBox* pdirectoriesBox = manage(new VBox());
	fileFrame->add(*pdirectoriesBox);
	
	// setup add include/exclude dirs option and its box and add to dir box
	{
		HBox* pdirOptionBox = manage(new HBox());
		pdirOptionBox->set_spacing(5);
		Menu* pmenu = manage(new Menu());

		pmenu->append(*manage(new MenuItem("Include")));
		pmenu->append(*manage(new MenuItem("Exclude")));

		Glib::ustring selected = mpConfig->get_option(mSection, "dir_behaviour");

		if(selected == "include")
			pmenu->set_active(0);
		else
			pmenu->set_active(1);

		mOptionDirBehaviour.set_menu(*pmenu);

		pdirOptionBox->pack_start(mOptionDirBehaviour, PACK_SHRINK);
		pdirOptionBox->pack_start(*manage(new Label("all but the directories specified below:")), PACK_SHRINK);
			
		pdirectoriesBox->pack_start(*pdirOptionBox, PACK_SHRINK);
	}
	


	// Init and add file list
	{
		mrDirListData = ListStore::create(mDirListColumnRecord);
		mrDirListData->set_sort_column_id(0, SORT_ASCENDING);
		mDirList.set_model(mrDirListData);
		mDirList.set_headers_visible(false);
		mDirList.append_column("Excluded directories", mDirListColumnRecord.mDir);

		// add entrys to list 
		tValueList files = mpConfig->get_multiple_option(mSection, "dirs");

		Gtk::TreeModel::Row row;
		for(tValueList::const_iterator iter = files.begin();
				iter != files.end();	++iter)
		{
			row = *(mrDirListData->append());
			row[mDirListColumnRecord.mDir] = *iter;
		}

		// put file list in scrollable widget
		ScrolledWindow* pScrolled = manage(new ScrolledWindow());
		pScrolled->add(mDirList);
		pScrolled->set_border_width(2);
		pScrolled->set_shadow_type(SHADOW_IN);
		pScrolled->set_policy(POLICY_AUTOMATIC, POLICY_AUTOMATIC);

		// "add/remove" buttons
		VButtonBox* pButtons = manage(new VButtonBox(BUTTONBOX_START, 3));

		Button* pAddButton = manage(new Button(Stock::ADD));
		pAddButton->signal_clicked().connect(
				sigc::mem_fun(*this, &PropertiesDialog::on_dir_add_button));

		Button* pRemoveButton =  manage(new Button(Stock::REMOVE));
		pRemoveButton->signal_clicked().connect(
				sigc::mem_fun(*this, &PropertiesDialog::on_dir_remove_button));

		pButtons->pack_start(*pAddButton);
		pButtons->pack_start(*pRemoveButton);
		pButtons->set_border_width(3);

		// add list and buttons to box and add this to directories box
		HBox* phbox = manage(new HBox());

		phbox->pack_start(*pScrolled, true, true);
		phbox->pack_start(*pButtons, false, false);

		pdirectoriesBox->pack_start(*phbox, PACK_EXPAND_WIDGET);
	}

	box->pack_start(*fileFrame, true, true);		
	
	show_all_children();
}

void PropertiesDialog::on_dir_add_button()
{

	// open dialog to get file/dir

	FileChooserDialog dialog("Please choose folders",
			FILE_CHOOSER_ACTION_SELECT_FOLDER);

	Glib::ustring root = mOptionRoot.get_text(); 

	dialog.set_current_folder(root);
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
			if(selected.substr(0, root.size()) != root) {
				legal = false;
			}else
			{	
				// strip off root path
				selected = selected.substr(root.size(), selected.size() - root.size());
				if(selected[0] == '/')
					selected = selected.substr(1, selected.size());

				// add to DirList
				Gtk::TreeModel::Row row;
				row = *(mrDirListData->append());
				row[mDirListColumnRecord.mDir] = selected;
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

void PropertiesDialog::on_dir_remove_button()
{
	// remove selected directory from list
	Glib::RefPtr<TreeSelection> refTreeSelection = mDirList.get_selection();
	TreeModel::const_iterator iter = refTreeSelection->get_selected();
	if(!iter)
		return;
	mrDirListData->erase(iter);
}

void PropertiesDialog::on_root_choose_button()
{
	// open dialog to get root dir

	FileChooserDialog dialog("Please choose root directory",
			FILE_CHOOSER_ACTION_SELECT_FOLDER);

	dialog.add_button("Select", Gtk::RESPONSE_OK);

	if(dialog.run() == RESPONSE_OK)
	{
		 mOptionRoot.set_text(dialog.get_filename());
	}

}

void PropertiesDialog::on_targetdir_choose_button()
{
	// open dialog to get target dir

	FileChooserDialog dialog("Please choose root directory",
			FILE_CHOOSER_ACTION_SELECT_FOLDER);

	dialog.add_button("Select", Gtk::RESPONSE_OK);

	if(dialog.run() == RESPONSE_OK)
	{
		 mOptionTarget.set_text(dialog.get_filename());
	}
}

void PropertiesDialog::save()
{
	// save options:
	// type
	Glib::ustring type = "full";
	if(mOptionType.get_history() == 1)
		type = "incremental";
	mpConfig->set_option(mSection, "type", type);

	// root
	mpConfig->set_option(mSection, "root", mOptionRoot.get_text());

	// target
	mpConfig->set_option(mSection, "target", mOptionTarget.get_text());

	// dir_behaviour
	Glib::ustring behaviour = "include";
	if(mOptionDirBehaviour.get_history() == 1)
		behaviour = "exclude";
	mpConfig->set_option(mSection, "dir_behaviour", behaviour);
	

	// dirs
	mpConfig->remove_multiple_option_entries(mSection, "dirs");
	for(ListStore::const_iterator iter =
			mrDirListData->children().begin();
			iter !=
			mrDirListData->children().end();
			++iter)
		mpConfig->add_multiple_option(mSection, "dirs",
				(*iter)[mDirListColumnRecord.mDir]);

	// save config
	mpConfig->save();
}
