#include <api.h>

void SkyWork::Init()
{
	for ( int i = 0; i < MAX_WORK; i++ )
	{
		W_FUNCTION[i].PROC = NULL;
		W_FUNCTION[i].SIZE = 0;
	}
}

//void SkyWork::Process( SkyUser* tUserInfo, int tRecvSizeFromUser )
//{
//	while ( TRUE )
//	{
//		#if defined SKYCORE_LOGIN || defined SKYCORE_ZONE
//		if ( tUserInfo->mTotalRecvSize < 9 )
//		#else
//		if ( tUserInfo->mTotalRecvSize < 1 )
//		#endif
//		{
//			return;
//		}
//		//if ( mSERVER_INFO.mTestServer )
//		//{
//		//	console.log( "recv:%d", tRecvSizeFromUser );
//		//}
//		BYTE tProtocol = ((CL_DEFAULT_PACKET*)tUserInfo->mBUFFER_FOR_RECV.Get(0))->tProtocol;
//		if ( W_FUNCTION[tProtocol].PROC == NULL )
//		{
//			console.error( "!Block User[%d] - Unknown Header[%d], Length[%d].", tUserInfo->mIndex, tProtocol, tRecvSizeFromUser );
//			tUserInfo->Quit();
//			return;
//		}
//		DWORD tProtocolSize = W_FUNCTION[tProtocol].SIZE;
//		#ifdef UNITY
//		if ( !tUserInfo->mFirstRecv )
//		{
//			tUserInfo->mPlatform = UPCORIGIN;
//			if ( tRecvSizeFromUser == 83 )
//			{
//				tUserInfo->mPlatform = UANDROID;
//			}
//			tUserInfo->mFirstRecv = TRUE;
//		}
//		if ( tUserInfo->mPlatform != UPCORIGIN )
//		{
//			tProtocolSize = ((CL_DEFAULT_PACKET*)&tUserInfo->mBUFFER_FOR_RECV[0])->tPacket1;
//		}
//		#endif
//		if ( tUserInfo->mTotalRecvSize < tProtocolSize )
//		{
//			console.error( "!Did not receive the whole packet. Size(%d) Remains (%d) : Packet ID(%d) .", tProtocolSize, tProtocolSize - tUserInfo->mTotalRecvSize, tProtocol );
//			return;
//		}
//		W_FUNCTION[tProtocol].PROC( tUserInfo );
//		if ( tUserInfo->mCheckConnectState )
//		{
//			if ( tUserInfo->mPlatform == UANDROID )
//			{
//				tProtocolSize = ToInt( tUserInfo->mBUFFER_FOR_RECV.Get(0) );
//			}
//			MoveMemory( tUserInfo->mBUFFER_FOR_RECV.Get(0), tUserInfo->mBUFFER_FOR_RECV.Get(tProtocolSize), ( tUserInfo->mTotalRecvSize - tProtocolSize ) );
//			tUserInfo->mTotalRecvSize -= tProtocolSize;
//			if ( tUserInfo->mTotalRecvSize < 0 )
//			{
//				tUserInfo->mTotalRecvSize = 0;
//			}
//		}
//	}
//}