#include "InterfaceStats.h"

InterfaceStats::InterfaceStats()
{
	p_rx = 0;
	p_tx = 0;
	rx = 0;
	tx = 0;
}

InterfaceStats::InterfaceStats ( const InterfaceStats& stats )
{
	p_rx = stats.p_rx;
	p_tx = stats.p_tx;
	rx = stats.rx;
	tx = stats.tx;
}




InterfaceStats::~InterfaceStats()
{

}

void InterfaceStats::update ( uint64_t _tx, uint64_t _rx )
{
	rx += ( _rx - p_rx );
	tx += ( _tx - p_tx );

	p_rx = _rx;
	p_tx = _tx;
}

uint64_t InterfaceStats::recieved ( void ) const
{
	return rx;
}

uint64_t InterfaceStats::transmited ( void ) const
{
	return tx;
}

