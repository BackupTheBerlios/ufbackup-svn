/* daraction.cpp
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

#include <daraction.hpp>

using namespace std;
using namespace libdar;

user_interaction * DARAction::clone() const
{
	return new DARAction(*this);
}

DARAction::DARAction()
{
	// check for compatible libdar version	
	U_I maj, med, min;
	get_version(maj, med, min);
	if(maj != LIBDAR_COMPILE_TIME_MAJOR)
	{
		stringstream message;
		message << "There is an incompatible version of the DAR library installed on this system. ufBackup needs major version " << LIBDAR_COMPILE_TIME_MAJOR << " instead of " << maj << "!" << endl;
		throw domain_error(message.str());
	}
}

DARAction::DARAction(Config* config, tSectionID section) : Action(config, section), user_interaction()
{
	DARAction();
}

void DARAction::warning(const std::string& msg) {
	error(msg.c_str());
}

void DARAction::pause(const std::string& msg) {
	if(not question(make_pair(QUESTION, msg)))
		throw Euser_abort("Backup aborted.");
}
