#ifndef SC_EXCEPTION_H
#define SC_EXCEPTION_H

class SkyException
{
private:
	std::string msg;
public:
	SkyException( int line = __LINE__, const char* file = __FILE__ )
	{
		msg = file;
		msg += ":";
		msg += std::to_string( line );
	}
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