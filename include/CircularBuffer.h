// -----------------------------------------------------
//	
//	Simple circular template buffer that assumes you are
//	using a type defined in the c++ standard.  
// 
// -----------------------------------------------------

#define DELETE_ARRAY( x ) { delete[] x; x = nullptr; }
#define DELETE_OBJ( x ) { delete x; x = nullptr; }

template <typename T> 
class CircularBuffer
{
	CircularBuffer()
	{
		_data = nullptr; // set our data pointer to nothing
		_width = _readPosition = _writePosition =_size = 0; // we don't know the size of these yet
	}

	~CircularBuffer()
	{
		this->cleanup();
	}

	void cleanup()
	{
		DELETE_ARRAY( _data );
		_width = _readPosition = _writePosition =_size = 0; 
	}

	bool initialize( unsigned int size )
	{
		this->cleanup(); // clean 

		if( !size ) // then we have size 0
		{
			return false; // not a good idea to make a buffer of size 0
		}

		_size = size; // number of T that we have
		_data = new T[_size]; // allocate 
		_width = sizeof( T ); // this is how big a single entry is 
		_maxIndex = _size - 1; // easy

		return true; // cool
	}

	void put( T entry ) // single put
	{
		_data[_writePosition] = entry; // put 
		_writePosition = ( _writePosition++ ) % _size; // if _writePosition == _size, set to 0 
	}

	void put( T* entries, unsigned int num_entries ) // buffered put
	{
		for( int i = 0; i < num_entries; i++ )
		{
			this->put( entries[i] ); // dude check that out
		}
	}

	T get() // single get
	{
		if( _writePosition == _readPosition )
		{
			return 0; // don't want to read the future while we write the past
		}

		T out = _data[_readPosition]; // buy it
		_readPosition = ( _readPosition++ ) % _size; // break it
		return out; // use it
	}

	void get( T* out, unsigned int num_outs ) // buffered get
	{
		for( int i = 0; i < num_outs; i++ ) // write = read will be caught by this->get
		{
			out[i] = this->get(); // dude check that out
		}
	}
	
	T* _data; // actual data
	unsigned int _size; // how many of T in _data
	unsigned int _maxIndex; // largest index of _data
	unsigned int _width; // size of a single T
	unsigned int _readPosition; // where are we reading
	unsigned int _writePosition; // where are we writing
};
