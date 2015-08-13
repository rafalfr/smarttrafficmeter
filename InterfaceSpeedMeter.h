#ifndef INTERFACESPEEDMETER_H
#define INTERFACESPEEDMETER_H
#include <list>
#include <map>

using namespace std;

class InterfaceSpeedMeter
{
private:

	list < map< string, uint64_t> >::size_type max_buf_items;
	list < map< string, uint64_t> > buf;

	static uint64_t get_time_milisecs ( void );

public:
	InterfaceSpeedMeter();
	InterfaceSpeedMeter(const InterfaceSpeedMeter&);
	virtual ~InterfaceSpeedMeter();
	uint64_t get_rx_speed ( void ) const;
	uint64_t get_tx_speed ( void ) const;
	void update ( uint64_t _rx, uint64_t _tx );

};

#endif // INTERFACESPEEDMETER_H
