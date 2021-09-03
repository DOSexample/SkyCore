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
		u_short port = (u_short)mSERVER_INFO.Port[MAIN_SERVER];
		int type = mSERVER_INFO.Type[MAIN_SERVER];
		SOCKADDR_IN mAddress = {};
		
		if ( !WSAStartup() )
		{
			throw TSkyException();
		}
		mSocket = ::socket( AF_INET, type, IPPROTO_IP );
		if ( mSocket == INVALID_SOCKET )
		{
			throw TSkyException();
		}
		if ( ( type == SOCK_STREAM && !SetSocket( mSocket, TRUE ) ) )
		{
			throw TSkyException();
		}
		mAddress.sin_family = AF_INET;
		mAddress.sin_addr.s_addr = inet_addr( ip );
		mAddress.sin_port = htons( port );
		if ( ::bind( mSocket, (LPSOCKADDR) &mAddress, sizeof( mAddress ) ) == INVALID_SOCKET )
		{
			throw TSkyException();
		}
		if ( ( type == SOCK_STREAM ) && ( ::listen( mSocket, backlog ) == INVALID_SOCKET ) )
		{
			throw TSkyException();
		}
		if ( type == SOCK_STREAM )
		{
			if( ( ::WSAAsyncSelect( mSocket, hWnd, WM_NETWORK_MESSAGE_1, FD_ACCEPT ) == INVALID_SOCKET ) )
			{
				throw TSkyException();
			}
		}
		else//log server
		{
			if( ( ::WSAAsyncSelect( mSocket, hWnd, WM_NETWORK_MESSAGE_1, FD_READ ) == INVALID_SOCKET ) )
			{
				throw TSkyException();
			}
		}
		
		mUSER.clear();
		for ( int i = 0; i < mSERVER_INFO.mMaxUserNum; i++ )
		{
			SkyUser* t = new SkyUser();
			if( !t )
				throw TSkyException();
			mUSER.push_back( t );
		}
		if ( (int)mUSER.size() != mSERVER_INFO.mMaxUserNum )
		{
			throw SkyException( "SkyUserMap:: error size( (%d) != (%d) )", mUSER.size(), mSERVER_INFO.mMaxUserNum );
		}
		for ( int i = 0; i < mSERVER_INFO.mMaxUserNum; i++ )
		{
			mUSER[i]->Init( i, 1000, 1000 );
		}

		mSERVER_INFO.mBaseTickCountForLogic = SkyTime::GetTickCount();
		mSERVER_INFO.mPostTickCountForLogic = mSERVER_INFO.mBaseTickCountForLogic;
		if( !SetTimer( mSERVER_INFO.hWnd, 1, (UINT)mSERVER_INFO.mTimeLogic, NULL ) )
			throw TSkyException();

		console.log( "Server is running on %s://%s:%d", SOCK_STREAM ? "tcp" : "udp", ip, port );
		return TRUE;
	}

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
		Logic();
		return 0;
	}

	LRESULT OnProc( HWND hWnd, WPARAM wPrm, LPARAM lPrm )
	{
		switch ( WSAGETSELECTEVENT( lPrm ) )
        {
        case FD_ACCEPT  :   OnAccept( hWnd );			break;    //TCP/UDP Incoming new client
        case FD_CLOSE   :   OnClose(  (SOCKET)wPrm );   break;    //TCP/UDP socket disconnected
        case FD_READ    :   OnRecv(   (SOCKET)wPrm );   break;    //TCP/UDP Incoming data to receive
        case FD_WRITE   :   OnSend(  (SOCKET)wPrm );	break;    //TCP/UDP Outcoming data to send
        }
		return 0;
	}

	void OnAccept( HWND hWnd )
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
		if( WSAAsyncSelect( tSocket, hWnd, WM_NETWORK_MESSAGE_1, ( FD_READ | FD_WRITE | FD_CLOSE ) ) == SOCKET_ERROR )
		{
			DEBUG();
			closesocket( tSocket );
			return;
		}
		tUSER->Set( UREADY, tSocket, inet_ntoa( tAddress.sin_addr  ) );
		DEBUG();
	}
	
	void OnClose( SOCKET tSocket )
	{
		DEBUG();
		for ( auto it : mUSER )
		{
			if ( it->mCheckConnectState && it->mSocket == tSocket )
			{
				it->Quit();
				return;
			}
		}
	}

	void OnRecv( SOCKET tSocket )
	{
		DEBUG();

		if ( mSERVER_INFO.Type[MAIN_SERVER] == SOCK_DGRAM )//log server
		{
			OnLogRecv( tSocket );
			return;
		}

		SkyUser* tUSER = NULL;
		int tRecvSizeFromUser;
		DWORD i;

		for ( auto it : mUSER )
		{
			if ( it->mCheckConnectState && it->mSocket == tSocket )
			{
				tUSER = it;
				break;
			}
		}
		if ( tUSER == NULL || tUSER->mBUFFER_FOR_RECV.Get() == NULL )
		{
			return;
		}
		while ( TRUE )
		{
			tRecvSizeFromUser = recv( tSocket, (char*)&tUSER->mBUFFER_FOR_RECV.Get()[tUSER->mTotalRecvSize], 100, 0 );
			if ( tRecvSizeFromUser <= 0 )
			{
				if ( tRecvSizeFromUser == SOCKET_ERROR )
				{
					if ( WSAGetLastError() == WSAEWOULDBLOCK )
					{
						return;
					}
				}
				tUSER->Quit();
				return;
			}
			if ( tUSER->mPacketEncryptionValue != 0 )
			{
				for ( i = tUSER->mTotalRecvSize; i < ( tUSER->mTotalRecvSize + tRecvSizeFromUser ); i++ )
				{
					tUSER->mBUFFER_FOR_RECV.Get()[i] ^= tUSER->mPacketEncryptionValue;
				}
			}
			tUSER->mTotalRecvSize += tRecvSizeFromUser;
			tUSER->mBUFFER_FOR_RECV.SetPosition( tUSER->mTotalRecvSize );
			break;
		}
		OnClose( tSocket );
		//mWORK.WorkerProcess( tUSER, tRecvSizeFromUser );
	}

	void OnSend( SOCKET tSocket )
	{
		DEBUG();
		for ( auto it : mUSER )
		{
			if ( it->mCheckConnectState && it->mSocket == tSocket )
			{
				it->Send( FALSE, NULL );
				return;
			}
		}
	}

	void OnLogRecv( SOCKET tSocket )
	{

	}
};

#endif //SKYSERVER_H