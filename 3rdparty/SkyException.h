#ifndef SC_EXCEPTION_H
#define SC_EXCEPTION_H

#define SCTHROW() SkyException( __FILE__, __LINE__ )
#define SCTHROW_ARG( ... ) SkyException( __FILE__, __LINE__, __VA_ARGS__ )
class SkyException
{
private:
	std::string msg;
public:
	SkyException( const char* file, int line )
	{
		SkyException( file, line );
	}
	SkyException( const char* file, int line, const char* fmt, ... )
	{
		char tempStr[2048];
		va_list args;
		va_start( args, fmt );
		vsprintf( tempStr, fmt, args );
		va_end( args );
		msg = file;
		msg += ":";
		msg += std::to_string( line );
		msg += " >> ";
		msg += tempStr;
	}
	~SkyException() {}

	const char* what() { return msg.c_str(); }
};

#endif //SC_EXCEPTION_H