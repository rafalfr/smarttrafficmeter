/*

Globals.h

Copyright (C) 2016 Rafał Frączek

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

#ifndef GLOBALS_H
#define GLOBALS_H

#include <map>
#include <string>
#include <boost/thread.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

#include "InterfaceInfo.h"
#include "InterfaceStats.h"
#include "InterfaceSpeedMeter.h"
#include "DataBaseDriver.h"

using namespace std;

class Globals
{
public:

    static bool terminate_program;

    static bool is_daemon;

    static string cwd;

    static string program_path;

    static boost::mutex data_load_save_mutex;

    static boost::scoped_ptr<boost::interprocess::shared_memory_object> shared_mem;

    static map<string, map<string, map<string, InterfaceStats> > > all_stats;

    static map<string, InterfaceSpeedMeter> speed_stats;

    static map<string, InterfaceInfo> interfaces;

    static DataBaseDriver db_drv;

};

#endif // GLOBALS_H
