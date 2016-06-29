#ifndef INTERFACEINFO_H
#define INTERFACEINFO_H
#include <string>

using namespace std;

class InterfaceInfo
{
private:
    string mac;
    string name;
    string desc;
    string ip4;
    string ip6;

public:
    InterfaceInfo();
    InterfaceInfo(const InterfaceInfo& info);

    ~InterfaceInfo();

    InterfaceInfo& operator=(const InterfaceInfo& right);

    string get_mac ( void ) const;
    string get_name ( void ) const;
    string get_desc(void) const;
    string get_ip4 ( void ) const;
    string get_ip6 ( void ) const;
    void set_mac ( const char* t_mac );
    void set_name ( const char* t_name );
    void set_desc(const char* t_desc);
    void set_ip4 ( const char* t_ip4 );
    void set_ip6 ( const char* t_ip6 );
};

#endif // INTERFACEINFO_H
