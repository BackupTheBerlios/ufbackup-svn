/* backup.hpp
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

#ifndef backup_hpp
#define backup_hpp

#include <daraction.hpp>
#include <memory>
#include <unistd.h>


/// Class which creates a backup of some data
class Backup : public DARAction
{
	public:
		Backup() : mLevel(1) {};
		Backup(Config* config, tSectionID section) : Action(config, section) { }
		virtual void start() { action(); }
	protected:
		void action();
	private:
		void set_level(unsigned int level) {
			mLevel = level;
		}
		bool is_incremental() {
			if(mpConfig->get_option(mSection, "type") == "incremental")
				return true;
			else
				return false;
		}
		unsigned int mLevel;
};

#endif
