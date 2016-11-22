/*

WebSiteContent.h

Copyright (C) 2016 Rafał Frączek

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

/**
 * @file WebSiteContent.h
 * @brief Class definition for the WebSiteContent
 *
 * This file contains the definition of the WebSiteContent class.
 *
 * @author Rafał Frączek
 * @bug No known bugs
 *
 */

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
