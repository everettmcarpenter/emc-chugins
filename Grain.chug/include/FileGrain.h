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

#include "../../include/Grain.h"
#include "../../Sampler.chug/include/Sampler.h"
#include "../../include/stk/include/FileRead.h"
#include "../../include/stk/include/Noise.h"
#include "../../include/stk/include/Stk.h"

class FileGrain
{
public:
	// constructor
	FileGrain::FileGrain( t_CKINT fs )
	{
		// set global sample rate
		stk::Stk::setSampleRate( fs );
		// create fresh member-classes
		myGrain = new Grain( fs );
		rand = new stk::Noise( 6589 );
		samp = new Sampler( fs );
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
        // set kinda random pit 
        if( !samp->pitch->isMoving() ) samp->playback->setFrequency( ( random_pitch * rand->tick() ) + ( myPitch * samp->oneHert ), FALSE );

        // granulate! ( finally )
        return myGrain->tick( samp->tick() );
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
		// set sampler
		samp->setPitch( pit ); 
		// return for fun
		return myPitch;
	}
	t_CKFLOAT FileGrain::getPitch() { return myPitch; }

	// setters and getters for window rate ( grain size )
	void FileGrain::setGrainSize( t_CKFLOAT rate )
	{
		if( rate > 0.f ) myGrain->setWindowSize( rate ); // call grain function ( which calls grain param function, which calls phasor )
	}
	t_CKFLOAT FileGrain::getGrainSize() { return myGrain->getWindowSize(); }

	// random size
	void FileGrain::setRandomGrainSize( t_CKFLOAT rand ) { myGrain->setRandomWindowSize( rand ); }
	t_CKFLOAT FileGrain::getRandomGrainSize() { return myGrain->getRandomWindowSize(); }

	// set random pitch
	void FileGrain::setRandomPitch( t_CKFLOAT random ) { random_pitch = random; }
	// get random pitch
	t_CKFLOAT FileGrain::getRandomPitch() { return random_pitch; }

	// set random position
	void FileGrain::setRandomPosition( t_CKFLOAT random ) { random_position = random; }
	// get random position 
	t_CKFLOAT FileGrain::getRandomPosition() { return random_position; }

	// create grain
	Grain* myGrain = nullptr;
	// file player manipulator
	Sampler* samp = nullptr;
	// randomness
	stk::Noise* rand = nullptr; 
	// internal position and pitch ( these insure that the center point of pitch or position do not change when random variations occur )
	t_CKFLOAT myPitch = 0.f;
	t_CKFLOAT myPosition = 0.f;
	// pitch interpolator
	t_CKFLOAT random_pitch = 0.f;
	// position interpolator
	t_CKFLOAT random_position = 0.f;
};

#endif