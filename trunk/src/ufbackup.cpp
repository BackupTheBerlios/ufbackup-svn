/* ufbackup.cpp
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

#include <rootwindow.hpp>
#include <gtkmm/main.h>
#include <glibmm/miscutils.h>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <ufbackup.h>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <config.hpp>

using namespace Gtk;
using namespace Glib;
using namespace std;

int main(int argc,char *argv[])
{
  Glib::thread_init();
  Main kit(argc, argv);

  try {
	  // get home dir
	  stringstream configFile;
	  configFile << get_home_dir() << '/' << __UFBACKUP_HOME;
	  ustring home = configFile.str();
	  configFile << '/';
	  
	  // check if config dir exist. If not, create it
	  struct stat buf;
	  if(stat(configFile.str().c_str(), &buf ) != 0) {
		  cerr << "Creating directory " << configFile.str() << endl;
		  mkdir(configFile.str().c_str(), 0777);
	  }

	  // complete path
	  configFile << __UFBACKUP_CONFIG;

	  // check if config files exist. If not, create it
	  bool existed = true;
	  if(stat(configFile.str().c_str(), &buf ) != 0) {
		  stringstream cmd;
		  cmd << "touch " << configFile.str();
		  system(cmd.str().c_str());
		  existed = false;
	  }

	  // read config
	  Config config(configFile.str().c_str());
	  config.set_option(0, "home", home);  // FIXME: should save temporary

	  // set needed options if new config file
	  if(not existed) {
		 config.set_option(0, "verbose", "false");
		 config.set_option(0, "notify_overwrite", "true");
		 config.set_option(0, "compr_min_file_size", "0");
		 config.set_option(0, "compr_algo", "bzip2");
		 config.save();
	  }

	  // open main window
	  RootWindow rootWindow(&config);
	  kit.run(rootWindow);
	  
  }catch(std::exception& e) {
	  // get error message and open dialog
	  cerr << e.what() << endl;
	  MessageDialog dialog(e.what(), MESSAGE_ERROR);
	  dialog.run();
	  return 1;
  }

  return 0;
}
