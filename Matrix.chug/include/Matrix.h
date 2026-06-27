// general matrix class with a signal processing twang
#include "../../include/Smoother.h"

const t_CKFLOAT ZERO_THRESHOLD = 1e-3;
const t_CKFLOAT SMOOTHING_MS = 5.0; // 5ms smoothing window

template <typename T>
class Matrix
{
public:
    Matrix::Matrix( unsigned int fs, unsigned int n_size ) : _size( n_size ), scale( 2.0 / n_size )
    {
        // create pointers to pointers
        _data = new T*[_size];
        _target = new T*[_size];
        _current = new T*[_size];
        
        // create arrays
        for( int i = 0; i < _size; i++ ) 
        {
            _data[i] = new T[_size]();
            _target[i] = new T[_size]();
            _current[i] = new T[_size]();
        }
        
        _smoothing = new bool*[_size];
        for( int i = 0; i < _size; i++ ) 
        {
            _smoothing[i] = new bool[_size]();
        }

        // zero out
        zero();
        _coeff = 0.0; // will be set in prepare()
        init( fs );
    }

    Matrix::~Matrix()
    {
        // delete pointers 
        for ( int i = 0; i < _size; i++ ) 
        {
            delete[] _data[i];
            delete[] _target[i];
            delete[] _current[i];
            delete[] _smoothing[i];
        }
        // delete pointer of pointers
        delete[] _data;
        delete[] _target;
        delete[] _current;
        delete[] _smoothing;
    }

    Matrix( const Matrix& ) = delete;
    Matrix& operator=( const Matrix& ) = delete;

    // call this once with the sample rate before processing
    void init( t_CKFLOAT fs )
    {
        // calc smoothing coefficient for exponential moving average
        // tau = smoothing time constant
        t_CKFLOAT tau = SMOOTHING_MS / 1000.f;
        _coeff = 1.0 - exp( -1.0 / ( tau * fs ) );
    }

    int Matrix::zero() 
    {
        for( int i = 0; i < _size; i++ )
        {
            for( int j = 0; j < _size; j++ )
            {
                _data[i][j] = 0;
                _target[i][j] = 0;
                _current[i][j] = 0;
                _smoothing[i][j] = false;
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
                set( i, j, val );
            }
        }
        return 0;
    }

    // set specific entry with smoothing
    int Matrix::set( unsigned int col, unsigned int row, T val )
    {
        if( col >= _size || row >= _size) return 1;
        
        // only update if value actually changed
        if( _target[col][row] != val ) 
        {
            _target[col][row] = val;
            _smoothing[col][row] = true;
            
            // init current value if this is the first change
            if( !_current[col][row] && _data[col][row] == 0 ) {
                _current[col][row] = _data[col][row];
            }
        }
        return 0;
    }
    
    // immediate set (no smoothing, for initialization)
    int Matrix::setImmediate( unsigned int col, unsigned int row, T val )
    {
        if( col >= _size || row >= _size) return 1;
        _data[col][row] = val;
        _target[col][row] = val;
        _current[col][row] = val;
        _smoothing[col][row] = false;
        return 0;
    }

    // get specific entry
    T Matrix::get( unsigned int col, unsigned int row )
    {
        if( col >= _size || row >= _size) return 0;
        return _data[col][row];
    }

    // create an identity matrix of the current size
    int Matrix::identity()
    {
        // clear all entries
        for( int i = 0; i < _size; i++ ) {
            for( int j = 0; j < _size; j++ ) {
                setImmediate( i, j, (i == j) ? 1 : 0 );
            }
        }
        return 0;
    }

    int Matrix::size() { return _size; }

    T Matrix::operator()( unsigned int col, unsigned int row )
    {
        return get( col, row );
    }

    void Matrix::operator+=( T add )
    {
        for( int i = 0; i < _size; i++ )
        {
            for( int j = 0; j < _size; j++ )
            {
                set( i, j, _target[i][j] + add );
            }
        }
    }

    void Matrix::operator*=( T mult )
    {
        for( int i = 0; i < _size; i++ )
        {
            for( int j = 0; j < _size; j++ )
            {
                set( i, j, _target[i][j] * mult );
            }
        }
    }

    void Matrix::tick( SAMPLE* in, SAMPLE* out, unsigned nframes )
    {
        memset( out, 0, sizeof( SAMPLE ) * _size * nframes );
        
        // smooth all coefficients
        if( _coeff > 0 ) {
            for( int i = 0; i < _size; i++ ) 
            {
                for( int j = 0; j < _size; j++ ) 
                {
                    if( _smoothing[i][j] ) 
                    {
                        // exponential moving average
                        _current[i][j] += _coeff * (_target[i][j] - _current[i][j]);
                        _data[i][j] = _current[i][j];
                        
                        // check if we've reached target
                        if( fabs(_current[i][j] - _target[i][j]) < ZERO_THRESHOLD ) {
                            _data[i][j] = _target[i][j];
                            _smoothing[i][j] = false;
                        }
                    }
                }
            }
        }
        
        // matrix multiply with current smoothed values
        for( int f = 0; f < nframes; f++ )
        {
            for( int i = 0; i < _size; i++ )
            {
                SAMPLE sum = 0;
                for( int j = 0; j < _size; j++ )
                {
                    sum += in[f * _size + j] * _data[i][j];
                }
                out[f * _size + i] += sum * scale;
            }
        }
    }

private:
    T** _data = nullptr;      // current active values
    T** _target = nullptr;    // target values
    T** _current = nullptr;   // interpolated values during smoothing
    bool** _smoothing = nullptr; // which coefficients are smoothing
    t_CKFLOAT _coeff = 0.0;   // smoothing coefficient
    t_CKFLOAT scale = 0.0;
    unsigned int _size = 0;
};