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
#define DEBUG() console.log( "%s:%s:%d", strrchr(__FILE__,FILE_SPLIT_FLAG)+1, __FUNCTION__, __LINE__ )

#include <SkyException.h>
#include <SkyTime.h>
#include <SkyApplication.h>
#include <SkyConsole.h>
#include <SkyBuffer.h>

#include <SkyUser.h>
#include <SkyServer.h>



#endif //INC_API_H