#include <api.h>

SkyApp mApp;

BOOL WINAPI ConsoleHandler( DWORD cEvent )
{
    //console.log( "%s:%d", __PRETTY_FUNCTION__, cEvent );
	SendMessage( mApp.hWnd, WM_DESTROY, 0, 0 );
    return TRUE;
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
	mApp.Init( "test", (WNDPROC)WndProc, (std::function<void()>)Logic );
	return mApp.Run();
}