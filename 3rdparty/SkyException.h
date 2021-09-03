#ifndef SC_EXCEPTION_H
#define SC_EXCEPTION_H

#define TSkyException() SkyException( __FILE__, __LINE__  )
class SkyException
{
private:
	std::string msg;
public:
	SkyException( const char* file, int line )
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
	void set( const char* fmt, ...  )
	{
		char tempStr[2048];
		va_list args;
		va_start( args, fmt );
		vsprintf( tempStr, fmt, args );
		va_end( args );
		msg = tempStr;
	}
};

#endif //SC_EXCEPTION_H