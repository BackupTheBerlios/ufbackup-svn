/* config.hpp
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

#ifndef config_hpp
#define config_hpp

#include <glibmm.h>
#include <stdexcept>
#include <string>
#include <sstream>
#include <fstream>
#include <utility>
#include <list>
#include <utility>
#include <vector>

enum configsection {
	UNDEF, GLOBAL, JOB };

typedef unsigned short tSectionID;
typedef long tFileNumber; // TODO: remove this type 
typedef std::list<Glib::ustring> tValueList;
typedef std::map<Glib::ustring, Glib::ustring> tOptionList;
typedef std::vector<std::pair<configsection, tOptionList> > tSectionList;
typedef std::list<tSectionID> tIdList;

class Config
{
	public:
		Config(const char* filename);
		~Config() {};

		// save config file
		void save() const;

		// add a new section and return its sectionid
		tSectionID add_section(configsection);
		// remove a section
		void remove_section(tSectionID);

		Glib::ustring get_option(unsigned short sectionid, const char* name) const;

		bool get_bool_option(tSectionID sectionid, const char* name) const {
			if(get_option(sectionid, name) == "true")
				return true;
			else
				return false;
		};
		
		long get_num_option(unsigned short sectionid, const char* name) const {
			tFileNumber num;
			std::string returnv = get_option(sectionid, name);
			std::stringstream ss(returnv);
			ss >> num;
			return num;
		};
		tValueList get_multiple_option(tSectionID sectionid, const char* name) const;
		
		void set_option(unsigned short sectionid, const char* name, Glib::ustring value);
		void set_option(unsigned short sectionid, const char* name, double id) {
			std::stringstream ss;
			ss << id;
			std::string value;
			ss >> value;
			set_option(sectionid, name, value);
		}

		void set_bool_option(tSectionID sectionid, const char* name, bool state) {
			Glib::ustring val = "true";
			if(state == false)
				val = "false";
			set_option(sectionid, name, val);
		};
		
		void add_multiple_option(tSectionID, Glib::ustring, Glib::ustring);

		tIdList get_ids(configsection section) const;

		// remove all entries of a multiple option
		void remove_multiple_option_entries(const tSectionID, Glib::ustring);
		
		
		
	private:
		void parse();
		tSectionList mSections;
		tSectionID mSectionCount;
		Glib::ustring mFilename;
};

#endif
