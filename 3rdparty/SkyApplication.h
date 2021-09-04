#ifndef SC_APPLICATION_H
#define SC_APPLICATION_H

extern void Logic();
class SkyApplication
{
public:
	void Init( SERVER_INFO& mSERVER_INFO, std::string name, WNDPROC proc )
	{
		mSERVER_INFO.wCls = { sizeof(WNDCLASSEX), 0, proc, 0, 0, NULL, 0, 0, 0, 0, name.c_str(), 0 };
		RegisterClassEx( &mSERVER_INFO.wCls );
		mSERVER_INFO.hWnd = CreateWindowEx( 0, name.c_str(), name.c_str(), WS_DISABLED, 0, 0, 0, 0, HWND_DESKTOP, NULL, NULL, NULL );
		if( !mSERVER_INFO.hWnd )
			throw SCTHROW_ARG( "!CreateWindowEx()" );
	}
	int Run( SERVER_INFO& mSERVER_INFO )
	{
		MSG nMss;
		HACCEL hAccelTable = LoadAccelerators( NULL, "" );
		while ( GetMessage( &nMss, NULL, 0, 0 ) )
		{
			if ( !TranslateAccelerator( nMss.hwnd, hAccelTable, &nMss ) )
			{
				TranslateMessage( &nMss );
				DispatchMessage( &nMss );
			}
			Logic();
		}
		UnregisterClass( mSERVER_INFO.wCls.lpszClassName, mSERVER_INFO.wCls.hInstance );
		return (int)nMss.wParam;
	}
};


#endif //SC_APPLICATION_H