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
	Frame* directoriesFrame = manage(new Frame());
	{
		directoriesFrame->set_label("Directories");
		directoriesFrame->set_border_width(3);
		box->pack_start(*directoriesFrame, true, true);		
	}

	// setup dir box and add to frame
	VBox* pdirectoriesBox = manage(new VBox());
	directoriesFrame->add(*pdirectoriesBox);

	// setup dir includes option and add to dir frame
	{
		pdirectoriesBox->pack_start(*manage(new Label("Include only these directories instead of all:")), PACK_SHRINK);
		mOptionDirectoryIncludes.set_config(mpConfig);
		mOptionDirectoryIncludes.set_section(mSection);
		mOptionDirectoryIncludes.set_name("include_dirs");
		mOptionDirectoryIncludes.set_root(mpConfig->get_option(mSection, "root"));
		mOptionDirectoryIncludes.reload();
		pdirectoriesBox->pack_start(mOptionDirectoryIncludes, PACK_EXPAND_WIDGET);
	}

	// setup prunes option and add to dir frame
	{
		pdirectoriesBox->pack_start(*manage(new Label("Never include these directories:")), PACK_SHRINK);
		mOptionDirectoryPrunes.set_config(mpConfig);
		mOptionDirectoryPrunes.set_section(mSection);
		mOptionDirectoryPrunes.set_name("prunes_dirs");
		mOptionDirectoryPrunes.reload();
		pdirectoriesBox->pack_start(mOptionDirectoryPrunes, PACK_EXPAND_WIDGET);
	}
	
	// setup filter frame and add to main box 
	Frame* filterFrame = manage(new Frame());
	{
		filterFrame->set_label("File filters");
		filterFrame->set_border_width(3);
		box->pack_start(*filterFrame, PACK_EXPAND_WIDGET);
	}

	// setup filter option view and add to frame
	{
		mFilterView.set_config(mpConfig);
		mFilterView.set_section(mSection);
		mFilterView.set_name("filters");
		mFilterView.reload();
		filterFrame->add(mFilterView);		
	}

	

	
	show_all_children();
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

	// dir options
	mOptionDirectoryPrunes.save_to_config();
	mOptionDirectoryIncludes.save_to_config();

	// filters
	mFilterView.save_to_config();

	// save config
	mpConfig->save();
}
