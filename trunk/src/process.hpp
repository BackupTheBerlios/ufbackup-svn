/* process.hpp
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

#ifndef process_hpp
#define process_hpp

#include <glibmm/ustring.h>
#include <sigc++/sigc++.h>
#include <stdexcept>
#include <csignal>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <log.hpp>
#include <iostream>

#ifdef DEBUG
using namespace std;
#endif

class Flag
{
	friend std::ostream& operator<<(std::ostream&, Flag&);
	public:
		Flag(Glib::ustring f) : flag(f), state(false) {};
		void set(bool s) {
			state = s;
		}
	private:
		bool state;
		Glib::ustring flag;
};


class Process //: public sigc::Object
{
	public:
		Process() : mVerbose("-v"), mRet(false), mOutBuffer(), mErrBuffer(),
		mSignalEnd() {
#ifdef DEBUG
			cerr << "Process created" << endl;
#endif
		};
		virtual ~Process() {
			kill(mPid, 9);
			mPid = 0;
		}

		// Will fork the process
		// Parent will do some important stuff and return
		// Child will do some important stuff and exec the program
		void start();
		void set_log(Log* log) { mpLog = log; }
		void set_verbose(bool v) { mVerbose.set(v); }
		bool get_ret() { return mRet; }
		Glib::ustring get_error() { return mErrorMessage; }
		SigC::Signal0<void> signal_end() { return mSignalEnd; }

	protected:
		Flag mVerbose;
		Log* mpLog;
		virtual const char* get_command() {};
		// parse messages from the child and return true if they
		// should be logged
		virtual bool parse_stderr(Glib::ustring& message) { return true; };
		virtual bool parse_stdout(Glib::ustring& message) { return true; };
		bool mRet;
	private:
		virtual bool parse(Glib::ustring&, int);
		SigC::Signal0<void> mSignalEnd;
		Glib::ustring mErrorMessage, mOutBuffer, mErrBuffer;
		
		bool on_child(int type, Glib::IOCondition);
		bool on_child_input(Glib::IOCondition);
		bool on_child_error(Glib::IOCondition);

		// communication pipes [0] = parent's end, [1] = child's end
		int mInPipe[2], mOutPipe[2], mErrPipe[2];
		
		int mPid;
		void end();
};

#endif
