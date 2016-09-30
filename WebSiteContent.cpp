#ifndef _NO_WEBSERVER
#include "config.h"
#include <fstream>
#include <memory>
#include <sstream>
#include <regex>
#include "WebSiteContent.h"
#include "json/json.h"
#include "defines.h"
#include "Utils.h"
#include "Logger.h"
#include "Globals.h"
#include "Settings.h"
#include "InterfaceInfo.h"
#include "InterfaceStats.h"
#include "InterfaceSpeedMeter.h"

#ifdef use_sqlite
#include "sqlite3.h"
#endif // use_sqlite

using namespace std;

/** @brief set_web_site_content
  *
  * @todo: document this function
  */
void WebSiteContent::set_web_site_content( SimpleWeb::Server<SimpleWeb::HTTP>& server )
{
    server.default_resource["GET"] = []( SimpleWeb::Server<SimpleWeb::HTTP>::Response & response, shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request )
    {
        // http://html-color-codes.info/html-editor/
        // http://jsbin.com/biwefacino/edit?html,css,js,output
        // http://www.awwwards.com/10-html-css-online-code-editors-for-web-developers.html
        string row;
        ifstream file;
        stringstream content_stream;

        uint32_t start_year;
        uint32_t start_month;
        uint32_t start_day;
        uint32_t start_hour;

        uint32_t end_year;
        uint32_t end_month;
        uint32_t end_day;
        uint32_t end_hour;

        string user_name;
        string host_name;
        string os_info;

        Utils::get_user_host( user_name, host_name );

        Utils::get_os_info( os_info );

        time_t t = time( nullptr );
        Utils::get_time_from_milisec( t, &end_year, &end_month, &end_day, &end_hour );

        string end_year_str = Utils::to_string( end_year );
        string end_month_str = Utils::to_string( end_month );
        string end_day_str = Utils::to_string( end_day );
        string end_hour_str = Utils::to_string( end_hour );

        Utils::get_time_from_milisec( t - 24ULL * 60ULL * 60ULL, &start_year, &start_month, &start_day, &start_hour );
        string start_year_str = Utils::to_string( start_year );
        string start_month_str = Utils::to_string( start_month );
        string start_day_str = Utils::to_string( start_day );
        string start_hour_str = Utils::to_string( start_hour );

        string hourly_past_day = "<a href=\"hourly/start=" + start_year_str + "-" + start_month_str + "-" + start_day_str + "-" + start_hour_str + "\">past day</a>";

        Utils::get_time_from_milisec( t - 24ULL * 60ULL * 60ULL * 7ULL, &start_year, &start_month, &start_day, &start_hour );
        start_year_str = Utils::to_string( start_year );
        start_month_str = Utils::to_string( start_month );
        start_day_str = Utils::to_string( start_day );
        start_hour_str = Utils::to_string( start_hour );
        string hourly_past_week = "<a href=\"hourly/start=" + start_year_str + "-" + start_month_str + "-" + start_day_str + "-" + start_hour_str + "\">past week</a>";
        string daily_past_week = "<a href=\"daily/start=" + start_year_str + "-" + start_month_str + "-" + start_day_str + "\">past week</a>";

        Utils::get_time_from_milisec( t - 24ULL * 60ULL * 60ULL * 30ULL, &start_year, &start_month, &start_day, &start_hour );
        start_year_str = Utils::to_string( start_year );
        start_month_str = Utils::to_string( start_month );
        start_day_str = Utils::to_string( start_day );
        start_hour_str = Utils::to_string( start_hour );
        string hourly_past_month = "<a href=\"hourly/start=" + start_year_str + "-" + start_month_str + "-" + start_day_str + "-" + start_hour_str + "\">past month</a>";
        string daily_past_month = "<a href=\"daily/start=" + start_year_str + "-" + start_month_str + "-" + start_day_str + "\">past month</a>";

        Utils::get_time_from_milisec( t - 24ULL * 60ULL * 60ULL * 365ULL, &start_year, &start_month, &start_day, &start_hour );
        start_year_str = Utils::to_string( start_year );
        start_month_str = Utils::to_string( start_month );
        start_day_str = Utils::to_string( start_day );
        start_hour_str = Utils::to_string( start_hour );
        string monthly_past_year = "<a href=\"monthly/start=" + start_year_str + "-" + start_month_str + "-" + start_day_str + "\">past year</a>";

        string page;

        page += "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n";
        page += "<html>\n";
        page += "<head>\n";
        page += "<meta content=\"text/html; charset=UTF-8\" http-equiv=\"content-type\">\n";
        page += "<title>Smart Traffic Meter</title>\n";
        page += "<style type=\"text/css\">\n";
        page += "h1 {\n";
        page += "font-size: xx-large;\n";
        page += "font-style: normal;\n";
        page += "text-align: center;\n";
        page += "font-family: Arial,Helvetica,sans-serif;\n";
        page += "font-weight: bold;\n";
        page += "text-transform: none;\n";
        page += "}\n";
        page += "h2 {\n";
        page += "font-size: large;\n";
        page += "font-style: normal;\n";
        page += "text-align: center;\n";
        page += "font-family: Arial,Helvetica,sans-serif;\n";
        page += "font-weight: bold;\n";
        page += "text-transform: none;\n";
        page += "}\n";
        page += "p {\n";
        page += "font-style: normal;\n";
        page += "font-family: Arial,Helvetica,sans-serif;\n";
        page += "text-transform: none;\n";
        page += "font-size: large;\n";
        page += "text-align: justify;\n";
        page += "font-weight: normal;\n";
        page += "color: black;\n";
        page += "}\n";
        page += "li {\n";
        page += "font-style: normal;\n";
        page += "font-family: Arial,Helvetica,sans-serif;\n";
        page += "text-transform: none;\n";
        page += "font-size: large;\n";
        page += "text-align: left;\n";
        page += "font-weight: normal;\n";
        page += "color: black;\n";
        page += "}\n";
        page += "tab1 {\n";
        page += "padding-left: 1em;\n";
        page += "padding-right: 1em;\n";
        page += "}\n";
        page += "</style>\n";
        page += "</head>\n";
        page += "<body>\n";
        page += "<table style=\"margin-left: auto; margin-right: auto; width: 437px;\">\n";
        page += "<tbody>\n";
        page += "<tr>\n";
        page += "<td>\n";
        page += "<h1>Smart Traffic Meter</h1>\n";
        page += "</td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td style=\"font-family: Verdana,Arial,Helvetica,sans-serif; font-size: x-large; font-style: normal; line-height: normal; color: #000000; vertical-align: middle; text-align: center; position: relative; visibility: visible;\">\n";
        page += "<pre>" + user_name + " @ " + host_name + "</pre>\n";
        page += "<pre>" + os_info + "</pre>\n";
        page += "</td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td>\n";
        page += "<br>\n";
        page += "<br>\n";
        page += "<h2>available network interfaces</h2>\n";
        page += "<ol>\n";

        for ( auto const & mac_info : Globals::interfaces )
        {
            const InterfaceInfo& interface_info = mac_info.second;

            const string& interface_mac = interface_info.get_mac();
            const string& interface_name = interface_info.get_name();
            const string& interface_description = interface_info.get_desc();
            const string& ip4 = interface_info.get_ip4();
            const string& ip6 = interface_info.get_ip6();

            page += "<li>\n";
            page += "<p>";

            if ( interface_description.empty() == false )
            {
                page += interface_description + ",\t";
            }

            page += interface_name + ",\t" + interface_mac + ",\t" + ip4 + ",\t" + ip6;
            page += "</p>\n";
            page += "</li>\n";
        }

        page += "</ol>\n</td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td>\n";
        page += "<br>\n";
        page += "<h2>hourly stats</h2>\n";
        page += "<p style=\"text-align: center;\">";
        page += "<tab1>";
        page += "<a href=\"/hourly/\">all hourly stats</a>";
        page += "</tab1>\n";
        page += "<tab1>";
        page += hourly_past_day;
        page += "</tab1>\n";
        page += "<tab1>";
        page += hourly_past_week;
        page += "</tab1>\n";
        page += "<tab1>";
        page += hourly_past_month;
        page += "</tab1>\n";
        page += "</p>\n";
        page += "</td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td>\n";
        page += "<br>\n";
        page += "<h2>daily stats</h2>\n";
        page += "<p style=\"text-align: center;\">";
        page += "<tab1>";
        page += "<a href=\"/daily/\">all daily stats</a>";
        page += "</tab1>\n";
        page += "<tab1>";
        page += daily_past_week;
        page += "</tab1>\n";
        page += "<tab1>";
        page += daily_past_month;
        page += "</tab1>\n";
        page += "</p>";
        page += "</td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td>\n";
        page += "<br>\n";
        page += "<h2>monthly stats</h2>\n";
        page += "<p style=\"text-align: center;\">";
        page += "<tab1>";
        page += "<a href=\"/monthly/\">all monthly stats</a>";
        page += "</tab1>\n";
        page += "<tab1>";
        page += monthly_past_year;
        page += "</tab1>\n";
        page += "</td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td>\n";
        page += "<br>\n";
        page += "<h2>yearly stats</h2>\n";
        page += "<p style=\"text-align: center;\">";
        page += "<tab1>";
        page += "<a href=\"/yearly/\">all yearly stats</a>";
        page += "</tab1>\n";
        page += "</td>\n";
        page += "</tr>\n";
        page += "</tbody>\n";
        page += "</table>\n";
        page += "<p style=\"text-align: center;\">";
        page += "<a href=\"/legalinfo/\">legal info</a>\n";
        page += "</p>";

        page += "</body>\n";
        page += "</html>\n";

        //content_stream << "<h1>Request from " << request->remote_endpoint_address << " (" << request->remote_endpoint_port << ")</h1>";
        //content_stream << request->method << " " << request->path << " HTTP/" << request->http_version << "<br>";

//        for ( auto& header : request->header )
//        {
//            content_stream << header.first << ": " << header.second << "<br>";
//        }

        content_stream << page;

        content_stream.seekp( 0, ios::end );

        response <<  "HTTP/1.1 200 OK\r\nContent-Length: " << content_stream.tellp() << "\r\n";
        response << "Content-Type: text/html; charset=utf-8" << "\r\n";
        response << "\r\n\r\n" << content_stream.rdbuf();
    };


    server.resource["^/legalinfo/?$"]["GET"] = []( SimpleWeb::Server<SimpleWeb::HTTP>::Response & response, shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request )
    {
        string page;

        page += "boost libraries";
        page += "<a href=\"http://www.boost.org/\">home page</a>\n";
        page += "<br>";
        page += "<a href=\"http://www.boost.org/users/license.html\">license</a>\n";
        page += "<br>";

        page += "chart.js";
        page += "<a href=\"http://www.chartjs.org/\">home page</a>\n";
        page += "<br>";
        page += "<a href=\"http://www.chartjs.org/docs/#notes-license\">license</a>\n";
        page += "<br>";

        page += "JsonCpp";
        page += "<a href=\"https://github.com/open-source-parsers/jsoncpp\">home page</a>\n";
        page += "<br>";
        page += "<a href=\"https://github.com/open-source-parsers/jsoncpp/blob/master/LICENSE\">license</a>\n";
        page += "<br>";

        page += "Simple-Web-Server";
        page += "<a href=\"https://github.com/eidheim/Simple-Web-Server\">home page</a>\n";
        page += "<br>";
        page += "<a href=\"https://github.com/eidheim/Simple-Web-Server/blob/master/LICENSE\">license</a>\n";
        page += "<br>";

        page += "SQLite";
        page += "<a href=\"https://www.sqlite.org/\">home page</a>\n";
        page += "<br>";
        page += "<a href=\"https://www.sqlite.org/copyright.html\">license</a>\n";
        page += "<br>";

        page += "TCLAP";
        page += "<a href=\"http://tclap.sourceforge.net/\">home page</a>\n";
        page += "<br>";
        page += "<a href=\"https://opensource.org/licenses/mit-license.php\">license</a>\n";
        page += "<br>";

        stringstream content_stream;
        content_stream << page;

        content_stream.seekp( 0, ios::end );
        response <<  "HTTP/1.1 200 OK\r\nContent-Length: " << content_stream.tellp() << "\r\n";
        response << "Content-Type: text/html; charset=utf-8" << "\r\n";
        response << "Cache-Control: public, max-age=1800";
        response << "\r\n\r\n" << content_stream.rdbuf();
    };

    server.resource["^\\/download\\/?(.*)?\\/?$"]["GET"] = []( SimpleWeb::Server<SimpleWeb::HTTP>::Response & response, shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request )
    {

        uint32_t y;
        uint32_t m;
        uint32_t d;
        uint32_t h;
        string csv;

        //option: name, value
        map<string, string> options;

        Utils::get_time( &y, &m, &d, &h );

        string current_time_str;

        // hourly, daily, monthly, yearly
        string stats_type = request->path_match[1];

        current_time_str = Utils::date_str( stats_type, y, m, d, h );

        //set default optional parameters values
        string start_date_str;

        if ( stats_type.compare( "yearly" ) == 0 )
        {
            start_date_str = "1900";
        }
        else if ( stats_type.compare( "monthly" ) == 0 )
        {
            start_date_str = "1900-01";
        }
        else if ( stats_type.compare( "daily" ) == 0 )
        {
            start_date_str = "1900-01-01";
        }
        else if ( stats_type.compare( "hourly" ) == 0 )
        {
            start_date_str = "1900-01-01-00";
        }

        string end_date_str = current_time_str;
        string options_str = request->path_match[2];

        //parse parameters string
        vector<string> options_items = Utils::split( options_str, "&" );

        for ( auto const & options_item : options_items )
        {
            vector<string> option_value = Utils::split( options_item, "=" );

            if ( option_value[0].size() > 0 && option_value[1].size() > 0 )
            {
                if ( regex_match( option_value[0], regex( "(start|end)" ) ) == true )
                {
                    options[option_value[0]] = option_value[1];
                }
            }
        }

        //get parameters from the options map
        if ( options.find( "start" ) != options.end() )
        {
            const string& value = options["start"];

            if ( stats_type.compare( "yearly" ) == 0 && regex_match( value, regex( "^[[:digit:]]{4}$" ) ) == true )
            {
                start_date_str = value;
            }
            else if ( stats_type.compare( "monthly" ) == 0 && regex_match( value, regex( "^[[:digit:]]{4}-[[:digit:]]{1,2}$" ) ) == true )
            {
                start_date_str = value;
            }
            else if ( stats_type.compare( "daily" ) == 0 && regex_match( value, regex( "^[[:digit:]]{4}-[[:digit:]]{1,2}-[[:digit:]]{1,2}$" ) ) == true )
            {
                start_date_str = value;
            }
            else if ( stats_type.compare( "hourly" ) == 0 && regex_match( value, regex( "^[[:digit:]]{4}-[[:digit:]]{1,2}-[[:digit:]]{1,2}-[[:digit:]]{1,2}$" ) ) == true )
            {
                start_date_str = value;
            }
        }

        if ( options.find( "end" ) != options.end() )
        {
            const string& value = options["end"];

            if ( stats_type.compare( "yearly" ) == 0 && regex_match( value, regex( "^[[:digit:]]{4}$" ) ) == true )
            {
                end_date_str = value;
            }
            else if ( stats_type.compare( "monthly" ) == 0 && regex_match( value, regex( "^[[:digit:]]{4}-[[:digit:]]{1,2}$" ) ) == true )
            {
                end_date_str = value;
            }
            else if ( stats_type.compare( "daily" ) == 0 && regex_match( value, regex( "^[[:digit:]]{4}-[[:digit:]]{1,2}-[[:digit:]]{1,2}$" ) ) == true )
            {
                end_date_str = value;
            }
            else if ( stats_type.compare( "hourly" ) == 0 && regex_match( value, regex( "^[[:digit:]]{4}-[[:digit:]]{1,2}-[[:digit:]]{1,2}-[[:digit:]]{1,2}$" ) ) == true )
            {
                end_date_str = value;
            }
        }

        // get stats for each network interface
        for ( auto const & mac_table : Globals::all_stats )
        {
            const string& mac = mac_table.first;

            string interface_name = "n/a";
            string interface_description = "n/a";
            string ip4 = "n/a";
            string ip6 = "n/a";

            // get interface inforamtion
            for ( auto const & mac_info : Globals::interfaces )
            {
                const InterfaceInfo& interface_info = mac_info.second;

                if ( interface_info.get_mac().compare( mac ) == 0 )
                {
                    interface_name = interface_info.get_name();
                    interface_description = interface_info.get_desc();
                    ip4 = interface_info.get_ip4();
                    ip6 = interface_info.get_ip6();
                }
            }

            const vector<string>& start_date_items = Utils::split( start_date_str, "-" );
            const vector<string>& end_date_items = Utils::split( end_date_str, "-" );

            struct date start_date;
            struct date end_date;

            if ( stats_type.compare( "yearly" ) == 0 )
            {
                start_date.year = Utils::stoi( start_date_items[0] );
                start_date.month = 0;
                start_date.day = 0;
                start_date.hour = 0;

                end_date.year = Utils::stoi( end_date_items[0] );
                end_date.month = 0;
                end_date.day = 0;
                end_date.hour = 0;
            }
            else if ( stats_type.compare( "monthly" ) == 0 )
            {
                start_date.year = Utils::stoi( start_date_items[0] );
                start_date.month = Utils::stoi( start_date_items[1] );
                start_date.day = 0;
                start_date.hour = 0;

                end_date.year = Utils::stoi( end_date_items[0] );
                end_date.month = Utils::stoi( end_date_items[1] );
                end_date.day = 0;
                end_date.hour = 0;
            }
            else if ( stats_type.compare( "daily" ) == 0 )
            {
                start_date.year = Utils::stoi( start_date_items[0] );
                start_date.month = Utils::stoi( start_date_items[1] );
                start_date.day = Utils::stoi( start_date_items[2] );
                start_date.hour = 0;

                end_date.year = Utils::stoi( end_date_items[0] );
                end_date.month = Utils::stoi( end_date_items[1] );
                end_date.day = Utils::stoi( end_date_items[2] );
                end_date.hour = 0;
            }
            else if ( stats_type.compare( "hourly" ) == 0 )
            {
                start_date.year = Utils::stoi( start_date_items[0] );
                start_date.month = Utils::stoi( start_date_items[1] );
                start_date.day = Utils::stoi( start_date_items[2] );
                start_date.hour = Utils::stoi( start_date_items[3] );

                end_date.year = Utils::stoi( end_date_items[0] );
                end_date.month = Utils::stoi( end_date_items[1] );
                end_date.day = Utils::stoi( end_date_items[2] );
                end_date.hour = Utils::stoi( end_date_items[3] );
            }

            //get stats for the current interface
            map<string, InterfaceStats> results = Globals::db_drv.get_stats( mac, stats_type, start_date, end_date );

            for ( auto & row_stats : results )
            {
                const InterfaceStats& stats = row_stats.second;
                const string& row = row_stats.first;
// TODO (rafal#1#09/30/16): implement csv creation

                //stats.recieved();
                //stats.transmited();
            }
        }
    };


    server.resource["^\\/(hourly|daily|monthly|yearly)\\/?(.*)?\\/?$"]["GET"] = []( SimpleWeb::Server<SimpleWeb::HTTP>::Response & response, shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request )
    {
        // http://127.0.0.1:8080/daily/

        uint32_t y;
        uint32_t m;
        uint32_t d;
        uint32_t h;

        uint64_t rx_in_period = 0ULL;
        uint64_t tx_in_period = 0ULL;
        string unit;
        float scale;

        string web_page;

        //option: name, value
        map<string, string> options;

        Utils::get_time( &y, &m, &d, &h );

        string current_time_str;

        // hourly, daily, monthly, yearly
        string stats_type = request->path_match[1];

        current_time_str = Utils::date_str( stats_type, y, m, d, h );

        //set default optional parameters values
        string start_date_str;

        if ( stats_type.compare( "yearly" ) == 0 )
        {
            start_date_str = "1900";
        }
        else if ( stats_type.compare( "monthly" ) == 0 )
        {
            start_date_str = "1900-01";
        }
        else if ( stats_type.compare( "daily" ) == 0 )
        {
            start_date_str = "1900-01-01";
        }
        else if ( stats_type.compare( "hourly" ) == 0 )
        {
            start_date_str = "1900-01-01-00";
        }

        string end_date_str = current_time_str;
        string up_color_str = "ff0011";
        string down_color_str = "3344ff";
        string chart_type_str = "bar";
        string chartwidth_str = "600";
        string chartheight_str = "450";
        string options_str = request->path_match[2];

        //parse parameters string
        vector<string> options_items = Utils::split( options_str, "&" );

        for ( auto const & options_item : options_items )
        {
            vector<string> option_value = Utils::split( options_item, "=" );

            if ( option_value[0].size() > 0 && option_value[1].size() > 0 )
            {
                if ( regex_match( option_value[0], regex( "(start|end|upcolor|downcolor|charttype|chartwidth|chartheight)" ) ) == true )
                {
                    options[option_value[0]] = option_value[1];
                }
            }
        }

        //get parameters from the options map
        if ( options.find( "start" ) != options.end() )
        {
            const string& value = options["start"];

            if ( stats_type.compare( "yearly" ) == 0 && regex_match( value, regex( "^[[:digit:]]{4}$" ) ) == true )
            {
                start_date_str = value;
            }
            else if ( stats_type.compare( "monthly" ) == 0 && regex_match( value, regex( "^[[:digit:]]{4}-[[:digit:]]{1,2}$" ) ) == true )
            {
                start_date_str = value;
            }
            else if ( stats_type.compare( "daily" ) == 0 && regex_match( value, regex( "^[[:digit:]]{4}-[[:digit:]]{1,2}-[[:digit:]]{1,2}$" ) ) == true )
            {
                start_date_str = value;
            }
            else if ( stats_type.compare( "hourly" ) == 0 && regex_match( value, regex( "^[[:digit:]]{4}-[[:digit:]]{1,2}-[[:digit:]]{1,2}-[[:digit:]]{1,2}$" ) ) == true )
            {
                start_date_str = value;
            }
        }

        if ( options.find( "end" ) != options.end() )
        {
            const string& value = options["end"];

            if ( stats_type.compare( "yearly" ) == 0 && regex_match( value, regex( "^[[:digit:]]{4}$" ) ) == true )
            {
                end_date_str = value;
            }
            else if ( stats_type.compare( "monthly" ) == 0 && regex_match( value, regex( "^[[:digit:]]{4}-[[:digit:]]{1,2}$" ) ) == true )
            {
                end_date_str = value;
            }
            else if ( stats_type.compare( "daily" ) == 0 && regex_match( value, regex( "^[[:digit:]]{4}-[[:digit:]]{1,2}-[[:digit:]]{1,2}$" ) ) == true )
            {
                end_date_str = value;
            }
            else if ( stats_type.compare( "hourly" ) == 0 && regex_match( value, regex( "^[[:digit:]]{4}-[[:digit:]]{1,2}-[[:digit:]]{1,2}-[[:digit:]]{1,2}$" ) ) == true )
            {
                end_date_str = value;
            }
        }

        if ( options.find( "upcolor" ) != options.end() )
        {
            const string& value = options["upcolor"];

            if ( regex_match( value, regex( "^[[:xdigit:]]{6}$" ) ) == true )
            {
                up_color_str = value;
            }
        }

        if ( options.find( "downcolor" ) != options.end() )
        {
            const string& value = options["downcolor"];

            if ( regex_match( value, regex( "^[[:xdigit:]]{6}$" ) ) == true )
            {
                down_color_str = value;
            }
        }

        if ( options.find( "charttype" ) != options.end() )
        {
            const string& value = options["charttype"];

            if ( regex_match( value, regex( "^(bar|line)$" ) ) == true )
            {
                chart_type_str = value;
            }
        }

        if ( options.find( "chartwidth" ) != options.end() )
        {
            const string& value = options["chartwidth"];

            if ( regex_match( value, regex( "^[[:digit:]]{1,4}$" ) ) == true )
            {
                chartwidth_str = value;
            }
        }

        if ( options.find( "chartheight" ) != options.end() )
        {
            const string& value = options["chartheight"];

            if ( regex_match( value, regex( "^[[:digit:]]{1,4}$" ) ) == true )
            {
                chartheight_str = value;
            }
        }

        // create chart colors
        string down_fill_color = rgba_color( down_color_str, 0.5f );
        string down_stroke_color = rgba_color( down_color_str, 0.8f );
        string down_highlightfill_color = rgba_color( down_color_str, 0.75f );
        string down_highlightstroke_color = rgba_color( down_color_str, 1.0f );

        string up_fill_color = rgba_color( up_color_str, 0.5f );
        string up_stroke_color = rgba_color( up_color_str, 0.8f );
        string up_highlightfill_color = rgba_color( up_color_str, 0.75f );
        string up_highlightstroke_color = rgba_color( up_color_str, 1.0f );

        // start generating web page
        web_page += "<!doctype html>\n";
        web_page += "<html>\n";
        web_page += "<head>\n";
        web_page += "<title>daily statistics</title>";
        web_page += "<script src=\"/Chart.js\"></script>\n";

        web_page += "<style>\n";
        web_page += "p {color:black; font-family: arial}\n";
        web_page += "</style>\n";

        web_page += "</head>\n";
        web_page += "<body>\n";
        web_page += "<div style=\"width: 100%; text-align:center\" align=\"center\">\n";
        web_page += "<p><a href=\"/savedata\">save all data</a></p>\n";
        web_page += "</div>";
        web_page += "<table style=\"text-align: center; width: " + chartwidth_str + "px; margin-left: auto; margin-right: auto;\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n";
        web_page += "<tbody>\n";
        web_page += "<tr>\n";
        web_page += "<td style=\"vertical-align: top; text-align: center; width: " + chartwidth_str + "px;\">\n";


        uint32_t chart_id = 0;

        // create chart for each network interface
        for ( auto const & mac_table : Globals::all_stats )
        {
            const string& mac = mac_table.first;

            string interface_name = "n/a";
            string interface_description = "n/a";
            string ip4 = "n/a";
            string ip6 = "n/a";

            // get interface inforamtion
            for ( auto const & mac_info : Globals::interfaces )
            {
                const InterfaceInfo& interface_info = mac_info.second;

                if ( interface_info.get_mac().compare( mac ) == 0 )
                {
                    interface_name = interface_info.get_name();
                    interface_description = interface_info.get_desc();
                    ip4 = interface_info.get_ip4();
                    ip6 = interface_info.get_ip6();
                }
            }

            string canvas_id = "canvas";
            canvas_id += Utils::to_string( chart_id );

            web_page += "<div title=\"interface name, mac, IP4, IP6\" style=\"width: 100%\" align=\"center\">\n";

#ifdef _WIN32
            web_page += "<p>" + interface_description + ", " + mac + ", " + ip4;
#endif // _WIN32

#ifdef __linux
            web_page += "<p>" + interface_name + ", " + mac + ", " + ip4 + ", " + ip6;
#endif // __linux

            web_page += "</p></div>";

            web_page += "<div style=\"width: 100%; text-align:center\" align=\"center\">\n";
            web_page += "<canvas id=\"" + canvas_id + "\" height=\"" + chartheight_str + "\" width=\"" + chartwidth_str + "\"></canvas>\n";
            web_page += "</div>\n";

            const vector<string>& start_date_items = Utils::split( start_date_str, "-" );
            const vector<string>& end_date_items = Utils::split( end_date_str, "-" );

            struct date start_date;
            struct date end_date;

            if ( stats_type.compare( "yearly" ) == 0 )
            {
                start_date.year = Utils::stoi( start_date_items[0] );
                start_date.month = 0;
                start_date.day = 0;
                start_date.hour = 0;

                end_date.year = Utils::stoi( end_date_items[0] );
                end_date.month = 0;
                end_date.day = 0;
                end_date.hour = 0;
            }
            else if ( stats_type.compare( "monthly" ) == 0 )
            {
                start_date.year = Utils::stoi( start_date_items[0] );
                start_date.month = Utils::stoi( start_date_items[1] );
                start_date.day = 0;
                start_date.hour = 0;

                end_date.year = Utils::stoi( end_date_items[0] );
                end_date.month = Utils::stoi( end_date_items[1] );
                end_date.day = 0;
                end_date.hour = 0;
            }
            else if ( stats_type.compare( "daily" ) == 0 )
            {
                start_date.year = Utils::stoi( start_date_items[0] );
                start_date.month = Utils::stoi( start_date_items[1] );
                start_date.day = Utils::stoi( start_date_items[2] );
                start_date.hour = 0;

                end_date.year = Utils::stoi( end_date_items[0] );
                end_date.month = Utils::stoi( end_date_items[1] );
                end_date.day = Utils::stoi( end_date_items[2] );
                end_date.hour = 0;
            }
            else if ( stats_type.compare( "hourly" ) == 0 )
            {
                start_date.year = Utils::stoi( start_date_items[0] );
                start_date.month = Utils::stoi( start_date_items[1] );
                start_date.day = Utils::stoi( start_date_items[2] );
                start_date.hour = Utils::stoi( start_date_items[3] );

                end_date.year = Utils::stoi( end_date_items[0] );
                end_date.month = Utils::stoi( end_date_items[1] );
                end_date.day = Utils::stoi( end_date_items[2] );
                end_date.hour = Utils::stoi( end_date_items[3] );
            }

            //get stats for the current interface
            map<string, InterfaceStats> results = Globals::db_drv.get_stats( mac, stats_type, start_date, end_date );

            //update results for the current time
            for ( auto & row_stats : results )
            {
                if ( row_stats.first.compare( current_time_str ) == 0 )
                {
                    uint64_t rx = Globals::all_stats[mac][stats_type][current_time_str].recieved();
                    uint64_t tx = Globals::all_stats[mac][stats_type][current_time_str].transmited();
                    row_stats.second.set_current_stats( tx, rx );
                }
            }

            //scale obtained stats
            uint64_t max_value = 0ULL;
            rx_in_period = 0ULL;
            tx_in_period = 0ULL;

            for ( auto & row_stats : results )
            {
                const InterfaceStats& stats = row_stats.second;

                if ( stats.recieved() > max_value )
                {
                    max_value = stats.recieved();
                }

                if ( stats.transmited() > max_value )
                {
                    max_value = stats.transmited();
                }

                rx_in_period += stats.recieved();
                tx_in_period += stats.transmited();
            }


            if ( max_value >= 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL )
            {
                scale = 1024.0f * 1024.0f * 1024.0f * 1024.0f * 1024.0f;
                unit = "PB";
            }
            else if ( max_value >= 1024ULL * 1024ULL * 1024ULL * 1024ULL )
            {
                scale = 1024.0f * 1024.0f * 1024.0f * 1024.0f;
                unit = "TB";
            }
            else if ( max_value >= 1024ULL * 1024ULL * 1024ULL )
            {
                scale = 1024.0f * 1024.0f * 1024.0f;
                unit = "GB";
            }
            else if ( max_value >= 1024ULL * 1024ULL )
            {
                scale = 1024.0f * 1024.0f;
                unit = "MB";
            }
            else if ( max_value >= 1024ULL )
            {
                scale = 1024.0f;
                unit = "KB";
            }
            else
            {
                scale = 1.0f;
                unit = "B";
            }

            //generate chart html
            string var_chart_data = "ChartData";
            var_chart_data += Utils::to_string( chart_id );

            string chart_data;
            chart_data += "var " + var_chart_data + " = {\nlabels : [\n";

            uint32_t i = 0;

            for ( auto const & row_stats : results )
            {
                chart_data += "\"";
                chart_data += Utils::replace( "_", ", ", row_stats.first );
                chart_data += "\"";

                if ( i < results.size() - 1 )
                {
                    chart_data += ",";
                }

                i++;
            }

            chart_data += "],\ndatasets : [\n{\nlabel: \"down\",\nbackgroundColor : \"" + down_fill_color + "\",\nborderColor : \"" + down_stroke_color + "\",\nhoverBackgroundColor : \"" + down_highlightfill_color + "\",\nhoverBorderColor : \"" + down_highlightstroke_color + "\",\ndata :\n [";

            i = 0;

            for ( auto const & row_stats : results )
            {
                const InterfaceStats& stats = row_stats.second;

                float value = ( ( float ) stats.recieved() ) / scale;

                chart_data += Utils::float_to_string( value );

                if ( i < results.size() - 1 )
                {
                    chart_data += ",";
                }

                i++;
            }

            chart_data += "]\n},\n{\nlabel: \"up\",\nbackgroundColor : \"" + up_fill_color + "\",\nborderColor : \"" + up_stroke_color + "\",hoverBackgroundColor : \"" + up_highlightfill_color + "\",hoverBorderColor : \"" + up_highlightstroke_color + "\",data :\n[";

            i = 0;

            for ( auto const & row_stats : results )
            {
                const InterfaceStats& stats = row_stats.second;

                float value = ( ( float ) stats.transmited() ) / scale;

                chart_data += Utils::float_to_string( value );

                if ( i < results.size() - 1 )
                {
                    chart_data += ",";
                }
            }

            chart_data += "]\n}\n]\n}";

            string var_options = "options";
            var_options += Utils::to_string( chart_id );

            string chart_options;
            chart_options += "var " + var_options + " = {\n";
            chart_options += "scales: {\n";
            chart_options += "yAxes: [{\n";
            chart_options += "ticks: {\n";
            chart_options += "callback: function(value, index, values) {\n";
            chart_options += "return ''+value.toFixed(2)+' " + unit + "';\n";
            chart_options += "}\n";
            chart_options += "}\n";
            chart_options += "}]\n";
            chart_options += "},\n";
            chart_options += "responsive : true\n";
            chart_options += "};";

            web_page += "<script>\n";
            web_page += chart_data;
            web_page += "\n";
            web_page += chart_options;

            string var_chart = "ctx";
            var_chart += Utils::to_string( chart_id );

            web_page += "</script>\n";
            web_page += "<br>\n";

            chart_id++;

            web_page += "<div style=\"width: 100%\" align=\"center\">\n";
            float value = ( ( float ) rx_in_period ) / scale;
            web_page += "<p>";
            web_page += "total download: " + Utils::float_to_string( value ) + " " + unit;
            web_page += ",  ";
            value = ( ( float ) tx_in_period ) / scale;
            web_page += "total upload: " + Utils::float_to_string( value ) + " " + unit;
            web_page += "</p>";
            web_page += "</div>";
            web_page += "<br>";
            web_page += "<hr>";
        }

        chart_id = 0;
        web_page += "<script>\n";
        web_page += "window.onload = function(){\n";

        //genrate chart object
        for ( auto const & mac_table : Globals::all_stats )
        {

            string canvas_id = "canvas";
            canvas_id += Utils::to_string( chart_id );

            string var_chart_data = "ChartData";
            var_chart_data += Utils::to_string( chart_id );

            string var_options = "options";
            var_options += Utils::to_string( chart_id );

            string var_context = "ctx";
            var_context += Utils::to_string( chart_id );

            string window_chart = "chart";
            window_chart += Utils::to_string( chart_id );

            web_page += "var " + var_context + " = document.getElementById(\"" + canvas_id + "\").getContext(\"2d\");\n";
            web_page += "var " + window_chart + " = new Chart(" + var_context + ",{\n";
            web_page += "type: '" + chart_type_str + "',\n";
            web_page += "data: " + var_chart_data + ",\n";
            web_page += "options: " + var_options + "\n";
            web_page += "});\n";
            chart_id++;
        }


        //finish generating html
        web_page += "}\n";
        web_page += "</script>\n";

        web_page += "</td>\n";
        web_page += "</tr>\n";
        web_page += "</tbody>\n";
        web_page += "</table>\n";
        web_page += "</body>\n";
        web_page += "</html>";

        //send html to the user
        stringstream out_stream;
        out_stream << web_page;

        out_stream.seekp( 0, ios::end );
        response <<  "HTTP/1.1 200 OK\r\nContent-Length: " << out_stream.tellp() << "\r\n";
        response << "Content-Type: text/html; charset=utf-8" << "\r\n";
        response << "Cache-Control: public, max-age=1800";
        response << "\r\n\r\n" << out_stream.rdbuf();

    };

    server.resource["\\/Chart.js$"]["GET"] = []( SimpleWeb::Server<SimpleWeb::HTTP>::Response & response, shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> )
    {
        string chartjs_path;
        chartjs_path += Globals::cwd + PATH_SEPARATOR + "webpage" + PATH_SEPARATOR + "Chart.js";

        ifstream file;
        file.open( chartjs_path, std::ifstream::in | std::ifstream::binary );

        if ( file.is_open() )
        {
            stringstream input_file_stream;
            input_file_stream << file.rdbuf();
            file.close();

            input_file_stream.seekp( 0, ios::end );
            response <<  "HTTP/1.1 200 OK\r\nContent-Length: " << input_file_stream.tellp() << "\r\n";
            response << "Content-Type: application/javascript; charset=utf-8" << "\r\n";
            response << "Cache-Control: public, max-age=1800";
            response << "\r\n\r\n" << input_file_stream.rdbuf();
        }
    };

    server.resource["\\/stats.json$"]["GET"] = []( SimpleWeb::Server<SimpleWeb::HTTP>::Response & response, shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> )
    {
        string row;
        stringstream content_stream;

        Json::Value root;
        Json::StyledWriter writer;
        uint32_t y;
        uint32_t m;
        uint32_t d;
        uint32_t h;
        Utils::get_time( &y, &m, &d, &h );

        for ( auto const & mac_speedinfo : Globals::speed_stats )
        {
            const string& mac = mac_speedinfo.first;
            const InterfaceSpeedMeter& ism = mac_speedinfo.second;

            // speed is in bits/s
            root[mac]["speed"]["down"] = Json::Value::UInt64 ( ism.get_rx_speed() );
            root[mac]["speed"]["up"] = Json::Value::UInt64 ( ism.get_tx_speed() );

            row.clear();
            row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 ) + "_" + Utils::to_string( h, 2 ) + ":00-" + Utils::to_string( h + 1, 2 ) + ":00";

            if ( Globals::all_stats[mac]["hourly"].find( row ) != Globals::all_stats[mac]["hourly"].end() )
            {
                root[mac]["hourly"]["down"] = Json::Value::UInt64 ( Globals::all_stats[mac]["hourly"][row].recieved() );
                root[mac]["hourly"]["up"] = Json::Value::UInt64 ( Globals::all_stats[mac]["hourly"][row].transmited() );
            }
            else
            {
                root[mac]["hourly"]["down"] = Json::Value::UInt64 ( 0ULL );
                root[mac]["hourly"]["up"] = Json::Value::UInt64 ( 0ULL );
            }

            row.clear();
            row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 );

            if ( Globals::all_stats[mac]["daily"].find( row ) != Globals::all_stats[mac]["daily"].end() )
            {
                root[mac]["daily"]["down"] = Json::Value::UInt64 ( Globals::all_stats[mac]["daily"][row].recieved() );
                root[mac]["daily"]["up"] = Json::Value::UInt64 ( Globals::all_stats[mac]["daily"][row].transmited() );
            }
            else
            {
                root[mac]["daily"]["down"] = Json::Value::UInt64 ( 0ULL );
                root[mac]["daily"]["up"] = Json::Value::UInt64 ( 0ULL );
            }

            row.clear();
            row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 );

            if ( Globals::all_stats[mac]["monthly"].find( row ) != Globals::all_stats[mac]["monthly"].end() )
            {
                root[mac]["monthly"]["down"] = Json::Value::UInt64 ( Globals::all_stats[mac]["monthly"][row].recieved() );
                root[mac]["monthly"]["up"] = Json::Value::UInt64 ( Globals::all_stats[mac]["monthly"][row].transmited() );
            }
            else
            {
                root[mac]["monthly"]["down"] = Json::Value::UInt64 ( 0ULL );
                root[mac]["monthly"]["up"] = Json::Value::UInt64 ( 0ULL );
            }

            row.clear();
            row += Utils::to_string( y );

            if ( Globals::all_stats[mac]["yearly"].find( row ) != Globals::all_stats[mac]["yearly"].end() )
            {
                root[mac]["yearly"]["down"] = Json::Value::UInt64 ( Globals::all_stats[mac]["yearly"][row].recieved() );
                root[mac]["yearly"]["up"] = Json::Value::UInt64 ( Globals::all_stats[mac]["yearly"][row].transmited() );
            }
            else
            {
                root[mac]["yearly"]["down"] = Json::Value::UInt64 ( 0ULL );
                root[mac]["yearly"]["up"] = Json::Value::UInt64 ( 0ULL );
            }
        }

        string str_response = writer.write( root );

        content_stream << str_response;

        content_stream.seekp( 0, ios::end );
        response <<  "HTTP/1.1 200 OK\r\nContent-Length: " << content_stream.tellp() << "\r\n";
        response << "Content-Type: application/json; charset=UTF-8" << "\r\n";
        response << "Cache-Control: no-cache, public";
        response << "\r\n\r\n" << content_stream.rdbuf();
    };

    server.resource["\\/savedata$"]["GET"] = []( SimpleWeb::Server<SimpleWeb::HTTP>::Response & response, shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> )
    {
        stringstream content_stream;

        const string& storage = Settings::settings["storage"];

        if ( Utils::contians( storage, "mysql" ) )
        {
#ifdef use_mysql
            //save_stats_to_mysql();
#endif // use_mysql
        }

        if ( Utils::contians( storage, "sqlite" ) )
        {
#ifdef use_sqlite
            Utils::save_stats_to_sqlite();
#endif // use_sqlite
        }

        if ( Utils::contians( storage, "files" ) )
        {
            Utils::save_stats_to_files();
        }

        string web_page;
        web_page += "<!doctype html>\n";
        web_page += "<html>\n";
        web_page += "<head>\n";
        web_page += "<title>data save</title>";
        web_page += "<style>\n";
        web_page += "p {color:black; font-family: arial}\n";
        web_page += "</style>\n";
        web_page += "</head>\n";
        web_page += "<body>\n";
        web_page += "<div style=\"width: 100%; text-align:center\" align=\"center\">\n";
        web_page += "<p>all data saved</p>\n";
        web_page += "</div>";
        web_page += "</body>\n";
        web_page += "</html>";

        content_stream << web_page;

        content_stream.seekp( 0, ios::end );
        response <<  "HTTP/1.1 200 OK\r\nContent-Length: " << content_stream.tellp() << "\r\n";
        response << "Content-Type: text/html; charset=utf-8" << "\r\n";
        response << "Cache-Control: no-cache, public";
        response << "\r\n\r\n" << content_stream.rdbuf();
    };

}
/** @brief rgba_color
  *
  * @todo: document this function
  */
