/* preferencesdialog.hpp
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

#ifndef preferencesdialog_hpp
#define preferencesdialog_hpp

#include <gtkmm/frame.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/notebook.h>
#include <gtkmm/tooltips.h>
#include <gtkmm/table.h>
#include <gtkmm/dialog.h>
#include <gtkmm/stock.h>
#include <gtkmm/optionmenu.h>


#include <config.hpp>

class PreferencesDialog : public Gtk::Dialog
{
	public:
		PreferencesDialog(Gtk::Window&, Config*);
		// save options to config
		void save();
	private:
		Gtk::Entry mOptionFileSize;
		Gtk::OptionMenu mOptionCompressionAlgo;
		Gtk::CheckButton mOptionNotifyOverwrite, mOptionVerbose;

		Config* mpConfig;
};	

#endif
