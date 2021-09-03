#include <api.h>

#define SWARP_INFO( name ) \
mSERVER_INFO.IP[MAIN_SERVER] = mSERVER_INFO.IP[##name##_SERVER]; \
mSERVER_INFO.Port[MAIN_SERVER] = mSERVER_INFO.Port[##name##_SERVER]; \
mSERVER_INFO.Type[MAIN_SERVER] = mSERVER_INFO.Type[##name##_SERVER]; \
mSERVER_INFO.mTimeLogic = (TIMETICK)mSERVER_INFO.Logic[##name##_SERVER];

SERVER_INFO mSERVER_INFO;
SkyApplication mAPP;
SkyConsole console;
SkyUserMap mUSER;
SkyServer gServer;

BOOL WINAPI ConsoleHandler(DWORD cEvent)
{
	console.log("%s:%d", __PRETTY_FUNCTION__, cEvent);
	if (cEvent == CTRL_C_EVENT)
	{
		console.error("CRTL + C");
		SendMessage(mSERVER_INFO.hWnd, WM_DESTROY, 0, 0);
		return TRUE;
	}
	return FALSE;
}

void Logic()
{
	gServer.OnLogic();
}

LRESULT WndProc( HWND hWnd, UINT nMss, WPARAM wPrm, LPARAM lPrm )
{
	console.log( "%s:%d", __FUNCTION__, nMss );
	switch ( nMss )
	{
	case WM_DESTROY:
		return gServer.Free();
	case WM_TIMER:
		return gServer.OnTimer();
	case WM_NETWORK_MESSAGE_1:
		gServer.OnTimer();
		return gServer.OnProc( hWnd, wPrm, lPrm );
	}
	return DefWindowProc( hWnd, nMss, wPrm, lPrm );
}

std::string tolower( const char* s )
{
	std::string str = s;
	for (auto& c : str)
		c = tolower(c);
	return str;
}
std::string toupper( std::string str )
{
	for (auto& c : str)
		c = toupper(c);
	return str;
}
int IsTCP( std::string str )
{
	if( str.length() == 3 && str[0] == 'T' && str[1] == 'C' && str[2] == 'P' ) return SOCK_STREAM;
	return SOCK_DGRAM;
}

#define DEFAULT_LOAD( name, name2 ) \
	mSERVER_INFO.IP[##name##_SERVER] = j[#name2##"_server"]["ip"].get<std::string>(); \
	mSERVER_INFO.Port[##name##_SERVER] = j[#name2##"_server"]["port"].get<int>(); \
	mSERVER_INFO.Type[##name##_SERVER] = IsTCP( toupper( j[#name2##"_server"]["type"].get<std::string>() ) ); \
	mSERVER_INFO.Logic[##name##_SERVER] = j[#name2##"_server"]["logic"].get<int>();

void LoadConfig( const char* path, SERVER_INFO &mSERVER_INFO )
{
	std::string str;
	std::ifstream file;
	try
	{
		file.open( path );
		if( !file.is_open() )
			return;
		if ( !file.good() )
			return;
	}
	catch ( std::exception& e )
	{
		console.error( e.what() );
		throw TSkyException();
	}
	std::getline( std::ifstream( path ), str, '\0' );
	console.log( str.c_str() );
	json j = json::parse( str );
	
	try
	{
		DEFAULT_LOAD( LOG, log );
		DEFAULT_LOAD( CENTER, center );
		DEFAULT_LOAD( EXTRA, extra );
		DEFAULT_LOAD( PLAYUSER, playuser );
		DEFAULT_LOAD( LOGIN, login );
		DEFAULT_LOAD( ZONE, zone );
	}
	catch ( json::exception& e )
	{
		console.error( e.what() );
		throw TSkyException();
	}
}

int main( int argc, char **argv )
{
	console.init( ConsoleHandler );

	//if( argc < 2 )
	//{
	// console.error( "please run with arguments log or center or extra or playuser or zone"  );
	// return;
	//}
	//

	try
	{
		LoadConfig( "./Config.json", mSERVER_INFO );


		//mServer.Init( SOMAXCONN );//TCP
		//mServer.Init( SOMAXCONN );//UDP
		
		#ifdef SKYCORE_LOG
		std::string name = "LogServer";
		//if( !strcmp( argv[1], "log"  ) )
			SWARP_INFO( LOG );
		#endif

		#ifdef SKYCORE_LOGIN
		std::string name =  "LoginServer";
		//if( !strcmp( argv[1], "login"  ) )
			SWARP_INFO( LOGIN );
		#endif

		//else if( !strcmp( argv[1], "center"  ) )
		//	SWARP_INFO( CENTER );
		//else if( !strcmp( argv[1], "extra"  ) )
		//	SWARP_INFO( EXTRA );
		//else if( !strcmp( argv[1], "playuser"  ) )
		//	SWARP_INFO( PLAYUSER );
		//else if( !strcmp( argv[1], "zone"  ) )
		//{
		//	if( argc < 3 )
		//	{
		//		console.error( "please run with arguments zone and zonnumber\n ex. zone 1 = run zone number 1"  );
		//		return 0;
		//	}
		//	SWARP_INFO( ZONE );
		//}

		mAPP.Init( mSERVER_INFO, name, (WNDPROC)WndProc );
		gServer.Init();
		return mAPP.Run( mSERVER_INFO );
	}
	catch ( SkyException& e )
	{
		console.error( "%s", e.what() );
		Sleep( 2000 );
	}
	exit( EXIT_FAILURE );
}