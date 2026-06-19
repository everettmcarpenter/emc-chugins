//=================================================
//
// Grain.h : Summer 2026 : everett m. carpenter
// Grain is a class which controls and links a
// file reader, and parameter bank. The parameter
// bank will store all current parameters and is 
// updated by function calls from the Grain class.
// File reading and playback is handled by an
// stk::FileWvIn
// 
//=================================================

#ifndef GRANULATOR_H
	#define GRANULATOR_H

#include "chugin.h"
#include "Phasor.h"
#include "Smoother.h"
#include "../include/stk/include/Noise.h"
#include "../include/stk/include/Stk.h"

// forward definitions
class Grain;
struct GrainParameters;

// parameter bank / windower
struct GrainParameters
{
	GrainParameters::GrainParameters( t_CKINT fs )
	{
		windower = new Phasor( fs, 10.0 );
		grain_duration = new Smoother( fs, 20.f );
		rand = new stk::Noise( 6449 );
	}

	// destructor
	GrainParameters::~GrainParameters()
	{
		CK_SAFE_DELETE_ARRAY( window ); // delete window
		delete windower; windower = nullptr; // window phasor
		delete grain_duration; grain_duration = nullptr;
		delete rand; rand = nullptr;
	}

	// returns window value
	t_CKFLOAT GrainParameters::tick()
	{
		// if( !grain_duration->isMoving() ) grain_duration->setTarget( myDuration + ( randomDuration * fabs( rand->tick() ) ) );
		// advance interpolation and set frequency, but don't reset
		windower->setFrequency( 1000.0 / ( grain_duration->tick() ), FALSE );
		// return the window
		return getWindowValue( windower->tick() * window_size );
	}

	// future tick, next window value ( +tick in the future )
	t_CKFLOAT GrainParameters::fick()
	{
		// get the interpolated value
		return getWindowValue( windower->next() );
	}

	// set internal window
	void GrainParameters::setWindow( t_CKFLOAT* win, t_CKUINT size )
	{
		// clear
		delete[] window;

		// fill
		window = new t_CKFLOAT[ size ];
		window_size = size;
		for( int i = 0; i < size; i++ ) { window[ i ] = win[ i ]; }
	}

	// set rate of granulation
	void GrainParameters::setWindowSize( t_CKFLOAT rate ) // rate is in ms ( how long is a window )
	{
		myDuration = rate; 
		// set frequency ( inverse of rate )
		grain_duration->setTarget( myDuration );
		// set the phasor to start
		windower->setFrequency( 1000.0 / grain_duration->tick(), FALSE );
	}

	// get window given an integer
	t_CKFLOAT GrainParameters::getWindowValue( t_CKINT index )
	{
		return window[ abs( index ) % window_size ];
	}

	// get window given a float index
	t_CKFLOAT GrainParameters::getWindowValue( t_CKFLOAT index )
	{
		// linear interpolation stolen straight from the stk's StkFrames obj 
		// ( having the window be stored in an StkFrames was too much of a headache )
		if ( index >= window_size - 1 ) return window[ window_size - 1 ];
		if ( index < 0 ) return window[ 0 ];
		t_CKUINT intdex = static_cast<t_CKUINT>( index ); // static cast rounds towards zero 
		t_CKFLOAT fractional = index - intdex; // fractional part 
		t_CKFLOAT out = window[ intdex ]; // return this
		// check for index range & interpolate!
		// one multiply linear interpolation, just like JOS taught me!
		// https://ccrma.stanford.edu/~jos/pasp/One_Multiply_Linear_Interpolation.html
		if( fractional > 0.0 ) out += ( fractional * ( window[ intdex + 1 ] - out ) );
		return out;
	}

	// set random grain size ( in ms )
	void GrainParameters::setRandomSize( t_CKFLOAT random ) { randomDuration = random; }
	// get random grain size ( in ms )
	t_CKFLOAT GrainParameters::getRandomSize() { return randomDuration; }

	Phasor* windower; // phasor to playback the window
	// grain size interpolator ( in ms )
	Smoother* grain_duration = nullptr; t_CKFLOAT myDuration = 0.f; t_CKFLOAT randomDuration = 0.f;
	// randomness for fun ( not random, TODO: give unique seed )
	stk::Noise* rand = nullptr;
	// window 
	t_CKFLOAT* window = nullptr; t_CKUINT window_size = 0;
};

// grain class
class Grain
{
public:
	// constructor 
	Grain::Grain( t_CKINT fs )
	{
		// create our helpers
		paramBank = new GrainParameters( fs );
		// init blackman
		t_CKFLOAT* temp = new t_CKFLOAT[ 2048 ];
		blackman( temp, 2048 );
		// set internal window
		paramBank->setWindow( temp, 2048 );
		// cleanup
		delete[] temp; temp = nullptr;
	}

	// destructor
	Grain::~Grain()
	{
		delete paramBank; paramBank = nullptr;
	}

	// tick function
	virtual SAMPLE Grain::tick( SAMPLE in )
	{
		return paramBank->tick() * in; 
	}

	// fill a buffer of some size with a hann window
	void Grain::hann( t_CKFLOAT* buffer, t_CKUINT size )
	{
		for( int i = 0; i < size; i++ )
		{
			buffer[i] = 0.5 - ( 0.5 * cos( ( CK_TWO_PI * i ) / size ) );
		}
	}

	// fill a buffer of some size with a blackman window
	void Grain::blackman( t_CKFLOAT* buffer, t_CKUINT size )
	{
		// taken from wikipedia https://en.wikipedia.org/wiki/Window_function#Blackman_window
		t_CKFLOAT alpha = 0.16;
		t_CKFLOAT a0,a1,a2;
		a0 = ( 1.f - alpha ) / 2.f;
		a1 = 0.5; a2 = alpha / 2.f;

		for( int i = 0; i < size; i++ )
		{
			buffer[i] = a0 - a1 * cos( ( CK_TWO_PI * i ) / size ) + a2 * cos( ( 2.f * CK_TWO_PI * i ) / size );
		}
	}

	// fill a buffer of some size with a rectangular window
	void Grain::rectangle( t_CKFLOAT* buffer, t_CKUINT size )
	{
		for( int i = 0; i < size; i++ )
		{
			buffer[i] = 1.f;
		}
	}

	// get sample rate
	t_CKFLOAT fs() { return paramBank->windower->getSR(); }

	// grain size ( in ms )
	void Grain::setWindowSize( t_CKFLOAT rate ) { paramBank->setWindowSize( rate ); }
	t_CKFLOAT Grain::getWindowSize() { return paramBank->grain_duration->current; }
	void Grain::setRandomWindowSize( t_CKFLOAT random ) { paramBank->setRandomSize( random ); }
	t_CKFLOAT Grain::getRandomWindowSize() { return paramBank->getRandomSize(); }

	// store the params
	GrainParameters* paramBank = nullptr;
};

#endif