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
	mOptionVerbose("Verbose output"), mOptionNotifyOverwrite("Ask before owerwriting archives")
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

		mOptionNotifyOverwrite.set_active(
				mpConfig->get_bool_option(0, "notify_overwrite"));
		pbox->pack_start(mOptionNotifyOverwrite, PACK_SHRINK, 5);
		ptooltips->set_tip(mOptionNotifyOverwrite,
				"Whether ufBackup should ask you before overwriting existing archives");
		
		pbox->set_border_width(2);
		pnotebook->append_page(*pbox, "General");
	}

	// setup compression options page and add it to notebook
	{
		VBox* pbox = manage(new VBox());

		Table* ptable = manage(new Table(2,2));
		int rowcount(0);
		pbox->pack_start(*ptable, PACK_EXPAND_WIDGET, 5);
				
		{
			Label* label = manage(new Label("Minimal file size:"));
			mOptionFileSize.set_text(
					mpConfig->get_option(0, "compr_min_file_size"));
			ptooltips->set_tip(mOptionFileSize,
					"Minimal size of files to compress (in bytes)");
			ptable->attach(*label, 0, 1, rowcount, rowcount + 1, FILL, FILL, 5, 0);
			ptable->attach(mOptionFileSize, 1, 2, rowcount, rowcount + 1, FILL, FILL, 5, 0);
			rowcount++;
		}		
		{
			{
				Menu* pmenu = manage(new Menu());

				pmenu->append(*manage(new MenuItem("bzip2")));
				pmenu->append(*manage(new MenuItem("gzip")));

				Glib::ustring selected = mpConfig->get_option(0, "compr_algo");

				if(selected == "bzip2")
					pmenu->set_active(0);
				else
					pmenu->set_active(1);

				mOptionCompressionAlgo.set_menu(*pmenu);
			}

			Label* label = manage(new Label("Algorithm:"));
			ptooltips->set_tip(mOptionCompressionAlgo,
					"Which compression algorithm should be used. You are advised to select bzip2.");
			ptable->attach(*label, 0, 1, rowcount, rowcount + 1, FILL, FILL, 5, 0);
			ptable->attach(mOptionCompressionAlgo, 1, 2, rowcount, rowcount + 1, FILL, FILL, 5, 0);
			rowcount++;
		}


		// add filter option
		{
			Frame* pframe = manage(new Frame("Compression filters"));
			pframe->add(mOptionCompressionFilter);
			mOptionCompressionFilter.set_config(mpConfig);
			mOptionCompressionFilter.set_section(0);
			mOptionCompressionFilter.set_name("compr_filters");
			mOptionCompressionFilter.reload();
			pbox->pack_start(*pframe, PACK_EXPAND_WIDGET);
		}

	
		pbox->set_border_width(2);
		pnotebook->append_page(*pbox, "Compression");
	}


	// done. show all
	show_all_children();
}

void PreferencesDialog::save()
{
	mpConfig->set_bool_option(0, "verbose",
			mOptionVerbose.get_active());
	mpConfig->set_bool_option(0, "notify_overwrite",
			mOptionNotifyOverwrite.get_active());
	mpConfig->set_option(0, "compr_min_file_size", mOptionFileSize.get_text().c_str());
	mOptionCompressionFilter.save_to_config();
	
	{
		Glib::ustring algo = "gzip";
		if(mOptionCompressionAlgo.get_history() == 0)
			algo = "bzip2";
		mpConfig->set_option(0, "compr_algo", algo);
	}
	mpConfig->save();
}
