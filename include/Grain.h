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
#include "GrainEvent.h"
#include "Smoother.h"
#include "../include/stk/include/Noise.h"
#include "../include/stk/include/Stk.h"

#define GRAIN_IN_PROGRESS false
#define GRAIN_ENDED true

// grain class
class Granulator
{
public:
	// constructor 
	Granulator::Granulator( t_CKUINT fs )
	{
		// create our helpers
		this->grain_size = 2048;
		win = new GrainEvent( fs, grain_size );
		random = new stk::Noise( time( NULL ) ); 
	}

	// destructor
	Granulator::~Granulator()
	{
		delete win; win = nullptr;
	}

	// tick function
	virtual SAMPLE Granulator::tick( SAMPLE in )
	{
		// if it's still going, keep going!
		if ( win->state() == GRAIN_IN_PROGRESS )
		{
			return win->tick() * in;
		}
		// if it's over make a new one!
		else if ( win->state() == GRAIN_ENDED )
		{
			win->newGrain( ( ( random->tick() + 1.0 ) * random_size ) + grain_size );
			return win->tick() * in; 
		}
	}

	void Granulator::setWindowSize( float n_size ) { this->grain_size = n_size; } // in ms
	float Granulator::getWindowSize() { return this->win->getSizeMs(); } // the current window's size in milliseconds

	void Granulator::setRandomSize( float n_rand_size ) { this->random_size = n_rand_size; }
	float Granulator::getRandomSize() { return this->random_size; } 

	bool Granulator::state() { return this->win->state(); }; // true? grain done, false? grain go

	// get sample rate
	unsigned int fs() { return win->_fs; }

	// store the params
	GrainEvent* win = nullptr;
	stk::Noise* random = nullptr;
	float grain_size = 1; float random_size = 0;
};

#endif /* GRANULATOR_H */