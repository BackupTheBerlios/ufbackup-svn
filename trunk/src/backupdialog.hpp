/* backupdialog.hpp
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

#ifndef backupdialog_hpp
#define backupdialog_hpp


#include <gtkmm/label.h>
#include <gtkmm/frame.h>
#include <gtkmm/button.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/textview.h>
#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/table.h>
#include <gtkmm/stock.h>
#include <gtkmm/dialog.h>
#include <gtkmm/messagedialog.h>

#include <backupthread.hpp>
#include <stdexcept>
#include <config.hpp>
#include <log.hpp>
#include <utility> // std::pair

#ifdef DEBUG
#include <iostream>
#endif

class BackupDialog : public Gtk::Dialog
{
	public:
		BackupDialog(Gtk::Window&, Config*, tSectionID);

	private:
		Config* mpConfig;
		BackupThread* mpBackupThread;
		TextBufferLog* mpLog;

		Glib::RefPtr<Gtk::TextBuffer> mrefLogBuffer;
		Gtk::TextView mLogView;
		Glib::RefPtr<Gtk::TextMark> mLogEnd;
		Glib::RefPtr<Gtk::TextBuffer::Tag> mrefTag;

		Gtk::Button mExecButton;
		tSectionID mSection;
		
		// options
		Gtk::SpinButton mOptionLevel;

		void on_button_exec();
		void on_buffer_insert(const Gtk::TextBuffer::iterator&, const Glib::ustring&, int);
		void on_backup_end();
		bool on_delete_event(GdkEventAny*);
		bool on_delete_it(GdkEventAny*);
		void on_response(int);
};

#endif
