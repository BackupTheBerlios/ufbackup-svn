/* propertiesdialog.hpp
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

#ifndef propertiesdialog_hpp
#define propertiesdialog_hpp

#include <gtkmm/frame.h>
#include <gtkmm/entry.h>
#include <gtkmm/optionmenu.h>
#include <gtkmm/box.h>
#include <gtkmm/stock.h>
#include <gtkmm/table.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/dialog.h>

#include <multipleoptionview.hpp>
#include <config.hpp>

#ifdef DEBUG
#include <iostream>
#endif

typedef SigC::Signal0<void> tSignalRefresh;

class PropertiesDialog : public Gtk::Dialog
{
	public:
	PropertiesDialog(Gtk::Window&, Config*, tSectionID);
	// save options to config
	void save();
	
	private:
	
	Config* mpConfig;
	tSectionID mSection;

	// option widgets
	Gtk::Label mOptionRoot, mOptionTarget;
	Gtk::OptionMenu mOptionType, mOptionDirBehaviour;

	void on_root_choose_button();
	void on_targetdir_choose_button();

	MultipleFilterOptionView mFilterView;
	MultipleDirOptionView mOptionDirectoryIncludes;
	MultipleFilterOptionView mOptionDirectoryPrunes;
};

#endif
