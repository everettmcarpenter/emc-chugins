#include "CircularBuffer.h"

template <typename T>
class Delay
{
    Delay()
    {
        this->initialize();
    }

    Delay( unsigned int length )
    {
        this->initialize( length );
    }

    ~Delay()
    {
        this->cleanup();
    }
    
    bool initialize( unsigned int length ) // setup everything
    {
        if( !length )
        {
            return false; // outta here
        }
        else 
        {
            _buffer = new CircularBuffer<T>; // allocate our buffer
            _buffer->initialize( length ); // set buffer length
        }
    }

    void cleanup() // get rid of everything
    {
        DELETE_OBJ( _buffer );
    }

    T tick( T in )
    {
        _buffer->put( in ); // input to buffer
        return _buffer->get(); // output from buffer
    }

    void tick( T* in, T* out, unsigned int num_elements )
    {
        _buffer->put( in, num_elements ); // input to buffer
        _buffer->get( out, num_elements ); // output from buffer
    }

private:
    CircularBuffer<T>* _buffer;
};