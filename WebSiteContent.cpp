/*

websitecontent.cpp

Copyright (C) 2017 Rafał Frączek

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


#ifndef _NO_WEBSERVER
#include "config.h"
#include <fstream>
#include <memory>
#include <sstream>
#include <regex>
#include "WebSiteContent.h"
#include "json/json.h"
#include "defines.h"
#include "Url.h"
#include "Utils.h"
#include "Logger.h"
#include "Globals.h"
#include "Settings.h"
#include "Resources.h"
#include "InterfaceInfo.h"
#include "InterfaceStats.h"
#include "InterfaceSpeedMeter.h"
#include "GroveStreamsUploader.h"

#ifdef use_sqlite
#include "sqlite3.h"
#endif // use_sqlite

using namespace std;

typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;

/** @brief set_web_site_content
  *
  * @todo: document this function
  */
void WebSiteContent::set_web_site_content( SimpleWeb::Server<SimpleWeb::HTTP>& server )
{
    server.default_resource["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
    {
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

        string os_info;

        Utils::get_os_info( os_info );

        time_t t = time( nullptr );
        Utils::seconds_to_date( t, &end_year, &end_month, &end_day, &end_hour );

        string end_year_str = Utils::to_string( end_year );
        string end_month_str = Utils::to_string( end_month );
        string end_day_str = Utils::to_string( end_day );
        string end_hour_str = Utils::to_string( end_hour );

        Utils::seconds_to_date( t - 24ULL * 60ULL * 60ULL, &start_year, &start_month, &start_day, &start_hour );
        string start_year_str = Utils::to_string( start_year );
        string start_month_str = Utils::to_string( start_month );
        string start_day_str = Utils::to_string( start_day );
        string start_hour_str = Utils::to_string( start_hour );

        string hourly_past_day = "<a href=\"hourly/start=" + start_year_str + "-" + start_month_str + "-" + start_day_str + "-" + start_hour_str + "\">past day</a>";

        Utils::seconds_to_date( t - 24ULL * 60ULL * 60ULL * 7ULL, &start_year, &start_month, &start_day, &start_hour );
        start_year_str = Utils::to_string( start_year );
        start_month_str = Utils::to_string( start_month );
        start_day_str = Utils::to_string( start_day );
        start_hour_str = Utils::to_string( start_hour );
        string hourly_past_week = "<a href=\"hourly/start=" + start_year_str + "-" + start_month_str + "-" + start_day_str + "-" + start_hour_str + "\">past week</a>";
        string daily_past_week = "<a href=\"daily/start=" + start_year_str + "-" + start_month_str + "-" + start_day_str + "\">past week</a>";

        Utils::seconds_to_date( t - 24ULL * 60ULL * 60ULL * 30ULL, &start_year, &start_month, &start_day, &start_hour );
        start_year_str = Utils::to_string( start_year );
        start_month_str = Utils::to_string( start_month );
        start_day_str = Utils::to_string( start_day );
        start_hour_str = Utils::to_string( start_hour );
        string hourly_past_month = "<a href=\"hourly/start=" + start_year_str + "-" + start_month_str + "-" + start_day_str + "-" + start_hour_str + "\">past month</a>";
        string daily_past_month = "<a href=\"daily/start=" + start_year_str + "-" + start_month_str + "-" + start_day_str + "\">past month</a>";

        Utils::seconds_to_date( t - 24ULL * 60ULL * 60ULL * 365ULL, &start_year, &start_month, &start_day, &start_hour );
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
        page += "<script src=\"/speed_update.js\"></script>\n";
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
        //page += "text-align: justify;\n";
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
        page += "<body background=\"background.png\">\n";
        page += "<table style=\"margin-left: auto; margin-right: auto; width: 437px;\">\n";
        page += "<tbody>\n";
        page += "<tr>\n";
        page += "<td>\n";
        page += "<h1>Smart Traffic Meter</h1>\n";
        page += "</td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td style=\"font-family: Verdana,Arial,Helvetica,sans-serif; font-size: x-large; font-style: normal; line-height: normal; color: #000000; vertical-align: middle; text-align: center; position: relative; visibility: visible;\">\n";
        page += "<pre>" + Globals::user_name + " @ " + Globals::host_name + "</pre>\n";
        page += "<pre>" + os_info + "</pre>\n";
        page += "<pre> pid: " + Utils::to_string( static_cast<uint64_t>( getpid() ), 1 ) + "</pre>\n";
        page += "</td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td>\n";
        page += "<br>\n";
        page += "<br>\n";
        page += "<h2>running network interfaces</h2>\n";
        page += "<table style=\"margin-left: auto; margin-right: auto;\" cellspacing=\"1\" cellpadding=\"1\">\n";
        page += "<tbody>\n";

        for ( auto const & mac_info : Globals::interfaces )
        {
            const InterfaceInfo& interface_info = mac_info.second;

            const string& interface_mac = interface_info.get_mac();
            const string& interface_name = interface_info.get_name();
            const string& ip4 = interface_info.get_ip4();

            page += "<tr>\n";

            page += "<td>\n";
            page += "<p>&#8226; " + interface_name + ",\t" + interface_mac + ",\t" + ip4; // + ",\t";// + ip6;
            page += "</p></td>\n";

            page += "<td><div style='width: 130px;'>\n";
            page += "<p><span style=\"color:green; margin: left\">";
            page += "&#11015;";
            page += "<span id=\"mac_" + interface_mac + "_down\"></span>\n";
            page += "</span></p>";
            page += "</div></td>\n";

            page += "<td><div style='width: 130px;'>\n";
            page += "<p><span style=\"color:red; margin: left\">";
            page += "&#11014;";
            page += "<span id=\"mac_" + interface_mac + "_up\"></span>\n";
            page += "</span></p>";
            page += "</div></td>\n";
            page += "</tr>\n";
        }

        page += "</tbody></table>\n";
        page += "<p style=\"text-align: center;\">";
        page += "<a style=\"padding-left:1em; padding-right:1em\" href=\"/history/\">network history</a>";
        page += "<a style=\"padding-left:1em; padding-right:1em\" href=\"/stopwatch/\">stopwatch</a>";
        page += "</p>\n";
        page += "</td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td>\n";
        page += "<p style=\"text-align: center;\">";
        page += "<a style=\"padding-left:1em; padding-right:1em\" href=\"/customtimespan/\">custom time span</a>";
        page += "</p>\n";
        page += "</td>\n";
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
        page += "<a href=\"/stop/\">stop Smart Traffic Meter</a>\n";
        page += "</p>";
        page += "<p style=\"text-align: center;\">";
        page += "<a href=\"/backup/\">back up stats</a>\n";
        page += "</p>";
        page += "<br><br>\n";
        page += "<div style=\"text-align:center\" align=\"center\">\n";
        page += "<p>\n";
        page += "<a href=\"/settings/\">settings</a>\n";
        page += "&emsp;";
        page += "<a href=\"/legalinfo/\">legal info</a>\n";
        page += "</p>\n";
        page += "</div>\n";

        page += "</body>\n";
        page += "</html>\n\n";

        content_stream = Utils::gz_compress(page);

        content_stream.seekp( 0, ios::end );

        *response << "HTTP/1.1 200 OK\r\n";
        *response << "Last-Modified: " << Utils::rfc1123_datetime( time( NULL ) ) << "\r\n";
        *response << "Content-Length: " << content_stream.tellp() << "\r\n";
        *response << "Content-Encoding: gzip\r\n";
        *response << "Content-Type: text/html; charset=utf-8" << "\r\n";
        *response << "Cache-Control: no-cache, no-store, public";
        *response << "\r\n\r\n";
        *response << content_stream.rdbuf();
    };

    server.resource["^/customrange/?$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
    {
        // https://jsfiddle.net/nulref/v29okbqr/
        string page;

        stringstream content_stream;
        content_stream << page;

        content_stream.seekp( 0, ios::end );

        *response <<  "HTTP/1.1 200 OK\r\n";
        *response << "Content-Length: " << content_stream.tellp() << "\r\n";
        *response << "Content-Type: text/html; charset=utf-8" << "\r\n";
        *response << "Cache-Control: no-cache, no-store, public" << "\r\n";
        content_stream.seekp( 0, ios::beg );
        *response << "\r\n\r\n" << content_stream.rdbuf();
    };


    server.resource["^/stopwatch/?$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
    {
        string page;

        page += "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n";
        page += "<html>\n";
        page += "<head>\n";
        page += "<meta content=\"text/html; charset=UTF-8\" http-equiv=\"content-type\">\n";
        page += "<title>Smart Traffic Meter Stopwatch</title>\n";
        page += "<script src=\"/stopwatch.js\"></script>\n";
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
        page += "<body background=\"../background.png\" onload=\"init()\">\n";

        page += "<h1>Stopwatch</h1>\n<br>\n";

        page += "<table style=\"margin-left: auto; margin-right: auto;\" cellspacing=\"1\" cellpadding=\"1\">\n";
        page += "<tbody>\n";

        for ( auto const & mac_info : Globals::interfaces )
        {
            const InterfaceInfo& interface_info = mac_info.second;

            const string& interface_mac = interface_info.get_mac();
            const string& interface_name = interface_info.get_name();
            const string& ip4 = interface_info.get_ip4();

            page += "<tr>\n";
            page += "<td><p style='text-align: center;'>" + interface_name + ",\t" + interface_mac + ",\t" + ip4; // + ",\t";// + ip6;
            page += "</p></td>\n";

            page += "<td><div style='width: 130px;'>\n";
            page += "<p><span style=\"color:green; margin: left\">&#11015;<span id=\"mac_" + interface_mac + "_down\"></span>\n";
            page += "</span></p></div></td>\n";
            page += "<td><div style='width: 130px;'>\n";
            page += "<p><span style=\"color:red; margin: left\">&#11014;<span id=\"mac_" + interface_mac + "_up\"></span>\n";
            page += "</span></p></div></td>\n";
            page += "<td><div style='width: 130px;'>\n";
            page += "<p><span style=\"color:blue; margin: left\"><span style=\"color:green\">&#11015;</span><span style=\"color:red\">&#11014;</span><span id=\"mac_" + interface_mac + "_total\"></span>\n";
            page += "</span></p></div></td>\n";
            page += "</tr>\n";
        }

        page += "<tr><td  colspan=\"4\"><p style='text-align: center;'><span id=\"time\">00:00:00</span></p></td></tr>\n";
        page += "</tbody>\n";
        page += "</table>\n";

        page += "<br>\n";
        page += "<center>\n";
        page += "<form name=\"stpw\">\n";
        page += "<input type=\"button\" name=\"startbutton\" value=\"Start\" id=\"start_button\" onclick=\"starttimer()\">\n";
        page += "<input type=\"button\" name=\"pausebutton\" value=\"Pause\" id=\"pause_button\" onclick=\"pausetimer()\">\n";
        page += "<input type=\"button\" name=\"reset\" value=\"Reset\" id=\"reset_button\" onclick=\"resetstats()\">\n";
        page += "</form>\n";
        page += "</center>\n";
        page += "<br>\n";
        page += "<div style=\"text-align:center\" align=\"center\">\n";
        page += "<p>\n<a href=\"/\">Home</a></p>\n";
        page += "</div>\n";
        page += "</body>\n";
        page += "</html>\n";

        stringstream content_stream;
        content_stream = Utils::gz_compress(page);

        content_stream.seekp( 0, ios::end );
        *response <<  "HTTP/1.1 200 OK\r\n";
        *response << "Content-Length: " << content_stream.tellp() << "\r\n";
        *response << "Content-Encoding: gzip\r\n";
        *response << "Content-Type: text/html; charset=utf-8" << "\r\n";
        *response << "Cache-Control: no-cache, no-store, public";
        *response << "\r\n\r\n" << content_stream.rdbuf();
    };

    server.resource["^/legalinfo/?$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
    {
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
        page += "<body background=\"../background.png\">\n";

        page += "<h1>legal information</h1>\n";
        page += "<br><br>\n";
        page += "<div align=\"center\">\n";
        page += "<p>\n";
        page += "Copyright &copy; 2017 Rafał Frączek\n";
        page += "</p>\n";
        page += "<p>\n";
        page += "Smart Traffic Meter is released under the <a href=\"https://opensource.org/licenses/GPL-3.0\">GNU General Public License, version 3 (GPL-3.0)</a> license.\n";
        page += "</p>\n";
        page += "<p><br></p>\n";
        page += "<p>\n";
        page += "Made possible with the following open source software\n";
        page += "</p>\n";
        page += "</div>\n";

        page += "<table border=\"0\" cellspacing=\"30\" cellpadding=\"10\" align=\"center\">\n";
        page += "<tbody>\n";
        page += "<tr>\n";
        page += "<td align=\"left\" valign=\"middle\"><p>boost libraries</p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"http://www.boost.org/\">home page</a></p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"http://www.boost.org/users/license.html\">license</a></p></td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td align=\"left\" valign=\"middle\"><p>Chart.js</p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"http://www.chartjs.org/\">home page</a></p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"https://opensource.org/licenses/MIT\">license</a></p></td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td align=\"left\" valign=\"middle\"><p>JsonCpp</p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"https://github.com/open-source-parsers/jsoncpp\">home page</a></p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"https://github.com/open-source-parsers/jsoncpp/blob/master/LICENSE\">license</a></p></td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td align=\"left\" valign=\"middle\"><p>Simple-Web-Server</p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"https://github.com/eidheim/Simple-Web-Server\">home page</a></p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"https://github.com/eidheim/Simple-Web-Server/blob/master/LICENSE\">license</a></p></td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td align=\"left\" valign=\"middle\"><p>SQLite</p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"https://www.sqlite.org/\">home page</a></p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"https://www.sqlite.org/copyright.html\">license</a></p></td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td align=\"left\" valign=\"middle\"><p>TCLAP</p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"http://tclap.sourceforge.net/\">home page</a></p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"https://opensource.org/licenses/mit-license.php\">license</a></p></td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td align=\"left\" valign=\"middle\"><p>Smoothie Charts</p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"http://smoothiecharts.org\">home page</a></p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"http://smoothiecharts.org/LICENSE.txt\">license</a></p></td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td align=\"left\" valign=\"middle\"><p>urlcpp</p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"https://github.com/larroy/urlcpp\">home page</a></p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"https://github.com/larroy/urlcpp/blob/master/LICENSE.txt\">license</a></p></td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td align=\"left\" valign=\"middle\"><p>jquery</p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"https://jquery.com\">home page</a></p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"https://github.com/jquery/jquery/blob/master/LICENSE.txt\">license</a></p></td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td align=\"left\" valign=\"middle\"><p>jscolor</p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"http://jscolor.com\">home page</a></p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"https://www.gnu.org/licenses/gpl-3.0.txt\">license</a></p></td>\n";
        page += "</tr>\n";
		page += "<tr>\n";
        page += "<td align=\"left\" valign=\"middle\"><p>hammer.js</p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"https://hammerjs.github.io\">home page</a></p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"https://github.com/hammerjs/hammer.js/blob/master/LICENSE.md\">license</a></p></td>\n";
        page += "</tr>\n";
		page += "<tr>\n";
        page += "<td align=\"left\" valign=\"middle\"><p>chartjs-plugin-zoom</p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"https://github.com/chartjs/chartjs-plugin-zoom\">home page</a></p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><p><a href=\"https://opensource.org/licenses/MIT\">license</a></p></td>\n";
        page += "</tr>\n";
        page += "</tbody>\n";
        page += "</table>\n";
        page += "<br>\n";
        page += "<div style=\"text-align:center\" align=\"center\">\n";
        page += "<p>\n<a href=\"/\">Home</a></p>\n";
        page += "</div>\n";

        page += "</body>\n";
        page += "</html>\n";

        stringstream content_stream;
        content_stream = Utils::gz_compress(page);

        content_stream.seekp( 0, ios::end );
        *response <<  "HTTP/1.1 200 OK\r\nContent-Length: " << content_stream.tellp() << "\r\n";
        *response << "Content-Encoding: gzip\r\n";
        *response << "Content-Type: text/html; charset=utf-8" << "\r\n";
        *response << "Cache-Control: public, max-age=1800";
        *response << "\r\n\r\n" << content_stream.rdbuf();
    };

    server.resource["^\\/download\\/(hourly|daily|monthly|yearly)\\/?(.*)?\\/?$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
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
        cout << stats_type << endl;

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

            // get interface information
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

            csv += mac + "\r\n";
            csv += "date, download, upload\r\n";

            for ( auto & row_stats : results )
            {
                const InterfaceStats& stats = row_stats.second;
                const string& row = row_stats.first;

                if ( stats.received() != 0ULL || stats.transmitted() != 0ULL )
                {
                    csv += row + "," + Utils::to_string( stats.received() ) + "," + Utils::to_string( stats.transmitted() ) + "\r\n";
                }
            }

            csv += "\r\n";
        }

        stringstream out_stream;
        out_stream << csv;

        out_stream.seekp( 0, ios::end );
        *response <<  "HTTP/1.1 200 OK\r\nContent-Length: " << out_stream.tellp() << "\r\n";
        *response << "Last-Modified: " << Utils::rfc1123_datetime( time( NULL ) ) << "\r\n";
        *response << "Content-Type: text/csv; charset=utf-8" << "\r\n";
        *response << "Cache-Control: public, max-age=0" << "\r\n";
        *response << "Content-Disposition: attachment; filename=\"" + stats_type + "_" + start_date_str + "_to_" + end_date_str + ".csv\"";
        *response << "\r\n\r\n" << out_stream.rdbuf();
    };


    server.resource["^\\/(hourly|daily|monthly|yearly)\\/?(.*)?\\/?$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
    {
        uint32_t y;
        uint32_t m;
        uint32_t d;
        uint32_t h;

        uint64_t rx_in_period = 0ULL;
        uint64_t tx_in_period = 0ULL;
        string unit;
        float scale;

        string page;

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

        // create chart colours
        string down_fill_color = rgba_color( down_color_str, 0.5f );
        string down_stroke_color = rgba_color( down_color_str, 0.8f );
        string down_highlightfill_color = rgba_color( down_color_str, 0.75f );
        string down_highlightstroke_color = rgba_color( down_color_str, 1.0f );

        string up_fill_color = rgba_color( up_color_str, 0.5f );
        string up_stroke_color = rgba_color( up_color_str, 0.8f );
        string up_highlightfill_color = rgba_color( up_color_str, 0.75f );
        string up_highlightstroke_color = rgba_color( up_color_str, 1.0f );

        // start generating web page
        page += "<!doctype html>\n";
        page += "<html>\n";
        page += "<head>\n";
        page += "<title>statistics</title>";
        page += "<script src=\"/Chart.js\"></script>\n";
        page += "<script src=\"/zoom.js\"></script>\n";
        page += "<script src=\"/hammer.js\"></script>\n";

        page += "<style>\n";
        page += "p {color:black; font-family: arial}\n";
        page += "</style>\n";

        page += "</head>\n";
        page += "<div style=\"width: 100%; text-align:center\" align=\"center\">\n";
        page += "<p><a href=\"/savedata\">save all data</a></p>\n";
        page += "</div>";
        page += "<table style=\"text-align: center; width: " + chartwidth_str + "px; margin-left: auto; margin-right: auto;\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n";
        page += "<tbody>\n";
        page += "<tr>\n";
        page += "<td style=\"vertical-align: top; text-align: center; width: " + chartwidth_str + "px;\">\n";


        uint32_t chart_id = 0;

        // create chart for each network interface
        for ( auto const & mac_info : Globals::interfaces )
        {
            const InterfaceInfo& interface_info = mac_info.second;

            const string& mac = interface_info.get_mac();
            const string& interface_name = interface_info.get_name();
            const string& ip4 = interface_info.get_ip4();
            const string& ip6 = interface_info.get_ip6();

            string canvas_id = "canvas";
            canvas_id += Utils::to_string( chart_id );

            page += "<div title=\"interface name, mac, IP4, IP6\" style=\"width: 100%\" align=\"center\">\n";

#ifdef _WIN32
            page += "<p>" + interface_description + ", " + mac + ", " + ip4;
#endif // _WIN32

#ifdef __linux
            page += "<p>" + interface_name + ", " + mac + ", " + ip4 + ", " + ip6;
#endif // __linux

            page += "</p></div>";

            page += "<div style=\"width: 100%; text-align:center\" align=\"center\">\n";
            page += "<canvas id=\"" + canvas_id + "\" height=\"" + chartheight_str + "\" width=\"" + chartwidth_str + "\"></canvas>\n";
            page += "</div>\n";

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
                    uint64_t rx = Globals::all_stats[mac][stats_type][current_time_str].received();
                    uint64_t tx = Globals::all_stats[mac][stats_type][current_time_str].transmitted();
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

                if ( stats.received() > max_value )
                {
                    max_value = stats.received();
                }

                if ( stats.transmitted() > max_value )
                {
                    max_value = stats.transmitted();
                }

                rx_in_period += stats.received();
                tx_in_period += stats.transmitted();
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

                float value = ( ( float ) stats.received() ) / scale;

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

                float value = ( ( float ) stats.transmitted() ) / scale;

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
            chart_options += "pan: {\n";
            chart_options += "enabled: true,\n";
            chart_options += "mode: 'xy',\n";
            chart_options += "speed: 2,\n";
            chart_options += "threshold: 2\n";
            chart_options += "},\n";
            chart_options += "zoom: {\n";
            chart_options += "enabled: true,\n";
            chart_options += "mode: 'y'\n";
            chart_options += "},\n";
            chart_options += "responsive : true\n";
            chart_options += "};";

            page += "<script>\n";
            page += chart_data;
            page += "\n";
            page += chart_options;

            string var_chart = "ctx";
            var_chart += Utils::to_string( chart_id );

            page += "</script>\n";
            page += "<br>\n";

            chart_id++;

            page += "<div style=\"width: 100%\" align=\"center\">\n";
            float value = ( ( float ) rx_in_period ) / scale;
            page += "<p>";
            page += "total download: " + Utils::float_to_string( value, 2 ) + " " + unit;
            page += ",  ";
            value = ( ( float ) tx_in_period ) / scale;
            page += "total upload: " + Utils::float_to_string( value, 2 ) + " " + unit;
            page += "</p>";
            page += "</div>";
            page += "<br>";
            page += "<hr>";
        }

        chart_id = 0;
        page += "<script>\n";
        page += "window.onload = function(){\n";

        //generate chart objects
        for ( auto it = Globals::all_stats.begin(); it != Globals::all_stats.end(); it++ )
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

            page += "var " + var_context + " = document.getElementById(\"" + canvas_id + "\").getContext(\"2d\");\n";
            page += "var " + window_chart + " = new Chart(" + var_context + ",{\n";
            page += "type: '" + chart_type_str + "',\n";
            page += "data: " + var_chart_data + ",\n";
            page += "options: " + var_options + "\n";
            page += "});\n";
            chart_id++;
        }

        page += "}\n";
        page += "</script>\n";

        page += "</td>\n";
        page += "</tr>\n";
        page += "</tbody>\n";
        page += "</table>\n";
        page += "<div style=\"width: 100%; text-align:center\" align=\"center\">\n<p>\n";
        page += "<a href=\"/download/" + stats_type + "/start=" + start_date_str + "&amp;end=" + end_date_str + "\">download this stats</a>\n</p>\n";
        page += "<p>\n<a href=\"/\">Home</a></p>\n";
        page += "</p>\n</div>\n";
        page += "</body>\n";
        page += "</html>";

        stringstream out_stream;
        out_stream = Utils::gz_compress(page);

        out_stream.seekp( 0, ios::end );
        *response <<  "HTTP/1.1 200 OK\r\nContent-Length: " << out_stream.tellp() << "\r\n";
        *response << "Last-Modified: " << Utils::rfc1123_datetime( time( NULL ) ) << "\r\n";
        *response << "Content-Encoding: gzip\r\n";
        *response << "Content-Type: text/html; charset=utf-8" << "\r\n";
        *response << "Cache-Control: no-cache, no-store, public";
        *response << "\r\n\r\n" << out_stream.rdbuf();
    };

    server.resource["^\\/history\\/?(.*)?\\/?$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
    {
        string row;
        ifstream file;
        stringstream content_stream;

        string page;

        page += "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n";
        page += "<html>\n";
        page += "<head>\n";
        page += "<meta content=\"text/html; charset=UTF-8\" http-equiv=\"content-type\">\n";
        page += "<title>Smart Traffic Meter Speed Charts</title>\n";
        page += "<script src=\"/smoothie.js\"></script>\n";
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
        page += "<table style=\"margin-left: auto; margin-right: auto; width: 830px;\">\n";
        page += "<tbody>\n";
        page += "<tr>\n";
        page += "<td>\n";
        page += "<br>\n";
        page += "<br>\n";
        page += "<h2>network history</h2>\n<br>\n";

        for ( auto const & mac_info : Globals::interfaces )
        {
            const InterfaceInfo& interface_info = mac_info.second;

            const string& interface_mac = interface_info.get_mac();
            const string& interface_name = interface_info.get_name();
            const string& ip4 = interface_info.get_ip4();

            page += "<p style='text-align: center;'>" + interface_name + ",\t" + interface_mac + ",\t" + ip4; // + ",\t";// + ip6;
            page += "</p>\n";

            string mac = Utils::replace( "-", "_", interface_info.get_mac() );

            page += "<div style='text-align: center;'><p>upload</p></div>\n";
            page += "<canvas id=\"mac_" + mac + "_up_canvas\" width=\"830\" height=\"180\"></canvas>\n";
            page += "<br><br>\n";
            page += "<div style='text-align: center;'><p>download</p></div>\n";
            page += "<canvas id=\"mac_" + mac + "_down_canvas\" width=\"830\" height=\"180\"></canvas>\n";
            page += "<br><br><hr><br>";
        }

        page += "<script>\n";
        page += "var down_series={};\n";
        page += "var up_series={};\n";

        for ( auto const & mac_info : Globals::interfaces )
        {
            const InterfaceInfo& interface_info = mac_info.second;
            const string& mac = Utils::replace( "-", "_", interface_info.get_mac() );

            string var_up_chart = "chart_up_" + mac;
            string var_down_chart = "chart_down_" + mac;

            page += var_up_chart;
            page += "= new SmoothieChart({millisPerPixel:50,maxValueScale:1.10,scaleSmoothing:0.954,minValue:0,grid:{fillStyle:'#ffffff',strokeStyle:'rgba(119,119,119,0.30)',sharpLines:true,millisPerLine:10000,borderVisible:true},labels:{fillStyle:'#000000',precision:0}});\n";

            page += var_down_chart;
            page += "= new SmoothieChart({millisPerPixel:50,maxValueScale:1.10,scaleSmoothing:0.954,minValue:0,grid:{fillStyle:'#ffffff',strokeStyle:'rgba(119,119,119,0.30)',sharpLines:true,millisPerLine:10000,borderVisible:true},labels:{fillStyle:'#000000',precision:0}});\n";

            page += "up_series['" + interface_info.get_mac() + "'] = new TimeSeries();\n";
            page += "down_series['" + interface_info.get_mac() + "'] = new TimeSeries();\n";

            page += var_up_chart;
            page += ".addTimeSeries(up_series['" + interface_info.get_mac() + "'], {lineWidth:2.1,strokeStyle:'#ff0000',fillStyle:'rgba(255,0,0,0.17)'});\n";

            page += var_down_chart;
            page += ".addTimeSeries(down_series['" + interface_info.get_mac() + "'], {lineWidth:2.1,strokeStyle:'#00ff00',fillStyle:'rgba(0,255,0,0.17)'});\n";

            page += var_up_chart;
            page += ".streamTo(document.getElementById('mac_" + mac + "_up_canvas" + "'), 1000);\n";

            page += var_down_chart;
            page += ".streamTo(document.getElementById('mac_" + mac + "_down_canvas" + "'), 1000);\n";
        }

        page += "</script>\n";

        page += "<script>\n";
        page += "var xhttp = new XMLHttpRequest();\n";
        page += "var timer = setInterval(LoadData, 1000);\n";

        page += "xhttp.onreadystatechange = function ()\n";
        page += "{\n";
        page += "if (this.readyState == 4 && this.status == 200)\n";
        page += "{\n";
        page += "var json_obj = JSON.parse(this.responseText);\n";
        page += "for (var mac in json_obj)\n";
        page += "{\n";
        page += "if (json_obj.hasOwnProperty(mac));\n";
        page += "{\n";
        page += "var up_id = mac + \"_up\";\n";
        page += "var down_id = mac + \"_down\";\n";
        page += "var down_speed = json_obj[mac].speed[\"down\"];\n";
        page += "var up_speed = json_obj[mac].speed[\"up\"];\n";

        page += "down_series[mac].append(new Date().getTime(), down_speed);\n";
        page += "up_series[mac].append(new Date().getTime(), up_speed);\n";
        page += "}\n";
        page += "}\n";
        page += "}\n";
        page += "};\n";

        page += "function LoadData()\n";
        page += "{\n";
        page += "xhttp.open(\"GET\", \"/speed.json\", true);\n";
        page += "xhttp.setRequestHeader(\"Access-Control-Allow-Origin\", \"*\");\n";
        page += "xhttp.setRequestHeader(\"Access-Control-Allow-Headers\", \"Origin, X-Requested-With, Content-Type, Accept\");\n";
        page += "xhttp.setRequestHeader(\"contentType\", \"application/json; charset=utf-8\");\n";
        page += "xhttp.overrideMimeType(\"application/json\");\n";
        page += "xhttp.send();\n";
        page += "}\n";
        page += "</script>\n";

        page += "</td>\n";
        page += "</tr>\n";
        page += "</tbody>\n";
        page += "</table>\n";
        page += "<br>\n";
        page += "<p style='text-align: center;'>\n<a href=\"/\">Home</a></p>\n";
        page += "</body>\n";
        page += "</html>\n";

        content_stream = Utils::gz_compress(page);

        content_stream.seekp( 0, ios::end );

        *response <<  "HTTP/1.1 200 OK\r\nContent-Length: " << content_stream.tellp() << "\r\n";
        *response << "Last-Modified: " << Utils::rfc1123_datetime( time( NULL ) ) << "\r\n";
        *response << "Content-Encoding: gzip\r\n";
        *response << "Content-Type: text/html; charset=utf-8" << "\r\n";
        *response << "Cache-Control: no-cache, no-store, public";
        *response << "\r\n\r\n" << content_stream.rdbuf();
    };


    server.resource["\\/background.png$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
    {

        *response << "HTTP/1.1 200 OK" << "\r\n";
        *response << "Accept-Ranges: bytes" << "\r\n";
        *response << "Content-Type: image/png" << "\r\n";
        *response << "Content-Length: " << Resources::background_image_length << "\r\n";
        *response << "Cache-Control: public, max-age=1800";
        *response << "\r\n\r\n";

        for ( uint32_t i = 0; i < Resources::background_image_length; i++ )
        {
            *response << Resources::background_image[i];
        }
    };


    server.resource["\\/Chart.js$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
    {
        *response << "HTTP/1.1 200 OK\r\n";
        *response << "Content-Type: application/javascript; charset=utf-8" << "\r\n";
        *response << "Content-Encoding: gzip\r\n";
        *response << "Content-Length: " << Resources::chart_js_length << "\r\n";
        *response << "Cache-Control: public, max-age=1800";
        *response << "\r\n\r\n";

        for ( uint32_t i = 0; i < Resources::chart_js_length; i++ )
        {
            *response << Resources::chart_js[i];
        }
    };

    server.resource["\\/smoothie.js$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
    {
        *response << "HTTP/1.1 200 OK\r\n";
        *response << "Content-Type: application/javascript; charset=utf-8" << "\r\n";
        *response << "Content-Encoding: gzip\r\n";
        *response << "Content-Length: " << Resources::smoothie_js_length << "\r\n";
        *response << "Cache-Control: public, max-age=1800";
        *response << "\r\n\r\n";

        for ( uint32_t i = 0; i < Resources::smoothie_js_length; i++ )
        {
            *response << Resources::smoothie_js[i];
        }
    };

    server.resource["\\/jquery.js$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
    {
        *response << "HTTP/1.1 200 OK\r\n";
        *response << "Content-Type: application/javascript; charset=utf-8" << "\r\n";
        *response << "Content-Encoding: gzip\r\n";
        *response << "Content-Length: " << Resources::jquery_js_length << "\r\n";
        *response << "Cache-Control: public, max-age=1800";
        *response << "\r\n\r\n";

        for ( uint32_t i = 0; i < Resources::jquery_js_length; i++ )
        {
            *response << Resources::jquery_js[i];
        }
    };

    server.resource["\\/jscolor.js$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
    {
        *response << "HTTP/1.1 200 OK\r\n";
        *response << "Content-Type: application/javascript; charset=utf-8" << "\r\n";
        *response << "Content-Encoding: gzip\r\n";
        *response << "Content-Length: " << Resources::jscolor_js_length << "\r\n";
        *response << "Cache-Control: public, max-age=1800";
        *response << "\r\n\r\n";

        for ( uint32_t i = 0; i < Resources::jscolor_js_length; i++ )
        {
            *response << Resources::jscolor_js[i];
        }
    };

    server.resource["\\/zoom.js$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
    {
        *response << "HTTP/1.1 200 OK\r\n";
        *response << "Content-Type: application/javascript; charset=utf-8" << "\r\n";
        *response << "Content-Encoding: gzip\r\n";
        *response << "Content-Length: " << Resources::zoom_js_length << "\r\n";
        *response << "Cache-Control: public, max-age=1800";
        *response << "\r\n\r\n";

        for ( uint32_t i = 0; i < Resources::zoom_js_length; i++ )
        {
            *response << Resources::zoom_js[i];
        }
    };

    server.resource["\\/hammer.js$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
    {
        *response << "HTTP/1.1 200 OK\r\n";
        *response << "Content-Type: application/javascript; charset=utf-8" << "\r\n";
        *response << "Content-Encoding: gzip\r\n";
        *response << "Content-Length: " << Resources::hammer_js_length << "\r\n";
        *response << "Cache-Control: public, max-age=1800";
        *response << "\r\n\r\n";

        for ( uint32_t i = 0; i < Resources::hammer_js_length; i++ )
        {
            *response << Resources::hammer_js[i];
        }
    };

    server.resource["\\/speed_update.js$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
    {
        *response << "HTTP/1.1 200 OK\r\n";
        *response << "Content-Type: application/javascript; charset=utf-8" << "\r\n";
        *response << "Content-Encoding: gzip\r\n";
        *response << "Content-Length: " << Resources::speed_update_js_length << "\r\n";
        *response << "Cache-Control: public, max-age=1800";
        *response << "\r\n\r\n";

        for ( uint32_t i = 0; i < Resources::speed_update_js_length; i++ )
        {
            *response << Resources::speed_update_js[i];
        }
    };

    server.resource["\\/stopwatch.js$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
    {
        *response << "HTTP/1.1 200 OK\r\n";
        *response << "Content-Type: application/javascript; charset=utf-8" << "\r\n";
        *response << "Content-Encoding: gzip\r\n";
        *response << "Content-Length: " << Resources::stopwatch_js_length << "\r\n";
        *response << "Cache-Control: public, max-age=1800";
        *response << "\r\n\r\n";

        for ( uint32_t i = 0; i < Resources::stopwatch_js_length; i++ )
        {
            *response << Resources::stopwatch_js[i];
        }
    };

    server.resource["\\/custom_time_span.js$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
    {
        *response << "HTTP/1.1 200 OK\r\n";
        *response << "Content-Type: application/javascript; charset=utf-8" << "\r\n";
        *response << "Content-Encoding: gzip\r\n";
        *response << "Content-Length: " << Resources::custom_time_span_js_length << "\r\n";
        *response << "Cache-Control: public, max-age=1800";
        *response << "\r\n\r\n";

        for ( uint32_t i = 0; i < Resources::custom_time_span_js_length; i++ )
        {
            *response << Resources::custom_time_span_js[i];
        }
    };

    server.resource["\\/stats.json$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
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

        for ( auto const & mac_info : Globals::interfaces )
        {
            const string& mac = mac_info.first;
            const InterfaceSpeedMeter& ism = Globals::speed_stats[mac];

            // speed is in bits/s
            root[mac]["speed"]["down"] = Json::Value::UInt64 ( ism.get_rx_speed() );
            root[mac]["speed"]["up"] = Json::Value::UInt64 ( ism.get_tx_speed() );

            root[mac]["session"]["down"] = Json::Value::UInt64( Globals::session_stats[mac].received() );
            root[mac]["session"]["up"] = Json::Value::UInt64( Globals::session_stats[mac].transmitted() );

            row.clear();
            row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 ) + "_" + Utils::to_string( h, 2 ) + ":00-" + Utils::to_string( h + 1, 2 ) + ":00";

            if ( Globals::all_stats[mac]["hourly"].find( row ) != Globals::all_stats[mac]["hourly"].end() )
            {
                root[mac]["hourly"]["down"] = Json::Value::UInt64 ( Globals::all_stats[mac]["hourly"][row].received() );
                root[mac]["hourly"]["up"] = Json::Value::UInt64 ( Globals::all_stats[mac]["hourly"][row].transmitted() );
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
                root[mac]["daily"]["down"] = Json::Value::UInt64 ( Globals::all_stats[mac]["daily"][row].received() );
                root[mac]["daily"]["up"] = Json::Value::UInt64 ( Globals::all_stats[mac]["daily"][row].transmitted() );
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
                root[mac]["monthly"]["down"] = Json::Value::UInt64 ( Globals::all_stats[mac]["monthly"][row].received() );
                root[mac]["monthly"]["up"] = Json::Value::UInt64 ( Globals::all_stats[mac]["monthly"][row].transmitted() );
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
                root[mac]["yearly"]["down"] = Json::Value::UInt64 ( Globals::all_stats[mac]["yearly"][row].received() );
                root[mac]["yearly"]["up"] = Json::Value::UInt64 ( Globals::all_stats[mac]["yearly"][row].transmitted() );
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
        *response <<  "HTTP/1.1 200 OK\r\nContent-Length: " << content_stream.tellp() << "\r\n";
        *response << "Last-Modified: " << Utils::rfc1123_datetime( time( NULL ) ) << "\r\n";
        *response << "Content-Type: application/json; charset=UTF-8" << "\r\n";
        *response << "Cache-Control: no-cache, no-store, public";
        *response << "\r\n\r\n" << content_stream.rdbuf();
    };

    server.resource["\\/speed.json$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
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

        for ( auto const & mac_info : Globals::interfaces )
        {
            const string& mac = mac_info.first;
            const InterfaceSpeedMeter& ism = Globals::speed_stats[mac];

            // speed is in bits/s
            root[mac]["speed"]["down"] = Json::Value::UInt64 ( ism.get_rx_speed() );
            root[mac]["speed"]["up"] = Json::Value::UInt64 ( ism.get_tx_speed() );
        }

        string str_response = writer.write( root );

        content_stream << str_response;

        content_stream.seekp( 0, ios::end );
        *response <<  "HTTP/1.1 200 OK\r\nContent-Length: " << content_stream.tellp() << "\r\n";
        *response << "Last-Modified: " << Utils::rfc1123_datetime( time( NULL ) ) << "\r\n";
        *response << "Content-Type: application/json; charset=UTF-8" << "\r\n";
        *response << "Cache-Control: no-cache, no-store, public";
        *response << "\r\n\r\n" << content_stream.rdbuf();
    };

    server.resource["\\/savedata$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
    {
        stringstream content_stream;

        const string& storage = Settings::settings["storage"];

        if ( Utils::contains( storage, "mysql" ) )
        {
#ifdef use_mysql
            //save_stats_to_mysql();
#endif // use_mysql
        }

        if ( Utils::contains( storage, "sqlite" ) )
        {
#ifdef use_sqlite
            Utils::save_stats_to_sqlite( "" );
#endif // use_sqlite
        }

        if ( Utils::contains( storage, "files" ) )
        {
            Utils::save_stats_to_files( "" );
        }

        string page;
        page += "<!doctype html>\n";
        page += "<html>\n";
        page += "<head>\n";
        page += "<title>data save</title>";
        page += "<style>\n";
        page += "p {color:black; font-family: arial}\n";
        page += "</style>\n";
        page += "</head>\n";
        page += "<body background=\"../background.png\">\n";
        page += "<div style=\"width: 100%; text-align:center\" align=\"center\">\n";
        page += "<p>all data saved</p>\n";
        page += "</div>";
        page += "</body>\n";
        page += "</html>";

        content_stream = Utils::gz_compress(page);

        content_stream.seekp( 0, ios::end );
        *response <<  "HTTP/1.1 200 OK\r\nContent-Length: " << content_stream.tellp() << "\r\n";
        *response << "Content-Encoding: gzip\r\n";
        *response << "Content-Type: text/html; charset=utf-8" << "\r\n";
        *response << "Cache-Control: no-cache, no-store, public";
        *response << "\r\n\r\n" << content_stream.rdbuf();
    };

    server.resource["^/stop/?$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
    {
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
        page += "<body background=\"../background.png\">\n";
        page += "<div style=\"width: 100%; text-align:center\" align=\"center\">\n";
        page += "<p>\n";
        page += "Smart Traffic Meter will terminate soon";
        page += "</p>\n";
        page += "</div>\n";
        page += "</body>\n";
        page += "</html>\n";

        stringstream content_stream;
        content_stream = Utils::gz_compress(page);

        content_stream.seekp( 0, ios::end );
        *response <<  "HTTP/1.1 200 OK\r\nContent-Length: " << content_stream.tellp() << "\r\n";
        *response << "Content-Encoding: gzip\r\n";
        *response << "Content-Type: text/html; charset=utf-8" << "\r\n";
        *response << "Cache-Control: no-cache, no-store, public";
        *response << "\r\n\r\n" << content_stream.rdbuf();

        Globals::terminate_program = true;
    };

    server.resource["^/backup/?$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
    {
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
        page += "<body background=\"../background.png\">\n";
        page += "<div style=\"width: 100%; text-align:center\" align=\"center\">\n";
        page += "<p>\n";
        page += "<a href=\"/grovestreamsupload/\">Upload all stats to grovestreams</a>\n";
        page += "</p>\n";
        page += "<br><br>\n";
        page += "<p>\n<a href=\"/\">Home</a></p>\n";
        page += "</div>\n";
        page += "</body>\n";
        page += "</html>\n";

        stringstream content_stream;
        content_stream = Utils::gz_compress(page);

        content_stream.seekp( 0, ios::end );
        *response <<  "HTTP/1.1 200 OK\r\nContent-Length: " << content_stream.tellp() << "\r\n";
        *response << "Content-Encoding: gzip\r\n";
        *response << "Content-Type: text/html; charset=utf-8" << "\r\n";
        *response << "Cache-Control: no-cache, no-store, public";
        *response << "\r\n\r\n" << content_stream.rdbuf();
    };

    server.resource["^/grovestreamsupload/?$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
    {
    	GroveStreamsUploader::upload_all();

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
        page += "<body background=\"../background.png\">\n";
        page += "<div style=\"width: 100%; text-align:center\" align=\"center\">\n";
        page += "<p>\n";
        page += "All stats have been uploaded to grovestreams";
        page += "</p>\n";
        page += "<br><br>\n";
        page += "<p>\n<a href=\"/\">Home</a></p>\n";
        page += "</div>\n";
        page += "</body>\n";
        page += "</html>\n";

        stringstream content_stream;
        content_stream = Utils::gz_compress(page);

        content_stream.seekp( 0, ios::end );
        *response <<  "HTTP/1.1 200 OK\r\nContent-Length: " << content_stream.tellp() << "\r\n";
        *response << "Content-Encoding: gzip\r\n";
        *response << "Content-Type: text/html; charset=utf-8" << "\r\n";
        *response << "Cache-Control: no-cache, no-store, public";
        *response << "\r\n\r\n" << content_stream.rdbuf();
    };

    server.resource["^/settings/?$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
    {
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
        page += "<body background=\"../background.png\">\n";

        page += "<h1>Settings</h1>\n";
        page += "<br><br>\n";

        page += "<form action=\"submitsettings\" method=\"post\">\n";
        page += "<table border=\"0\" cellspacing=\"30\" cellpadding=\"10\" align=\"center\">\n";
        page += "<tbody>\n";
        page += "<tr>\n";
        page += "<td align=\"left\" valign=\"middle\"><p>storage</p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><input type=\"text\" style=\"font-size: 12pt\" name=\"storage\" value=\"" + Settings::settings["storage"] + "\"></td>\n";
        page += "<td align=\"center\" valign=\"middle\"></td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td align=\"left\" valign=\"middle\"><p>database directory</p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><input type=\"text\" style=\"font-size: 12pt\" name=\"database_directory\" value=\"" + Settings::settings["database directory"] + "\"></td>\n";
        page += "<td align=\"center\" valign=\"middle\"></td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td align=\"left\" valign=\"middle\"><p>stats refresh interval in seconds</p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><input type=\"text\" style=\"font-size: 12pt\" name=\"stats_refresh_interval\" value=\"" + Settings::settings["stats refresh interval"] + "\"></td>\n";
        page += "<td align=\"center\" valign=\"middle\"></td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td align=\"left\" valign=\"middle\"><p>stats save interval in seconds</p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><input type=\"text\" style=\"font-size: 12pt\" name=\"stats_save_interval\" value=\"" + Settings::settings["stats save interval"] + "\"></td>\n";
        page += "<td align=\"center\" valign=\"middle\"></td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td align=\"left\" valign=\"middle\"><p>web server port</p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><input type=\"text\" style=\"font-size: 12pt\" name=\"web_server_port\" value=\"" + Settings::settings["web server port"] + "\"></td>\n";
        page += "<td align=\"center\" valign=\"middle\"></td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td align=\"left\" valign=\"middle\"><p>grovestreams api key</p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><input type=\"text\" style=\"font-size: 12pt\" name=\"grovestreams_api_key\" value=\"" + Settings::settings["grovestreams api key"] + "\"></td>\n";
        page += "<td align=\"center\" valign=\"middle\"></td>\n";
        page += "</tr>\n";
        page += "<td align=\"left\" valign=\"middle\"><p>grovestreams update interval</p></td>\n";
        page += "<td align=\"center\" valign=\"middle\"><input type=\"text\" style=\"font-size: 12pt\" name=\"grovestreams_update_interval\" value=\"" + Settings::settings["grovestreams update interval"] + "\"></td>\n";
        page += "<td align=\"center\" valign=\"middle\"></td>\n";
        page += "</tr>\n";

        page += "</tbody>\n";
        page += "</table>\n";
        page += "<div style=\"text-align:center\" align=\"center\">\n";
        page += "<input type=\"submit\" style=\"font-size: 12pt\" value=\"Submit\">\n";
        page += "</div>\n";
        page += "</form>\n";
        page += "<br>\n";
        page += "<div style=\"text-align:center\" align=\"center\">\n";
        page += "<p>\n<a href=\"/\">Home</a></p>\n";
        page += "</div>\n";

        page += "</body>\n";
        page += "</html>\n";

        stringstream content_stream;
        content_stream = Utils::gz_compress(page);

        content_stream.seekp( 0, ios::end );
        *response << "HTTP/1.1 200 OK\r\n";
        *response << "Content-Length: " << content_stream.tellp() << "\r\n";
        *response << "Content-Encoding: gzip\r\n";
        *response << "Content-Type: text/html; charset=utf-8" << "\r\n";
        *response << "Cache-Control: no-cache, no-store, public";
        *response << "\r\n\r\n" << content_stream.rdbuf();
    };


    server.resource["^/submitsettings/?$"]["POST"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
    {
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
        page += "<body background=\"../background.png\">\n";

        page += "<h1>Settings</h1>\n";
        page += "<br><br>\n";

        auto content = request->content.string();

        //parse parameters string
        vector<string> content_items = Utils::split( content, "&" );

        for ( auto const & content_item : content_items )
        {
            vector<string> content_value = Utils::split( content_item, "=" );

            if ( content_value[0].size() > 0 && content_value[1].size() > 0 )
            {
                //page+=content_value[0]+" is "+content_value[1]+"<br>\n";
                string& key = content_value[0];
                string& value = content_value[1];

                key = Utils::replace( "_", " ", key );
                value = url::Url::unescape( value );
                Settings::settings[key] = value;
            }
        }

        bool result = Utils::save_settings();

        page += "<div style=\"text-align:center\" align=\"center\">\n";

        if ( result == true )
        {
            page += "<p>All settings saved successfully.</p>\n";
            page += "<p>The application must be restarted for the changes to take effect.</p>\n";
        }
        else
        {
            page += "<p>Error while saving settings!</p><br>\n";
        }

        page += "<br><br>\n";
        page += "<p>\n<a href=\"/\">Home</a></p>\n";
        page += "</div>\n";

        page += "</body>\n";
        page += "</html>\n";

        stringstream content_stream;
        content_stream = Utils::gz_compress(page);

        content_stream.seekp( 0, ios::end );
        *response << "HTTP/1.1 200 OK\r\n";
        *response << "Content-Length: " << content_stream.tellp() << "\r\n";
        *response << "Content-Encoding: gzip\r\n";
        *response << "Content-Type: text/html; charset=utf-8" << "\r\n";
        *response << "Cache-Control: no-cache, no-store, public";
        *response << "\r\n\r\n" << content_stream.rdbuf();
    };


    server.resource["^/customtimespan/?$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
    {
        uint32_t y;
        uint32_t m;
        uint32_t d;
        uint32_t h;
        string page;

        Utils::get_time( &y, &m, &d, &h );

        page += "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n";
        page += "<html>\n";
        page += "<head>\n";
        page += "<meta content=\"text/html; charset=UTF-8\" http-equiv=\"content-type\">\n";
        page += "<title>Select time span</title>";
        page += "<style type=\"text/css\">\n";
        page += "h1 {\n";
        page += "font-size: xx-large;\n";
        page += "font-style: normal;\n";
        page += "text-align: center;\n";
        page += "font-family: Arial, Helvetica, sans-serif;\n";
        page += "font-weight: bold;\n";
        page += "text-transform: none;\n";
        page += "}\n";
        page += "h2 {\n";
        page += "font-size: large;\n";
        page += "font-style: normal;\n";
        page += "text-align: center;\n";
        page += "font-family: Arial, Helvetica, sans-serif;\n";
        page += "font-weight: bold;\n";
        page += "text-transform: none;\n";
        page += "}\n";
        page += "p {\n";
        page += "font-style: normal;\n";
        page += "font-family: Arial, Helvetica, sans-serif;\n";
        page += "text-transform: none;\n";
        page += "font-size: large;\n";
        page += "font-weight: normal;\n";
        page += "color: black;\n";
        page += "}\n";
        page += "li {\n";
        page += "font-style: normal;\n";
        page += "font-family: Arial, Helvetica, sans-serif;\n";
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
        page += "select {\n";
        page += "float: left;\n";
        page += "margin-right: 5px;\n";
        page += "}\n";
        page += "</style>\n";
        page += "<script src=\"/jquery.js\" type=\"text/javascript\"></script>\n";
        page += "<script src=\"/jscolor.js\" type=\"text/javascript\"></script>\n";
        page += "<script src=\"/custom_time_span.js\" type=\"text/javascript\"></script>\n";
        page += "</head>\n";
        page += "<body background=\"../background.png\" onload=\"Settings()\">\n";
        page += "<h1>custom chart options</h1>\n";
        page += "<script type=\"application/javascript\">\n";
        page += "</script>\n";

        page += "<table style=\"margin-left: auto; margin-right: auto;\" cellspacing=\"1\" cellpadding=\"1\">\n";
        page += "<tbody>\n";
        page += "<tr>\n";
        page += "<td>\n";
        page += "<li>select stats type</li>\n";
        page += "<br>\n";
        page += "<form class=\"statstype\">\n";
        page += "<input type=\"radio\" name=\"statstype\" value=\"hourly\" checked> hourly stats\n";
        page += "<input type=\"radio\" name=\"statstype\" value=\"daily\"> daily stats\n";
        page += "<input type=\"radio\" name=\"statstype\" value=\"monthly\"> monthly stats\n";
        page += "<input type=\"radio\" name=\"statstype\" value=\"yearly\"> yearly stats\n";
        page += "</form>\n";
        page += "<br><br>\n";
        page += "</td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td>\n";
        page += "<li>select time span</li>\n";
        page += "<br> start date\n";
        page += "<div>\n";
        page += "<select class=\"start_year\">\n";
        page += "<option selected=\"selected\" value=\"start_year\">Year</option>\n";

        for ( uint32_t i = y; i > ( y - 6 ); i-- )
        {
            page += "<option value=\"" + Utils::to_string( i ) + "\">" + Utils::to_string( i ) + "</option>\n";
        }

        page += "</select>\n";
        page += "</div>\n";
        page += "<div>\n";
        page += "<select class=\"start_month\">\n";
        page += "<option selected=\"selected\" value=\"start_month\">Month</option>\n";
        page += "<option value=\"01\">January</option>\n";
        page += "<option value=\"02\">February</option>\n";
        page += "<option value=\"03\">March</option>\n";
        page += "<option value=\"04\">April</option>\n";
        page += "<option value=\"05\">May</option>\n";
        page += "<option value=\"06\">June</option>\n";
        page += "<option value=\"07\">July</option>\n";
        page += "<option value=\"08\">August</option>\n";
        page += "<option value=\"09\">September</option>\n";
        page += "<option value=\"10\">October</option>\n";
        page += "<option value=\"11\">November</option>\n";
        page += "<option value=\"12\">December</option>\n";
        page += "</select>\n";
        page += "</div>\n";
        page += "<div>\n";
        page += "<select class=\"start_day\">\n";
        page += "<option selected=\"selected\" value=\"start_day\">Day</option>\n";
        page += "<option value=\"01\">1</option>\n";
        page += "<option value=\"02\">2</option>\n";
        page += "<option value=\"03\">3</option>\n";
        page += "<option value=\"04\">4</option>\n";
        page += "<option value=\"05\">5</option>\n";
        page += "<option value=\"06\">6</option>\n";
        page += "<option value=\"07\">7</option>\n";
        page += "<option value=\"08\">8</option>\n";
        page += "<option value=\"09\">9</option>\n";
        page += "<option value=\"10\">10</option>\n";
        page += "<option value=\"11\">11</option>\n";
        page += "<option value=\"12\">12</option>\n";
        page += "<option value=\"13\">13</option>\n";
        page += "<option value=\"14\">14</option>\n";
        page += "<option value=\"15\">15</option>\n";
        page += "<option value=\"16\">16</option>\n";
        page += "<option value=\"17\">17</option>\n";
        page += "<option value=\"18\">18</option>\n";
        page += "<option value=\"19\">19</option>\n";
        page += "<option value=\"20\">20</option>\n";
        page += "<option value=\"21\">21</option>\n";
        page += "<option value=\"22\">22</option>\n";
        page += "<option value=\"23\">23</option>\n";
        page += "<option value=\"24\">24</option>\n";
        page += "<option value=\"25\">25</option>\n";
        page += "<option value=\"26\">26</option>\n";
        page += "<option value=\"27\">27</option>\n";
        page += "<option value=\"28\">28</option>\n";
        page += "<option value=\"29\">29</option>\n";
        page += "<option value=\"30\">30</option>\n";
        page += "<option value=\"31\">31</option>\n";
        page += "</select>\n";
        page += "</div>\n";
        page += "<div>\n";
        page += "<select class=\"start_hour\">\n";
        page += "<option selected=\"selected\" value=\"start_hour\">Hour</option>\n";
        page += "<option value=\"0\">0</option>\n";
        page += "<option value=\"01\">1</option>\n";
        page += "<option value=\"02\">2</option>\n";
        page += "<option value=\"03\">3</option>\n";
        page += "<option value=\"04\">4</option>\n";
        page += "<option value=\"05\">5</option>\n";
        page += "<option value=\"06\">6</option>\n";
        page += "<option value=\"07\">7</option>\n";
        page += "<option value=\"08\">8</option>\n";
        page += "<option value=\"09\">9</option>\n";
        page += "<option value=\"10\">10</option>\n";
        page += "<option value=\"11\">11</option>\n";
        page += "<option value=\"12\">12</option>\n";
        page += "<option value=\"13\">13</option>\n";
        page += "<option value=\"14\">14</option>\n";
        page += "<option value=\"15\">15</option>\n";
        page += "<option value=\"16\">16</option>\n";
        page += "<option value=\"17\">17</option>\n";
        page += "<option value=\"18\">18</option>\n";
        page += "<option value=\"19\">19</option>\n";
        page += "<option value=\"20\">20</option>\n";
        page += "<option value=\"21\">21</option>\n";
        page += "<option value=\"22\">22</option>\n";
        page += "<option value=\"23\">23</option>\n";
        page += "</select>\n";
        page += "</div>\n";
        page += "<br><br> end date\n";
        page += "<div>\n";
        page += "<select class=\"end_year\">\n";
        page += "<option selected=\"selected\" value=\"end_year\">Year</option>\n";

        for ( uint32_t i = y; i > ( y - 6 ); i-- )
        {
            page += "<option value=\"" + Utils::to_string( i ) + "\">" + Utils::to_string( i ) + "</option>\n";
        }

        page += "</select>\n";
        page += "</div>\n";
        page += "<div>\n";
        page += "<select class=\"end_month\">\n";
        page += "<option selected=\"selected\" value=\"end_month\">Month</option>\n";
        page += "<option value=\"01\">January</option>\n";
        page += "<option value=\"02\">February</option>\n";
        page += "<option value=\"03\">March</option>\n";
        page += "<option value=\"04\">April</option>\n";
        page += "<option value=\"05\">May</option>\n";
        page += "<option value=\"06\">June</option>\n";
        page += "<option value=\"07\">July</option>\n";
        page += "<option value=\"08\">August</option>\n";
        page += "<option value=\"09\">September</option>\n";
        page += "<option value=\"10\">October</option>\n";
        page += "<option value=\"11\">November</option>\n";
        page += "<option value=\"12\">December</option>\n";
        page += "</select>\n";
        page += "</div>\n";
        page += "<div>\n";
        page += "<select class=\"end_day\">\n";
        page += "<option selected=\"selected\" value=\"end_day\">Day</option>\n";
        page += "<option value=\"01\">1</option>\n";
        page += "<option value=\"02\">2</option>\n";
        page += "<option value=\"03\">3</option>\n";
        page += "<option value=\"04\">4</option>\n";
        page += "<option value=\"05\">5</option>\n";
        page += "<option value=\"06\">6</option>\n";
        page += "<option value=\"07\">7</option>\n";
        page += "<option value=\"08\">8</option>\n";
        page += "<option value=\"09\">9</option>\n";
        page += "<option value=\"10\">10</option>\n";
        page += "<option value=\"11\">11</option>\n";
        page += "<option value=\"12\">12</option>\n";
        page += "<option value=\"13\">13</option>\n";
        page += "<option value=\"14\">14</option>\n";
        page += "<option value=\"15\">15</option>\n";
        page += "<option value=\"16\">16</option>\n";
        page += "<option value=\"17\">17</option>\n";
        page += "<option value=\"18\">18</option>\n";
        page += "<option value=\"19\">19</option>\n";
        page += "<option value=\"20\">20</option>\n";
        page += "<option value=\"21\">21</option>\n";
        page += "<option value=\"22\">22</option>\n";
        page += "<option value=\"23\">23</option>\n";
        page += "<option value=\"24\">24</option>\n";
        page += "<option value=\"25\">25</option>\n";
        page += "<option value=\"26\">26</option>\n";
        page += "<option value=\"27\">27</option>\n";
        page += "<option value=\"28\">28</option>\n";
        page += "<option value=\"29\">29</option>\n";
        page += "<option value=\"30\">30</option>\n";
        page += "<option value=\"31\">31</option>\n";
        page += "</select>\n";
        page += "</div>\n";
        page += "<div>\n";
        page += "<select class=\"end_hour\">\n";
        page += "<option selected=\"selected\" value=\"end_hour\">Hour</option>\n";
        page += "<option value=\"0\">0</option>\n";
        page += "<option value=\"01\">1</option>\n";
        page += "<option value=\"02\">2</option>\n";
        page += "<option value=\"03\">3</option>\n";
        page += "<option value=\"04\">4</option>\n";
        page += "<option value=\"05\">5</option>\n";
        page += "<option value=\"06\">6</option>\n";
        page += "<option value=\"07\">7</option>\n";
        page += "<option value=\"08\">8</option>\n";
        page += "<option value=\"09\">9</option>\n";
        page += "<option value=\"10\">10</option>\n";
        page += "<option value=\"11\">11</option>\n";
        page += "<option value=\"12\">12</option>\n";
        page += "<option value=\"13\">13</option>\n";
        page += "<option value=\"14\">14</option>\n";
        page += "<option value=\"15\">15</option>\n";
        page += "<option value=\"16\">16</option>\n";
        page += "<option value=\"17\">17</option>\n";
        page += "<option value=\"18\">18</option>\n";
        page += "<option value=\"19\">19</option>\n";
        page += "<option value=\"20\">20</option>\n";
        page += "<option value=\"21\">21</option>\n";
        page += "<option value=\"22\">22</option>\n";
        page += "<option value=\"23\">23</option>\n";
        page += "</select>\n";
        page += "</div>\n";
        page += "<br><br>\n";
        page += "</td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td>\n";
        page += "<li>pick chart colours</li>\n";
        page += "<br> upload color <input name=\"up_color\" id=\"up_color\" value=\"ff0000\" class=\"jscolor {width:243, height:200, position:'right',\n";
        page += "borderColor:'#FFF', insetColor:'#FFF', backgroundColor:'#666'}\">\n";
        page += "<br> download color <input name=\"down_color\" id=\"down_color\" value=\"00ff00\" class=\"jscolor {width:243, height:200, position:'right',\n";
        page += "borderColor:'#FFF', insetColor:'#FFF', backgroundColor:'#666'}\">\n";
        page += "<br><br>\n";
        page += "</td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td>\n";
        page += "<li>select chart type</li>\n";
        page += "<br>\n";
        page += "<form id=\"charttype\">\n";
        page += "<input type=\"radio\" name=\"charttype\" value=\"bar\" checked> bar chart<br>\n";
        page += "<input type=\"radio\" name=\"charttype\" value=\"line\"> line chart<br>\n";
        page += "</form>\n";
        page += "<br><br>\n";
        page += "</td>\n";
        page += "</tr>\n";
        page += "<tr>\n";
        page += "<td style='text-align: center;'>\n";
        page += "<input id=\"submit_button\" type=\"button\" value=\"Show stats\" />\n";
        page += "</td>\n";
        page += "</tr>\n";
        page += "</tbody>\n";
        page += "</table>\n";
        page += "<div style=\"text-align:center\" align=\"center\">\n";
        page += "<p>\n";
        page += "<a href=\"/\">Home</a>\n";
        page += "</p>\n";
        page += "</div>\n";
        page += "</body>\n";
        page += "</html>\n\n";

        stringstream content_stream;
        content_stream = Utils::gz_compress(page);

        content_stream.seekp( 0, ios::end );
        *response << "HTTP/1.1 200 OK\r\n";
        *response << "Content-Length: " << content_stream.tellp() << "\r\n";
        *response << "Content-Encoding: gzip\r\n";
        *response << "Content-Type: text/html; charset=utf-8" << "\r\n";
        *response << "Cache-Control: no-cache, no-store, public";
        *response << "\r\n\r\n" << content_stream.rdbuf();
    };

//    server.resource["\\/test.html$"]["GET"] = [&server]( shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request )
//    {
//        ifstream file;
//        file.open( "../../webpage/test3.html", std::ifstream::in | std::ifstream::binary );
//
//        if ( file.is_open() )
//        {
//            stringstream input_file_stream;
//            input_file_stream << file.rdbuf();
//            file.close();
//
//            input_file_stream.seekp( 0, ios::end );
//            *response <<  "HTTP/1.1 200 OK\r\nContent-Length: " << input_file_stream.tellp() << "\r\n";
//            *response << "Content-Type: text/html; charset=utf-8" << "\r\n";
//            *response << "Cache-Control: no-cache, no-store, public" << "\r\n";
//            *response << "\r\n\r\n" << input_file_stream.rdbuf();
//        }
//    };
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
