/* backup.cpp
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

#include <backup.hpp>

using namespace std;
using namespace libdar;


void Backup::action()
{
	try {
		message("Starting Backup");
		deci tmp("0");

		Glib::ustring rootPath = mpConfig->get_option(mSection, "root");

		// setup directorie mask
		mask* dirMask;
		if(mpConfig->get_num_option(mSection, "dirs") == 0) {
			dirMask = new bool_mask(true);
		}
		else
		{
			tValueList dirList = mpConfig->get_multiple_option(mSection, "dirs");
			ou_mask ouMask;

			for(tValueList::const_iterator iter = dirList.begin();
					iter != dirList.end();	++iter)
			{
				stringstream path;
				path << rootPath << '/' << *iter;

				if(mpConfig->get_option(mSection, "dir_behaviour") == "include")
					ouMask.add_mask(exclude_dir_mask(path.str(), true));
				else
					ouMask.add_mask(simple_path_mask(path.str(), true));
			}

			// exclude or include these directories			
			if(mpConfig->get_option(mSection, "dir_behaviour") == "include")
			{
				dirMask = new not_mask(ouMask);
			}
			else
			{
				dirMask = new ou_mask(ouMask);
			}
		}

		statistics stat = 
			op_create(
					*this,
					rootPath.c_str(),
					mpConfig->get_option(mSection, "target").c_str(),
					NULL, // archive reference (for incremental backups)
					bool_mask(true), // file includes, excludes
					*dirMask, // prunes
					mpConfig->get_option(mSection, "name").c_str(),
					"dar",
					true, // allow overwrite
					true, // warn overwrite
					mpConfig->get_bool_option(0, "verbose"),
					false, // pause between slices
					true, // pruned dir -> create empty
					bzip2,
					9, // compression level 0..9
					tmp.computer(), // slice size
					tmp.computer(), // first slice size
					false, // root ea
					false, // user ea
					"", // command between slices
					crypto_none,
					"", // crypto pass
					bool_mask(true), // compression mask
					tmp.computer(), // min file size to compress
					false, // nodump flag
					false, // ignore ownership
					tmp.computer(), // hourshift
					false, // dummy run
					NULL,
					false, // alter atime
					false // stay on same fs
						);
					delete dirMask;

					error("Done.");
	} catch(Egeneric& e) {
		error(e.get_message().c_str());
	}
}
