/* rootwindow.cpp
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

#include <rootwindow.hpp>

using namespace Gtk;
using namespace Glib;


ToolButton& get_finished_toolbutton(Tooltips* tt, char* stock, char* label, char* tip)
{
	ToolButton* ptoolbutton;
	ptoolbutton = manage(new ToolButton(
				*manage(new Image(StockID(stock), ICON_SIZE_SMALL_TOOLBAR)),
				label));
	ptoolbutton->set_tooltip(*tt, tip);
	ptoolbutton->set_use_underline();
	return *ptoolbutton;
}

	RootWindow::RootWindow(Config* config)
: mpConfig(config)
{
	set_title("ufBackup 0.1.0 (unstable/development)");
	set_default_size(650,400);

	// setup tooltips class
	Tooltips* ptooltips = manage(new Tooltips());

	// setup main box and add to app window
	VBox* pmainbox = manage(new VBox());
	{
		add(*pmainbox);
	}

	// setup menu
	{
		MenuBar* pmenuBar = manage(new MenuBar());

		// ufbackup menu
		Menu* pmenu;
		pmenu = manage(new Menu());
		pmenu->items().push_back(
				Menu_Helpers::StockMenuElem(Gtk::Stock::PREFERENCES,
					sigc::mem_fun(*this, &RootWindow::on_button_preferences) ) );

		pmenu->items().push_back(
				Menu_Helpers::SeparatorElem());

		pmenu->items().push_back(
				Menu_Helpers::StockMenuElem(Gtk::Stock::QUIT,
					sigc::mem_fun(*this, &RootWindow::hide) ) );

		pmenuBar->items().push_back(
				Menu_Helpers::MenuElem("_ufBackup", *pmenu));

		// jobs menu
		pmenu = manage(new Menu());
		pmenu->items().push_back(
				Menu_Helpers::StockMenuElem(Gtk::Stock::ADD,
					sigc::mem_fun(*this, &RootWindow::on_button_add_job) ) );

		pmenu->items().push_back(
				Menu_Helpers::StockMenuElem(Gtk::Stock::REMOVE,
					sigc::mem_fun(*this, &RootWindow::on_button_remove_job) ) );

		pmenu->items().push_back(
				Menu_Helpers::SeparatorElem());

		pmenu->items().push_back(
				Menu_Helpers::StockMenuElem(Gtk::Stock::PROPERTIES,
					sigc::mem_fun(*this, &RootWindow::on_button_job_properties) ) );

		pmenu->items().push_back(
				Menu_Helpers::SeparatorElem());

		pmenu->items().push_back(
				Menu_Helpers::ImageMenuElem("_Start backup", *(new Image(StockID("gtk-execute"), ICON_SIZE_MENU)),
					sigc::mem_fun(*this, &RootWindow::on_button_start_backup) ) );

		pmenu->items().push_back(
				Menu_Helpers::ImageMenuElem("_Restore backup", *(new Image(StockID("gtk-undo"), ICON_SIZE_MENU)),
					sigc::mem_fun(*this, &RootWindow::on_button_restore_backup) ) );

		pmenu->items().push_back(
				Menu_Helpers::ImageMenuElem("_List contents", *(new Image(StockID("gtk-find"), ICON_SIZE_MENU)),
					sigc::mem_fun(*this, &RootWindow::on_button_listing) ) );


		pmenuBar->items().push_back(
				Menu_Helpers::MenuElem("_Job", *pmenu));

		// help menu
		pmenu = manage(new Menu());
		pmenu->items().push_back( Gtk::Menu_Helpers::MenuElem("_Contents",
					sigc::mem_fun(*this, &RootWindow::on_button_about) ) );

		pmenu->items().push_back( Gtk::Menu_Helpers::MenuElem("_About",
					sigc::mem_fun(*this, &RootWindow::on_button_about) ) );
 		pmenuBar->items().push_back( Gtk::Menu_Helpers::StockMenuElem(Gtk::Stock::HELP, *pmenu) );
		
		// add to main box
		pmainbox->pack_start(*pmenuBar, PACK_SHRINK);
	}

	// setup main toolbar and add to mainbox 
	{
		Toolbar* ptoolbar = manage(new Toolbar());

		ptoolbar->set_orientation(ORIENTATION_HORIZONTAL);
		ptoolbar->set_border_width(2);

		ptoolbar->append(
				get_finished_toolbutton(ptooltips, "gtk-preferences",
					"_Preferences", "ufBackup's preferences"),
				sigc::mem_fun(*this, &RootWindow::on_button_preferences));

		ptoolbar->append(
				*manage(new SeparatorToolItem()));

		ptoolbar->append(
				get_finished_toolbutton(ptooltips, "gtk-add",
					"Add", "Create new job"),
				sigc::mem_fun(*this, &RootWindow::on_button_add_job));

		ptoolbar->append(
				get_finished_toolbutton(ptooltips, "gtk-remove",
					"Remove", "Remove selected job"),
				sigc::mem_fun(*this, &RootWindow::on_button_remove_job));

		ptoolbar->append(
				get_finished_toolbutton(ptooltips, "gtk-properties",
					"Properties", "Configure selected job"),
				sigc::mem_fun(*this, &RootWindow::on_button_job_properties));

		ptoolbar->append(
				*manage(new SeparatorToolItem()));

		ptoolbar->append(
				get_finished_toolbutton(ptooltips, "gtk-execute",
					"Backup", "Start the selected job"),
				sigc::mem_fun(*this, &RootWindow::on_button_start_backup));

		ptoolbar->append(
				get_finished_toolbutton(ptooltips, "gtk-undo",
					"Restore", "Restore a backup of the selected job"),
				sigc::mem_fun(*this, &RootWindow::on_button_restore_backup));

	ptoolbar->append(
				get_finished_toolbutton(ptooltips, "gtk-find",
					"List", "List archive contents"),
				sigc::mem_fun(*this, &RootWindow::on_button_listing));

		pmainbox->pack_start(*ptoolbar, PACK_SHRINK);
	}

	// setup joblist frame and add to main box
	Frame* pjobListFrame = manage(new Frame());
	{
		pjobListFrame->set_label("Backup jobs");
		pmainbox->pack_start(*pjobListFrame, PACK_EXPAND_WIDGET);
	}

	// setup joblist scrolled window and add to joblist frame	
	ScrolledWindow* pscrolledWindow = manage(new ScrolledWindow());
	{
		pscrolledWindow->set_border_width(2);
		pscrolledWindow->set_shadow_type(SHADOW_IN);
		pscrolledWindow->set_policy(POLICY_AUTOMATIC, POLICY_AUTOMATIC);
		pjobListFrame->add(*pscrolledWindow);
	}

	// setup joblist and add to joblist scrolled window
	{
		mrJobListData = ListStore::create(mJobListColumnRecord);
		mrJobListData->set_sort_column_id(0, SORT_ASCENDING);
		mrJobListData->signal_row_changed().connect(
				sigc::mem_fun(*this, &RootWindow::on_jobname_change));

		mJobList.set_model(mrJobListData);
		mJobList.append_column_editable("Name", mJobListColumnRecord.mShort);
		mJobList.append_column_editable("Description", mJobListColumnRecord.mLong);

		refresh_joblist();

		pscrolledWindow->add(mJobList);
	}
	show_all_children();
}

void RootWindow::on_button_preferences()
{
	// create dialog
	PreferencesDialog dialog(*this, mpConfig);

	// run and check if we have to save the config
	if(dialog.run() == 1)
		dialog.save();
}

void RootWindow::on_button_add_job()
{
	tSectionID sectionID = mpConfig->add_section(JOB);
	mpConfig->set_option(sectionID, "name", "Unnamed");
	mpConfig->set_option(sectionID, "desc", "Click the cell to change the name or description");
	mpConfig->set_option(sectionID, "root", "/");
	mpConfig->set_option(sectionID, "type", "full");
	mpConfig->set_option(sectionID, "level", "0");
	mpConfig->set_option(sectionID, "dirs", "0");
	mpConfig->set_option(sectionID, "dir_behaviour", "include");
	mpConfig->set_option(sectionID, "targettype", "dir");
	mpConfig->set_option(sectionID, "target", "/");
	mpConfig->save();
	refresh_joblist();
}

void RootWindow::on_button_remove_job()
{
	Glib::RefPtr<TreeSelection> refTreeSelection = mJobList.get_selection();
	TreeModel::const_iterator iter = refTreeSelection->get_selected();
	if(!iter) 
		return;
	mpConfig->remove_section((*iter)[mJobListColumnRecord.mSectionID]);
	mpConfig->save();
	refresh_joblist();
}

void RootWindow::on_button_job_properties()
{
	// get selected job id
	tSectionID selected = get_selected_job_id();
	if(selected == 0)
		return;

	// create dialog
	PropertiesDialog dialog(*this, mpConfig,
			selected);

	// run and check if we have to save the config
	if(dialog.run() == 1)
		dialog.save();
}

void RootWindow::on_button_listing()
{
	// get selected job id
	tSectionID selected = get_selected_job_id();
	if(selected == 0)
		return;

	// create dialog
	ListingDialog dialog(*this, mpConfig,
			selected);
	dialog.run();
}

void RootWindow::on_button_start_backup()
{
	// get selected job id
	tSectionID selected = get_selected_job_id();
	if(selected == 0)
		return;

	// create dialog
	BackupDialog dialog(*this, mpConfig,
			selected);
	dialog.run();
}

void RootWindow::on_button_about()
{
	AboutDialog dialog(*this);
	dialog.run();
}

tSectionID RootWindow::get_selected_job_id()
{
	// get sectionid of selected job
	Glib::RefPtr<TreeSelection> refTreeSelection = mJobList.get_selection();
	TreeModel::const_iterator iter = refTreeSelection->get_selected();
	if(!iter) 
		return 0;
	return (*iter)[mJobListColumnRecord.mSectionID];
}

void RootWindow::refresh_joblist()
{
	mrJobListData->clear();

	TreeModel::Row row;
	tIdList::const_iterator iter;
	tIdList list = mpConfig->get_ids(JOB);

	for(iter = list.begin(); iter != list.end(); ++iter) {
		row = *(mrJobListData->append());
		row[mJobListColumnRecord.mShort] = mpConfig->get_option(*iter, "name");
		row[mJobListColumnRecord.mLong] = mpConfig->get_option(*iter, "desc");
		row[mJobListColumnRecord.mSectionID] = *iter;
	}
}

void RootWindow::on_jobname_change(const Gtk::TreeModel::Path&, const Gtk::TreeModel::iterator& iter) 
{
	// get selected job id
	tSectionID selected = get_selected_job_id();
	if(selected == 0)
		return;

	// save job's name and description
	mpConfig->set_option(selected, "name", (*iter)[mJobListColumnRecord.mShort]);
	mpConfig->set_option(selected, "desc", (*iter)[mJobListColumnRecord.mLong]);
	mpConfig->save();
}
