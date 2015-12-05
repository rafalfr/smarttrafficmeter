#include <limits>
#include "InterfaceStats.h"

InterfaceStats::InterfaceStats()
{
	first_update = true;
	p_rx = 0ULL;
	p_tx = 0ULL;
	rx = 0ULL;
	tx = 0ULL;
}

InterfaceStats::InterfaceStats( const InterfaceStats& stats )
{
	first_update = stats.first_update;
	p_rx = stats.p_rx;
	p_tx = stats.p_tx;
	rx = stats.rx;
	tx = stats.tx;
}

InterfaceStats::~InterfaceStats()
{

}

void InterfaceStats::set_initial_stats( uint64_t _tx, uint64_t _rx )
{
	rx = _rx;
	tx = _tx;
	first_update = true;
}

void InterfaceStats::update( uint64_t _tx, uint64_t _rx )
{
	if ( first_update == true )
	{
		p_rx = _rx;
		p_tx = _tx;
		first_update = false;
	}

	if ( _rx >= p_rx )
	{
		rx +=  _rx - p_rx ;
	}
	else
	{
		rx += ( _rx + ( uint64_t )std::numeric_limits<uint32_t>::max() ) - p_rx;
	}

	if ( _tx >= p_tx )
	{
		tx +=  _tx - p_tx ;
	}
	else
	{
		tx += ( _tx + ( uint64_t )std::numeric_limits<uint32_t>::max() ) - p_tx;
	}

	p_rx = _rx;
	p_tx = _tx;
}

uint64_t InterfaceStats::recieved( void ) const
{
	return rx;
}

uint64_t InterfaceStats::transmited( void ) const
{
	return tx;
}

