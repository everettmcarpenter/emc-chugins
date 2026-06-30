#ifndef WINDOW_FUNCTIONS_H
	#define WINDOW_FUNCTIONS_H

#ifndef ONE_PI
	#define ONE_PI 3.14159265358979323846
#endif
#ifndef DOS_PI
	#define DOS_PI 2.0 * ONE_PI
#endif

#include "math.h"

// fill a buffer of some size with a hann window
void hann( double* buffer, unsigned size )
{
	for( int i = 0; i < size; i++ )
	{
		buffer[i] = 0.5 - ( 0.5 * cos( ( DOS_PI * i ) / size ) );
	}
}

// fill a buffer of some size with a hamming window
void hamming( double* buffer, unsigned size )
{
	for( int i = 0; i < size; i++ )
	{
		buffer[i] = 0.53836 - ( 0.46164 * cos( ( DOS_PI * i ) / size ) );
	}
}


// fill a buffer of some size with a blackman window
void blackman( double* buffer, unsigned size )
{
	// taken from wikipedia https://en.wikipedia.org/wiki/Window_function#Blackman_window
	double alpha = 0.16;
	double a0,a1,a2;
	a0 = ( 1.f - alpha ) / 2.f;
	a1 = 0.5; a2 = alpha / 2.f;

	for( int i = 0; i < size; i++ )
	{
		buffer[i] = a0 - a1 * cos( ( DOS_PI * i ) / size ) + a2 * cos( ( 2.f * DOS_PI * i ) / size );
	}
}

// fill a buffer of some size with a rectangular window
void rectangle( double* buffer, unsigned size )
{
	for( int i = 0; i < size; i++ )
	{
		buffer[i] = 1.f;
	}
}
#endif /* WINDOW_FUNCTIONS_H */