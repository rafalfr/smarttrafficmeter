#include <fstream>
#include <memory>
#include <sstream>
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
        string row;
        ifstream file;
        string page;
        stringstream content_stream;
        uint32_t y;
        uint32_t m;
        uint32_t d;
        uint32_t h;
        Utils::get_time( &y, &m, &d, &h );

        file.open( "stats.html", std::ifstream::in | std::ifstream::binary );

        if ( file.is_open() )
        {
            stringstream input_file_stream;
            input_file_stream << file.rdbuf();
            file.close();

            page = input_file_stream.str();

            string tables;

            for ( auto const & mac_speedinfo : Globals::speed_stats )
            {
                const string& mac = mac_speedinfo.first;
                const InterfaceSpeedMeter& ism = mac_speedinfo.second;

                // speed is in bits/s
                uint64_t speed_down =  ism.get_rx_speed();
                uint64_t speed_up = ism.get_tx_speed();

                row.clear();
                row += Utils::to_string( y ) + "-" + Utils::to_string( m ) + "-" + Utils::to_string( d ) + "_" + Utils::to_string( h ) + ":00-" + Utils::to_string( h + 1 ) + ":00";

                uint64_t hourly_down = 0;
                uint64_t hourly_up = 0;

                if ( Globals::all_stats[mac]["hourly"].find( row ) != Globals::all_stats[mac]["hourly"].end() )
                {
                    hourly_down = Globals::all_stats[mac]["hourly"][row].recieved();
                    hourly_up = Globals::all_stats[mac]["hourly"][row].transmited();
                }


                row.clear();
                row += Utils::to_string( y ) + "-" + Utils::to_string( m ) + "-" + Utils::to_string( d );

                uint64_t daily_up = 0;
                uint64_t daily_down = 0;

                if ( Globals::all_stats[mac]["daily"].find( row ) != Globals::all_stats[mac]["daily"].end() )
                {
                    daily_down = Globals::all_stats[mac]["daily"][row].recieved();
                    daily_up = Globals::all_stats[mac]["daily"][row].transmited();
                }

                row.clear();
                row += Utils::to_string( y ) + "-" + Utils::to_string( m );

                uint64_t monthly_up = 0;
                uint64_t monthly_down = 0;

                if ( Globals::all_stats[mac]["monthly"].find( row ) != Globals::all_stats[mac]["monthly"].end() )
                {
                    monthly_down = Globals::all_stats[mac]["monthly"][row].recieved();
                    monthly_up = Globals::all_stats[mac]["monthly"][row].transmited();
                }

                row.clear();
                row += Utils::to_string( y );

                uint64_t yearly_up = 0;
                uint64_t yearly_down = 0;

                if ( Globals::all_stats[mac]["yearly"].find( row ) != Globals::all_stats[mac]["yearly"].end() )
                {
                    yearly_down = Globals::all_stats[mac]["yearly"][row].recieved();
                    yearly_up = Globals::all_stats[mac]["yearly"][row].transmited();
                }


                tables += "<table class=\"stats_table\" >";
                tables += "<tbody>";
                tables += "<tr>";
                tables += "<td style=\"text-align: center;\" colspan=\"3\"><strong>" + mac + "<br /></strong></td>";
                tables += "</tr>";
                tables += "<tr>";
                tables += "<td rowspan=\"2\"><strong>speed</strong></td>";
                tables += "<td><span style=\"color: #800000;\"><strong>up</strong></span></td>";
                tables += "<td>";
                tables += "<strong><span style=\"color: #800000;\"><div id=\"" + mac + "_speed_up\">0</div></span></strong>";
                tables += "</td>";
                tables += "</tr>";
                tables += "<tr>";
                tables += "<td><span style=\"color: #008000;\"><strong>down</strong></span></td>";
                tables += "<td>";
                tables += "<strong><span style=\"color: #008000;\"><div id=\"" + mac + "_speed_down\">0</div></span></strong>";
                tables += "</td>";
                tables += "</tr>";
                tables += "<tr style=\"background-color: #eeeeee;\">";
                tables += "<td rowspan=\"2\"><strong>hour</strong></td>";
                tables += "<td><span style=\"color: #800000;\"><strong>up</strong></span></td>";
                tables += "<td>";
                tables += "<strong><span style=\"color: #800000;\"><div id=\"" + mac + "_hourly_up\">1</div></span></strong>";
                tables += "</td>";
                tables += "</tr>";
                tables += "<tr style=\"background-color: #eeeeee;\">";
                tables += "<td><span style=\"color: #008000;\"><strong>down</strong></span></td>";
                tables += "<td>";
                tables += "<span style=\"color: #008000;\"><strong><div id=\"" + mac + "_hourly_down\">2</div></strong></span>";
                tables += "</td>";
                tables += "</tr>";
                tables += "<tr>";
                tables += "<td rowspan=\"2\"><strong>day</strong></td>";
                tables += "<td><strong><span style=\"color: #800000;\">up</span></strong></td>";
                tables += "<td>";
                tables += "<strong><span style=\"color: #800000;\"><div id=\"" + mac + "_daily_up\">3</div></span></strong>";
                tables += "</td>";
                tables += "</tr>";
                tables += "<tr>";
                tables += "<td><strong><span style=\"color: #008000;\">down</span></strong></td>";
                tables += "<td>";
                tables += "<strong><span style=\"color: #008000;\"><div id=\"" + mac + "_daily_down\">4</div></span></strong>";
                tables += "</td>";
                tables += "</tr>";
                tables += "<tr style=\"background-color: #eeeeee;\">";
                tables += "<td rowspan=\"2\"><strong>month</strong></td>";
                tables += "<td><strong><span style=\"color: #800000;\">up</span></strong></td>";
                tables += "<td>";
                tables += "<strong><span style = \"color: #800000;\"><div id=\"" + mac + "_monthly_up\">5</div></span></strong>";
                tables += "</td>";
                tables += "</tr>";
                tables += "<tr style=\"background-color: #eeeeee;\">";
                tables += "<td><strong><span style=\"color: #008000;\">down</span></strong></td>";
                tables += "<td><strong><span style=\"color: #008000;\"><div id=\"" + mac + "_monthly_down\">6</div></span></strong>";
                tables += "</td>";
                tables += "</tr>";
                tables += "<tr>";
                tables += "<td rowspan=\"2\"><strong>year</strong></td>";
                tables += "<td><strong><span style=\"color: #800000;\">up</span></strong></td>";
                tables += "<td>";
                tables += "<strong><span style=\"color: #800000;\"><div id=\"" + mac + "_yearly_up\">7</div></span></strong>";
                tables += "</td>";
                tables += "</tr>";
                tables += "<tr>";
                tables += "<td><strong><span style=\"color: #008000;\">down</span></strong></td>";
                tables += "<td>";
                tables += "<strong><span style=\"color: #008000;\"><div id=\"" + mac + "_yearly_down\">8</div></span></strong>";
                tables += "</td>";
                tables += "</tr>";
                tables += "</tbody>";
                tables += "</table>";
                tables += "<p>&nbsp;</p>";
            }

            page += tables;
            content_stream << page;
        }
        else
        {
            content_stream << "<h1>Request from " << request->remote_endpoint_address << " (" << request->remote_endpoint_port << ")</h1>";
            content_stream << request->method << " " << request->path << " HTTP/" << request->http_version << "<br>";

            for ( auto& header : request->header )
            {
                content_stream << header.first << ": " << header.second << "<br>";
            }
        }

        content_stream.seekp( 0, ios::end );

        response <<  "HTTP/1.1 200 OK\r\nContent-Length: " << content_stream.tellp() << "\r\n\r\n" << content_stream.rdbuf();
    };


    server.resource["^\\\/daily\\\/([0-9]{4}-[0-9]{1,2}-[0-9]{1,2})\\\/([0-9]{4}-[0-9]{1,2}-[0-9]{1,2})\\\/?([[:xdigit:]]{6})?\\\/?([[:xdigit:]]{6})?\\\/?(bar|line|radar|polarArea|pie)?\\\/?$"]["GET"] = []( SimpleWeb::Server<SimpleWeb::HTTP>::Response & response, shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request )
    {
        // http://127.0.0.1:8080/daily/2015-04-04/2016-06-04

        uint32_t y;
        uint32_t m;
        uint32_t d;
        uint32_t h;

        string web_page;

        //get parameters
        string start_date_str = request->path_match[1];
        string end_date_str = request->path_match[2];
        string up_color_str = request->path_match[3];
        string down_color_str = request->path_match[4];
        string chart_type_str = request->path_match[5];

        // set default values
        if ( up_color_str.compare( "" ) == 0 )
        {
            up_color_str = "ffbb00";
        }

        if ( down_color_str.compare( "" ) == 0 )
        {
            down_color_str = "0055ff";
        }

        if ( chart_type_str.compare( "" ) == 0 )
        {
            chart_type_str = "bar";
        }

        Utils::get_time( &y, &m, &d, &h );

        string current_time_str = Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 );

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
        web_page += "<table style=\"text-align: center; width: 1028px; margin-left: auto; margin-right: auto;\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n";
        web_page += "<tbody>\n";
        web_page += "<tr>\n";
        web_page += "<td style=\"vertical-align: top; text-align: center; width: 700px;\">\n";


        uint32_t chart_id = 0;

        // create chart for each network interface
        for ( auto const & mac_table : Globals::all_stats )
        {
            const string& mac = mac_table.first;

            string interface_name = "n/a";
            string ip4 = "n/a";
            string ip6 = "n/a";

            for ( auto const & name_info : Globals::interfaces )
            {
                const InterfaceInfo& interface_info = name_info.second;

                if ( interface_info.get_mac().compare( mac ) == 0 )
                {
                    interface_name = interface_info.get_name();
                    ip4 = interface_info.get_ip4();
                    ip6 = interface_info.get_ip6();
                }
            }

            string canvas_id = "canvas";
            canvas_id += Utils::to_string( chart_id );

            web_page += "<div title=\"interface name, mac, IP4, IP6\" style=\"width: 100%\" align=\"center\">\n";
            web_page += "<p>" + interface_name + ", " + mac + ", " + ip4 + ", " + ip6;
            web_page += "</p></div>";

            web_page += "<div style=\"width: 100%; text-align:center\" align=\"center\">\n";
            web_page += "<canvas id=\"" + canvas_id + "\" height=\"450\" width=\"600\"></canvas>\n";
            web_page += "</div>\n";

            const vector<string>& start_date_items = Utils::split( start_date_str, "-" );
            const vector<string>& end_date_items = Utils::split( end_date_str, "-" );

            struct date start_date;
            struct date end_date;

            start_date.year = stoi( start_date_items[0], nullptr, 10 );
            start_date.month = stoi( start_date_items[1], nullptr, 10 );
            start_date.day = stoi( start_date_items[2], nullptr, 10 );
            start_date.hour = 0;

            end_date.year = stoi( end_date_items[0], nullptr, 10 );
            end_date.month = stoi( end_date_items[1], nullptr, 10 );
            end_date.day = stoi( end_date_items[2], nullptr, 10 );
            end_date.hour = 0;


            map<string, InterfaceStats> results = Globals::db_drv.get_stats( mac, "daily", start_date, end_date );

            //update results for the current time
            for ( auto & row_stats : results )
            {
                if ( row_stats.first.compare( current_time_str ) == 0 )
                {
                    uint64_t rx = Globals::all_stats[mac]["daily"][current_time_str].recieved();
                    uint64_t tx = Globals::all_stats[mac]["daily"][current_time_str].transmited();
                    row_stats.second.set_current_stats( tx, rx );
                }
            }

            uint64_t max_value = 0ULL;

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
            }

            string unit;
            float scale;

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


            string var_chart_data = "ChartData";
            var_chart_data += Utils::to_string( chart_id );

            string chart_data;
            chart_data += "var " + var_chart_data + " = {\nlabels : [\n";

            uint32_t i = 0;

            for ( auto const & row_stats : results )
            {
                chart_data += "\"";
                chart_data += row_stats.first;
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

                float value = ( ( float )stats.recieved() ) / scale;

                chart_data += std::to_string( value );

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

                float value = ( ( float )stats.transmited() ) / scale;

                chart_data += std::to_string( value );

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
        }

        chart_id = 0;
        web_page += "<script>\n";
        web_page += "window.onload = function(){\n";

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

        web_page += "}\n";
        web_page += "</script>\n";

        web_page += "</td>\n";
        web_page += "</tr>\n";
        web_page += "</tbody>\n";
        web_page += "</table>\n";
        web_page += "</body>\n";
        web_page += "</html>";

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
        ifstream file;
        file.open( "../../webpage/Chart.js", std::ifstream::in | std::ifstream::binary );

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
            root[mac]["speed"]["down"] = Json::Value::UInt64( ism.get_rx_speed() );
            root[mac]["speed"]["up"] = Json::Value::UInt64( ism.get_tx_speed() );

            row.clear();
            row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 ) + "_" + Utils::to_string( h, 2 ) + ":00-" + Utils::to_string( h + 1, 2 ) + ":00";

            if ( Globals::all_stats[mac]["hourly"].find( row ) != Globals::all_stats[mac]["hourly"].end() )
            {
                root[mac]["hourly"]["down"] = Json::Value::UInt64( Globals::all_stats[mac]["hourly"][row].recieved() );
                root[mac]["hourly"]["up"] = Json::Value::UInt64( Globals::all_stats[mac]["hourly"][row].transmited() );
            }
            else
            {
                root[mac]["hourly"]["down"] = Json::Value::UInt64( 0ULL );
                root[mac]["hourly"]["up"] = Json::Value::UInt64( 0ULL );
            }

            row.clear();
            row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 );

            if ( Globals::all_stats[mac]["daily"].find( row ) != Globals::all_stats[mac]["daily"].end() )
            {
                root[mac]["daily"]["down"] = Json::Value::UInt64( Globals::all_stats[mac]["daily"][row].recieved() );
                root[mac]["daily"]["up"] = Json::Value::UInt64( Globals::all_stats[mac]["daily"][row].transmited() );
            }
            else
            {
                root[mac]["daily"]["down"] = Json::Value::UInt64( 0ULL );
                root[mac]["daily"]["up"] = Json::Value::UInt64( 0ULL );
            }

            row.clear();
            row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 );

            if ( Globals::all_stats[mac]["monthly"].find( row ) != Globals::all_stats[mac]["monthly"].end() )
            {
                root[mac]["monthly"]["down"] = Json::Value::UInt64( Globals::all_stats[mac]["monthly"][row].recieved() );
                root[mac]["monthly"]["up"] = Json::Value::UInt64( Globals::all_stats[mac]["monthly"][row].transmited() );
            }
            else
            {
                root[mac]["monthly"]["down"] = Json::Value::UInt64( 0ULL );
                root[mac]["monthly"]["up"] = Json::Value::UInt64( 0ULL );
            }

            row.clear();
            row += Utils::to_string( y );

            if ( Globals::all_stats[mac]["yearly"].find( row ) != Globals::all_stats[mac]["yearly"].end() )
            {
                root[mac]["yearly"]["down"] = Json::Value::UInt64( Globals::all_stats[mac]["yearly"][row].recieved() );
                root[mac]["yearly"]["up"] = Json::Value::UInt64( Globals::all_stats[mac]["yearly"][row].transmited() );
            }
            else
            {
                root[mac]["yearly"]["down"] = Json::Value::UInt64( 0ULL );
                root[mac]["yearly"]["up"] = Json::Value::UInt64( 0ULL );
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
    out += std::to_string( a );
    out += ")";

    return out;
}




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

