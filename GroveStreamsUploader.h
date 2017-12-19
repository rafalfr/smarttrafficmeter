/*

GroveStreamsUploader.h

Copyright (C) 2018 Rafał Frączek

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

#ifndef GROVESTREAMSUPLOADER_H
#define GROVESTREAMSUPLOADER_H


class GroveStreamsUploader
{

private:
    static size_t download_handler( void *, size_t size, size_t nmemb, void * interface );

public:

    GroveStreamsUploader();
    ~GroveStreamsUploader();

    static uint32_t upload_all( void );
};

#endif // GROVESTREAMSUPLOADER_H
