#ifndef GRAIN_EVENT_H
#define GRAIN_EVENT_H

// an event grain, produces one window of a grain

#include "../../include/Phasor.h"
#include "../../include/stk/include/Stk.h"
#include "../../include/stk/include/FileRead.h"
#include "WindowFunctions.h"

struct GrainEvent
{
	// constructor
	GrainEvent::GrainEvent( unsigned int fs, unsigned int length_ms, int type = -1 ) // create window, the rest of this structs lifespan is playing the window back
	{
		// set internal sample rate
		this->_fs = fs;
		// set internal size
		this->size_samp = msToNormalizedSamp( length_ms );
		// create window reader
		reader = new Phasor( fs, 1000.0 / length_ms, true );
		// create window buffer
		window = new double[ size_samp ];
		// audio buffer
		buffer = new stk::StkFrames( 1, 1 );
		// buffer playback
		bufferPlayer = new Phasor( fs, 1.0, true );
		// pitch slew
		pit = new Smoother( fs );
		// make new window
		this->setWindow( size_samp, type );
	}

	// destructor
	GrainEvent::~GrainEvent() { delete reader; reader = nullptr; 
								delete window; window = nullptr; 
								delete buffer; buffer = nullptr;
								delete pit; pit = nullptr;
								delete bufferPlayer; bufferPlayer = nullptr; };

	// tick func
	double GrainEvent::tick() 
	{ 
		// get index
		float index = reader->tick() * size_samp;
		// linear interpolation stolen straight from the stk's StkFrames obj 
		// ( having the window be stored in an StkFrames was too much of a headache )
		if ( index >= size_samp - 1 ) return window[ size_samp - 1 ];
		else if ( index == 0 ) return window[ 0 ];
		unsigned int intdex = static_cast<unsigned int>( index ); // static cast rounds towards zero 
		double fractional = index - intdex; // fractional part 
		double out = window[ intdex ]; // return this
		// check for index range & interpolate!
		// one multiply linear interpolation, just like JOS taught me!
		// https://ccrma.stanford.edu/~jos/pasp/One_Multiply_Linear_Interpolation.html
		if( fractional > 0.0 ) out += ( fractional * ( window[ intdex + 1 ] - out ) );
		return out;
	}

	// are we there yet
	int GrainEvent::state() { return this->reader->state(); }

	// make a new grain
	void GrainEvent::newGrain( float length_ms, int position, stk::FileRead* source, int type = -1 )
	{
		// resize buffer
		this->buffer->resize( msToNormalizedSamp( length_ms ) + 1 );
		// read
		source->read( *buffer, position, true );
		// resize window
		if ( length_ms != normalizedSampToMs( size_samp ) )
		{
			// set internal size
			this->size_samp = msToNormalizedSamp( length_ms );
			// set freq
			reader->setFrequency( 1000.0 / length_ms, TRUE );
			// buffer 
			bufferPlayer->setFrequency( source->fileRate() / source->fileSize() );
			// get rid of old window
			delete[] window;
			// new buffer
			window = new double[ size_samp ];
			// make new window
			this->setWindow( size_samp, type );
			// go!
			reader->trigger(); bufferPlayer->trigger();
		}
		else { reader->trigger(); bufferPlayer->trigger(); }
	}

	// create a window 
	void GrainEvent::setWindow( unsigned int size, int type )
	{
		switch( type )
		{
		case 0 :
			rectangle( window, size );
			break;
		case 1 :
			hann( window, size );
			break;
		case 2 :
			blackman( window, size );
			break;
		default :
			blackman( window, size );
			break;
		}
	}

	// return size in milliseconds
	float GrainEvent::getSizeMs() { return normalizedSampToMs( this->size_samp ); }

	unsigned int GrainEvent::msToNormalizedSamp( float ms ) { return static_cast<unsigned int>( 0.5 + ( ms * 0.001 ) * this->_fs ); }
	float GrainEvent::normalizedSampToMs( unsigned int samp ) { return ( samp * 1000.0f ) / this->_fs; }

	Phasor* bufferPlayer = nullptr;
	Phasor* reader = nullptr; // reads through window
	Smoother* pit = nullptr;
	stk::StkFrames* buffer = nullptr;
	double* window = nullptr; // the actual window
	unsigned int size_samp = 1;
	unsigned int _fs = 0;
};

#endif /* GRAIN_EVENT_H */