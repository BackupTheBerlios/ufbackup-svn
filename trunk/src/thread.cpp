/* thread.cpp
 *
 * Copyright (C) 2002 Christian Neumann
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

#include <thread.hpp>

using namespace ufBackup;
using namespace std;

void Thread::start()
{
	// connect flush_message() to the new-message notifier
	cerr << "Thread start" << endl;
	mNewMessageNotifier.connect(
			sigc::mem_fun(*this, &Thread::flush_message));
	// start backup thread
/*	mpBackupThread = Glib::Thread::create(
			sigc::mem_fun(*this, &Thread::action_), true); */
			ost::Thread::start();
}

void Thread::stop()
{
	terminate();
	error("Backup aborted.");
	mEndNotifier.emit();
}

void Thread::run()
{
	cerr << "Thread started :)" << endl;
	action();
	mEndNotifier.emit();
}

bool Thread::question(tMessage message)
{
	// first, screen the message
	write(message);
	
	// wait if last message has not been flushed yet.
	Glib::Mutex::Lock lock (mMessageMutex);
	if(mMessage.second.size() > 0) {
		mMessageCondition.wait(mMessageMutex);
	}

	return mAnswer;
}

void Thread::flush_message()
{
	Glib::Mutex::Lock lock (mMessageMutex);

	// "move" message to log
	if(mMessage.first == QUESTION) // FIXME: add sth like mpLog->ask()
	{
		Gtk::MessageDialog dialog(
				mMessage.second,
				false,
				Gtk::MESSAGE_QUESTION,
				Gtk::BUTTONS_YES_NO);
		int ret = dialog.run();
		if(ret == Gtk::RESPONSE_YES)
			mAnswer = true;
		else
			mAnswer = false;
		
	}else {
		mpLog->write(mMessage);
	}
	
	mMessage.second.clear();

	// tell the other thread that the message has been flushed
	mMessageCondition.signal();
}

void Thread::write(tMessage message)
{
	Glib::Mutex::Lock lock (mMessageMutex);
	// wait if last message has not been flushed yet.
	if(mMessage.second.size() > 0) {
		mMessageCondition.wait(mMessageMutex);
	}
	
	// dump message to buffer
	mMessage = message;
	
	// tell caller thread about the new message
	mNewMessageNotifier.emit();
}

void Thread::exit()
{
//	throw Glib::Thread::Exit();
}
