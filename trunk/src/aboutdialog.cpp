/* aboutdialog.cpp
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

#include <aboutdialog.hpp>

using namespace Gtk;

AboutDialog::AboutDialog(Window& parent)
	: Dialog("About", parent)
{
	get_vbox()->set_border_width(10);
	
	// add ok button
	add_button(Stock::OK, 1);

	// add label
	{
		Label* plabel;
		Frame* pframe;

		plabel = manage(new Label( "ufBackup version 0.1.0 (unstable/development)\n" \
					"Copyright (C) 2004 Christian Neumann <christian@altfrau.de>"));

		get_vbox()->pack_start(*plabel, PACK_EXPAND_WIDGET);

		pframe = manage(new Frame());
		pframe->set_border_width(10);
		pframe->set_label("WARNING");

		plabel = manage(new Label(
					"This is an unstable and untested version of\n" \
					"ufBackup. Don't use it to backup important\n" \
					"data! You have been warned."));
		pframe->add(*plabel);
		get_vbox()->pack_start(*pframe, PACK_EXPAND_WIDGET);

		pframe = manage(new Frame());
		pframe->set_border_width(10);
		pframe->set_label("License information");
		plabel = manage(new Label(
               "This program is free software; you can redistribute it and/or modify\n" \
               "it under the terms of the GNU General Public License as published by\n" \
               "the Free Software Foundation; either version 2 of the License, or\n" \
               "(at your option) any later version.\n" \
               "This program is distributed in the hope that it will be useful,\n" \
               "but WITHOUT ANY WARRANTY; without even the implied warranty of\n" \
               "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n" \
               "GNU General Public License for more details.\n\n" \
               "You should have received a copy of the GNU General Public License\n" \
               "along with this program; if not, write to the Free Software\n" \
               "Foundation, Inc., 59 Temple Place - Suite 330,\n" \
               "Boston, MA 02111-1307, USA."));
		pframe->add(*plabel);
		get_vbox()->pack_start(*pframe, PACK_EXPAND_WIDGET);
	}
	show_all_children();
}
