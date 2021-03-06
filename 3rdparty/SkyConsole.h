#ifndef SC_CONSOLE_H
#define SC_CONSOLE_H

#define CL_LightBlue rang::fgB::blue
#define CL_LightGreen rang::fgB::green
#define CL_LightRed rang::fgB::red
#define CL_LightYellow rang::fgB::yellow
extern BOOL WINAPI ConsoleHandler( DWORD CtrlType );

//#ifdef USE_LOCK
//CMutex mConsoleLock;
//#define  CONSOLE_LOCK() mConsoleLock.Lock()
//#define  CONSOLE_UNLOCK() mConsoleLock.UnLock()
//#else
#define  CONSOLE_LOCK()
#define  CONSOLE_UNLOCK()
//#endif

#define va_format( format, logType, color ) \
	CONSOLE_LOCK(); \
	sprintf( mConsoleBuffer, "[%04d-%02d-%02d %02d:%02d:%02d] [ ", SkyTime::Year(), SkyTime::Month(), SkyTime::Day(), SkyTime::Hour(), SkyTime::Minute(), SkyTime::Second() );\
	std::cout << mConsoleBuffer << color << logType << rang::style::reset << std::flush;\
	va_list args; \
	va_start( args, format ); \
	vsprintf( mConsoleBuffer, format, args ); \
	va_end( args ); \
	std::cout << "] :: " << mConsoleBuffer << std::endl;\
	CONSOLE_UNLOCK()

class SkyConsole
{
public:
	char mConsoleBuffer[40960];

    SkyConsole() : mConsoleBuffer()
	{
        BindStdHandlesToConsole();
	}
    void init()
    {
        if ( !SetConsoleCtrlHandler( ConsoleHandler, TRUE ) )
	    {
		    MessageBox( NULL, "!SetConsoleCtrlHandler()", "ERROR", ( MB_OK | MB_ICONERROR ) );
		    exit(0);
	    }
    }
	void log( const char* fmt, ... )
	{
        va_format( fmt, "Log\t", CL_LightGreen );
	}
	void error( const char* fmt, ... )
	{
        va_format( fmt, "Error\t", CL_LightRed );
	}

private:
	void BindStdHandlesToConsole()
    {
	    #ifndef _CONSOLE
	    AllocConsole();
        //TODO: Add Error checking.
    
        // Redirect the CRT standard input, output, and error handles to the console
        (void)freopen( "CONIN$", "r", stdin );
        (void)freopen( "CONOUT$", "w", stdout );
    
        // Note that there is no CONERR$ file
        HANDLE hStdout = CreateFile( "CONOUT$",  GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
        HANDLE hStdin = CreateFile( "CONIN$",  GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    
        SetStdHandle( STD_OUTPUT_HANDLE, hStdout );
        SetStdHandle( STD_ERROR_HANDLE, hStdout );
        SetStdHandle( STD_INPUT_HANDLE, hStdin );

        //Clear the error state for each of the C++ standard stream objects. 
        std::wclog.clear();
        std::clog.clear();
        std::wcout.clear();
        std::cout.clear();
        std::wcerr.clear();
        std::cerr.clear();
        std::wcin.clear();
        std::cin.clear();
	    #endif
    }

};
extern SkyConsole console;

#endif //SC_CONSOLE_H