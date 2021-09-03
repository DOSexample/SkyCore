#include <00_Object.cpp>

LRESULT WndProc( HWND hWnd, UINT nMss, WPARAM wPrm, LPARAM lPrm )
{
	console.log( "%s:%d", __FUNCTION__, nMss );
	mServer.OnTimer( mBaseTickCountForLogic, mPostTickCountForLogic, mCheckLogicFlag );
	switch ( nMss )
	{
	case WM_DESTROY:
		return mServer.Free( mApp.hWnd );
	case WM_TIMER:
		return 0;
	case WM_NETWORK_MESSAGE_1:
		return mServer.OnProc( hWnd, wPrm, lPrm );
	}
	return DefWindowProc( hWnd, nMss, wPrm, lPrm );
}

int main( int argc, char **argv )
{
	console.init( ConsoleHandler );

	mApp.Init( "test", (WNDPROC)WndProc );

	try
	{
		//mServer.Init( SOCK_STREAM, IPPROTO_IP, mApp.hWnd, "127.0.0.1", 10000, WM_NETWORK_MESSAGE_1, FD_ACCEPT, SOMAXCONN );//TCP
		//mServer.Init( SOCK_DGRAM,  IPPROTO_IP, mApp.hWnd, "127.0.0.1", 10000, WM_NETWORK_MESSAGE_1, FD_READ,   SOMAXCONN );//UDP
		mServer.Init( SOCK_STREAM, IPPROTO_IP, mApp.hWnd, "127.0.0.1", 10000, WM_NETWORK_MESSAGE_1 );
		mServer.SetLogic( mApp.hWnd, mBaseTickCountForLogic, mPostTickCountForLogic, mCheckLogicFlag, 1, NULL, 1000 );
		InitSkyUser( 1000 );
		return mApp.Run();
	}
	catch ( SkyException& e )
	{
		console.error( "%s", e.what() );
	}
	exit( EXIT_FAILURE );
}