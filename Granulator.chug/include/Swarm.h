//==========================================================
//
// Swarm.h : Summer 2026 : everett m. carpenter
// Granulator is a class which utilizes Grains as a means
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
#include "../../include/Sampler.h"
#include "../../include/stk/include/FileRead.h"
#include "../../include/stk/include/Noise.h"
#include "../../include/stk/include/Stk.h"

class GrainSwarm
{
public:
	// constructor
	GrainSwarm::GrainSwarm( t_CKINT fs, t_CKUINT num = 1, t_CKFLOAT init_size = 100.f ) : num_grains( num ), scalar( 1.0 / num )
	{
		// set global sample rate
		stk::Stk::setSampleRate( fs );
		// create fresh member-classes
		myGrains = new Granulator*[ num_grains ];
		for( int i = 0; i < num_grains; i++ ) { myGrains[i] = new Granulator( fs ); myGrains[i]->setWindowSize( init_size ); }
		// the rest
		rand = new stk::Noise( time( NULL ) );
		samp = new Sampler( fs );
		samp->setPitch( 1.0 ); // init pitch
	}

	GrainSwarm::~GrainSwarm()
	{
		// delete
		delete myGrains; myGrains = nullptr;
		delete rand; rand = nullptr;
		delete samp; samp = nullptr;
	}

	// tick ( eventually this will just take the next sample in the file reader and granulate it )
	SAMPLE GrainSwarm::tick()
    {
		SAMPLE out = 0.f; 

		// accumulate
		for( int i = 0; i < num_grains; i++ ) 
		{
			out += myGrains[i]->tick( samp->tick() );

			// set random pit 
			if( this->myGrains[i]->state() ) 
			{
				addedPitch = ( random_pitch * rand->tick() );
				// move pitch
				samp->setPitch( addedPitch + myPitch, 0.1 );
			}
		}

		// lets not blow up the dac
		out *= scalar;

        // granulate! ( finally )
        return out;
    }

	// open file and update
	void GrainSwarm::openFile( const char* path )
	{
		// underlying sampler
		samp->openFile( path );
	}

	// pitch
	t_CKFLOAT GrainSwarm::setPitch( t_CKFLOAT pit )
	{
		// save
		myPitch = pit;
		// return for fun
		return myPitch;
	}
	t_CKFLOAT GrainSwarm::getPitch() { return myPitch; }

	// setters and getters for window rate ( grain size )
	void GrainSwarm::setGrainSize( t_CKFLOAT rate )
	{
		if( rate > 0.f ) 
		{
			for ( int i = 0; i < num_grains; i++ )
			{
				myGrains[i]->setWindowSize( rate ); // in ms
				// sync sampler with window size ( this will be a little asychronous when we have a non-zero random size value )
				samp->setLoopLengthWithWindow( myGrains[0]->getWindowSize() + ( 2.0 * myGrains[0]->random_size ) );
			}
		}
	}
	t_CKFLOAT GrainSwarm::getGrainSize() { return myGrains[0]->getWindowSize(); }

	// random size
	void GrainSwarm::setRandomGrainSize( t_CKFLOAT rand ) 
	{ 
		for ( int i = 0; i < num_grains; i++ )
		{
			myGrains[i]->setRandomSize( rand ); 
		}
	}
	t_CKFLOAT GrainSwarm::getRandomGrainSize() { return myGrains[0]->getRandomSize(); }

	// set random pitch
	void GrainSwarm::setRandomPitch( t_CKFLOAT random ) { random_pitch = random; }
	// get random pitch
	t_CKFLOAT GrainSwarm::getRandomPitch() { return random_pitch; }

	// set random position
	void GrainSwarm::setRandomPosition( t_CKFLOAT random ) { random_position = random; }
	// get random position 
	t_CKFLOAT GrainSwarm::getRandomPosition() { return random_position; }

	// create grain
	Granulator** myGrains = nullptr;
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
	// num grains
	t_CKUINT num_grains = 0;
	// scale 
	t_CKFLOAT scalar = 0.f;
};

#endif