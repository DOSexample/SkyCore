#ifndef INC_API_H
#define INC_API_H

#include <Windows.h>
#include <functional>


typedef std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> wnd_proc_t;


#include "json.h"
#include "SkyCommon/Application.h"


#endif //INC_API_H