#include "Globals.h"

bool Globals::is_daemon=false;
string Globals::cwd;

DataBaseDriver Globals::db_drv;
map<string, map<string, map<string, InterfaceStats> > > Globals::all_stats;
map<string, InterfaceSpeedMeter> Globals::speed_stats;
map<string, InterfaceInfo> Globals::interfaces;
