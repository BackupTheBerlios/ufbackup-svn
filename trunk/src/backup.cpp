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
		auto_ptr<mask> dirMask;
		if(mpConfig->get_num_option(mSection, "dirs") == 0) {
			dirMask = auto_ptr<mask>(new bool_mask(true));
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
				dirMask = auto_ptr<mask>(new not_mask(ouMask));
			}
			else
			{
				dirMask = auto_ptr<mask>(new ou_mask(ouMask));
			}
		}

		archive* pnewArchivePtr;

		statistics stats = 
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
					&pnewArchivePtr,
					false, // alter atime
					false // stay on same fs
						);
		auto_ptr<archive> pnewArchive(pnewArchivePtr);

		// print statistics
		{
			stringstream msg;
			msg << "Backup done: " << endl
				<< stats.treated << " files/directories added" << endl
				<< stats.errored << " files could not be saved" << endl
				<< stats.ignored << " files/trees have been filtered out" <<endl;
			if(is_incremental())
				msg << stats.skipped << " files unchanged" << endl
					 << stats.deleted << " files have been deleted";
			message(msg.str().c_str());
		}

		message("Saving archive's catalogue...");

		// isolate archive
		{
			stringstream filename;
			filename << mpConfig->get_option(mSection, "name");
			if(is_incremental())
				filename << '.' << mLevel;
						
			op_isolate(
					*this,
					mpConfig->get_option(0, "home").c_str(),
					pnewArchive.get(),
					filename.str(),
					"dar",	
					true, // alow overwrite
					false, // notify overwrite
					mpConfig->get_bool_option(0, "verbose"),
					false, // pause
					bzip2,
					9,
					tmp.computer(), // slice size
					tmp.computer(), // first slice
					"", // execute
					crypto_none,
					"" // pass
					);
		}

		message("Done.");


	} catch(Egeneric& e) {
		error(e.get_message().c_str());
	}
}
