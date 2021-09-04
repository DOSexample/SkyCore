#ifndef SKYSERVER_H
#define SKYSERVER_H

class SkyServer
{
private:
	SOCKET mSocket;
public:
	SkyServer() : mSocket(0)
	{
	}

	BOOL WSAStartup()
	{
		WSADATA mWsaData;
		ZeroMemory( &mWsaData, sizeof( WSADATA ) );
		return ::WSAStartup( MAKEWORD( 2, 2 ), &mWsaData ) == 0;
	}
	BOOL Init( long backlog = 128 )
	{
		HWND hWnd = mSERVER_INFO.hWnd;
		const char* ip = mSERVER_INFO.IP[MAIN_SERVER].c_str();
		u_short port = mSERVER_INFO.Port[MAIN_SERVER] + mSERVER_INFO.mServerNumber;
		int type = mSERVER_INFO.Type[MAIN_SERVER];
		SOCKADDR_IN mAddress = {};
		
		if ( !WSAStartup() )
		{
			throw SCTHROW_ARG( "!WSAStartup()" );
		}
		mSocket = ::socket( AF_INET, type, IPPROTO_IP );
		if ( mSocket == INVALID_SOCKET )
		{
			throw SCTHROW_ARG( "!socket()" );
		}
		if ( ( type == SOCK_STREAM && !SetSocket( mSocket, TRUE ) ) )
		{
			throw SCTHROW_ARG( "!SetSocket()" );
		}
		mAddress.sin_family = AF_INET;
		mAddress.sin_addr.s_addr = inet_addr( ip );
		mAddress.sin_port = htons( port );
		if ( ::bind( mSocket, (LPSOCKADDR) &mAddress, sizeof( mAddress ) ) == INVALID_SOCKET )
		{
			throw SCTHROW_ARG( "!bind()" );
		}
		if ( ( type == SOCK_STREAM ) && ( ::listen( mSocket, backlog ) == INVALID_SOCKET ) )
		{
			throw SCTHROW_ARG( "!listen()" );
		}
		if ( type == SOCK_STREAM )
		{
			if( ( ::WSAAsyncSelect( mSocket, hWnd, WM_NETWORK_MESSAGE_1, FD_ACCEPT ) == INVALID_SOCKET ) )
			{
				throw SCTHROW_ARG( "!WSAAsyncSelect()" );
			}
		}
		else//log server
		{
			if( ( ::WSAAsyncSelect( mSocket, hWnd, WM_NETWORK_MESSAGE_1, FD_READ ) == INVALID_SOCKET ) )
			{
				throw SCTHROW_ARG( "!WSAAsyncSelect()" );
			}
		}
		
		mUSER.clear();
		for ( int i = 0; i < mSERVER_INFO.mMaxUserNum; i++ )
		{
			SkyUser* t = new SkyUser();
			if( !t )
			{
				throw SCTHROW_ARG( "!new SkyUser(%d)", i );
			}
			mUSER.push_back( t );
		}
		if ( (int)mUSER.size() != mSERVER_INFO.mMaxUserNum )
		{
			throw SCTHROW_ARG( "SkyUser :: error size( (%d) != (%d) )", mUSER.size(), mSERVER_INFO.mMaxUserNum );
		}
		console.log( "size( (%d) != (%d) )", mUSER.size(), mSERVER_INFO.mMaxUserNum );
		for ( int i = 0; i < mSERVER_INFO.mMaxUserNum; i++ )
		{
			mUSER[i]->Init( i, mSERVER_INFO.mMaxSendSize, mSERVER_INFO.mMaxRecvSize );
		}

		mSERVER_INFO.mTimeLogic -= 1;
		if ( (int)mSERVER_INFO.mTimeLogic < 1 ) {
			mSERVER_INFO.mTimeLogic = 1;
		}
		mSERVER_INFO.mBaseTickCountForLogic = SkyTime::GetTickCount();
		mSERVER_INFO.mPostTickCountForLogic = mSERVER_INFO.mBaseTickCountForLogic;
		if( !SetTimer( mSERVER_INFO.hWnd, 1, (UINT)mSERVER_INFO.mTimeLogic, NULL ) )
			throw SCTHROW_ARG( "!SetTimer()" );

		ConsoleTitle( TRUE );

		return TRUE;
	}

	std::string GetMaxUser()
	{
		if ( mUSER.size() < 1 )
		{
			return "0/0";
		}
		int i = 0;
		for( auto it : mUSER )
		{
			if ( it->mCheckConnectState )
			{
				i++;
			}
		}
		return std::to_string( i ) + "/" + std::to_string( mUSER.size() );
	}

