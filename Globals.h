#ifndef GLOBALS_H
#define GLOBALS_H

#include <map>
#include <string>
#include <mutex>

#include "InterfaceInfo.h"
#include "InterfaceStats.h"
#include "InterfaceSpeedMeter.h"
#include "DataBaseDriver.h"

using namespace std;

class Globals
{
public:

    static bool is_daemon;
    static string cwd;
    static string program_path;

    static mutex data_load_save_mutex;

    //mac, table, date, stats
    static map<string, map<string, map<string, InterfaceStats> > > all_stats;
    static map<string, InterfaceSpeedMeter> speed_stats;
    static map<string, InterfaceInfo> interfaces;

    static DataBaseDriver db_drv;

};

#endif // GLOBALS_H