string WebSiteContent::rgba_color( string& hex_color, float a )
{
    string out;

    vector<string> rgb = Utils::hexcolor_to_strings( hex_color );

    out += "rgba(";
    out += rgb[0];
    out += ",";
    out += rgb[1];
    out += ",";
    out += rgb[2];
    out += ",";
    out += Utils::float_to_string( a );
    out += ")";

    return out;
}

#endif // _NO_WEBSERVER


//http://html-online.com/editor/

/*

<!-- #######  YAY, I AM THE SOURCE EDITOR! #########-->
<p>&nbsp;</p>
<table style="height: 72px; margin-left: auto; margin-right: auto;" width="595">
<tbody>
<tr>
<td style="text-align: center;" colspan="3"><strong>eth0, 00-1f-AC-FB, 192.168.0.1<br /></strong></td>
</tr>

<tr>
<td rowspan="2"><strong>speed</strong></td>
<td><span style="color: #800080;"><strong>up</strong></span></td>
<td><span style="color: #800080;"><strong>135 Kbps</strong></span></td>
</tr>
<tr>
<td><span style="color: #008000;"><strong>down</strong></span></td>
<td><span style="color: #008000;"><strong>1.4567 Mbps</strong></span></td>
</tr>

</tbody>
</table>
*/

//http://www.w3schools.com/js/js_timing.asp
//http://www.tutorialspoint.com/ajax/index.htm
//http://stackoverflow.com/questions/4672691/ajax-responsetext-comes-back-as-undefined
//https://mathiasbynens.be/notes/xhr-responsetype-json
//http://www.flotcharts.org/
//http://www.jqplot.com/
//https://www.amcharts.com/demos/
//http://www.uibox.in/item/68

