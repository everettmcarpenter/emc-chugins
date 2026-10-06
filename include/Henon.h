// ------------------------------------------------
//	
// Henon Map noise gen snagged from Neil Thornock's
// implementation for RTCmix
//
// emc.
//	
// ------------------------------------------------

#ifndef HENON_H
	#define HENON_H

// we need want the math
#include <iostream>
#include <math.h>

class HenonMap
{
public:
	// this is an awesome constructor
	HenonMap::HenonMap( unsigned int fs ) : x( 0 ), y( 0 ), del( 0 ), alpha( 0 ), beta( 0 ) {};
	
	// give me sample
	double HenonMap::tick()
	{
		return map();
	}

	// give me buffer
	void HenonMap::tick( double* output, unsigned int nframes )
	{
		// assuming we have a one channel buffer
		for( unsigned int f = 0; f < nframes; f++ )
			output[f] = map();
	}
	
	// give me interleaved buffer
	void HenonMap::tick( double* output, unsigned int nframes, unsigned int channels )
	{
		// assuming we have a one channel buffer
		for( unsigned int f = 0; f < nframes; f++ )
			for( unsigned int c = 0; c < channels; c++ )
				output[f * channels + c] = map();
		// this indexing will always confuse me but i know it works
		// frame = 0, channels = 2, c = 1, ( 1 )
		// frame = 1, channels = 2, c = 1, ( 3 )
		// frame = 6, channels = 2, c = 0, ( 12 )
	}

	// mapping
	double map()
	{
		// our out
		double out;
		// map! R^2 -> R^2 weighted feedback + 1.0 - independently weighted feedback squared 
		// for a variation, maybe try using an input to drive the map ( but how??? ) 
		x = y + 1.0 - ( alpha * del * del );
		// y is essentially weighted feedback 
		y = beta * del;
		// save for later
		del = x;
		
		// edge case ( this was neil's og edge case, but we might try something else ) 
		if( x > 1.0 || x < -1.0 )
			out = sin( x ); 
		else 
			out = x; 

		std::cerr << out << '\n';
		// send it out
		return out;

	}

	// give me set me alpha
	double HenonMap::a( double newA ) { alpha = newA; return alpha; }
	// give me set me beta
	double HenonMap::b( double newB ) { beta = newB; return beta; }
	
	// give me alpha
	double HenonMap::a() { return alpha; }
	// give me beta
	double HenonMap::b() { return beta; }

private:
	double x, y, del, alpha, beta;
};

#endif /* HENON_H */