#include "Globals.h"

bool Globals::is_daemon=false;
string Globals::cwd;
string Globals::program_path;
boost::mutex Globals::data_load_save_mutex;
boost::scoped_ptr<boost::interprocess::shared_memory_object> Globals::shared_mem;

DataBaseDriver Globals::db_drv;
map<string, map<string, map<string, InterfaceStats> > > Globals::all_stats;
map<string, InterfaceSpeedMeter> Globals::speed_stats;
map<string, InterfaceInfo> Globals::interfaces;
