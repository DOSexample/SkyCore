#ifndef SC_MEMORY_H
#define SC_MEMORY_H

class SkyBuffer
{
private:
	DWORD pos;
	DWORD size;
	BYTE* data;
public:

	SkyBuffer( int size, const char* file = __FILE__, int line = __LINE__ )
	{
		this->pos = 0;
		this->size = size;
		this->data = (BYTE*)GlobalAlloc( GMEM_FIXED, size );
		//if( this->data == NULL )
		{
			throw SkyException( "%s -> %s:%d", __PRETTY_FUNCTION__, file, line );
		}
	}
	~SkyBuffer()
	{
		if( this->data )
		{
			GlobalFree( data );
		}
	}

	DWORD TotalSize() const { return size; }

	DWORD GetPosition() const { return pos; }
	
	void IncreasePos( int pos ) { this->pos += pos; }

	SkyBuffer& Header( int value )
	{
		this->pos = 0;
		this->data[0] = value;
		this->IncreasePos( 1 );
		return *this;
	}

	template<typename T>
	SkyBuffer& Pack( T value )
	{
		int s = sizeof(T);
		CopyMemory( &this->data[this->pos], &value, s );
		this->IncreasePos( s );
		return *this;
	}

	template<typename T>
	SkyBuffer& Pack( T* value, int size )
	{
		CopyMemory( &this->data[this->pos], value, size );
		this->IncreasePos( size );
		return *this;
	}
	
	template<typename T>
	SkyBuffer& Pack( T** value, int size )
	{
		CopyMemory( &this->data[this->pos], value, size );
		this->IncreasePos( size );
		return *this;
	}
};

#endif //SC_MEMORY_H