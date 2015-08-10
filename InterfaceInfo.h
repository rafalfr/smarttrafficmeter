#ifndef INTERFACEINFO_H
#define INTERFACEINFO_H
#include <string>

using namespace std;

class InterfaceInfo
{
private:
	string mac;
	string name;
	string ip4;
	string ip6;

public:
	InterfaceInfo();
	~InterfaceInfo();

	string get_mac ( void ) const;
	string get_name ( void ) const;
	string get_ip4 ( void ) const;
	string get_ip6 ( void ) const;
	void set_mac ( const char* _mac );
	void set_name ( const char* _name );
	void set_ip4 ( const char* _ip4 );
	void set_ip6 ( const char* _ip6 );
};

#endif // INTERFACEINFO_H
