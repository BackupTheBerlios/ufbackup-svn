/* action.hpp
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

#ifndef action_hpp
#define action_hpp

#include <config.hpp>
#include <log.hpp>

/** Some action like doing a backup/restore/etc. */
class Action
{
	public:
		Action() {std::cout << "Action()" << endl;}
		Action(Config* config, tSectionID section) : mpConfig(config), mSection(section) 
		{std::cout << "Action(params)" << endl;}

		/** Start the action. */
		virtual void start() {}
	protected:
		/** Action root function (should be called by start()).*/
		virtual void action() {}
		/** Write some message. */
		virtual void write(tMessage) {}
		/** Ask some question. */
		virtual bool question(tMessage) {}
		/** Convenient way to write a normal message */
		void message(const char* m) {
			write(std::make_pair(MESSAGE, m)); }
		/** Convenient way to write an error message */
		void error(const char* m) {
			write(std::make_pair(ERROR_MESSAGE, m)); }
		
		Config* mpConfig;
		tSectionID mSection;
};

#endif
