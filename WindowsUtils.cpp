#ifdef _WIN32
#include <fstream>
#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#include <iphlpapi.h>

#include "config.h"
#include "WindowsUtils.h"
#include "Utils.h"
#include "Logger.h"
#include "Debug.h"
#include "Globals.h"
#include "Settings.h"
#include "InterfaceStats.h"
#include "InterfaceSpeedMeter.h"

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

using namespace std;

/** @brief MeterThread
  *
  * @todo: document this function
  */
void WindowsUtils::MeterThread( void )
{
// https://msdn.microsoft.com/en-us/library/windows/desktop/aa365943(v=vs.85).aspx

//https://github.com/eidheim/Simple-WebSocket-Server

    uint32_t y = 0;
    uint32_t m = 0;
    uint32_t d = 0;
    uint32_t h = 0;
    uint32_t ph = 0;
    static uint64_t p_time = 0;

    uint32_t refresh_interval=1;	//statistics refresh interval in seconds
    uint32_t save_interval=30*60; 	//save interval in seconds

    char buf[16];

    try
    {
        refresh_interval = Utils::stoi( Settings::settings["stats refresh interval"] );
    }
    catch ( ... )
    {
        refresh_interval = 1;
    }

    try
    {
        save_interval = Utils::stoi( Settings::settings["stats save interval"] );
    }
    catch ( ... )
    {
        save_interval = 30 * 60;
    }

    string row;
    string current_row;
    vector<map<string, InterfaceStats>::const_iterator> rows4remove;

    while ( true )
    {

        DWORD dwSize = 0;
        DWORD dwRetVal = 0;

        MIB_IFTABLE *pIfTable;
        MIB_IFROW *pIfRow;

        // Allocate memory for our pointers.
        pIfTable = ( MIB_IFTABLE * ) MALLOC( sizeof( MIB_IFTABLE ) );

        if ( pIfTable == NULL )
        {
            return;
        }

        dwSize = sizeof( MIB_IFTABLE );

        if ( GetIfTable( pIfTable, &dwSize, FALSE ) == ERROR_INSUFFICIENT_BUFFER )
        {
            FREE( pIfTable );
            pIfTable = ( MIB_IFTABLE * ) MALLOC( dwSize );

            if ( pIfTable == NULL )
            {
                return;
            }
        }

        if ( ( dwRetVal = GetIfTable( pIfTable, &dwSize, FALSE ) ) == NO_ERROR )
        {

            for (uint32_t i = 0; i < pIfTable->dwNumEntries; i++ )
            {
                pIfRow = ( MIB_IFROW * ) & pIfTable->table[i];

                DWORD status=pIfRow->dwOperStatus;

                // skip inactive interfaces
                if (status!=IF_OPER_STATUS_OPERATIONAL)
                {
                    continue;
                }

                // skip interfaces with no mac
                if ((pIfRow->dwPhysAddrLen)==0)
                {
                    continue;
                }

                if (pIfRow->dwDescrLen==0)
                {
                    continue;
                }

                string mac;

                mac.clear();
                for (uint32_t j = 0; j < pIfRow->dwPhysAddrLen; j++ )
                {

                    memset( buf, 0, 16 * sizeof( char ) );
                    sprintf( buf, "%.2x", ( int ) pIfRow->bPhysAddr[j] );
                    mac.append( buf );

                    if ( j < ( pIfRow->dwPhysAddrLen - 1 ) )
                    {
                        mac += "-";
                    }
                }

                DWORD rx_bytes=pIfRow->dwInOctets;
                DWORD tx_bytes=pIfRow->dwOutOctets;

                Utils::get_time( &y, &m, &d, &h );

                if ( Globals::speed_stats.find( mac ) == Globals::speed_stats.end() )
                {
                    InterfaceSpeedMeter ism;
                    Globals::speed_stats[mac] = ism;
                }
                row.clear();
                row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 ) + "_" + Utils::to_string( h, 2 ) + ":00-" + Utils::to_string( h + 1, 2 ) + ":00";

                if ( Globals::all_stats[mac]["hourly"].find( row ) == Globals::all_stats[mac]["hourly"].end() )
                {
                    InterfaceStats hstats;
                    Globals::all_stats[mac]["hourly"][row] = hstats;
                }

                Globals::all_stats[mac]["hourly"][row].update( tx_bytes, rx_bytes );


                row.clear();
                row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 );

                if ( Globals::all_stats[mac]["daily"].find( row ) == Globals::all_stats[mac]["daily"].end() )
                {
                    InterfaceStats dstats;
                    Globals::all_stats[mac]["daily"][row] = dstats;
                }

                Globals::all_stats[mac]["daily"][row].update( tx_bytes, rx_bytes );

                row.clear();
                row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 );

                if ( Globals::all_stats[mac]["monthly"].find( row ) == Globals::all_stats[mac]["monthly"].end() )
                {
                    InterfaceStats mstats;
                    Globals::all_stats[mac]["monthly"][row] = mstats;
                }

                Globals::all_stats[mac]["monthly"][row].update( tx_bytes, rx_bytes );


                row.clear();
                row += Utils::to_string( y );

                if ( Globals::all_stats[mac]["yearly"].find( row ) == Globals::all_stats[mac]["yearly"].end() )
                {
                    InterfaceStats ystats;
                    Globals::all_stats[mac]["yearly"][row] = ystats;
                }

                Globals::all_stats[mac]["yearly"][row].update( tx_bytes, rx_bytes );

                Globals::speed_stats[mac].update( rx_bytes, tx_bytes );
            }
        }
        else
        {
            if ( pIfTable != NULL )
            {
                FREE( pIfTable );
                pIfTable = nullptr;
            }

            return;
        }

        if ( pIfTable != NULL )
        {
            FREE( pIfTable );
            pIfTable = nullptr;
        }

        if ( Globals::is_daemon == false )
        {
            for ( auto const & mac_speedinfo : Globals::speed_stats )
            {
                const string& mac = mac_speedinfo.first;
                const InterfaceSpeedMeter& ism = mac_speedinfo.second;

                cout << mac << endl;

                double speed = ( double )ism.get_tx_speed();

                if ( speed < 1000.0 )
                {
                    cout << "up: " << speed << " b/s\t";
                }
                else if ( speed >= 1000.0 && speed < 1000.0 * 1000.0 )
                {
                    speed /= 1000.0;
                    cout << "up: " << speed << " Kb/s\t";
                }
                else if ( speed >= 1000.0 * 1000.0 && speed < 1000.0 * 1000.0 * 1000.0 )
                {
                    speed /= 1000.0 * 1000.0;
                    cout << "up: " << speed << " Mb/s\t";
                }
                else if ( speed >= 1000.0 * 1000.0 * 1000.0 && speed < 1000.0 * 1000.0 * 1000.0 * 1000.0 )
                {
                    speed /= 1000.0 * 1000.0 * 1000.0;
                    cout << "up: " << speed << " Gb/s\t";
                }

                speed = ( double )ism.get_rx_speed();

                if ( speed < 1000.0 )
                {
                    cout << "down: " << speed << " b/s" << endl;
                }
                else if ( speed >= 1000.0 && speed < 1000.0 * 1000.0 )
                {
                    speed /= 1000.0;
                    cout << "down: " << speed << " Kb/s" << endl;
                }
                else if ( speed >= 1000.0 * 1000.0 && speed < 1000.0 * 1000.0 * 1000.0 )
                {
                    speed /= 1000.0 * 1000.0;
                    cout << "down: " << speed << " Mb/s" << endl;
                }
                else if ( speed >= 1000.0 * 1000.0 * 1000.0 && speed < 1000.0 * 1000.0 * 1000.0 * 1000.0 )
                {
                    speed /= 1000.0 * 1000.0 * 1000.0;
                    cout << "down: " << speed << " Gb/s" << endl;
                }

                cout << endl;
            }

            cout << "-------------------------------" << endl;
        }

        SYSTEMTIME st;
        GetSystemTime(&st);
        uint64_t c_time = st.wSecond * 1000ULL + st.wMilliseconds;

        MSG msg;
        bool close_event=PeekMessage(&msg, NULL, WM_CLOSE, WM_CLOSE, PM_NOREMOVE);

        if (close_event)
        {
            Logger::LogInfo("closing event");
        }

        if ( c_time >= p_time + ( 1000ULL * save_interval ) || ( h != ph ) || close_event==true)
        {
            const string& storage = Settings::settings["storage"];

            if ( Utils::contians( storage, "mysql" ) )
            {
#ifdef use_mysql
                //save_stats_to_mysql();
#endif // use_mysql
            }

            if ( Utils::contians( storage, "sqlite" ) )
            {
#ifdef use_sqlite
                Utils::save_stats_to_sqlite();
#endif // use_sqlite
            }

            if ( Utils::contians( storage, "files" ) )
            {
                Utils::save_stats_to_files();
            }

            /* remove unused rows from the all_stats container */

            for ( auto const & mac_table : Globals::all_stats )
            {
                const string& mac = mac_table.first;

                const map<string, map<string, InterfaceStats> > & table = mac_table.second;

                for ( auto const & table_row : table )
                {
                    const string& table_name = table_row.first;	//hourly, daily, etc..

                    current_row.clear();

                    if ( table_name.compare( "hourly" ) == 0 )
                    {
                        current_row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 ) + "_" + Utils::to_string( h, 2 ) + ":00-" + Utils::to_string( h + 1, 2 ) + ":00";
                    }
                    else if ( table_name.compare( "daily" ) == 0 )
                    {
                        current_row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 );
                    }
                    else if ( table_name.compare( "monthly" ) == 0 )
                    {
                        current_row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 );
                    }
                    else if ( table_name.compare( "yearly" ) == 0 )
                    {
                        current_row += Utils::to_string( y );
                    }

                    const map<string, InterfaceStats> & row = table_row.second;

                    rows4remove.clear();

                    for ( map<string, InterfaceStats>::const_iterator it = row.cbegin(); it != row.cend(); )
                    {
                        const string& row_in_table = it->first;	//subsequent rows in the current table

                        if ( row_in_table.compare( current_row ) != 0 )
                        {
                            rows4remove.push_back( it );
                        }

                        it++;
                    }

                    for ( uint32_t i = 0; i < rows4remove.size(); i++ )
                    {
                        Globals::all_stats[mac][table_name].erase( rows4remove[i] );
                    }

                    rows4remove.clear();
                }
            }

            p_time = c_time;
            ph = h;
        }

        Sleep( refresh_interval*1000);
    }
}

