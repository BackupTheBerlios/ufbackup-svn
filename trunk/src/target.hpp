/* target.hpp
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

#ifndef target_hpp
#define target_hpp

#include <glibmm/ustring.h>
#include <sstream>

// Class representing the target where the backup should go
class Target
{
	public:
		Target(bool doprecmd = false) : mDoPrecmd(doprecmd) {}
		virtual Glib::ustring get_precmd_string() {};
		virtual Glib::ustring get_target_string() {};
		bool do_precmd() { return mDoPrecmd; };
	protected:
		bool mDoPrecmd;
};

// Backup to normal directory
class DirTarget : public Target
{
	public:
		DirTarget(Glib::ustring dirname, Glib::ustring backupname) 
			: Target()
			{
				std::stringstream name;
				name << dirname << '/' << backupname << ".ufbackup.cpio";
				mBackupname = name.str();
			}
		Glib::ustring get_target_string() {
			return mBackupname;
		}
	private:
		Glib::ustring mBackupname;
		
};

#endif
