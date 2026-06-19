// general matrix class with a signal processing twang

#include "../../include/Smoother.h"

const t_CKFLOAT ZERO_THRESHOLD = 1e-3;

template <unsigned int n_size, typename T >
class Matrix
{
public:
	Matrix::Matrix( unsigned ) : _size( n_size )
	{
		// create pointers to pointers
		_data = new T*[_size];
		// create pointers 
		for( int i = 0; i < _size; i++ ) _data[i] = new T[_size]();
		// zero out
		zero();
	}

	Matrix::~Matrix()
	{
		// delete  pointers 
		for ( int i = 0; i < _size; i++ ) delete[] _data[i];
		// delete pointer of pointers
		delete[] _data;
	}

	Matrix( const Matrix& ) = delete;
	Matrix& operator=( const Matrix& ) = delete;

	int Matrix::zero() 
	{
		for( int i = 0; i < _size; i++ )
		{
			for( int j = 0; j < _size; j++ )
			{
				_data[i][j] = 0;
			}
		}
		return 0;
	}

	// fill all with one value
	int Matrix::fill( T val )
	{
		for( int i = 0; i < _size; i++ )
		{
			for( int j = 0; j < _size; j++ )
			{
				_data[i][j] = val;
			}
		}
		return 0;
	}

	// set specific entry
	int Matrix::set( unsigned int col, unsigned int row, T val )
	{
		if( col >= _size || row >= _size) return 1;
		else _data[col][row] = val;
		return 0;
	}

	// get specific entry
	int Matrix::get( unsigned int col, unsigned int row )
	{
		if( col >= _size || row >= _size) return 1;
		else return _data[col][row];
	}

	// create an identity matrix of the current size
	int Matrix::identity()
	{
		// clear all entries
		zero();
		// set the diagonal
		for( int i = 0; i < _size; i++ ) _data[i][i] = 1;
		return 0;
	}

	// get size
	int Matrix::size() { return _size; }

	T Matrix::operator()( unsigned int col, unsigned int row )
	{
		if( col >= _size || row >= _size) return 0;
		else return _data[col][row];
	}

	void Matrix::operator+=( T add )
	{
		for( int i = 0; i < _size; i++ )
		{
			for( int j = 0; j < _size; j++ )
			{
				_data[i][j] += add;
			}
		}
	}

	void Matrix::operator*=( T mult )
	{
		for( int i = 0; i < _size; i++ )
		{
			for( int j = 0; j < _size; j++ )
			{
				_data[i][j] *= mult;
			}
		}
	}

	void Matrix::tick( SAMPLE* in, SAMPLE* out, unsigned nframes )
	{
		memset( out, 0, sizeof( SAMPLE ) * _size * nframes ); // clear
		// matrix mult
		for( int f = 0; f < nframes; f++ )
		{
			for( int i = 0; i < _size; i++ )
			{
				for( int j = 0; j < _size; j++ ) // yikes!
				{
					out[f * _size + i] += scale * in[f * _size + j] * _data[i][j];
				}
			}
		}
	}

private:
	T** _data = nullptr;
	t_CKFLOAT scale = 1.0 / n_size;
	unsigned int _size = n_size;
};