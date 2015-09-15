#ifndef INTERFACESTATS_H
#define INTERFACESTATS_H
#include <stdint.h>

class InterfaceStats
{
private:
	bool first_update;
	uint64_t rx;
	uint64_t tx;
	uint64_t p_rx;
	uint64_t p_tx;

public:
	InterfaceStats();
	InterfaceStats( const InterfaceStats& );
	~InterfaceStats();

	void set_initial_stats( uint64_t _tx, uint64_t _rx );
	void update( uint64_t _tx, uint64_t _rx );
	uint64_t recieved( void ) const;
	uint64_t transmited( void ) const;
};

#endif // INTERFACESTATS_H
