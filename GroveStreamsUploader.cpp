#include "config.h"
#include <string>
#include "Utils.h"
#include "Logger.h"
#include "Globals.h"
#include "Settings.h"
#include "GroveStreamsUploader.h"

#ifdef use_sqlite
#include "sqlite3.h"
#endif // use_sqlite

#include <curl/curl.h>

using namespace std;

/** @brief upload_all
  *
  * Public method that uploads all statistics to grovestreams.
  *
  * @param void
  * @return number of sent bytes
  *
  */
uint32_t GroveStreamsUploader::upload_all( void )
{
    CURL *curl;

    string api_key = Settings::settings["grovestreams api key"];

    if ( api_key.compare( "" ) == 0 )
    {
        return 0U;
    }

    curl_global_init( CURL_GLOBAL_DEFAULT );

    curl = curl_easy_init();

    if ( curl )
    {
        struct curl_slist *headers = NULL;

        curl_easy_setopt( curl, CURLOPT_NOSIGNAL, 1 );

        curl_easy_setopt( curl, CURLOPT_TIMEOUT, 10L );

        curl_easy_setopt( curl, CURLOPT_TCP_KEEPALIVE, 1L );

        curl_easy_setopt( curl, CURLOPT_TCP_KEEPIDLE, 120L );

        curl_easy_setopt( curl, CURLOPT_TCP_KEEPINTVL, 60L );

        curl_easy_setopt( curl, CURLOPT_DNS_CACHE_TIMEOUT, 0L );

        curl_easy_setopt( curl, CURLOPT_VERBOSE, 0L );

        curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, download_handler );

        headers = curl_slist_append( headers, "Content-Type: application/json" );

        curl_easy_setopt( curl, CURLOPT_HTTPHEADER, headers );

        string base_url = "http://grovestreams.com:80/api/feed?";

        const map<string, InterfaceInfo>& interfaces = Utils::get_all_interfaces();

        uint32_t sent_bytes=0U;

        for ( auto const & kv : interfaces )
        {
            const string mac = kv.first;

            string url;
            url.clear();
            url += base_url;
            url += "compId=";
            url += mac;
            url += "&api_key=";
            url += api_key;

            string json;
            json.clear();
            json += "[\n";

            uint32_t y;
            uint32_t m;
            uint32_t d;
            uint32_t h;

            Utils::get_time( &y, &m, &d, &h );

            struct date start_date;
            struct date end_date;

            start_date.year = 1900;
            start_date.month = 1;
            start_date.day = 1;
            start_date.hour = 0;

            end_date.year = y;
            end_date.month = m;
            end_date.day = d;
            end_date.hour = h;

            vector<string> stats_types{"yearly", "monthly", "daily", "hourly"};

            for ( const string& stats_type : stats_types )
            {
                map<string, InterfaceStats> results = Globals::db_drv.get_stats( mac, stats_type, start_date, end_date );

                for ( auto const & row_stats : results )
                {
                    uint32_t y, m, d, h;

                    const string row = row_stats.first;
                    const InterfaceStats& stats = row_stats.second;
                    Utils::str2date( row, stats_type, &y, &m, &d, &h );

                    if ( y == 0U && m == 0U && d == 0U && h == 0U )
                    {
                        continue;
                    }

                    time_t time_stamp = Utils::date_to_seconds( y, m, d, h );

                    uint64_t rx = stats.received();
                    uint64_t tx = stats.transmitted();

                    json += "{\n";
                    json += "\"compId\": \"" + mac + "\",\n";
                    json += "\"streamId\": \"" + stats_type + " down\",\n";
                    json += "\"data\": " + Utils::to_string( rx ) + ",\n";
                    json += "\"time\": " + Utils::to_string( static_cast<uint64_t>( time_stamp * 1000ULL ) ) + "\n";
                    json += "},\n";
                    json += "{\n";
                    json += "\"compId\": \"" + mac + "\",\n";
                    json += "\"streamId\": \"" + stats_type + " up\",\n";
                    json += "\"data\": " + Utils::to_string( tx ) + ",\n";
                    json += "\"time\": " + Utils::to_string( static_cast<uint64_t>( time_stamp * 1000ULL ) ) + "\n";
                    json += "},\n";
                }
            }

            json.pop_back();
            json.pop_back();

            json += "\n]";

            curl_easy_setopt( curl, CURLOPT_URL, url.c_str() );
            curl_easy_setopt( curl, CURLOPT_CUSTOMREQUEST, "PUT" );

            curl_easy_setopt( curl, CURLOPT_POSTFIELDS, json.c_str() );

            curl_easy_perform( curl );

            sent_bytes+=json.size();
        }

        curl_slist_free_all( headers );

        curl_easy_cleanup( curl );

        return sent_bytes;
    }
    else
	{
		return 0U;
	}
}

/** @brief download_handler
  *
  * Private method that receives response data from the remote host.
  * The method is not used outside the scope of the GroveStreamsUploader class.
  *
  */
size_t GroveStreamsUploader::download_handler( void*, size_t size, size_t nmemb, void* )
{
    return size * nmemb;
}


/** @brief GroveStreamsUploader
  *
  * The default constructor
  *
  * @param void
  * @return void
  *
  */
GroveStreamsUploader::GroveStreamsUploader()
{
}

/** @brief ~GroveStreamsUploader
  *
  * The default destructor
  *
  * @param void
  * @return void
  *
  */
GroveStreamsUploader::~GroveStreamsUploader()
{
}
