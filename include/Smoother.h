#ifndef SMOOTHER_H
	#define SMOOTHER_H

#include "chugin.h"

class Smoother
{
public:
	// constructor
	Smoother::Smoother( t_CKINT fs )
	{
		this->fs = fs;
	}

	// overloaded constructor
	Smoother::Smoother( t_CKINT fs, t_CKFLOAT init )
	{
		this->fs = fs;
		// set init value
		instant( init );
	}

	// set the target
	void Smoother::setTarget( t_CKFLOAT target, t_CKFLOAT howLongMs = 30.f )
	{
		// convert interpolation time to hz and normalize it to the sample rate, then round it
		t_CKINT rampLength = static_cast<t_CKINT>( ( howLongMs * 0.001 ) * fs );
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
	t_CKFLOAT Smoother::tick()
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
	t_CKFLOAT Smoother::fick()
	{
		// return this
		t_CKFLOAT next = current;
		// if there are no more steps to take, jump to the target
		if ( remaining != 0 ) next += increment;
		// return for courtesy
		return next;
	}

	// instant setup
	void Smoother::instant( t_CKFLOAT target )
	{
		this->target = target;
		current = target;
		remaining = 0;
	}

	// what's the current value?
	t_CKFLOAT Smoother::getCurrent() { return current; }
	// what's the target?
	t_CKFLOAT Smoother::getTarget() { return target; }
	// are we moving? 
	t_CKBOOL Smoother::isMoving() 
	{ 
		if( remaining > 0 ) return TRUE;
		else return FALSE; 
	}

	// sample rate
	t_CKFLOAT fs = 0.f; 
	// target value 
	t_CKFLOAT target = 0.f;
	// current value
	t_CKFLOAT current = 0.f;
	// increment
	t_CKFLOAT increment = 0.2f;
	// how many samples "left" 
	t_CKINT remaining = 0;
};

#endif