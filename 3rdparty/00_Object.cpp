#include <api.h>

SkyApplication mApp;
SkyConsole console;
SkyUserMap mUSER;
SkyServer mServer;
TIMETICK mBaseTickCountForLogic;
TIMETICK mPostTickCountForLogic;
BOOL mCheckLogicFlag;

BOOL WINAPI ConsoleHandler( DWORD cEvent )
{
	console.log( "%s:%d", __PRETTY_FUNCTION__, cEvent );
	if ( cEvent == CTRL_C_EVENT )
	{
		console.error( "CRTL + C" );
		Sleep( 10000 );
		SendMessage( mApp.hWnd, WM_DESTROY, 0, 0 );
		return TRUE;
	}
	return FALSE;
}

void Logic()
{
	mServer.OnLogic( mCheckLogicFlag );
}