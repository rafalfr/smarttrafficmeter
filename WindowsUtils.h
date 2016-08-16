#ifndef WINDOWSUTILS_H
#define WINDOWSUTILS_H

#ifdef __WIN32
#include <map>
#include <string>
#include <windows.h>
#include "InterfaceInfo.h"


class WindowsUtils
{
public:
    static void MeterThread(void);
    static std::map<std::string, InterfaceInfo> get_all_interfaces( void );
    static LONG signal_handler( LPEXCEPTION_POINTERS p );
    static BOOL WINAPI console_ctrl_handler( DWORD fdwCtrlType );
    static void set_signals_handler(void);
    static int become_daemon(void);
    static void make_program_run_at_startup(void);
    static bool check_one_instance( void );
    static string get_program_path(void);
    static void handle_endsession_message(void);

private:
    static LRESULT APIENTRY WndProc(HWND handle, UINT umsg, WPARAM wparam, LPARAM lparam);
};

#endif // _WIN32

#endif // WINDOWSUTILS_H
