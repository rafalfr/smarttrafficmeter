#include "InterfaceInfo.h"


InterfaceInfo::InterfaceInfo()
{

}

InterfaceInfo::~InterfaceInfo()
{

}

string InterfaceInfo::get_mac ( void ) const
{
	return mac;
}

string InterfaceInfo::get_name ( void ) const
{
	return name;
}

string InterfaceInfo::get_ip4 ( void ) const
{
	return ip4;
}
string InterfaceInfo::get_ip6(void) const
{
    return ip6;
}

void InterfaceInfo::set_mac ( const char* _mac )
{
	mac.clear();
	mac.append ( _mac );
}

void InterfaceInfo::set_name ( const char* _name )
{
	name.clear();
	name.append ( _name );
}

void InterfaceInfo::set_ip4 ( const char* _ip4 )
{
	ip4.clear();
	ip4.append ( _ip4 );
}

void InterfaceInfo::set_ip6(const char* _ip6)
{
	ip6.clear();
	ip6.append ( _ip6 );
}
