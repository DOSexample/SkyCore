#ifndef SC_DATETIME_H
#define SC_DATETIME_H

static int CUSTOM_YEAR;
static int CUSTOM_MONTH;
static int CUSTOM_DAY;
static int CUSTOM_WDAY;
static int CUSTOM_HOUR;
static int CUSTOM_MINUTE;
static int CUSTOM_SECOND;
static const float TICK_SECOND = 1000.0f;
static const float TICK_MINUTE = (TICK_SECOND * 60.0f);
static const float TICK_HOUR = (TICK_MINUTE * 60.0f);
static const float TICK_DAY = (TICK_HOUR * 24.0f);
static time_t COUNT_SECONDS;
static struct tm* PRESENT_TIME;
inline static int GetSecondFromTick(float tSecond) { return (int)((float)TICK_SECOND * (float)tSecond); }
inline static int GetMinuteFromTick(float tMinute) { return (int)((float)TICK_MINUTE * (float)tMinute); }
inline static int GetHourFromTick(float tHour) { return (int)((float)TICK_HOUR * (float)tHour); }
inline static int GetDayFromTick(float tDay) { return (int)((float)TICK_DAY * (float)tDay); }
inline static struct tm* GetDateTime()
{
	COUNT_SECONDS = time(NULL);
	PRESENT_TIME = localtime( &COUNT_SECONDS );
	return PRESENT_TIME;
}

