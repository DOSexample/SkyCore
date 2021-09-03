#ifndef INC_API_H
#define INC_API_H

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define strdup _strdup

#include <Windows.h>
#include <functional>
#include <time.h>
#include <stdio.h>
#include <string>
#include <ostream>
#include <fstream>
#include <WinSock2.h>
#pragma comment( lib, "ws2_32.lib" )

#ifdef _MSC_VER 
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif


typedef std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> wnd_proc_t;


#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <rang/rang.hpp>


#define WM_NETWORK_MESSAGE_1	(WM_APP+1)
#define WM_NETWORK_MESSAGE_5	(WM_APP+5)
#define DEBUG() console.log( "%s:%s:%d", strrchr(__FILE__,FILE_SPLIT_FLAG)+1, __FUNCTION__, __LINE__ )


#define SERVER_TYPE_NUM			7
#define LOG_SERVER				0
#define CENTER_SERVER			1
#define EXTRA_SERVER			2
#define PLAYUSER_SERVER			3
#define LOGIN_SERVER			4
#define ZONE_SERVER				5
#define MAIN_SERVER				6

#ifndef USE_TICK64
#define USE_TICK64
typedef DWORD64 TIMETICK;
#else
typedef DWORD TIMETICK;
#endif

typedef struct {
	std::string IP[SERVER_TYPE_NUM];
	int Port[SERVER_TYPE_NUM];
	int Type[SERVER_TYPE_NUM];
	int Logic[SERVER_TYPE_NUM];
	int MaxUser[SERVER_TYPE_NUM];
	int mMaxUserNum;

	WNDCLASSEX wCls;
	HWND hWnd;
	TIMETICK mTimeLogic;
	TIMETICK mBaseTickCountForLogic;
	TIMETICK mPostTickCountForLogic;
	BOOL mCheckLogicFlag;
} SERVER_INFO;
extern SERVER_INFO mSERVER_INFO;


#include <SkyException.h>
#include <SkyTime.h>
#include <SkyApplication.h>
#include <SkyConsole.h>
#include <SkyBuffer.h>

#include <SkyUser.h>
#include <SkyServer.h>



#endif //INC_API_H