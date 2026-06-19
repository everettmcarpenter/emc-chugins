#ifndef PHASOR_H
    #define PHASOR_H

#include "chugin.h"

//-----------------------------------------------------------------------------
// class definition for internal chugin data
// (NOTE this isn't strictly necessary, but is one example of a recommended approach)
//-----------------------------------------------------------------------------
class Phasor
{
public:
    // constructor
    Phasor::Phasor( t_CKFLOAT fs )
    {
        this->fs = fs;
    }

    // overloaded constructor
    Phasor::Phasor( t_CKFLOAT fs, t_CKFLOAT freq )
    {
        this->fs = fs;
        setFrequency( freq );
    }

    // for chugins extending UGen
    SAMPLE Phasor::tick()
    {
        SAMPLE out = phase;
        phase += normFreq;
        // wrap around
        if ( phase >= 1.0 ) phase -= 1.0;
        return out;
    }

    // return next tick value without advancing
    SAMPLE Phasor::next()
    {
        SAMPLE out = phase;
        out += normFreq;
        if( out >= 1.0 ) out -= 1.0;
        return out;
    }

    // set freq
    void Phasor::setFrequency( t_CKFLOAT freq, t_CKBOOL reset = TRUE ) 
    { 
        if ( freq <= 0.0 ) { this->freq = normFreq = 0.f; }
        else { this->freq = freq; normFreq = this->freq / fs; }
        // reset if specified
        if( reset ) this->reset();
    }
    // get freq
    t_CKFLOAT Phasor::getFrequency() { return freq; }
    
    // safely reset sampling frequency
    void Phasor::setSR( t_CKFLOAT fs ) { this->fs = fs; setFrequency( freq ); }
    // retreive sampling frequency
    t_CKFLOAT Phasor::getSR() { return fs; }

    // reset
    void Phasor::reset() { phase = 0.f; }

private:
    // instance data
    t_CKFLOAT fs = 0.f;
    t_CKFLOAT freq = 1.f;
    t_CKFLOAT normFreq = 1.f;
    t_CKFLOAT phase = 0.f;
};

#endif