namespace SkyTime
{

inline static TIMETICK GetTickCount() { return GetTickCount64(); };
inline static struct tm* CustomDateTime( time_t __t )
{
	time_t _t = __t;
	struct tm* _tm;
	if ( __t == NULL )
	{
		_t = time( &__t );
	}
	_tm = localtime( &_t );
	if ( _tm )
	{
		CUSTOM_YEAR = _tm->tm_year + 1900;
		CUSTOM_MONTH = _tm->tm_mon + 1;
		CUSTOM_DAY = _tm->tm_mday;
		CUSTOM_WDAY = _tm->tm_wday;
		CUSTOM_HOUR = _tm->tm_hour;
		CUSTOM_MINUTE = _tm->tm_min;
		CUSTOM_SECOND = _tm->tm_sec;
	}
	return _tm;
}

inline int Year() { return ( ( GetDateTime() == NULL ) ? 0 : ( PRESENT_TIME->tm_year + 1900 ) ); }
inline int Month() { return ( ( GetDateTime() == NULL ) ? 0 : ( PRESENT_TIME->tm_mon + 1 ) ); }
inline int Day() { return ( ( GetDateTime() == NULL ) ? 0 : ( PRESENT_TIME->tm_mday ) ); }
inline int WDay() { return ( ( GetDateTime() == NULL ) ? 0 : ( PRESENT_TIME->tm_wday ) ); }
inline int Hour() { return ( ( GetDateTime() == NULL ) ? 0 : ( PRESENT_TIME->tm_hour ) ); }
inline int Minute() { return (( GetDateTime() == NULL ) ? 0 : ( PRESENT_TIME->tm_min ) ); }
inline int Second() { return ( ( GetDateTime() == NULL ) ? 0 : ( PRESENT_TIME->tm_sec ) ); }

inline int ReturnNowDate( void )
{
	char tempString01[100];
	sprintf( tempString01, "%04d%02d%02d", Year(), Month(), Day() );
	return atoi( tempString01 );
}

inline int ReturnNowTime()
{
	char tempString01[100];
	sprintf( tempString01, "%02d%02d", Hour(), Minute() );
	return atoi( tempString01 );
}

inline int ReturnNowTimeWithSecond()
{
	char tempString01[100];
	sprintf( tempString01, "%02d%02d%02d", Hour(), Minute(), Second() );
	return atoi( tempString01 );
}

inline char* NowDateTime()
{
	char tempString01[100];
	sprintf( tempString01, "%04d-%02d-%02d %02d:%02d:%02d", Year(), Month(), Day(), Hour(), Minute(), Second() );
	return strdup( tempString01 );
}

inline int ReturnSubDate( int tPostDateValue, int tSubDayValue )
{
	if( tPostDateValue <= tSubDayValue )
	{
		return 0;
	}
	int index01;
	char tempString01[100];
	char tempString02[100];
	int tMonthDay[12];
	int tYear[2];
	int tMonth[2];
	int tDay[2];

	tMonthDay[ 0] = 31;
	tMonthDay[ 1] = 29;
	tMonthDay[ 2] = 31;
	tMonthDay[ 3] = 30;
	tMonthDay[ 4] = 31;
	tMonthDay[ 5] = 30;
	tMonthDay[ 6] = 31;
	tMonthDay[ 7] = 31;
	tMonthDay[ 8] = 30;
	tMonthDay[ 9] = 31;
	tMonthDay[10] = 30;
	tMonthDay[11] = 31;

	sprintf( tempString01, "%d", tPostDateValue);
	CopyMemory( &tempString02[0], &tempString01[0], 4 );
	tempString02[4] = '\0';
	tYear[0] = atoi( tempString02 );
	if( ( tYear[0] < 1 ) || ( tYear[0] > 9000 ) )
	{
		return 0;
	}
	CopyMemory( &tempString02[0], &tempString01[4], 2 );
	tempString02[2] = '\0';
	tMonth[0] = atoi( tempString02 );
	if( ( tMonth[0] < 1 ) || ( tMonth[0] > 12 ) )
	{
		return 0;
	}
	CopyMemory( &tempString02[0], &tempString01[6], 2 );
	tempString02[2] = '\0';
	tDay[0] = atoi( tempString02 );
	if( ( tDay[0] < 1 ) || ( tDay[0] > 31 ) )
	{
		return 0;
	}
	tDay[0] += ( tYear[0] * 365 );
	for( index01 = 0 ; index01 < ( tMonth[0] - 1 ) ; index01++ )
	{
		tDay[0] += tMonthDay[index01];
	}
	sprintf( tempString01, "%d", tSubDayValue );
	CopyMemory( &tempString02[0], &tempString01[0], 4 );
	tempString02[4] = '\0';
	tYear[1] = atoi( tempString02 );
	if( ( tYear[1] < 1 ) || ( tYear[1] > 9000 ) )
	{
		return 0;
	}
	CopyMemory( &tempString02[0], &tempString01[4], 2 );
	tempString02[2] = '\0';
	tMonth[1] = atoi( tempString02 );
	if( ( tMonth[1] < 1 ) || ( tMonth[1] > 12 ) )
	{
		return 0;
	}
	CopyMemory( &tempString02[0], &tempString01[6], 2 );
	tempString02[2] = '\0';
	tDay[1] = atoi( tempString02 );
	if( ( tDay[1] < 1 ) || ( tDay[1] > 31 ) )
	{
		return 0;
	}
	tDay[1] += ( tYear[1] * 365 );
	for( index01 = 0 ; index01 < ( tMonth[1] - 1 ) ; index01++ )
	{
		tDay[1] += tMonthDay[index01];
	}
	return ( tDay[0] - tDay[1] );
}

inline static int ReturnAddDate( int tPostDateValue, int tAddDayValue )
{
	if( tAddDayValue < 0 )
	{
		return -1;
	}
	char tempString01[9];
	int tPresentDateValue;
	time_t tCountSeconds = time( NULL );
	sprintf( tempString01, "%04d%02d%02d", Year(), Month(), Day() );
	tPresentDateValue = atoi( tempString01 );
	if( tPostDateValue > tPresentDateValue )
	{
		tAddDayValue += ReturnSubDate( tPostDateValue, tPresentDateValue );
	}
	time( &tCountSeconds );
	tCountSeconds += (time_t)tAddDayValue * (time_t)24 * (time_t)3600;
	if( CustomDateTime( tCountSeconds ) == NULL )
	{
		return -1;
	}
	sprintf( tempString01, "%04d%02d%02d", CUSTOM_YEAR, CUSTOM_MONTH, CUSTOM_DAY );
	return atoi( tempString01 );
}

inline static int ReturnVoteDate( void )
{
	char tempString01[9];
	sprintf( tempString01, "%04d%02d06", Year(), Month() );
	return atoi( tempString01 );
}

}

#endif //SC_DATETIME_H