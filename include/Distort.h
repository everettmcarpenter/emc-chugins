#include "math.h"
#include "string.h"

#ifndef DISTORT_H
	#define DISTORT_H

#define PASS 0
#define SOFT_DIST 1
#define HARD_CLIP 2
#define SOFT_SYM_CLIP 3

class Distort
{
public:

	Distort() {};
	Distort( unsigned int fs ) { _fs = fs; };
	~Distort() {};

	double tick( double in )
	{
		switch( type )
		{
			case PASS:
				return pass( in );
				break;
				
			case SOFT_DIST:
				return softDist( in );
				break;

			case HARD_CLIP:
				return hardClip( in );
				break;

			case SOFT_SYM_CLIP:
				return symSoftClip( in );
				break;
				
			default:
				return pass( in );
		}
	}

	void tick( double* in, double* out, unsigned int nframes )
	{
		switch( type )
		{
			case PASS:
				return pass( in, out, nframes );
				break;
				
			case SOFT_DIST:
				return softDist( in, out, nframes );
				break;

			case HARD_CLIP:
				return hardClip( in, out, nframes );
				break;

			case SOFT_SYM_CLIP:
				return symSoftClip( in, out, nframes );
				break;
				
			default:
				return pass( in, out, nframes );
		}
	}

protected:

	// single sample
	double softDist( double in )
	{
		if( in > 0.0 )
			return 1.0 - exp( -1.0 * abs( in ) );
		else if( in < 0.0 ) 
			return -1.0 + exp( -1.0 * abs( in ) );
		else 
			return 0.0; 
	}

	// vectorized soft distortion
	void softDist( double* in, double* out, unsigned int nframes )
	{
		for( unsigned int f = 0; f < nframes; f++ )
		{
			out[f] = softDist( in[f] );
		}
	}

	// single sample
	double symSoftClip( double in )
	{
		double absin = abs( in );
		
		if( absin <= 0.3333 && absin >= 0 )
			return 2.0 * in;
		else if( 0.3333 <= absin && absin <= 0.6666 ) 
			return ( 3.0 - ( ( 2.0 - 3.0 * in ) * ( 2.0 - 3.0 * in ) ) ) * 0.3333;
		else if( 0.6666 <= absin && absin <= 1.0 ) 
			return 1.0; 
		else return in;
	}

	// vectorized soft symmetrical clipping
	void symSoftClip( double* in, double* out, unsigned int nframes )
	{
		for( unsigned int f = 0; f < nframes; f++ )
		{
			out[f] = symSoftClip( in[f] );
		}
	}

	// single sample
	double hardClip( double in )
	{
		if( in > 1.0 )
			return 1.0;
		else if( in < -1.0 ) 
			return -1.0;
		else 
			return in; 
	}

	// vectorized hard clip
	void hardClip( double* in, double* out, unsigned int nframes )
	{
		for( unsigned int f = 0; f < nframes; f++ )
		{
			out[f] = hardClip( in[f] );
		}
	}
	
	// single sample pass
	double pass( double in )
	{
		return in;
	}

	// vectorized pass
	void pass( double* in, double* out, unsigned int nframes )
	{
		for( unsigned int f = 0; f < nframes; f++ )
		{
			out[f] = in[f];
		}
	}

public:

	bool setMode( int mode )
	{
		// if we don't have a corresponding mode, just keep the current one and throw failure
		if( mode > SOFT_SYM_CLIP || mode < 0 ) return false; 
		else { type = mode; return true; } // else 
	}

	int getMode() { return type; }

private:

	unsigned int _fs = 0;
	double gain = 1.0;
	int type = 0; // 0 no sound, 1 soft clip
};

#endif
