/* log.cpp
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

#include <log.hpp>

using namespace Glib;
using namespace Gtk;
using namespace std;

void TextBufferLog::write(tMessage message)
{
	cerr << "log something" << endl;
	Gtk::TextBuffer::iterator iter = mrefBuffer->end();
	// add newline if it doesnt exist
	if(*message.second.end() != '\n')
		message.second.push_back('\n');
	if(message.first == ERROR_MESSAGE)
		iter = mrefBuffer->insert_with_tag(iter, message.second, mrefTag);
	else
	iter = mrefBuffer->insert(iter, message.second);
}

