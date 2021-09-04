#ifndef SC_WORK_H
#define SC_WORK_H

typedef struct {
	void (*PROC)( SkyUser* tUserInfo );
	int SIZE;
} WFUNCTION;
#define MAX_WORK	256
#pragma pack(push,1)
typedef struct {
	int tPacket1;
	int tPacket2;
	BYTE tProtocol;
} CL_DEFAULT_PACKET;
#pragma pack(pop)
#define UPCORIGIN	-1
#define UWINDOW		0
#define UANDROID	1
static int ToInt( void* ptr )
{
	int value;
	CopyMemory( &value, ptr, 4 );
	return value;
}
class SkyWork
{

public:
	WFUNCTION W_FUNCTION[MAX_WORK];
	void Init();

	void Free() {}

	void Process( SkyUser* tUserInfo )
	{
		while ( TRUE )
		{
			#if defined SKYCORE_LOGIN || defined SKYCORE_ZONE
			if ( tUserInfo->mTotalRecvSize < 9 )
			#else
			if ( tUserInfo->mTotalRecvSize < 1 )
			#endif
			{
				return;
			}
			//if ( mSERVER_INFO.mTestServer )
			//{
			//	console.log( "recv:%d", tRecvSizeFromUser );
			//}
			#if defined SKYCORE_LOGIN || defined SKYCORE_ZONE
			BYTE tProtocol = *tUserInfo->mBUFFER_FOR_RECV.Get(8);//from buffer ->client to login/zone
			#else
			BYTE tProtocol = *tUserInfo->mBUFFER_FOR_RECV.Get(0);//from buffer ->server to server
			#endif
			if ( W_FUNCTION[tProtocol].PROC == NULL )
			{
				console.error( "!Block User[%d] - Unknown Header[%d], Length[%d].", tUserInfo->mIndex, tProtocol, tUserInfo->mTotalRecvSize );
				tUserInfo->Quit();
				return;
			}
			DWORD tProtocolSize = W_FUNCTION[tProtocol].SIZE;
			#ifdef UNITY
			if ( !tUserInfo->mFirstRecv )
			{
				tUserInfo->mPlatform = UPCORIGIN;
				if ( tRecvSizeFromUser == 83 )
				{
					tUserInfo->mPlatform = UANDROID;
				}
				tUserInfo->mFirstRecv = TRUE;
			}
			if ( tUserInfo->mPlatform != UPCORIGIN )
			{
				tProtocolSize = ((CL_DEFAULT_PACKET*)&tUserInfo->mBUFFER_FOR_RECV[0])->tPacket1;
			}
			#endif
			if ( tUserInfo->mTotalRecvSize < tProtocolSize )
			{
				console.error( "!Did not receive the whole packet. Size(%d) Remains (%d) : Packet ID(%d) .", tProtocolSize, tProtocolSize - tUserInfo->mTotalRecvSize, tProtocol );
				return;
			}
			W_FUNCTION[tProtocol].PROC( tUserInfo );
			if ( tUserInfo->mCheckConnectState )
			{
				if ( tUserInfo->mPlatform == UANDROID )
				{
					tProtocolSize = ToInt( tUserInfo->mBUFFER_FOR_RECV.Get(0) );
				}
				MoveMemory( tUserInfo->mBUFFER_FOR_RECV.Get(0), tUserInfo->mBUFFER_FOR_RECV.Get(tProtocolSize), ( tUserInfo->mTotalRecvSize - tProtocolSize ) );
				tUserInfo->mTotalRecvSize -= tProtocolSize;
				if ( tUserInfo->mTotalRecvSize < 0 )
				{
					tUserInfo->mTotalRecvSize = 0;
				}
			}
		}
	}
};
extern SkyWork mWORK;
#endif //SC_WORK_H