/** @brief get_all_interfaces
  *
  * @todo: document this function
  */
map<std::string, InterfaceInfo> WindowsUtils::get_all_interfaces( void )
{
    char buf[16];
    map<string, InterfaceInfo> interfaces;

    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;
    UINT i;

    ULONG ulOutBufLen = sizeof( IP_ADAPTER_INFO );
    pAdapterInfo = ( IP_ADAPTER_INFO * ) MALLOC( sizeof( IP_ADAPTER_INFO ) );

    if ( pAdapterInfo == NULL )
    {
        return interfaces;
    }

    if ( GetAdaptersInfo( pAdapterInfo, &ulOutBufLen ) == ERROR_BUFFER_OVERFLOW )
    {
        FREE( pAdapterInfo );
        pAdapterInfo = ( IP_ADAPTER_INFO * ) MALLOC( ulOutBufLen );

        if ( pAdapterInfo == NULL )
        {
            return interfaces;
        }
    }

    if ( ( dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen ) ) == NO_ERROR )
    {
        InterfaceInfo in;

        pAdapter = pAdapterInfo;

        while ( pAdapter )
        {
            in.set_name( pAdapter->AdapterName );
            in.set_desc( pAdapter->Description );

            string mac;

            for ( i = 0; i < pAdapter->AddressLength; i++ )
            {
                memset( buf, 0, 16 * sizeof( char ) );
                sprintf( buf, "%.2x", ( int ) pAdapter->Address[i] );
                mac.append( buf );

                if ( i < ( pAdapter->AddressLength - 1 ) )
                {
                    mac += "-";
                }
            }

            in.set_mac( mac.c_str() );
            in.set_ip4( pAdapter->IpAddressList.IpAddress.String );
            interfaces[in.get_name()] = in;

            pAdapter = pAdapter->Next;
            printf( "\n" );
        }
    }
    else
    {
        return interfaces;

    }

    if ( pAdapterInfo )
        FREE( pAdapterInfo );

    return interfaces;
}

