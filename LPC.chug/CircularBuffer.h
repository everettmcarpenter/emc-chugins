// CircularBuffer.h by emc : summer 2026
//
// A circular buffer is used via read() and write() methods
// these allow you to interact with two tape heads
// when you read or write on either tape head, they must move
// reading will return the value that the readHead is on, and then you will not be able to get that value again
// likewise, if you write with the writeHead, you will write to a segment of the buffer and cannot write that segment again until calling write() m_size times

template<typename T>
struct CircularBuffer
{
	// constructor
	CircularBuffer::CircularBuffer( unsigned int n_size ) : m_size( n_size ), buffer ( new T[m_size] ) {}

	// destructor
	CircularBuffer::~CircularBuffer() { delete[] buffer; }

	CircularBuffer( const CircularBuffer& ) = delete;
    CircularBuffer& operator=( const CircularBuffer& ) = delete;

	// full or nah
	bool CircularBuffer::full() { return fullOrNah; }

	// are we empty ( is it NOT full and do the read and write equal each other? )
	bool CircularBuffer::empty() { return !fullOrNah && ( readHead == writeHead ); }

	// read and advance
	T CircularBuffer::read() 
	{
		if( this->empty() ) return 0;
		else 
		{
			// read!
			T read = buffer[readHead];
			// none of that module stuff
			if( ++readHead == m_size ) readHead = 0;
			// we can't be full if we just read
			fullOrNah = false;
			// send it off
			return read;
		}
	}

	void CircularBuffer::write( const T input )
	{
		if( !this->full() ) 
		{
			// write!
			buffer[writeHead] = input;
			// again
			if( ++writeHead == m_size ) writeHead = 0;
			// full check
			if( writeHead == readHead ) fullOrNah = true;
		}
	}

	T* buffer = nullptr;
	unsigned int readHead = 0;
	unsigned int writeHead = 0;
	unsigned int m_size = 0;
	bool fullOrNah = false;
};