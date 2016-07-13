#ifndef WEBSITECONTENT_H
#define WEBSITECONTENT_H

#ifndef _NO_WEBSERVER
#include <string>
#include "server_http.hpp"

using namespace std;

class WebSiteContent
{
private:
	static string rgba_color(string& hex_color, float a);


public:
	static void set_web_site_content( SimpleWeb::Server<SimpleWeb::HTTP>& server );
};

#endif // _NO_WEBSERVER

#endif // WEBSITECONTENT_H
