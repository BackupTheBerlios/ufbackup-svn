/* process.cpp
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

#include <process.hpp>

#ifdef DEBUG
using namespace std;
#endif

using namespace Glib;

std::ostream& operator<<(std::ostream& os, Flag& f)
{
	if(f.state == TRUE)
		os << f.flag;
	else
		os << " ";
};

void Process::start()
{
	if(pipe(mInPipe) != 0 || pipe(mOutPipe) != 0 || pipe(mErrPipe) != 0)
		throw domain_error("Can't create pipes");
	
	mPid = fork();
	
	if(mPid == -1)
		throw domain_error("Can't fork process");

	if(mPid > 0) // Parent
	{
		close(mInPipe[1]);
		close(mErrPipe[1]);
		close(mOutPipe[1]);

		// connect IO handlers (called if child says something through the pipes)
		signal_io().connect(sigc::mem_fun(*this, &Process::on_child_input),
				mOutPipe[0], IO_IN | IO_HUP);
		signal_io().connect(sigc::mem_fun(*this, &Process::on_child_error),
				mErrPipe[0], IO_IN | IO_HUP);

	}else // Child
	{
		try {
			close(mInPipe[0]);	
			close(mErrPipe[0]);	
			close(mOutPipe[0]);	

			if(dup2(mOutPipe[1], STDOUT_FILENO) != STDOUT_FILENO
					|| dup2(mErrPipe[1], STDERR_FILENO) != STDERR_FILENO
					|| dup2(mInPipe[1], STDIN_FILENO) != STDIN_FILENO)
				throw domain_error("e3");

			close(mInPipe[1]);
			close(mErrPipe[1]);
			close(mOutPipe[1]);

			int ret = system(get_command());
			if(ret == -1)
				throw domain_error("e1");
			if(WEXITSTATUS(ret) != 0)
				throw domain_error("e2");
		} catch (domain_error e) {
			cerr << "@ufbackup: " << e.what() << endl;
			exit(0);				
		}		

		cerr << "@ufbackup: done" << endl;
		exit(0);
	}
}

void Process::end()
{
#ifdef DEBUG
	cerr << "Process End" << endl;
#endif
	if(mPid != 0) {
		// check if there is still data in any of the two pipes.
		// if so, simply return: end() will be called again after
		// there is no data left in the other pipe (IO_HUP)
		if(mErrPipe[0] != -1 || mOutPipe[0] != -1)
			return;
		mSignalEnd.emit();
	}
}

bool Process::parse(ustring& message, int type)
{
	if(message[0] == '@') // some message for ufbackup?
	{
		if(message.substr(0, 12) == "@ufbackup: e") // error
		{
			mErrorMessage = message.substr(11, message.size() - 1); // number without \n
			mRet = false;
			return false;
		}
		else if(message == "@ufbackup: done\n") // successfull
			mRet = true;
	}
	else if(type == 1)
		return parse_stdout(message);
	else
		return parse_stderr(message);		
	return true;
}

ustring read_pipe(int fd, int& count)
{
	char buf[1024];
	count = read(fd, buf, sizeof(buf));
	buf[count] = '\0';
	return buf;
}

bool Process::on_child(int type, Glib::IOCondition cond)
{
	int* pPipe;
	ustring* pBuffer;

	if(type == 1) {
		pPipe = &mOutPipe[0];
		pBuffer = &mOutBuffer;
	}
	else { 
		pPipe = &mErrPipe[0];
		pBuffer = &mErrBuffer;
	}
	
	if(*pPipe == -1)
		return false;
	
	if(cond == IO_HUP) // broken pipe
	{ 
		// parse remainings of the buffer
		if(pBuffer->size() > 0) {
			if(parse(*pBuffer, type) == true)
				mpLog->write(*pBuffer, type);
		}
		close(*pPipe);
		*pPipe = -1;
		end();
		return false; // disconnect this IO handler
	}

	// read the pipe's data
	int count;
	*pBuffer += read_pipe(*pPipe, count);

	if(count > 0) { // erm. is this possible?
		int pos;
		while((pos = pBuffer->find('\n')) && (pos != Glib::ustring::npos)) { // for each full line
			// parse message (and remove from buffer) and write it if true is returned
			ustring message = pBuffer->substr(0, pos + 1);
			pBuffer->erase(0, pos + 1);

			if(parse(message, type) == true)
				mpLog->write(message, type);
		} 
		return true;
	}
	else
		if(type == 1)
			on_child_input(IO_HUP);
		else
			on_child_error(IO_HUP);
}

bool Process::on_child_input(Glib::IOCondition cond)
{
	return on_child(1, cond);
}

bool Process::on_child_error(Glib::IOCondition cond)
{
	return on_child(2, cond);
}
