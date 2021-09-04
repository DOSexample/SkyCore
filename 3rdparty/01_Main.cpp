#include <api.h>


#define DEFAULT_LOAD( name, name2 ) \
mSERVER_INFO.IP[##name##_SERVER] = j[#name2##"_server"]["ip"].get<std::string>(); \
mSERVER_INFO.Port[##name##_SERVER] = j[#name2##"_server"]["port"].get<int>(); \
mSERVER_INFO.Type[##name##_SERVER] = IsTCP( toupper( j[#name2##"_server"]["type"].get<std::string>() ) ); \
mSERVER_INFO.Logic[##name##_SERVER] = j[#name2##"_server"]["logic"].get<int>(); \
mSERVER_INFO.MaxUser[##name##_SERVER] = j[#name2##"_server"]["maxuser"].get<int>(); \
mSERVER_INFO.MaxRecv[##name##_SERVER] = j[#name2##"_server"]["maxrecv"].get<int>(); \
mSERVER_INFO.MaxSend[##name##_SERVER] = j[#name2##"_server"]["maxsend"].get<int>(); \
mSERVER_INFO.MaxTransfer[##name##_SERVER] = j[#name2##"_server"]["maxtransfer"].get<int>();

#define SWAP_INFO( name ) \
mSERVER_INFO.IP[MAIN_SERVER] = mSERVER_INFO.IP[##name##_SERVER]; \
mSERVER_INFO.Port[MAIN_SERVER] = mSERVER_INFO.Port[##name##_SERVER]; \
mSERVER_INFO.Type[MAIN_SERVER] = mSERVER_INFO.Type[##name##_SERVER]; \
mSERVER_INFO.mTimeLogic = (TIMETICK)mSERVER_INFO.Logic[##name##_SERVER]; \
mSERVER_INFO.mMaxUserNum = mSERVER_INFO.MaxUser[##name##_SERVER]; \
mSERVER_INFO.mMaxRecvSize = mSERVER_INFO.MaxRecv[##name##_SERVER]; \
mSERVER_INFO.mMaxSendSize = mSERVER_INFO.MaxSend[##name##_SERVER]; \
mSERVER_INFO.mMaxTransferSize = mSERVER_INFO.MaxTransfer[##name##_SERVER];

SERVER_INFO mSERVER_INFO;
SkyApplication mAPP;
SkyConsole console;
SkyUserMap mUSER;
SkyServer gServer;
SkyWork mWORK;

BOOL WINAPI ConsoleHandler( DWORD CtrlType )
{
	switch ( CtrlType )
	{
	case CTRL_C_EVENT:
		console.error( "CTRL_C_EVENT" );
		SendMessage( mSERVER_INFO.hWnd, WM_DESTROY, 0, 0 );
		return TRUE;
	case CTRL_CLOSE_EVENT:
		console.error( "CTRL_CLOSE_EVENT" );
		SendMessage( mSERVER_INFO.hWnd, WM_DESTROY, 0, 0 );
		return TRUE;
	}
	console.log( "%s:%d", __PRETTY_FUNCTION__, CtrlType );
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
		throw SCTHROW_ARG( e.what() );
	}
	std::getline( std::ifstream( path ), str, '\0' );
	console.log( str.c_str() );
	try
	{
		json j = json::parse( str );
		DEFAULT_LOAD( LOG, log );
		DEFAULT_LOAD( CENTER, center );
		DEFAULT_LOAD( EXTRA, extra );
		DEFAULT_LOAD( PLAYUSER, playuser );
		DEFAULT_LOAD( LOGIN, login );
		DEFAULT_LOAD( ZONE, zone );
		DEFAULT_LOAD( WEB, web );
		#ifdef SKYCORE_WEB
		mSERVER_INFO.WebDir = j["web_server"]["dir"].get<std::string>();
		#endif
	}
	catch ( json::exception& e )
	{
		throw SCTHROW_ARG( e.what() );
	}
}

int main( int argc, char **argv )
{
	console.init();

	try
	{
		mSERVER_INFO.mServerNumber = 0;
		LoadConfig( "./Config.json", mSERVER_INFO );
		
		#ifdef SKYCORE_LOG
		mSERVER_INFO.AppName = "LogServer";
		SWAP_INFO( LOG );
		#endif
		
		#ifdef SKYCORE_CENTER
		mSERVER_INFO.AppName = "CenterServer";
		SWAP_INFO( CENTER );
		#endif
		
		#ifdef SKYCORE_EXTRA
		mSERVER_INFO.AppName = "ExtraServer";
		SWAP_INFO( EXTRA );
		#endif

		#ifdef SKYCORE_LOGIN
		mSERVER_INFO.AppName =  "LoginServer";
		SWAP_INFO( LOGIN );
		#endif
		
		#ifdef SKYCORE_PLAYUSER
		mSERVER_INFO.AppName = "PlayUserServer";
		SWAP_INFO( PLAYUSER );
		#endif
		
		#ifdef SKYCORE_ZONE
		//	if( argc < 2 )
		//	{
		//		console.error( "please run with arguments zonnumber\n ex. zoneserver.exe 1 = run zone number 1"  );
		//		return 0;
		//	}
		mSERVER_INFO.AppName = "ZoneServer";
		SWAP_INFO( ZONE );
		#endif

		#ifdef SKYCORE_WEB
		mSERVER_INFO.AppName = "WebServer";
		SWAP_INFO( WEB );
		#endif

		mAPP.Init( mSERVER_INFO, mSERVER_INFO.AppName, (WNDPROC)WndProc );

		#ifdef SKYCORE_WEB
			gServer.InitWeb();
		#else
			gServer.Init();//SOMAXCONN
		#endif

		return mAPP.Run( mSERVER_INFO );
	}
	catch ( SkyException& e )
	{
		console.error( "SkyException :: %s", e.what() );
	}
	catch ( std::system_error& e )
	{
		console.error( "std::system_error :: %s", e.what() );
	}
	catch ( std::exception& e )
	{
		console.error( "std::exception :: %s", e.what() );
	}
	Sleep( 2000 );
	exit( EXIT_FAILURE );
}