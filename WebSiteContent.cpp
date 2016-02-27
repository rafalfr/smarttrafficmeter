#include <fstream>
#include <memory>
#include <sstream>
#include "WebSiteContent.h"
#include "json/json.h"
#include "Utils.h"
#include "Settings.h"
#include "InterfaceInfo.h"
#include "InterfaceStats.h"
#include "InterfaceSpeedMeter.h"

#ifdef use_sqlite
#include "sqlite3.h"
#endif // use_sqlite

using namespace std;

extern map<string, map<string, map<string, InterfaceStats> > > all_stats;
extern map<string, InterfaceSpeedMeter> speed_stats;

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

			for ( auto const & mac_speedinfo : speed_stats )
			{
				const string& mac = mac_speedinfo.first;
				const InterfaceSpeedMeter& ism = mac_speedinfo.second;

				// speed is in bits/s
				uint64_t speed_down =  ism.get_rx_speed();
				uint64_t speed_up = ism.get_tx_speed();

				row.clear();
				row += std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d ) + "_" + std::to_string( h ) + ":00-" + std::to_string( h + 1 ) + ":00";

				uint64_t hourly_down = 0;
				uint64_t hourly_up = 0;

				if ( all_stats[mac]["hourly"].find( row ) != all_stats[mac]["hourly"].end() )
				{
					hourly_down = all_stats[mac]["hourly"][row].recieved();
					hourly_up = all_stats[mac]["hourly"][row].transmited();
				}


				row.clear();
				row += std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d );

				uint64_t daily_up = 0;
				uint64_t daily_down = 0;

				if ( all_stats[mac]["daily"].find( row ) != all_stats[mac]["daily"].end() )
				{
					daily_down = all_stats[mac]["daily"][row].recieved();
					daily_up = all_stats[mac]["daily"][row].transmited();
				}

				row.clear();
				row += std::to_string( y ) + "-" + std::to_string( m );

				uint64_t monthly_up = 0;
				uint64_t monthly_down = 0;

				if ( all_stats[mac]["monthly"].find( row ) != all_stats[mac]["monthly"].end() )
				{
					monthly_down = all_stats[mac]["monthly"][row].recieved();
					monthly_up = all_stats[mac]["monthly"][row].transmited();
				}

				row.clear();
				row += std::to_string( y );

				uint64_t yearly_up = 0;
				uint64_t yearly_down = 0;

				if ( all_stats[mac]["yearly"].find( row ) != all_stats[mac]["yearly"].end() )
				{
					yearly_down = all_stats[mac]["yearly"][row].recieved();
					yearly_up = all_stats[mac]["yearly"][row].transmited();
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


	server.resource["^\\/daily\\/([0-9]{4}-[0-9]{1,2}-[0-9]{1,2})\\/([0-9]{4}-[0-9]{1,2}-[0-9]{1,2})\\/?$"]["GET"] = []( SimpleWeb::Server<SimpleWeb::HTTP>::Response & response, shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request )
	{
		string start_date = request->path_match[1];
		string end_date = request->path_match[2];

		stringstream content_stream;

		content_stream << start_date << endl << end_date << endl;

		content_stream.seekp( 0, ios::end );
		response <<  "HTTP/1.1 200 OK\r\nContent-Length: " << content_stream.tellp() << "\r\n\r\n" << content_stream.rdbuf();
	};


	server.resource["\\/stats.json$"]["GET"] = []( SimpleWeb::Server<SimpleWeb::HTTP>::Response & response, shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request )
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

		for ( auto const & mac_speedinfo : speed_stats )
		{
			const string& mac = mac_speedinfo.first;
			const InterfaceSpeedMeter& ism = mac_speedinfo.second;

			// speed is in bits/s
			root[mac]["speed"]["down"] = Json::Value::UInt64( ism.get_rx_speed() );
			root[mac]["speed"]["up"] = Json::Value::UInt64( ism.get_tx_speed() );

			row.clear();
			row += std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d ) + "_" + std::to_string( h ) + ":00-" + std::to_string( h + 1 ) + ":00";

			if ( all_stats[mac]["hourly"].find( row ) != all_stats[mac]["hourly"].end() )
			{
				root[mac]["hourly"]["down"] = Json::Value::UInt64( all_stats[mac]["hourly"][row].recieved() );
				root[mac]["hourly"]["up"] = Json::Value::UInt64( all_stats[mac]["hourly"][row].transmited() );
			}
			else
			{
				root[mac]["hourly"]["down"] = Json::Value::UInt64( 0ULL );
				root[mac]["hourly"]["up"] = Json::Value::UInt64( 0ULL );
			}

			row.clear();
			row += std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d );

			if ( all_stats[mac]["daily"].find( row ) != all_stats[mac]["daily"].end() )
			{
				root[mac]["daily"]["down"] = Json::Value::UInt64( all_stats[mac]["daily"][row].recieved() );
				root[mac]["daily"]["up"] = Json::Value::UInt64( all_stats[mac]["daily"][row].transmited() );
			}
			else
			{
				root[mac]["daily"]["down"] = Json::Value::UInt64( 0ULL );
				root[mac]["daily"]["up"] = Json::Value::UInt64( 0ULL );
			}

			row.clear();
			row += std::to_string( y ) + "-" + std::to_string( m );

			if ( all_stats[mac]["monthly"].find( row ) != all_stats[mac]["monthly"].end() )
			{
				root[mac]["monthly"]["down"] = Json::Value::UInt64( all_stats[mac]["monthly"][row].recieved() );
				root[mac]["monthly"]["up"] = Json::Value::UInt64( all_stats[mac]["monthly"][row].transmited() );
			}
			else
			{
				root[mac]["monthly"]["down"] = Json::Value::UInt64( 0ULL );
				root[mac]["monthly"]["up"] = Json::Value::UInt64( 0ULL );
			}

			row.clear();
			row += std::to_string( y );

			if ( all_stats[mac]["yearly"].find( row ) != all_stats[mac]["yearly"].end() )
			{
				root[mac]["yearly"]["down"] = Json::Value::UInt64( all_stats[mac]["yearly"][row].recieved() );
				root[mac]["yearly"]["up"] = Json::Value::UInt64( all_stats[mac]["yearly"][row].transmited() );
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

