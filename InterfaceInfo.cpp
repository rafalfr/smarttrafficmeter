#include "InterfaceInfo.h"


InterfaceInfo::InterfaceInfo() : mac( "" ), name( "" ), desc(""), ip4( "" ), ip6( "" )
{

}

InterfaceInfo::InterfaceInfo( const InterfaceInfo& info ) : mac( info.get_mac() ), name( info.get_name() ), desc(""), ip4( info.get_ip4() ), ip6( info.get_ip6() )
{

}

InterfaceInfo::~InterfaceInfo()
{
    mac.clear();
    ip4.clear();
    ip6.clear();
    name.clear();
    desc.clear();
}
/** @brief operator=
  *
  * @todo: document this function
  */
InterfaceInfo& InterfaceInfo::operator=( const InterfaceInfo& right )
{
    if ( & right != this )
    {
        mac.clear();
        mac.append( right.get_mac() );
        ip4.clear();
        ip4.append( right.get_ip4() );
        ip6.clear();
        ip6.append( right.get_ip6() );
        name.clear();
        name.append( right.get_name() );
        desc.clear();
        desc.append(right.get_desc());
    }

    return * this;
}

string InterfaceInfo::get_mac( void ) const
{
    return mac;
}

string InterfaceInfo::get_name( void ) const
{
    return name;
}

/** @brief get_desc
  *
  * @todo: document this function
  */
string InterfaceInfo::get_desc(void) const
{
    return desc;
}

string InterfaceInfo::get_ip4( void ) const
{
    return ip4;
}
string InterfaceInfo::get_ip6( void ) const
{
    return ip6;
}

void InterfaceInfo::set_mac( const char* t_mac )
{
    mac.clear();
    mac.append( t_mac );
}

void InterfaceInfo::set_name( const char* t_name )
{
    name.clear();
    name.append( t_name );
}

/** @brief set_desc
  *
  * @todo: document this function
  */
void InterfaceInfo::set_desc(const char* t_desc)
{
    desc.clear();
    desc.append(t_desc);
}

void InterfaceInfo::set_ip4( const char* t_ip4 )
{
    ip4.clear();
    ip4.append( t_ip4 );
}

void InterfaceInfo::set_ip6( const char* t_ip6 )
{
    ip6.clear();
    ip6.append( t_ip6 );
}

