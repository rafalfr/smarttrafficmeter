/*

globals.cpp

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

#include "Globals.h"

bool Globals::terminate_program=false;
bool Globals::is_daemon=false;
string Globals::cwd;
string Globals::program_path;
boost::mutex Globals::data_load_save_mutex;
boost::scoped_ptr<boost::interprocess::shared_memory_object> Globals::shared_mem;

DataBaseDriver Globals::db_drv;
map<string, map<string, map<string, InterfaceStats> > > Globals::all_stats;
map<string, InterfaceSpeedMeter> Globals::speed_stats;
map<string, InterfaceInfo> Globals::interfaces;
