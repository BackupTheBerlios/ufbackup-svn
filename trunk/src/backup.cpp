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


mask* Backup::gen_mask_from_multiple_option(const Glib::ustring& name, tMaskType maskType)
{
	Glib::ustring rootPath = mpConfig->get_option(mSection, "root");

	mask* pmask;
	if(mpConfig->get_num_option(mSection, name.c_str()) == 0) {
		if(maskType == INCLUDE_DIRS)
			pmask = new bool_mask(true);
		else if(maskType == EXCLUDE_DIRS)
			pmask = new bool_mask(false);
	}
	else
	{
		tValueList dirList = mpConfig->get_multiple_option(mSection, name.c_str());
		ou_mask ouMask;
		
		for(tValueList::const_iterator iter = dirList.begin();
				iter != dirList.end();	++iter)
		{
			stringstream path;
			path << rootPath << '/' << *iter;
			if(maskType == INCLUDE_DIRS)
				ouMask.add_mask(simple_path_mask(path.str(), true));
			else if(maskType == EXCLUDE_DIRS)
				ouMask.add_mask(simple_mask(path.str(), true));
		}
		pmask = new ou_mask(ouMask);
	}
	return pmask;
}


void Backup::action()
{
	try {
		message("Starting backup.\n");
		deci minFileSize("0");
		deci tmp("0");
		if(mpConfig->get_option(0, "compr_min_file_size") != "")
			minFileSize = deci(mpConfig->get_option(0, "compr_min_file_size"));

		Glib::ustring rootPath = mpConfig->get_option(mSection, "root");

		// setup directorie mask
		et_mask dirMask;
		{
			// include all from include_dirs option (or all if no dirs specified):
			dirMask.add_mask(*gen_mask_from_multiple_option("include_dirs", INCLUDE_DIRS));
			// with exception of the dirs of the prunes_dirs option:
			dirMask.add_mask(not_mask(*gen_mask_from_multiple_option("prunes_dirs", EXCLUDE_DIRS)));		
		}	

		// setup compression mask
		auto_ptr<mask> compressionMask;
		if(mpConfig->get_num_option(0, "compr_filters") == 0) {
			compressionMask = auto_ptr<mask>(new bool_mask(true));
			message("Using compression for all");
		}
		else
		{
			tValueList dirList = mpConfig->get_multiple_option(0, "compr_filters");
			ou_mask ouMask;

			for(tValueList::const_iterator iter = dirList.begin();
					iter != dirList.end();	++iter)
				ouMask.add_mask(simple_mask((*iter).c_str(), false));

			compressionMask = auto_ptr<mask>(new not_mask(ouMask));
		}

		// setup filter mask TODO: make function (compressoin + filter mask are the same)
		auto_ptr<mask> filterMask;
		if(mpConfig->get_num_option(mSection, "filters") == 0) {
			filterMask = auto_ptr<mask>(new bool_mask(true));
		}
		else
		{
			tValueList dirList = mpConfig->get_multiple_option(mSection, "filters");
			ou_mask ouMask;

			for(tValueList::const_iterator iter = dirList.begin();
					iter != dirList.end();	++iter)
				ouMask.add_mask(simple_mask((*iter).c_str(), false));

			filterMask = auto_ptr<mask>(new not_mask(ouMask));
		}

		archive* pnewArchivePtr;
		auto_ptr<archive> refArchive;

		if(is_incremental() && mLevel > 1)
		{
			message("Loading catalogue of previous level...");
			stringstream filename;
			filename << mpConfig->get_option(mSection, "name") << '.' << (mLevel -1);

			refArchive = auto_ptr<archive>(new archive(
					*this,
					mpConfig->get_option(0, "home").c_str(),
					filename.str(),
					"dar",
					crypto_none,
					"", // pass
					"", // in pipe
					"", // out pipe
					"", // execute
					mpConfig->get_bool_option(0, "verbose")					
					));
			message("Done.\n");
		}

		message("Creating archive...");

		compression comprAlgo;
		if(mpConfig->get_option(0, "compr_algo") == "bzip2"){
			message("Using bzip2");	
			comprAlgo = bzip2;
		}
		else if(mpConfig->get_option(0, "compr_algo") == "gzip")
			comprAlgo = gzip;
		else
			comprAlgo = none;
		

		statistics stats = 
			op_create(
					*this,
					rootPath.c_str(),
					mpConfig->get_option(mSection, "target").c_str(),
					refArchive.get(), // archive reference (for incremental backups)
					*filterMask, // file includes, excludes
					dirMask, // prunes
					mpConfig->get_option(mSection, "name").c_str(),
					"dar",
					true, // allow overwrite
					mpConfig->get_bool_option(0, "notify_overwrite"),
					mpConfig->get_bool_option(0, "verbose"),
					false, // pause between slices
					true, // pruned dir -> create empty
					comprAlgo,
					9, // compression level 0..9
					minFileSize.computer(), // slice size
					minFileSize.computer(), // first slice size
					false, // root ea
					false, // user ea
					"", // command between slices
					crypto_none,
					"", // crypto pass
					*compressionMask, // compression mask
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
			msg << endl << "Backup done: " << endl
				<< stats.treated << " files/directories added" << endl
				<< stats.errored << " files could not be saved" << endl
				<< stats.ignored << " files/trees have been filtered out" <<endl;
			if(is_incremental() && mLevel > 1)
				msg << stats.skipped << " files unchanged" << endl
					 << stats.deleted << " files have been deleted" << endl;
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
					comprAlgo,
					9,
					tmp.computer(), // slice size
					tmp.computer(), // first slice
					"", // execute
					crypto_none,
					"" // pass
					);
		}

		message("All done.");


	} catch(Egeneric& e) {
		error(e.get_message().c_str());
	}
}
