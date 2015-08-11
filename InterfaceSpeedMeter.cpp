#include "InterfaceSpeedMeter.h"

#include <sys/time.h>

InterfaceSpeedMeter::InterfaceSpeedMeter()
{

}

InterfaceSpeedMeter::~InterfaceSpeedMeter()
{

}
uint64_t InterfaceSpeedMeter::get_rx_speed ( void )
{
	return 0;
}

uint64_t InterfaceSpeedMeter::get_tx_speed ( void )
{
	return 0;
}

uint64_t InterfaceSpeedMeter::get_time_milisecs ( void )
{
	struct timeval  tv;
	gettimeofday ( &tv, NULL );

	return ( tv.tv_sec ) * 1000LL + ( tv.tv_usec ) / 1000 ;
}


void InterfaceSpeedMeter::update ( uint64_t _rx, uint64_t _tx )
{
	map<string, uint64_t> item;

	uint64_t time = get_time_milisecs();

	item["rx"] = _rx;
	item["tx"] = _tx;
	item["time"] = time;

	buf.push_front ( item );

	if ( buf.size() >= max_buf_items )
	{
		buf.pop_back();
	}
}
