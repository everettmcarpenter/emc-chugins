// windower is a support class which operates at the sample level
// it's tick function will return the next window value if provided no arguments
// if provided an argument, it will return that value "windowed"
// windower's lookup table is static, thus changing the size of it's windows is done by changing the playback phasor's frequency

#ifndef WINDOWER_H
	#define WINDOWER_H

#include "WindowFunctions.h"
#include "Phasor.h"

class Windower
{
public:
	Windower::Windower( unsigned int fs, unsigned int init_size = 200.0 )
	{
		// set initial size, allocate members
		_fs = fs;
		size_ms = init_size;
		phasor = new Phasor( fs, 1.0, true );
		_window = new double[table_size];
		// init 
		makeBlackman();
	}

	Windower::~Windower()
	{
		delete phasor; phasor = nullptr;
		delete _window; _window = nullptr;
	}

	// window
	double Windower::tick()
	{
		// get index ( phasor [0.0,1.0] * ( size_ms converted to samples ) )
		double index = phasor->tick() * ( table_size - 1 );
		// range check & clamp lookup
		if( index >= table_size - 1 ) return _window[table_size - 1];
		else if( index == 0 ) return _window[0];
		unsigned int intdex = static_cast<unsigned int>( index ); // round downwards;
		double fractional = index - intdex; // get the decimal
		double out = _window[intdex]; // nearest point
		if( fractional > 0.0 ) out += ( fractional * ( _window[intdex + 1] - out ) ); // draw a linear line between the two and find the mid point
		return out;
	}

	// window an input
	double Windower::tick( double input )
	{
		// get index ( phasor [0.0,1.0] * ( size_ms converted to samples ) )
		double index = phasor->tick() * ( table_size - 1 );
		// range check & clamp lookup
		if( index >= table_size - 1 ) return _window[table_size - 1] * input;
		else if( index == 0 ) return _window[0] * input;
		unsigned int intdex = static_cast<unsigned int>( index ); // round downwards;
		double fractional = index - intdex; // get the decimal
		double out = _window[intdex]; // nearest point
		if( fractional > 0.0 ) out += ( fractional * ( _window[intdex + 1] - out ) ); // draw a linear line between the two and find the mid point
		return out * input;
	}

	void Windower::trigger()
	{
		phasor->trigger();
	}

	bool Windower::state()
	{
		return phasor->state();
	}

	// set window size
	void Windower::setSize( float n_size_ms )
	{
		size_ms = n_size_ms;
		size_samp = msToNormalizedSamp( size_ms );
		phasor->setFrequency( 1000.f / size_ms );
	}
	float Windower::getSizeMs() { return size_ms; }
	unsigned int Windower::getSizeSamp() { return size_samp; }

	// set to blackman window
	void Windower::makeBlackman()
	{
		blackman( _window, table_size );
	}

	void Windower::makeHann()
	{
		hann( _window, table_size );
	}

	void Windower::makeHamming()
	{
		hamming( _window, table_size );
	}

	void Windower::makeRectangle()
	{
		rectangle( _window, table_size );
	}

    void Windower::reset()
    {
        phasor->reset();
    }

	unsigned int Windower::msToNormalizedSamp( float ms ) { return static_cast<unsigned int>( 0.5 + ( ms * 0.001 ) * this->_fs ); }
	float Windower::normalizedSampToMs( unsigned int samp ) { return ( samp * 1000.0f ) / this->_fs; }

private:
	Phasor* phasor = nullptr;
	double* _window = nullptr;
	float size_ms = 0.f;
	unsigned int size_samp = 0;
	unsigned int _fs = 0;
	unsigned int table_size = 4096;
};

#endif /* WINDOWER_H */