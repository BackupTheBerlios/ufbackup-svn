/* daraction.hpp
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

#ifndef daraction_hpp
#define daraction_hpp

#include <libdar.hpp>
#include <sstream>
#include <string>
#include <action.hpp>
#include <sigc++/sigc++.h>


void write_callback(const std::string& msg);
bool pause_callback(const std::string& msg);


class DARAction : public virtual Action, public libdar::user_interaction
{
	public:
		DARAction();
		DARAction(Config* config, tSectionID section);
		virtual void warning(const std::string& msg);
		virtual void pause(const std::string& msg);
		user_interaction * clone() const;
};

#endif