/** @brief console_ctrl_handler
  *
  * @todo: document this function
  */
BOOL WINAPI WindowsUtils::console_ctrl_handler(DWORD fdwCtrlType)
{
    signal_handler(nullptr);
}


/** @brief windows_signal_handler
  *
  * @todo: document this function
  */
LONG WindowsUtils::signal_handler(LPEXCEPTION_POINTERS exceptionInfo)
{
//http://win32easy.blogspot.com/2011/03/exception-handling-inform-your-users.html

    const string& storage = Settings::settings["storage"];


    if (exceptionInfo!=nullptr)
    {
        char message[512];
        memset(message,0,512*sizeof(char));
        sprintf(message,"An exception occured which wasnt handled!\nCode: 0x%08X\nAddress: 0x%08X",
                exceptionInfo->ExceptionRecord->ExceptionCode,
                exceptionInfo->ExceptionRecord->ExceptionAddress);

        Logger::LogError(message);
    }


    if ( Utils::contians( storage, "mysql" ) )
    {
#ifdef use_mysql
        //save_stats_to_mysql();
#endif // use_mysql
    }

    if ( Utils::contians( storage, "sqlite" ) )
    {
#ifdef use_sqlite
        Utils::save_stats_to_sqlite();
#endif // use_sqlite
    }

    if ( Utils::contians( storage, "files" ) )
    {
        Utils::save_stats_to_files();
    }

    exit(0);
}


