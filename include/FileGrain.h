//==========================================================
//
// FileGrain.h : Summer 2026 : everett m. carpenter
// FileGrain is a class which utilizes Grain as a means
// of granulating a file or input. It uses a tick function 
// with an input, where the input value is ideally a sample 
// value from an audio file or input.
// 
//==========================================================

#ifndef FILE_GRAIN_H
	#define FILE_GRAIN_H

#define __LITTLE_ENDIAN__
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

#define MAXIMUM_BUFFER_SIZE 600000000 // maximum number of frames an StkFrames object can have, equates to 0.6 Gb using 8 byte doubles

#include "Grain.h"
#include "Sampler.h"
#include "stk/include/FileRead.h"
#include "stk/include/Noise.h"
#include "stk/include/Stk.h"

class FileGrain
{
public:
	// constructor
	FileGrain::FileGrain( t_CKINT fs, t_CKFLOAT init_size = 100.f )
	{
		// set global sample rate
		stk::Stk::setSampleRate( fs );
		// create fresh member-classes
		myGrain = new Granulator( fs );
		rand = new stk::Noise( time( NULL ) );
		samp = new Sampler( fs );
		samp->setPitch( 1.0 ); // init pitch
		myGrain->setWindowSize( init_size );
	}

	FileGrain::~FileGrain()
	{
		// delete
		delete myGrain; myGrain = nullptr;
		delete rand; rand = nullptr;
		delete samp; samp = nullptr;
	}

	// tick ( eventually this will just take the next sample in the file reader and granulate it )
	SAMPLE FileGrain::tick()
    {
		SAMPLE out = myGrain->tick( samp->tick() );
		// move pitch
		samp->setPitch( addedPitch + myPitch, 0.1 );

        // set random pit 
        if( this->myGrain->state() ) 
		{
			addedPitch = ( random_pitch * rand->tick() );
			// sync sampler with window size
			samp->setLoopLengtWithWindow( myGrain->getWindowSize() );
		}


        // granulate! ( finally )
        return out;
    }

	// overloaded tick 
	SAMPLE FileGrain::tick( SAMPLE in )
	{
		return myGrain->tick( in ); // have the grain window the input 
	}

	// open file and update
	void FileGrain::openFile( const char* path )
	{
		// underlying sampler
		samp->openFile( path );
	}

	// pitch
	t_CKFLOAT FileGrain::setPitch( t_CKFLOAT pit )
	{
		// save
		myPitch = pit;
		// return for fun
		return myPitch;
	}
	t_CKFLOAT FileGrain::getPitch() { return myPitch; }

	// setters and getters for window rate ( grain size )
	void FileGrain::setGrainSize( t_CKFLOAT rate )
	{
		if( rate > 0.f ) myGrain->setWindowSize( rate ); // in ms
	}
	t_CKFLOAT FileGrain::getGrainSize() { return myGrain->getWindowSize(); }

	// random size
	void FileGrain::setRandomGrainSize( t_CKFLOAT rand ) { myGrain->setRandomSize( rand ); }
	t_CKFLOAT FileGrain::getRandomGrainSize() { return myGrain->getRandomSize(); }

	// set random pitch
	void FileGrain::setRandomPitch( t_CKFLOAT random ) { random_pitch = random; }
	// get random pitch
	t_CKFLOAT FileGrain::getRandomPitch() { return random_pitch; }

	// set random position
	void FileGrain::setRandomPosition( t_CKFLOAT random ) { random_position = random; }
	// get random position 
	t_CKFLOAT FileGrain::getRandomPosition() { return random_position; }

	// create grain
	Granulator* myGrain = nullptr;
	// file player manipulator
	Sampler* samp = nullptr;
	// randomness
	stk::Noise* rand = nullptr; 
	// internal position and pitch ( these insure that the center point of pitch or position do not change when random variations occur )
	t_CKFLOAT myPitch = 1.f;
	t_CKFLOAT addedPitch = 0.f;
	t_CKFLOAT myPosition = 0.f;
	// pitch interpolator
	t_CKFLOAT random_pitch = 0.f;
	// position interpolator
	t_CKFLOAT random_position = 0.f;
};

#endif