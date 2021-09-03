#ifndef INC_API_H
#define INC_API_H

#define _CRT_SECURE_NO_WARNINGS
#define strdup _strdup

#include <Windows.h>
#include <functional>


typedef std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> wnd_proc_t;


#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <rang/rang.hpp>

#include "SkyCommon/DateTime.h"
#include "SkyCommon/Application.h"
#include "SkyCommon/Console.h"



#ifdef _MSC_VER 
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif


#endif //INC_API_H