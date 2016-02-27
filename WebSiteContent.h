#ifndef WEBSITECONTENT_H
#define WEBSITECONTENT_H

#include "server_http.hpp"

class WebSiteContent
{

public:
	static void set_web_site_content( SimpleWeb::Server<SimpleWeb::HTTP>& server );
};

#endif // WEBSITECONTENT_H
