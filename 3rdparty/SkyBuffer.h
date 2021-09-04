#ifndef SC_BUFFER_H
#define SC_BUFFER_H

class SkyBuffer
{
private:
	DWORD alloc_size;
	DWORD current_pos;
	BYTE* data;
public:

	SkyBuffer() : data(0), alloc_size(0), current_pos(0)
	{
	}
	~SkyBuffer()
	{
		this->alloc_size = 0;
		this->current_pos = 0;
		if( this->data )
		{
			GlobalFree( this->data );
		}
	}

	void Init( int size, const char* file = __FILE__, int line = __LINE__ )
	{
		alloc_size = size;
		this->current_pos = 0;
		this->data = (BYTE*)GlobalAlloc( GMEM_FIXED, size );
		if( this->data == NULL )
		{
			throw SCTHROW_ARG( "%s", __PRETTY_FUNCTION__ );
		}
	}

	DWORD Size() const { return this->current_pos; }

	BYTE* Get( DWORD pos ) { return pos >= this->alloc_size ? NULL : data ? &data[pos] : NULL; }
	
	SkyBuffer& Add( void* value, int size )
	{
		CopyMemory( Get(this->current_pos), value, size );
		this->current_pos += size;
		return *this;
	}


	void Move( int position, int moveSize )
	{
		MoveMemory( data, &data[position], moveSize );
		this->current_pos -= moveSize;
	}
	void Set( DWORD position ) { this->current_pos = position; }

	SkyBuffer& Header( int value )
	{
		this->current_pos = 0;
		return this->Add( &value, 1 );
	}

	template<typename T>
	SkyBuffer& Pack( T value )
	{
		int size = sizeof(T);
		return this->Add( &value, size );
	}

	template<typename T>
	SkyBuffer& Pack( T* value, int size )
	{
		return this->Add( value, size );
	}
	
	template<typename T>
	SkyBuffer& Pack( T** value, int size )
	{
		return this->Add( value, size );
	}
};

#endif //SC_BUFFER_H