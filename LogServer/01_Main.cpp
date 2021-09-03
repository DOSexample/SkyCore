#include <api.h>

SkyApplication mApp;
SkyConsole console;

BOOL WINAPI ConsoleHandler( DWORD cEvent )
{
	console.log( "%s:%d", __PRETTY_FUNCTION__, cEvent );
	if ( cEvent == CTRL_C_EVENT )
	{
		SendMessage( mApp.hWnd, WM_DESTROY, 0, 0 );
		return TRUE;
	}
	return FALSE;
}

void Logic()
{
}

LRESULT WndProc( HWND hWnd, UINT nMss, WPARAM wPrm, LPARAM lPrm )
{
	switch ( nMss )
	{
	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;
	}
	return DefWindowProc( hWnd, nMss, wPrm, lPrm );
}
int main( int argc, char **argv )
{
	console.init( ConsoleHandler );

	mApp.Init( "test", (WNDPROC)WndProc, (std::function<void()>)Logic );

	SkyBuffer* m;
	try {
		m = new SkyBuffer(100);
		return mApp.Run();
	}
	catch ( SkyException& e )
	{
		console.error( "%s", e.what() );
	}
	exit( EXIT_FAILURE );


}