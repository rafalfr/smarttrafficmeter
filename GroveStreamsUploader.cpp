#include <string>
#include "Utils.h"
#include "Logger.h"
#include "Globals.h"
#include "Settings.h"
#include "GroveStreamsUploader.h"

#include <curl/curl.h>

using namespace std;

/** @brief run
  *
  * Public method that uploads current statistics to grovestreams.
  *
  * @param void
  * @return void
  *
  */
void GroveStreamsUploader::run( void )
{
    CURL *curl;
    CURLcode res;

    string api_key = Settings::settings["grovestreams api key"];

    if ( api_key.compare( "" ) == 0 )
    {
        return;
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

        for ( auto const & mac_table : Globals::all_stats )
        {
            const string& mac = mac_table.first;

            string url;
            url += base_url;
            url += "compId=";
            url += mac;
            url += "&api_key=";
            url += api_key;

            string json;
            json += "[\n";

            const map<string, map<string, InterfaceStats> > & table = mac_table.second;

            for ( auto const & table_row : table )
            {
                const string& table_name = table_row.first;

                const map<string, InterfaceStats> & row = table_row.second;

                for ( auto const & row_stats : row )
                {
                    uint32_t y, m, d, h;

                    const string& row = row_stats.first;
                    const InterfaceStats& stats = row_stats.second;
                    Utils::str2date( row, table_name, &y, &m, &d, &h );
                    time_t time_stamp = Utils::date_to_seconds( y, m, d, h );

                    uint64_t rx = stats.recieved();
                    uint64_t tx = stats.transmited();

                    json += "{\n";
                    json += "\"compId\": \"" + mac + "\",\n";
                    json += "\"streamId\": \"" + table_name + " down\",\n";
                    json += "\"data\": " + Utils::to_string( rx ) + ",\n";
                    json += "\"time\": " + Utils::to_string( static_cast<uint64_t>( time_stamp * 1000ULL ) ) + "\n";
                    json += "},\n";
                    json += "{\n";
                    json += "\"compId\": \"" + mac + "\",\n";
                    json += "\"streamId\": \"" + table_name + " up\",\n";
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

            res = curl_easy_perform( curl );

        }

        curl_slist_free_all( headers );

        curl_easy_cleanup( curl );
    }
}

/** @brief download_handler
  *
  * Private method that receives response data from the remote host.
  * The method is not used outside the scope of the GroveStreamsUploader class.
  *
  */
size_t GroveStreamsUploader::download_handler( void*, size_t size, size_t nmemb, void* interface )
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
