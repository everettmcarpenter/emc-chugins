// ------------------------------------------------
//	
// Henon Map noise gen snagged from Neil Thornock's
// implementation for RTCmix
//
// emc. ( who added the soft clipping )
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
	HenonMap( unsigned int fs ) : x( 0 ), y( 0 ), del( 0 ), alpha( 0 ), beta( 0 ), x_1( 0 ), y_1( 0 ) {};
	
	// give me sample
	double tick()
	{
		return map();
	}

	// give me buffer
	void tick( double* output, unsigned int nframes )
	{
		// assuming we have a one channel buffer
		for( unsigned int f = 0; f < nframes; f++ )
			output[f] = map();
	}
	
	// give me interleaved buffer
	void tick( double* output, unsigned int nframes, unsigned int channels )
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
		x = y + 1.0 - ( alpha * x * x );
		// y is essentially weighted feedback 
		y = beta * del;
		// save for later
		del = x;
		
		// edge case ( this was neil's og edge case, but we might try something else ) 
		if( x > 1.0 || x < -1.0 )
		{
			out = sin( x + y );  
		}
		else
			out = x;

		// send it out
		return dcBlock( out );
	}

	double dcBlock( double in )
	{
		double out;
		// explanation for magic constant may be found here https://ccrma.stanford.edu/~jos/filters/DC_Blocker.html
		out = in - x_1 + 0.9 * y_1;
		x_1 = in; // woo
		y_1 = out; // woo
		return out;
	}

	// give me set me alpha
	double a( double newA ) { alpha = newA; return alpha; }
	// give me set me beta
	double b( double newB ) { beta = newB; return beta; }
	
	// give me alpha
	double a() { return alpha; }
	// give me beta
	double b() { return beta; }

private:
	double x, y, del, alpha, beta;
	double y_1; // this is used for dc blocking
	double x_1; // this too
};

#endif /* HENON_H */
