/* preferencesdialog.cpp
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

#include <preferencesdialog.hpp>

using namespace Gtk;

PreferencesDialog::PreferencesDialog(Window& parent, Config* config)
	: Dialog("Preferences", parent), mpConfig(config),
	mOptionVerbose("Verbose output")
{
	set_default_size(350,250);
	// add close button
	add_button(Stock::SAVE, 1);
	add_button(Stock::CANCEL, 0);

	// add tooltips class
	Tooltips* ptooltips = manage(new Tooltips());
		
	// setup main box
	VBox* pvBox = get_vbox(); 
	pvBox->set_border_width(2);
	
	// setup notebook and add it to box
	Notebook* pnotebook = manage(new Notebook());
	pvBox->pack_start(*pnotebook, PACK_EXPAND_WIDGET);

	// setup general options page and add it to notebook
	{
		VBox* pbox = manage(new VBox());

		mOptionVerbose.set_active(
				mpConfig->get_bool_option(0, "verbose"));
		pbox->pack_start(mOptionVerbose, PACK_SHRINK, 5);
		ptooltips->set_tip(mOptionVerbose,
				"Whether the backends should print verbose messages");

		pbox->set_border_width(2);
		pnotebook->append_page(*pbox, "General");
	}

	// setup command options page and add it to notebook
	{
		Table* ptable = manage(new Table(1,2));
		ptable->set_row_spacings(3);
		ptable->set_col_spacings(7);
		ptable->set_border_width(2);

		Label* pLabel;
		int row = 0;

		pLabel = manage(new Label("Afio:"));
		pLabel->set_alignment(1,0.5);
		ptable->attach(*pLabel, 0, 1, row, row + 1, SHRINK, SHRINK);
		ptable->attach(mOptionAfio, 1, 2, row, row + 1, SHRINK, SHRINK);
		ptooltips->set_tip(mOptionAfio,
				"afio is the main backend of ufBackup\nDefault: afio");
		mOptionAfio.set_text(
				mpConfig->get_option(0, "afio"));
		row++;

		pnotebook->append_page(*ptable, "Commands");
	}

	// done. show all
	show_all_children();
}

void PreferencesDialog::save()
{
	mpConfig->set_bool_option(0, "verbose",
			mOptionVerbose.get_active());
	mpConfig->set_option(0, "afio",
			mOptionAfio.get_text());
	mpConfig->save();
}
