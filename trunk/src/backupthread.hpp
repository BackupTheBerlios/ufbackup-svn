/* backupthread.hpp
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

#ifndef backupthread_hpp
#define backupthread_hpp

#include <backup.hpp>
#include <thread.hpp>

class BackupThread : public Backup, public ufBackup::Thread
{
	public:
		BackupThread(Config* config, tSectionID section) : Action(config, section) {}
		void start() { ufBackup::Thread::start(); }
};
#endif
