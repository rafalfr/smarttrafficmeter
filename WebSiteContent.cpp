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
void WebSiteContent::set_web_site_content ( SimpleWeb::Server<SimpleWeb::HTTP>& server )
{
	server.default_resource["GET"] = [] ( SimpleWeb::Server<SimpleWeb::HTTP>::Response & response, shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request )
	{
		string row;
		ifstream file;
		string page;
		stringstream content_stream;
		uint32_t y;
		uint32_t m;
		uint32_t d;
		uint32_t h;
		Utils::get_time ( &y, &m, &d, &h );

		file.open ( "stats.html", std::ifstream::in | std::ifstream::binary );

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
				row += Utils::to_string ( y ) + "-" + Utils::to_string ( m ) + "-" + Utils::to_string ( d ) + "_" + Utils::to_string ( h ) + ":00-" + Utils::to_string ( h + 1 ) + ":00";

				uint64_t hourly_down = 0;
				uint64_t hourly_up = 0;

				if ( Globals::all_stats[mac]["hourly"].find ( row ) != Globals::all_stats[mac]["hourly"].end() )
				{
					hourly_down = Globals::all_stats[mac]["hourly"][row].recieved();
					hourly_up = Globals::all_stats[mac]["hourly"][row].transmited();
				}


				row.clear();
				row += Utils::to_string ( y ) + "-" + Utils::to_string ( m ) + "-" + Utils::to_string ( d );

				uint64_t daily_up = 0;
				uint64_t daily_down = 0;

				if ( Globals::all_stats[mac]["daily"].find ( row ) != Globals::all_stats[mac]["daily"].end() )
				{
					daily_down = Globals::all_stats[mac]["daily"][row].recieved();
					daily_up = Globals::all_stats[mac]["daily"][row].transmited();
				}

				row.clear();
				row += Utils::to_string ( y ) + "-" + Utils::to_string ( m );

				uint64_t monthly_up = 0;
				uint64_t monthly_down = 0;

				if ( Globals::all_stats[mac]["monthly"].find ( row ) != Globals::all_stats[mac]["monthly"].end() )
				{
					monthly_down = Globals::all_stats[mac]["monthly"][row].recieved();
					monthly_up = Globals::all_stats[mac]["monthly"][row].transmited();
				}

				row.clear();
				row += Utils::to_string ( y );

				uint64_t yearly_up = 0;
				uint64_t yearly_down = 0;

				if ( Globals::all_stats[mac]["yearly"].find ( row ) != Globals::all_stats[mac]["yearly"].end() )
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

		content_stream.seekp ( 0, ios::end );

		response <<  "HTTP/1.1 200 OK\r\nContent-Length: " << content_stream.tellp() << "\r\n\r\n" << content_stream.rdbuf();
	};


	server.resource["^\\/(hourly|daily|monthly|yearly)\\/?(.*)?\\/?$"]["GET"] = [] ( SimpleWeb::Server<SimpleWeb::HTTP>::Response & response, shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request )
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

		Utils::get_time ( &y, &m, &d, &h );

		string current_time_str;

		// hourly, daily, monthly, yearly
		string stats_type = request->path_match[1];

		current_time_str = Utils::date_str ( stats_type, y, m, d, h );

		//set default optional parameters values
		string start_date_str;

		if ( stats_type.compare ( "yearly" ) == 0 )
		{
			start_date_str = "1900";
		}
		else if ( stats_type.compare ( "monthly" ) == 0 )
		{
			start_date_str = "1900-01";
		}
		else if ( stats_type.compare ( "daily" ) == 0 )
		{
			start_date_str = "1900-01-01";
		}
		else if ( stats_type.compare ( "hourly" ) == 0 )
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
		vector<string> options_items = Utils::split ( options_str, "&" );

		for ( auto const & options_item : options_items )
		{
			vector<string> option_value = Utils::split ( options_item, "=" );

			if ( option_value[0].size() > 0 && option_value[1].size() > 0 )
			{
				if ( regex_match ( option_value[0], regex ( "(start|end|upcolor|downcolor|charttype|chartwidth|chartheight)" ) ) == true )
				{
					options[option_value[0]] = option_value[1];
				}
			}
		}

		//get parameters from the options map
		if ( options.find ( "start" ) != options.end() )
		{
			const string& value = options["start"];

			if ( stats_type.compare ( "yearly" ) == 0 && regex_match ( value, regex ( "^[[:digit:]]{4}$" ) ) == true )
			{
				start_date_str = value;
			}
			else if ( stats_type.compare ( "monthly" ) == 0 && regex_match ( value, regex ( "^[[:digit:]]{4}-[[:digit:]]{2}$" ) ) == true )
			{
				start_date_str = value;
			}
			else if ( stats_type.compare ( "daily" ) == 0 && regex_match ( value, regex ( "^[[:digit:]]{4}-[[:digit:]]{2}-[[:digit:]]{2}$" ) ) == true )
			{
				start_date_str = value;
			}
			else if ( stats_type.compare ( "hourly" ) == 0 && regex_match ( value, regex ( "^[[:digit:]]{4}-[[:digit:]]{2}-[[:digit:]]{2}-[[:digit:]]{2}$" ) ) == true )
			{
				start_date_str = value;
			}
		}

		if ( options.find ( "end" ) != options.end() )
		{
			const string& value = options["end"];

			if ( stats_type.compare ( "yearly" ) == 0 && regex_match ( value, regex ( "^[[:digit:]]{4}$" ) ) == true )
			{
				end_date_str = value;
			}
			else if ( stats_type.compare ( "monthly" ) == 0 && regex_match ( value, regex ( "^[[:digit:]]{4}-[[:digit:]]{2}$" ) ) == true )
			{
				end_date_str = value;
			}
			else if ( stats_type.compare ( "daily" ) == 0 && regex_match ( value, regex ( "^[[:digit:]]{4}-[[:digit:]]{2}-[[:digit:]]{2}$" ) ) == true )
			{
				end_date_str = value;
			}
			else if ( stats_type.compare ( "hourly" ) == 0 && regex_match ( value, regex ( "^[[:digit:]]{4}-[[:digit:]]{2}-[[:digit:]]{2}-[[:digit:]]{2}$" ) ) == true )
			{
				end_date_str = value;
			}
		}

		if ( options.find ( "upcolor" ) != options.end() )
		{
			const string& value = options["upcolor"];

			if ( regex_match ( value, regex ( "^[[:xdigit:]]{6}$" ) ) == true )
			{
				up_color_str = value;
			}
		}

		if ( options.find ( "downcolor" ) != options.end() )
		{
			const string& value = options["downcolor"];

			if ( regex_match ( value, regex ( "^[[:xdigit:]]{6}$" ) ) == true )
			{
				down_color_str = value;
			}
		}

		if ( options.find ( "charttype" ) != options.end() )
		{
			const string& value = options["charttype"];

			if ( regex_match ( value, regex ( "^(bar|line)$" ) ) == true )
			{
				chart_type_str = value;
			}
		}

		if ( options.find ( "chartwidth" ) != options.end() )
		{
			const string& value = options["chartwidth"];

			if ( regex_match ( value, regex ( "^[[:digit:]]{1,4}$" ) ) == true )
			{
				chartwidth_str = value;
			}
		}

		if ( options.find ( "chartheight" ) != options.end() )
		{
			const string& value = options["chartheight"];

			if ( regex_match ( value, regex ( "^[[:digit:]]{1,4}$" ) ) == true )
			{
				chartheight_str = value;
			}
		}

		// create chart colors
		string down_fill_color = rgba_color ( down_color_str, 0.5f );
		string down_stroke_color = rgba_color ( down_color_str, 0.8f );
		string down_highlightfill_color = rgba_color ( down_color_str, 0.75f );
		string down_highlightstroke_color = rgba_color ( down_color_str, 1.0f );

		string up_fill_color = rgba_color ( up_color_str, 0.5f );
		string up_stroke_color = rgba_color ( up_color_str, 0.8f );
		string up_highlightfill_color = rgba_color ( up_color_str, 0.75f );
		string up_highlightstroke_color = rgba_color ( up_color_str, 1.0f );

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

				if ( interface_info.get_mac().compare ( mac ) == 0 )
				{
					interface_name = interface_info.get_name();
					interface_description = interface_info.get_desc();
					ip4 = interface_info.get_ip4();
					ip6 = interface_info.get_ip6();
				}
			}

			string canvas_id = "canvas";
			canvas_id += Utils::to_string ( chart_id );

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

			const vector<string>& start_date_items = Utils::split ( start_date_str, "-" );
			const vector<string>& end_date_items = Utils::split ( end_date_str, "-" );

			struct date start_date;
			struct date end_date;

			if ( stats_type.compare ( "yearly" ) == 0 )
			{
				start_date.year = Utils::stoi ( start_date_items[0] );
				start_date.month = 0;
				start_date.day = 0;
				start_date.hour = 0;

				end_date.year = Utils::stoi ( end_date_items[0] );
				end_date.month = 0;
				end_date.day = 0;
				end_date.hour = 0;
			}
			else if ( stats_type.compare ( "monthly" ) == 0 )
			{
				start_date.year = Utils::stoi ( start_date_items[0] );
				start_date.month = Utils::stoi ( start_date_items[1] );
				start_date.day = 0;
				start_date.hour = 0;

				end_date.year = Utils::stoi ( end_date_items[0] );
				end_date.month = Utils::stoi ( end_date_items[1] );
				end_date.day = 0;
				end_date.hour = 0;
			}
			else if ( stats_type.compare ( "daily" ) == 0 )
			{
				start_date.year = Utils::stoi ( start_date_items[0] );
				start_date.month = Utils::stoi ( start_date_items[1] );
				start_date.day = Utils::stoi ( start_date_items[2] );
				start_date.hour = 0;

				end_date.year = Utils::stoi ( end_date_items[0] );
				end_date.month = Utils::stoi ( end_date_items[1] );
				end_date.day = Utils::stoi ( end_date_items[2] );
				end_date.hour = 0;
			}
			else if ( stats_type.compare ( "hourly" ) == 0 )
			{
				start_date.year = Utils::stoi ( start_date_items[0] );
				start_date.month = Utils::stoi ( start_date_items[1] );
				start_date.day = Utils::stoi ( start_date_items[2] );
				start_date.hour = Utils::stoi ( start_date_items[3] );

				end_date.year = Utils::stoi ( end_date_items[0] );
				end_date.month = Utils::stoi ( end_date_items[1] );
				end_date.day = Utils::stoi ( end_date_items[2] );
				end_date.hour = Utils::stoi ( end_date_items[3] );
			}

			//get stats for the current interface
			map<string, InterfaceStats> results = Globals::db_drv.get_stats ( mac, stats_type, start_date, end_date );

			//update results for the current time
			for ( auto & row_stats : results )
			{
				if ( row_stats.first.compare ( current_time_str ) == 0 )
				{
					uint64_t rx = Globals::all_stats[mac][stats_type][current_time_str].recieved();
					uint64_t tx = Globals::all_stats[mac][stats_type][current_time_str].transmited();
					row_stats.second.set_current_stats ( tx, rx );
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
			var_chart_data += Utils::to_string ( chart_id );

			string chart_data;
			chart_data += "var " + var_chart_data + " = {\nlabels : [\n";

			uint32_t i = 0;

			for ( auto const & row_stats : results )
			{
				chart_data += "\"";
				chart_data += Utils::replace ( "_", ", ", row_stats.first );
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

				chart_data += Utils::float_to_string ( value );

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

				chart_data += Utils::float_to_string ( value );

				if ( i < results.size() - 1 )
				{
					chart_data += ",";
				}
			}

			chart_data += "]\n}\n]\n}";

			string var_options = "options";
			var_options += Utils::to_string ( chart_id );

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
			var_chart += Utils::to_string ( chart_id );

			web_page += "</script>\n";
			web_page += "<br>\n";

			chart_id++;

			web_page += "<div style=\"width: 100%\" align=\"center\">\n";
			float value = ( ( float ) rx_in_period ) / scale;
			web_page += "<p>";
			web_page += "total download: " + Utils::float_to_string ( value ) + " " + unit;
			web_page += ",  ";
			value = ( ( float ) tx_in_period ) / scale;
			web_page += "total upload: " + Utils::float_to_string ( value ) + " " + unit;
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
			canvas_id += Utils::to_string ( chart_id );

			string var_chart_data = "ChartData";
			var_chart_data += Utils::to_string ( chart_id );

			string var_options = "options";
			var_options += Utils::to_string ( chart_id );

			string var_context = "ctx";
			var_context += Utils::to_string ( chart_id );

			string window_chart = "chart";
			window_chart += Utils::to_string ( chart_id );

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

		out_stream.seekp ( 0, ios::end );
		response <<  "HTTP/1.1 200 OK\r\nContent-Length: " << out_stream.tellp() << "\r\n";
		response << "Content-Type: text/html; charset=utf-8" << "\r\n";
		response << "Cache-Control: public, max-age=1800";
		response << "\r\n\r\n" << out_stream.rdbuf();

	};

	server.resource["\\/Chart.js$"]["GET"] = [] ( SimpleWeb::Server<SimpleWeb::HTTP>::Response & response, shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> )
	{
		string chartjs_path;
		chartjs_path += Globals::cwd + PATH_SEPARATOR + "webpage" + PATH_SEPARATOR + "Chart.js";

		ifstream file;
		file.open ( chartjs_path, std::ifstream::in | std::ifstream::binary );

		if ( file.is_open() )
		{
			stringstream input_file_stream;
			input_file_stream << file.rdbuf();
			file.close();

			input_file_stream.seekp ( 0, ios::end );
			response <<  "HTTP/1.1 200 OK\r\nContent-Length: " << input_file_stream.tellp() << "\r\n";
			response << "Content-Type: application/javascript; charset=utf-8" << "\r\n";
			response << "Cache-Control: public, max-age=1800";
			response << "\r\n\r\n" << input_file_stream.rdbuf();
		}
	};

	server.resource["\\/stats.json$"]["GET"] = [] ( SimpleWeb::Server<SimpleWeb::HTTP>::Response & response, shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> )
	{
		string row;
		stringstream content_stream;

		Json::Value root;
		Json::StyledWriter writer;
		uint32_t y;
		uint32_t m;
		uint32_t d;
		uint32_t h;
		Utils::get_time ( &y, &m, &d, &h );

		for ( auto const & mac_speedinfo : Globals::speed_stats )
		{
			const string& mac = mac_speedinfo.first;
			const InterfaceSpeedMeter& ism = mac_speedinfo.second;

			// speed is in bits/s
			root[mac]["speed"]["down"] = Json::Value::UInt64 ( ism.get_rx_speed() );
			root[mac]["speed"]["up"] = Json::Value::UInt64 ( ism.get_tx_speed() );

			row.clear();
			row += Utils::to_string ( y ) + "-" + Utils::to_string ( m, 2 ) + "-" + Utils::to_string ( d, 2 ) + "_" + Utils::to_string ( h, 2 ) + ":00-" + Utils::to_string ( h + 1, 2 ) + ":00";

			if ( Globals::all_stats[mac]["hourly"].find ( row ) != Globals::all_stats[mac]["hourly"].end() )
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
			row += Utils::to_string ( y ) + "-" + Utils::to_string ( m, 2 ) + "-" + Utils::to_string ( d, 2 );

			if ( Globals::all_stats[mac]["daily"].find ( row ) != Globals::all_stats[mac]["daily"].end() )
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
			row += Utils::to_string ( y ) + "-" + Utils::to_string ( m, 2 );

			if ( Globals::all_stats[mac]["monthly"].find ( row ) != Globals::all_stats[mac]["monthly"].end() )
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
			row += Utils::to_string ( y );

			if ( Globals::all_stats[mac]["yearly"].find ( row ) != Globals::all_stats[mac]["yearly"].end() )
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

		string str_response = writer.write ( root );

		content_stream << str_response;

		content_stream.seekp ( 0, ios::end );
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
string WebSiteContent::rgba_color ( string& hex_color, float a )
{
	string out;

	vector<string> rgb = Utils::hexcolor_to_strings ( hex_color );

	out += "rgba(";
	out += rgb[0];
	out += ",";
	out += rgb[1];
	out += ",";
	out += rgb[2];
	out += ",";
	out += Utils::float_to_string ( a );
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

