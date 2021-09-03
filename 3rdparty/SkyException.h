#ifndef SC_EXCEPTION_H
#define SC_EXCEPTION_H

class SkyException
{
public:
	std::string msg;
	SkyException( const char* fmt, ... )
	{
		char tempStr[2048];
		va_list args;
		va_start( args, fmt );
		vsprintf( tempStr, fmt, args );
		va_end( args );
		msg = tempStr;
	}
	~SkyException() {}

	const char* what() { return msg.c_str(); }
};

#endif //SC_EXCEPTION_H