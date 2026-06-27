#include "math.h"
#include "CircularBuffer.h"

template< typename T >
double mean( const T* values, unsigned int num )
{
	double average = 0.0;
	for( int i = 0; i < num; i++ ) average += values[i];
	return average /= num;
}

template< typename T >
double standard_dev( const T* values, unsigned int num )
{
	double average = mean( values, num );
	double deviation = 0;
	for( int i = 0; i < num; i++ ) deviation += ( values[i] - average ) * ( values[i] - average );
	return sqrt( deviation / num );
}

template< typename T >
double l2( const T* vec, unsigned int size )
{
	double length = 0.0;
	for( int i = 0; i < size; i++ ) length += vec[i];
	return sqrt( length );
}

template< typename T >
double lp( const T* vec, unsigned int size, float p )
{
	double measure = 0.0;
	for( int i = 0; i < size; i++ ) measure += vec[i];
	return pow( measure, 1.0 / p );
}

class LinearPredictiveCoder
{
public:
	LinearPredictiveCoder::LinearPredictiveCoder( unsigned int order )
	{
	}

protected:
	CircularBuffer<double>* in_buf = nullptr;
	CircularBuffer<double>* out_buf = nullptr;
};