	void ConsoleTitle( BOOL isStartup = FALSE )
	{
		char buffer[1024];
		const char* ip = mSERVER_INFO.IP[MAIN_SERVER].c_str();
		u_short port = mSERVER_INFO.Port[MAIN_SERVER] + mSERVER_INFO.mServerNumber;
		int type = mSERVER_INFO.Type[MAIN_SERVER];
		int zone = mSERVER_INFO.mServerNumber;
		const char* app = mSERVER_INFO.AppName.c_str();

		sprintf( buffer, "%s is running on %s://%s:%d | User:%s", app, SOCK_STREAM ? "tcp" : "udp", ip, port, GetMaxUser().c_str() );
		#ifdef SKYCORE_ZONE
			sprintf( &buffer[strlen(buffer)]," | Zone:%d", zone );
		#endif
		
		SetConsoleTitleA( buffer );
		if( isStartup )
		{
			console.log( buffer);
		}
	}


	#ifdef SKYCORE_WEB
	httplib::Server svr;
	void InitWeb()
	{
		// Mount / to ./www directory
		auto ret = svr.set_mount_point( "/",  mSERVER_INFO.WebDir.c_str() );
		if ( !ret ) {
		  // The specified base directory doesn't exist...
		}
		svr.set_error_handler([](const auto& req, auto& res) {
			auto fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
			char buf[BUFSIZ];
			snprintf(buf, sizeof(buf), fmt, res.status);
			res.set_content(buf, "text/html");
		});
		svr.set_exception_handler([](const auto& req, auto& res, std::exception& e) {
			res.status = 500;
			auto fmt = "<h1>Error 500</h1><p>%s</p>";
			char buf[BUFSIZ];
			snprintf(buf, sizeof(buf), fmt, e.what());
			res.set_content(buf, "text/html");
		});
		svr.listen( mSERVER_INFO.IP[WEB_SERVER].c_str(), mSERVER_INFO.Port[WEB_SERVER] ) ;
	}
	#endif

	LRESULT Free()
	{
		for ( auto it : mUSER )
		{
			it->Quit();
		}
		mUSER.clear();

		closesocket( mSocket );
		KillTimer( mSERVER_INFO.hWnd, 1 );
		PostQuitMessage( 0 );
		return 0;
	}

	BOOL SetSocket( SOCKET tSocket, BOOL tServer )
	{
		//int RCVBUF = 20480;
		//int SNDBUF = 204800;
		int flag = 1;
		u_long iMode = 1;
		if ( ::ioctlsocket( tSocket, FIONBIO, &iMode ) != 0 )
		{
			return FALSE;
		}
		if ( ::setsockopt( tSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag) ) != 0 )
		{
			return FALSE;
		}
		/*if ( setsockopt( tSocket, SOL_SOCKET, SO_SNDBUF, (char *)&SNDBUF, sizeof(SNDBUF) ) == SOCKET_ERROR )
		{
			logError( "SO_SNDBUF" );
			return FALSE;
		}
		if ( setsockopt( tSocket, SOL_SOCKET, SO_RCVBUF, (char *)&RCVBUF, sizeof(RCVBUF) ) == SOCKET_ERROR )
		{
			logError( "SO_RCVBUF" );
			return FALSE;
		}*/

		#ifdef USE_KEEPALIVE_SOCKET
		int iResult = 0;
		BOOL bOptVal = FALSE;
		int bOptLen = sizeof(BOOL);

		int iOptVal = 0;
		int iOptLen = sizeof(int);

		bOptVal = TRUE;

		if ( !tServer )
		{
		iResult = setsockopt( tSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&bOptVal, bOptLen );
		if ( iResult == SOCKET_ERROR ) {
			wprintf( L"setsockopt for SO_KEEPALIVE failed with error: %u\n", WSAGetLastError() );
			return FALSE;
		}
		iResult = getsockopt( tSocket, SOL_SOCKET, SO_KEEPALIVE, (char *) &iOptVal, &iOptLen );
		if ( iResult == SOCKET_ERROR ) {
			wprintf( L"getsockopt for SO_KEEPALIVE failed with error: %u\n", WSAGetLastError());
			return FALSE;
		}
		}
		#endif

