/* thread.hpp
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

#ifndef thread_hpp
#define thread_hpp


#include <glibmm/thread.h>
#include <glibmm/dispatcher.h>
#include <gtkmm/messagedialog.h>
#include <action.hpp>
#include <log.hpp>
#include <sstream>
#include <cc++/thread.h>

namespace ufBackup {

	/** Thread. */
	class Thread : public virtual Action, public ost::Thread
	{
		public:
			Thread() {}
			/** Start thread execution. */
			void start();
			/** Returns dispatcher which will be emitted at end of thread execution. */
			Glib::Dispatcher& signal_end() { return mEndNotifier; }
			void set_log(Log* log) { mpLog = log; }
			void stop();

		private:
			
			/** Write the executed thread's message. */
			void flush_message();

			void action_();
			void run();

			void write(tMessage);
			bool question(tMessage);

			/** Stop thread's execution. */
			void exit();

		//	Glib::Thread *mpBackupThread;
			Glib::Dispatcher
				// Notify caller thread of new message
				mNewMessageNotifier,
			// Notify caller thread of end of backup 
			mEndNotifier;
			// secure access to mMessage
			Glib::Mutex mMessageMutex;
			// messagecondition: mMessage.size() > 0. write() waits, flush_message() signals
			Glib::Cond mMessageCondition;
			// the communication buffer
			tMessage mMessage;
			// answer buffer
			bool mAnswer;
			// the messages are flushed to this log
			Log* mpLog;
	};

}
#endif
