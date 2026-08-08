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
    Phasor( t_CKFLOAT fs )
    {
        this->fs = fs;
    }

    // overloaded constructor
    Phasor( t_CKFLOAT fs, t_CKFLOAT freq, bool mode = false )
    {
        this->fs = fs;
        this->oneShot = mode;
        setFrequency( freq );
    }

    // tick
    SAMPLE tick()
    {
        SAMPLE out = phase;
        if( !done ) phase += normFreq; // if we aren't done, move forward
        // wrap around, if it's one shot, note that we are done
        if ( phase >= 1.0 ) 
        { 
            phase -= 1.0; 
            if ( oneShot ) 
            { 
                done = true; 
                phase = 0.f;
            }
        }
        return out;
    }

    // return next tick value without advancing
    SAMPLE next()
    {
        SAMPLE out = phase;
        out += normFreq;
        if( out >= 1.0 ) out -= 1.0;
        return out;
    }

    // set freq
    void setFrequency( t_CKFLOAT freq, t_CKBOOL reset = TRUE ) 
    { 
        if ( freq <= 0.0 ) { this->freq = normFreq = 0.f; }
        else { this->freq = freq; normFreq = this->freq / fs; }
        // reset if specified
        if( reset ) this->reset();
    }
    // get freq
    t_CKFLOAT getFrequency() { return freq; }
    
    // safely reset sampling frequency
    void setSR( t_CKFLOAT fs ) { this->fs = fs; setFrequency( freq ); }
    // retreive sampling frequency
    t_CKFLOAT getSR() { return fs; }

    // shoot!
    void trigger() { done = false; phase = 0.f; }

    // is it on
    bool state() { return done; }

    // reset
    void reset() { phase = 0.f; }

private:
    // instance data
    t_CKFLOAT fs = 0.f;
    t_CKFLOAT freq = 1.f;
    t_CKFLOAT normFreq = 1.f;
    t_CKFLOAT phase = 0.f;
    bool oneShot = false;
    bool done = false;
};

#endif