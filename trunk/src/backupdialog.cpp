/* backupdialog.cpp
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

#include <backupdialog.hpp>

using namespace Gtk;
using namespace __gnu_cxx;
using namespace Glib;
using namespace std;

BackupDialog::BackupDialog(Gtk::Window& parent, Config* ppConfig, tSectionID section)
	: Dialog("Backup", parent), mSection(section),
	mpConfig(ppConfig), mpBackupThread(NULL), mOptionLevel(1.0),
	mExecButton(Stock::EXECUTE)
{
#ifdef DEBUG
	cerr << "BackupDialog constructor" << endl;
#endif

	set_default_size(650,550);
	// add close button
	add_button(Stock::CLOSE, 1);

	VBox* mainbox = get_vbox(); 
	
	// add Option frame
	Frame* poptionFrame = manage(new Frame());

	poptionFrame->set_border_width(2);
	poptionFrame->set_label("Options");
	mainbox->pack_start(*poptionFrame, false, false);	

	/// add options
	{
		ustring backupType = mpConfig->get_option(mSection, "type");
		Table* optionTable = manage(new Table(2,2));

		Label* label = manage(new Label("Level:"));
		optionTable->attach(*label, 0, 1, 1, 2, Gtk::FILL, Gtk::FILL, 5, 0);
		optionTable->attach(mOptionLevel, 1, 2, 1, 2, Gtk::FILL, Gtk::FILL, 5, 0);						
		label->set_sensitive(false);
		mOptionLevel.set_sensitive(false);

		if(backupType == "incremental") // incremental backup
		{
//TODO: automatic level			mOptionLevel.set_value(mpConfig->get_num_option(mSection, "level"));
			mOptionLevel.set_range(1,999);
			mOptionLevel.set_increments(1,2);
			mOptionLevel.set_numeric();
			mOptionLevel.set_sensitive();
			label->set_sensitive();
		}
		
		// add option table to option frame
		poptionFrame->add(*optionTable);
	}


	// Buttons
	HButtonBox* buttons = manage(new HButtonBox(BUTTONBOX_START));
	buttons->set_border_width(2);
	buttons->set_spacing(10);
	mainbox->pack_start(*buttons, false, false);

	mExecButton.set_use_stock(true);
	mExecButton.signal_clicked().connect( sigc::mem_fun(*this, &BackupDialog::on_button_exec) );
	buttons->pack_start(mExecButton);

	// Log Frame
	Frame* logframe = manage(new Frame());
	logframe->set_border_width(2);
	logframe->set_label("Output");
	mainbox->pack_start(*logframe, true, true);
	
	ScrolledWindow* scrolled = manage(new ScrolledWindow());
	scrolled->set_policy(POLICY_AUTOMATIC, POLICY_ALWAYS);
	scrolled->set_shadow_type(SHADOW_IN);
	scrolled->set_border_width(2);
	logframe->add(*scrolled);

	// Log buffer

	mrefTag = Gtk::TextBuffer::Tag::create();
	mrefTag->property_foreground() = "red";

	Glib::RefPtr<Gtk::TextBuffer::TagTable> refTagTable = Gtk::TextBuffer::TagTable::create();
	refTagTable->add(mrefTag);
	
	mrefLogBuffer = Gtk::TextBuffer::create(refTagTable);
	mLogEnd = mrefLogBuffer->create_mark("bottom", mrefLogBuffer->end());
	mrefLogBuffer->signal_insert().connect(sigc::mem_fun(*this, &BackupDialog::on_buffer_insert));
	
	mLogView.set_buffer(mrefLogBuffer);
	mLogView.set_editable(false);
	mLogView.set_cursor_visible(false);
	scrolled->add(mLogView);

	mpLog = new TextBufferLog(mrefTag, mrefLogBuffer, &mLogView);
	signal_delete_event().connect(
			sigc::mem_fun(*this, &BackupDialog::on_delete_it));

	show_all_children();
}

void BackupDialog::on_button_exec()
{

	if(mpBackupThread != NULL) {
		mExecButton.set_sensitive(false);
		mpBackupThread->stop();
		return;
	}
	
	mExecButton.set_label("gtk-stop");

	try {
		cerr << "try" << endl;
		mpBackupThread = new BackupThread(mpConfig, mSection); 
		mpBackupThread->set_log(mpLog);
		mpBackupThread->set_level((unsigned int) mOptionLevel.get_value());
		
		cerr << "try done" << endl;
	
		// Connect end signal and start
		mpBackupThread->signal_end().connect(sigc::mem_fun(*this, &BackupDialog::on_backup_end));

		mpBackupThread->start();
	}
	catch(exception& e) {
		cerr << "error catched" << endl;
		MessageDialog dialog(*this, e.what(), false, MESSAGE_ERROR);
		dialog.run();
		on_backup_end();
	}
}

void BackupDialog::on_backup_end()
{
	delete mpBackupThread;
	mpBackupThread = NULL;
	mExecButton.set_label("gtk-execute");
	mExecButton.set_sensitive(true);
}

bool BackupDialog::on_delete_event(GdkEventAny*)
{
	cerr << "delete_event" << endl;
	return true;
}


bool BackupDialog::on_delete_it(GdkEventAny*)
{
	cerr << "delete_event" << endl;
	return true;
}
void BackupDialog::on_response(int response_id)
{
	cerr << "response: " << response_id << endl;
}

void BackupDialog::on_buffer_insert(
const TextBuffer::iterator&,
const Glib::ustring&, int)
{
//	if(mpBackupThread)
//		mpBackupThread->log_mutex().enterMutex();
	mrefLogBuffer->delete_mark(mLogEnd);
	mLogEnd = mrefLogBuffer->create_mark("bottom", mrefLogBuffer->end());
	mLogView.scroll_mark_onscreen(mLogEnd);
//	if(mpBackupThread)
//		mpBackupThread->log_mutex().leaveMutex();
}
