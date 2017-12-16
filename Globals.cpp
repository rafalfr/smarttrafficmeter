/*

Globals.cpp

Copyright (C) 2018 Rafał Frączek

This file is part of Smart Traffic Meter.

Smart Traffic Meter is free software:
you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

Smart Traffic Meter is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Smart Traffic Meter.
If not, see http://www.gnu.org/licenses/.

*/

#include "Globals.h"
/**<
A global flag that is used to terminate the program.
If terminate_program is set true, the program will terminate immediately.
 */
bool Globals::terminate_program = false;

/**<
A global flag which indicates whether the program is running as a daemon or not.
*/
bool Globals::is_daemon = false;

/**<
The program current working directory
*/
string Globals::cwd;

/**<
the full path to the program executable file
The variable is set during the program start-up.
*/
string Globals::program_path;

/**<
the name of the host computer
The variable is set during the program start-up.
*/
string Globals::host_name;

/**<
the name of the user that launched the program
The variable is set during the program start-up.
*/
string Globals::user_name;

/**<
the global mutex that is used to protect save operations (e.g database files)
*/
boost::mutex Globals::data_load_save_mutex;

/**<
The global object that is used to prevent multiple instances of the program.
*/
boost::scoped_ptr<boost::interprocess::shared_memory_object> Globals::shared_mem;

/**<
The global object that stores the interface to the database operations.
*/
DataBaseDriver Globals::db_drv;

/**<
The global map object that stores current statistics for each available network interface.
The map structure is as follows:
MAC address,stats type, date, InterfaceStats object
*/
map<string, map<string, map<string, InterfaceStats> > > Globals::all_stats;

/**<
The global map object that stores session statistics for each available network interface.
Session stats are statistics from the program start until the end of execution.
The map structure is as follows:
MAC address, InterfaceStats object
*/
map<string, InterfaceStats> Globals::session_stats;

/**<
The global map object that stores current data speed for each interface.
The map structure is as follows:
MAC address, InterfaceSpeedMeter object
*/
map<string, InterfaceSpeedMeter> Globals::speed_stats;

/**<
The global map object that stores information for each interface.
The map structure is as follows:
MAC address, InterfaceInfo object
*/
map<string, InterfaceInfo> Globals::interfaces;

/**<
The global vector object that stores pointers to
threads which upload statistics to web services
like grovestreams and others.
*/
vector<boost::thread*> Globals::upload_threads;