/** @brief set_signals_handler
  *
  * @todo: document this function
  */
void WindowsUtils::set_signals_handler(void)
{
//http://stackoverflow.com/questions/3640633/setconsolectrlhandler-routine-issue
//http://stackoverflow.com/questions/8698881/intercept-wm-close-for-cleanup-operations
//http://stackoverflow.com/questions/9478684/how-does-task-manager-kill-my-program

    //SetConsoleCtrlHandler( ( PHANDLER_ROUTINE ) &WindowsUtils::signal_handler, TRUE );
    SetConsoleCtrlHandler( (PHANDLER_ROUTINE) WindowsUtils::console_ctrl_handler, TRUE );
    SetUnhandledExceptionFilter( ( LPTOP_LEVEL_EXCEPTION_FILTER ) &WindowsUtils::signal_handler );
}

/** @brief become_daemon
  *
  * @todo: document this function
  */
int WindowsUtils::become_daemon(void)
{
    // START /B "" SmartTrafficMeter --daemon
    //fclose(stdout);
    //fclose(stderr);
    //fclose(stdin);
    if (FreeConsole()!=0)
    {
        return 0;
    }
    else
    {
        return -1;
    }

    return 0;
}

/** @brief make_program_run_at_startup
  *
  * @todo: document this function
  */
void WindowsUtils::make_program_run_at_startup(void)
{
    //http://www.cplusplus.com/forum/windows/58636/

    HKEY hKey = NULL;

    LONG sts = RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run\\"), 0, KEY_ALL_ACCESS, &hKey);

    if (sts==ERROR_SUCCESS)
    {
        char data[1024];
        DWORD data_length = 1024*sizeof(char);
        DWORD type = REG_SZ;
        memset(data,0,data_length);
        LONG nError = RegQueryValueEx(hKey, "SmartTrafficMeter", NULL, &type, (LPBYTE)&data, &data_length);

        if (nError==ERROR_FILE_NOT_FOUND)
        {
            Logger::LogInfo("creating registry value");

            string command("\"");
            command+=Globals::program_path;
            command+="\"";
            command+=" --daemon";
            DWORD commnad_length=strlen(command.c_str())*sizeof(char);
            LONG nError=RegSetValueEx(hKey, TEXT("SmartTrafficMeter"), NULL, REG_SZ, (LPBYTE)command.c_str(), commnad_length);

            if (nError!=ERROR_SUCCESS)
            {
                Logger::LogError("Cannot create registry value");

                LPVOID lpMsgBuf;
                DWORD dw = GetLastError();

                FormatMessage(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    dw,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPTSTR) &lpMsgBuf,
                    0, NULL );

                Logger::LogDebug((const char*)lpMsgBuf);

                LocalFree(lpMsgBuf);
            }
        }
    }

    RegCloseKey(hKey);
}

/** @brief check_one_instance
  *
  * @todo: document this function
  */
bool WindowsUtils::check_one_instance(void)
{
//http://stackoverflow.com/questions/4191465/how-to-run-only-one-instance-of-application

    HANDLE  m_hStartEvent = CreateEventW( NULL, FALSE, FALSE, L"Global\\SMTAPP" );

    if(m_hStartEvent == NULL)
    {
        CloseHandle( m_hStartEvent );
        return false;
    }

    if ( GetLastError() == ERROR_ALREADY_EXISTS )
    {
        CloseHandle( m_hStartEvent );
        m_hStartEvent = NULL;
        // already exist
        return false;
    }

    // the only instance, start in a usual way
    return true;
}
/** @brief get_program_path
  *
  * @todo: document this function
  */
string WindowsUtils::get_program_path(void)
{
    char ownPth[MAX_PATH];

    memset(ownPth,0,MAX_PATH*sizeof(char));

    HMODULE hModule = GetModuleHandle(NULL);
    if (hModule != NULL)
    {
        GetModuleFileName(hModule,ownPth, (sizeof(ownPth)));

        return string(ownPth);
    }
    else
    {
        return "";
    }
}



#endif // _WIN32

