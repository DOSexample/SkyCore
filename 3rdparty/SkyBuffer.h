#ifndef SC_BUFFER_H
#define SC_BUFFER_H

class SkyBuffer
{
private:
	DWORD size;
	BYTE* data;
public:

	SkyBuffer() : data(0), size(0)
	{
	}
	~SkyBuffer()
	{
		if( this->data )
		{
			GlobalFree( this->data );
		}
	}

	void Init( int size, const char* file = __FILE__, int line = __LINE__ )
	{
		this->size = 0;
		this->data = (BYTE*)GlobalAlloc( GMEM_FIXED, size );
		if( this->data == NULL )
		{
			throw SkyException( "%s -> %s:%d", __PRETTY_FUNCTION__, file, line );
		}
	}

	DWORD Size() const { return this->size; }
	
	void IncreasePos( int size ) { this->size += size; }

	BYTE* Get() { return data; }

	SkyBuffer& Header( int value )
	{
		this->size = 0;
		this->data[0] = value;
		this->IncreasePos( 1 );
		return *this;
	}

	template<typename T>
	SkyBuffer& Pack( T value )
	{
		int s = sizeof(T);
		CopyMemory( &this->data[this->size], &value, s );
		this->IncreasePos( s );
		return *this;
	}

	template<typename T>
	SkyBuffer& Pack( T* value, int size )
	{
		CopyMemory( &this->data[this->size], value, size );
		this->IncreasePos( size );
		return *this;
	}
	
	template<typename T>
	SkyBuffer& Pack( T** value, int size )
	{
		CopyMemory( &this->data[this->size], value, size );
		this->IncreasePos( size );
		return *this;
	}

	void Move( int position, int moveSize )
	{
		MoveMemory( data, &data[position], moveSize );
		this->size -= moveSize;
	}
	void SetPosition( int position ) { this->size = position; }
};

#endif //SC_BUFFER_H