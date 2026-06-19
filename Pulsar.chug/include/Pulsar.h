#ifndef PULSAR_H
#define PULSAR_H

#include "../../include/Grain.h"

class Pulsar : public Grain
{
public:
	// inherit grain constructor
	using Grain::Grain;

	// tick
	SAMPLE Pulsar::tick()
	{
		// are you cosine or sine
		SAMPLE out = isCos ? this->paramBank->tick() * cos( phase ) : sin( phase );
		// progress
		phase += increment;
		// wrap
		if( phase >= CK_TWO_PI ) phase -= CK_TWO_PI;

		// window
		return this->paramBank->tick() * out;
	}

	// set frequency
	void Pulsar::setFrequency( t_CKFLOAT fr ) 
	{ 
		if( fr > 0.f )
		{
			this->freq = fr; 
			this->increment = CK_TWO_PI * ( fr / this->fs() );
		}
		else 
		{
			this->freq = -1.0 * fr;
			this->increment = CK_TWO_PI * ( -1.0 * fr / this->fs() );
		}
	}
	// get frequency
	t_CKFLOAT Pulsar::getFrequency() { return this->freq; }

	// setters and getters for window rate ( grain size )
	void Pulsar::setRate(t_CKFLOAT rate)
	{
		if (rate > 0.f) this->setWindowSize( rate ); // call grain function ( which calls grain param function, which calls phasor )
	}
	t_CKFLOAT Pulsar::getRate() { return this->getWindowSize(); }
	// set randomness of grains
	void Pulsar::setRandomRate(t_CKFLOAT rand) { this->setRandomWindowSize( rand ); }
	t_CKFLOAT Pulsar::getRandomRate() { return this->getRandomWindowSize(); }

	// pitch
	t_CKFLOAT freq = 1.f;
	// increment
	t_CKFLOAT increment = 0.f;
	// phase
	t_CKFLOAT phase = 0.f;
	// is it cosine or sine
	t_CKBOOL isCos = FALSE;
};

#endif