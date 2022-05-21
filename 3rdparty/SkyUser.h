#ifndef SKYUSER_H
#define SKYUSER_H

#ifdef _MSC_VER
#define FILE_SPLIT_FLAG '\\'
#else
#define FILE_SPLIT_FLAG '/'
#endif
#define Quit() Exit( strrchr(__FILE__,FILE_SPLIT_FLAG)+1, __FUNCTION__, __LINE__ )
#define UCLOSE	0
#define UINIT	1
#define UREADY	2

#include <vector>

class SkyUser
{
public:
	SOCKET mSocket;
	BOOL mCheckConnectState;
	DWORD mTotalSendSize;
	DWORD mTotalRecvSize;
	SkyBuffer mBUFFER_FOR_SEND;
	SkyBuffer mBUFFER_FOR_RECV;
	BYTE mPacketEncryptionValue;
	std::string mIP;

	TIMETICK mConnectTime;
	TIMETICK mRegisterTime;
	TIMETICK mUseTime;

	int mPlatform;

public:

	int mIndex;

	void Init( int tIndex, int tSendSize, int tRecvSize )
	{
		mIndex = tIndex;
		mBUFFER_FOR_SEND.Init( tSendSize );
		mBUFFER_FOR_RECV.Init( tRecvSize );
		Set( UINIT, INVALID_SOCKET, "0.0.0.0" );
	}

	void Send( BOOL tCheckBuffer, SkyBuffer* tBuffer )
	{
		if ( !mCheckConnectState )
		{
			return;
		}
		if ( mTotalSendSize < 1 )
		{
			mBUFFER_FOR_SEND.Set( mTotalSendSize );
			return;
		}
		if ( mBUFFER_FOR_SEND.Get(0) == NULL )
		{
			return;
		}
		if ( tCheckBuffer && tBuffer )
		{
			mBUFFER_FOR_SEND.Pack( tBuffer->Get(mTotalSendSize), tBuffer->Size() );
			mTotalSendSize += tBuffer->Size();
		}

		int tSendValue01 = 0;
		int tNetworkMsg = 0;

		while ( mTotalSendSize > 0 )
		{
			tSendValue01 = send( mSocket, (const char*)mBUFFER_FOR_SEND.Get(0), mTotalSendSize, 0 );
			if ( tSendValue01 <= 0 )
			{
				if ( tSendValue01 == 0 )
				{
					Quit();
					goto END;
				}
				tNetworkMsg = h_errno;
				if ( tNetworkMsg == WSAEINTR )
				{
					continue;
				}
				switch ( tNetworkMsg )
				{
				case 0:
				case WSATRY_AGAIN:
				case WSAEWOULDBLOCK:
					break;
				default:
					#ifdef USE_KEEPALIVE_SOCKET
					if ( tNetworkMsg == WSAECONNRESET )
					{
						mSendErrorCount++;
						if ( mSendErrorCount < 20 )
						{
							goto END;
						}
					}
					#endif
					console.error( "WSA Error : %d,%d", mIndex, tNetworkMsg );
					Quit();
					break;
				}
				goto END;
			}
			if ( ( mTotalSendSize - tSendValue01 ) > 0 )
			{
				mBUFFER_FOR_SEND.Move( tSendValue01, ( mTotalSendSize - tSendValue01 ) );
			}
			mTotalSendSize -= tSendValue01;
		}
		END:
		;
	}

	void Exit( const char* file, const char* function, int line )
	{
		if( !mCheckConnectState )
		{
			return;
		}
		console.error( "mUSER[%d].Quit(%s,%s,%d);", mIndex, file, function, line );
		Set( UCLOSE, INVALID_SOCKET, "0.0.0.0" );
	}
	void Set( int tStatus, SOCKET tSocket, const char* tIP )
	{
		mTotalSendSize = 0;
		mTotalRecvSize = 0;
		mIP = tIP;
		if ( tStatus == UREADY )
		{
			mCheckConnectState = TRUE;
			mSocket = tSocket;
			mPacketEncryptionValue = 0;

			mConnectTime = SkyTime::GetTickCount();
		}
		else
		{
			mCheckConnectState = FALSE;
			closesocket( mSocket );
			mSocket = tSocket;
		}
	}
};
typedef std::vector<SkyUser*> SkyUserMap;
extern SkyUserMap mUSER;

#endif //SKYUSER_H
