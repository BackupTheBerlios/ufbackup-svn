/* aboutdialog.hpp
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

#ifndef aboutdialog_hpp
#define aboutdialog_hpp

#include <gtkmm/messagedialog.h>
#include <gtkmm/stock.h>
#include <gtkmm/label.h>
#include <gtkmm/frame.h>

class AboutDialog : public Gtk::Dialog
{
	public:
	AboutDialog(Gtk::Window& parent);
	private:
};

#endif
