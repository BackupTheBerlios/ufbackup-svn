/* log.hpp
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

#ifndef log_hpp
#define log_hpp

#include <gtkmm/textbuffer.h>
#include <gtkmm/textview.h>
#include <utility>
#include <libdar.hpp>
#include <glibmm/ustring.h>

enum tMessageType {
	MESSAGE, ERROR_MESSAGE, QUESTION };
typedef std::pair<tMessageType, Glib::ustring> tMessage;

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

// Where backup's messages should go
class Log
{
	public:
		Log() {};
		virtual void write(tMessage) {}
};

// write log to a gtk textbuffer
class TextBufferLog : public Log
{
	public:
		TextBufferLog(Glib::RefPtr<Gtk::TextBuffer::Tag> tag, Glib::RefPtr<Gtk::TextBuffer> buffer, Gtk::TextView* view) : mrefTag(tag), mrefBuffer(buffer), mpView(view) {
#ifdef DEBUG
			cerr << "TextBufferLog created" << endl;
#endif	
			}
		virtual void write(tMessage);
	private:
		Glib::RefPtr<Gtk::TextBuffer> mrefBuffer;
		Glib::RefPtr<Gtk::TextBuffer::Tag> mrefTag;
		Gtk::TextView* mpView;
};

#endif
