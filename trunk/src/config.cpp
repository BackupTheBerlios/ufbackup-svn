/* config.cpp
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

#include <config.hpp>

#ifdef DEBUG
#include <iostream>
#endif

using namespace std;
using namespace Glib;

Config::Config(const char* filename) : mFilename(filename), mSections(), mSectionCount(0)
{
#ifdef DEBUG
	cerr << "Config constructed" << endl;
#endif
	mSections.resize(1);
	mSections[1].first = GLOBAL;
	parse();
}

ustring Config::get_option(unsigned short sectionid, const char* name) const
{
#ifdef DEBUG
	cerr << "get option @sectionid " << sectionid << " (" << mSections[sectionid].first << ") : name=" << name;
#endif
	tOptionList::const_iterator iter;

	if(sectionid >= mSections.size())
		throw out_of_range("Sectionid too high");

	iter = mSections[sectionid].second.find(name);
	if(iter != mSections[sectionid].second.end()) {
#ifdef DEBUG
	cerr << "	-> Success, got: " << iter->second << endl;
#endif
		return iter->second;
	}
	else {
		return "";
	}
}

tValueList Config::get_multiple_option(tSectionID sectionid, const char* name) const
{
	tValueList list;

	// get basename of option (e.g. files => file)
	ustring optname = name;
	ustring basename = optname.substr(0, optname.size() - 1);

	tFileNumber filenumber, files;
	files = get_num_option(sectionid, name);
	for(filenumber = 0; filenumber != files; ++filenumber)
	{
		std::stringstream option;
		option << basename << filenumber;
		list.push_back(get_option(sectionid, option.str().c_str()));
	}
	return list;
}

void Config::set_option(unsigned short sectionid, const char* name, ustring value)
{
#ifdef DEBUG
	cerr << "set option @sectionid " << sectionid << " (" << mSections[sectionid].first << ") : name=" << name << " value=" << value << endl;
#endif
	tOptionList::iterator iter;

	iter = mSections[sectionid].second.find(name);

	if(iter != mSections[sectionid].second.end())
		iter->second = value;
	else
		mSections[sectionid].second.insert(make_pair(name, value));
}

void Config::save() const
{
	// open config file
	ofstream confFile(mFilename.c_str());
	if(not confFile.is_open())
		throw domain_error("Could not write to config file");

	// for each section
	tSectionList::const_iterator iter;
	for(iter = mSections.begin(); iter != mSections.end(); ++iter)
	{
		// write section header
		ustring header;
		if(iter->first == JOB)
			header = "job";
		else
			header = "global";
		confFile << '[' << header << ']' << endl;

		// for each option
		tOptionList::const_iterator option_iter;
		for(option_iter = iter->second.begin(); option_iter != iter->second.end(); ++option_iter)
		{
			// write option
			confFile << option_iter->first << '=' << option_iter->second << endl;
		}

		confFile << endl;
	}
	
	// done. close file
	confFile.close();
}

void Config::add_multiple_option(tSectionID sectionid, Glib::ustring name, Glib::ustring value)
{
	// add option (samples -> sampleN) and increase the option's value count by one
	long files = get_num_option(sectionid, name.c_str());
	
	std::stringstream option;
	option << name.substr(0, name.size() -1) << files;
	set_option(sectionid, option.str().c_str(), value);	
	
	set_option(sectionid, name.c_str(), ++files);
}

void Config::parse()
{
#ifdef DEBUG
	cerr << "Parsing config file" << endl;
#endif

	// using std::string instead of Glib::ustring because of incompatible getline()
	// could be dangerous? (see buffer.substr)
	string buffer;
	unsigned short sectioncount(0);
	unsigned short sectionid;
	bool new_section(false);
	configsection section(UNDEF);
	ifstream confFile(mFilename.c_str());
	if(not confFile.is_open())
		throw domain_error("Could not read config file");
	int line(1);

	while(not confFile.eof()) {
		getline(confFile, buffer);
		if(buffer == "[global]")
		{
			section = GLOBAL;
			sectionid = 0;
		}
		else if(buffer == "[job]")
		{
			section = JOB;
		   sectionid = add_section(section);
			new_section = true;
		}
		else if(buffer != "" && section != UNDEF) {
			int pos;
			string::const_iterator iter;
			iter = find(buffer.begin(), buffer.end(), '=');
			if(iter == buffer.end()) {
				stringstream error;
				error << "Error parsing config file at line " << line;
				throw domain_error(error.str());
			}
			pos = iter - buffer.begin();

			set_option(sectionid, buffer.substr(0, pos).c_str(), buffer.substr(pos + 1, buffer.size()));
		}
		else if(buffer != "") { 
				stringstream error;
				error << "Error parsing config file at line " << line;
				throw domain_error(error.str());
		}
		if(new_section)
		{
			if(sectionid == mSections.size()) // no mSections[sectionid] but mSections[sectionid -1]
			{
				mSections.resize(sectionid + 1);
				mSections[sectionid].first = section; 
			}
			else if(sectionid > mSections.size())
				throw domain_error("Unknown sectionid");
			new_section = false;			
		}
		line++;
	}
}

tIdList Config::get_ids(configsection section) const
{
	tIdList list;
	for(tSectionList::const_iterator iter = mSections.begin(); iter != mSections.end(); ++iter)
		if(iter->first == section) {
#ifdef DEBUG
	cerr << "Pushed sectionid " << mSections.begin() - iter << endl;
#endif
			list.push_back(iter - mSections.begin());
		}
	return list;
}

void Config::remove_multiple_option_entries(const tSectionID sectionid,
		ustring name)
{
#ifdef DEBUG
	cerr << "remove multiple option \"" << name
		<< "\" entries" << endl;
#endif
	// get basename of option (e.g. files => file)
	ustring basename = name.substr(0, name.size() - 1);

	// erase each entry (name0 .. nameN) from optionlist
	for(long id = 0; id != get_num_option(sectionid, name.c_str()); ++id)
	{
		stringstream key;
		key << basename << id;
		mSections[sectionid].second.erase(key.str());
	};

	// new entry count: 0
	set_option(sectionid, name.c_str(), 0);
}	

tSectionID Config::add_section(configsection section)
{
	mSections.resize(++mSectionCount + 1);
	mSections[mSectionCount].first = section;
	return mSectionCount;
}

void Config::remove_section(tSectionID sectionid)
{
	mSections.erase(mSections.begin() + sectionid);
	mSectionCount--;
#ifdef DEBUG
	cerr << "Removed section " << sectionid << endl
		<< "	Sections remaining: " << mSectionCount << endl;
#endif
}
