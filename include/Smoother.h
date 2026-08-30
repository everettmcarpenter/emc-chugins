#ifndef SMOOTHER_H
	#define SMOOTHER_H

#include "chugin.h"

class Smoother
{
public:
	// constructor
	Smoother( int fs )
	{
		this->fs = fs;
	}

	// overloaded constructor
	Smoother( int fs, double init )
	{
		this->fs = fs;
		// set init value
		instant( init );
	}

	// set the target
	void setTarget( double target, double howLongMs = 30.f )
	{
		// ms -> samples & round up ( number of samples we will need to get to target )
		int rampLength = static_cast<int>( msToSamples( howLongMs ) );
		// if it's going to take less than 1 sample to interpolate, just use one sample
		if( rampLength < 1 ) rampLength = 1;

		// set new target
		this->target = target;
		// new increment 
		increment = ( this->target - current ) / rampLength;
		// how many left ( ramp length )
		remaining = rampLength;
	}

	// set the target
	void setTarget( double target, unsigned int samplesToGo = 1000 )
	{
		// ms -> samples & round up ( number of samples we will need to get to target )
		unsigned int rampLength = samplesToGo;
		// if it's going to take less than 1 sample to interpolate, just use one sample
		if( rampLength < 1 ) rampLength = 1;

		// set new target
		this->target = target;
		// new increment 
		increment = ( this->target - current ) / rampLength;
		// how many left ( ramp length )
		remaining = rampLength;
	}

	// move in time
	double tick()
	{
		if( remaining > 0 )
		{
			// increment
			current += increment;
			// we have one less sample to worry about
			remaining--; 
		}
		
		// if there are no more steps to take, jump to the target
		if( remaining == 0 ) 
		{
			current = target;
		}

		// return for courtesy
		return current;
	}

	//	predict the future
	double fick()
	{
		// return this
		double next = current;
		// if there are no more steps to take, jump to the target
		if ( remaining != 0 ) next += increment;
		// return for courtesy
		return next;
	}

	// instant setup
	void instant( double target )
	{
		this->target = target;
		current = target;
		remaining = 0;
	}

	double msToSamples( double ms )
	{
		return ( ( ms * fs ) / 1000.0 );
	}

	// what's the current value?
	double getCurrent() { return current; }
	// what's the target?
	double getTarget() { return target; }
	// are we moving? 
	int isMoving() 
	{ 
		if( remaining > 0 ) return TRUE;
		else return FALSE; 
	}

	// sample rate
	double fs = 0.f; 
	// target value 
	double target = 0.f;
	// current value
	double current = 0.f;
	// increment
	double increment = 0.2f;
	// how many samples "left" 
	int remaining = 0;
};

#endif