		return TRUE;
	}
	
	void OnLogic()
	{
		DEBUG();
		if ( mSERVER_INFO.mCheckLogicFlag )
		{
			//mGAME.Logic( ( (float)mSERVER_INFO.mTimeLogic * 0.001f ) );
			mSERVER_INFO.mCheckLogicFlag = FALSE;
			ConsoleTitle();
		}
	}
	LRESULT OnTimer()
	{
		DEBUG();
		mSERVER_INFO.mBaseTickCountForLogic = SkyTime::GetTickCount();
		if ( ( mSERVER_INFO.mBaseTickCountForLogic - mSERVER_INFO.mPostTickCountForLogic ) >= mSERVER_INFO.mTimeLogic )
		{
			mSERVER_INFO.mPostTickCountForLogic = mSERVER_INFO.mBaseTickCountForLogic;
			mSERVER_INFO.mCheckLogicFlag = TRUE;
		}
		OnLogic();
		return 0;
	}

	LRESULT OnProc( HWND hWnd, WPARAM wPrm, LPARAM lPrm )
	{
		switch ( WSAGETSELECTEVENT( lPrm ) )
        {
        case FD_ACCEPT  :   OnAccept();							break;    //TCP/UDP Incoming new client
        case FD_CLOSE   :   OnClose ( wPrm, GetUser( wPrm ) );	break;    //TCP/UDP socket disconnected
        case FD_READ    :   OnRecv  ( wPrm, GetUser( wPrm ) );	break;    //TCP/UDP Incoming data to receive
        case FD_WRITE   :   OnSend  ( wPrm, GetUser( wPrm ) );	break;    //TCP/UDP Outcoming data to send
        }
		OnTimer();
		return 0;
	}

	SkyUser* GetUser( WPARAM wPrm )
	{
		SkyUser* tUSER = NULL;
		for ( auto it : mUSER )
		{
			if ( it->mCheckConnectState && it->mSocket == (WPARAM)wPrm )
			{
				tUSER = it;
				break;
			}
		}
		return tUSER;
	}

	void OnAccept()
	{
		DEBUG();
		SkyUser* tUSER = NULL;
		SOCKET tSocket;
		int tAddressLength;
		SOCKADDR_IN	tAddress;

		tAddressLength = sizeof( tAddress );
		tSocket = accept( mSocket, (LPSOCKADDR)&tAddress, &tAddressLength );
		if( tSocket == INVALID_SOCKET )
		{
			DEBUG();
			return;
		}
		for ( auto it : mUSER )
		{
			if ( !it->mCheckConnectState )
			{
				tUSER = it;
				break;
			}
		}
		if ( tUSER == NULL )
		{
			DEBUG();
			closesocket( tSocket );
			return;
		}
		if ( !SetSocket( tSocket, FALSE ) )
		{
			DEBUG();
			closesocket( tSocket );
			return;
		}
		if( WSAAsyncSelect( tSocket, mSERVER_INFO.hWnd, WM_NETWORK_MESSAGE_1, ( FD_READ | FD_WRITE | FD_CLOSE ) ) == SOCKET_ERROR )
		{
			DEBUG();
			closesocket( tSocket );
			return;
		}
		tUSER->Set( UREADY, tSocket, inet_ntoa( tAddress.sin_addr  ) );
		DEBUG();
	}
	
	void OnClose( WPARAM wPrm, SkyUser* tUSER )
	{
		DEBUG();
		if ( tUSER )
		{
			tUSER->Quit();
		}
	}

	void OnRecv( WPARAM wPrm, SkyUser* tUSER )
	{
		DEBUG();
		if ( mSERVER_INFO.Type[MAIN_SERVER] == SOCK_DGRAM )//log server
		{
			OnLogRecv( (SOCKET)wPrm );
			return;
		}
		if ( tUSER == NULL )
		{
			return;
		}
		while ( TRUE )
		{
			int tRecvSize = recv( tUSER->mSocket, (char*)tUSER->mBUFFER_FOR_RECV.Get(tUSER->mTotalRecvSize), ( mSERVER_INFO.mMaxRecvSize - tUSER->mTotalRecvSize ), 0 );
			if ( tRecvSize <= 0 )
			{
				if ( tRecvSize == SOCKET_ERROR )
				{
					if ( WSAGetLastError() == WSAEWOULDBLOCK )
					{
						return;
					}
				}
				tUSER->Quit();
				return;
			}
			#if defined SKYCORE_LOGIN || defined SKYCORE_ZONE
			if ( tUSER->mPacketEncryptionValue != 0 )
			{
				for ( DWORD i = tUSER->mTotalRecvSize; i < ( tUSER->mTotalRecvSize + tRecvSize ); i++ )
				{
					*tUSER->mBUFFER_FOR_RECV.Get(i) ^= tUSER->mPacketEncryptionValue;
				}
			}
			#endif
			tUSER->mTotalRecvSize += tRecvSize;
			tUSER->mBUFFER_FOR_RECV.Set( tUSER->mTotalRecvSize );
			break;
		}
		mWORK.Process( tUSER );
	}

	void OnSend( WPARAM wPrm, SkyUser* tUSER )
	{
		DEBUG();
		if ( tUSER )
		{
			tUSER->Send( FALSE, NULL );
		}
	}

	void OnLogRecv( SOCKET tSocket )
	{

	}
};

#endif //SKYSERVER_H