// a quark is a small ( intended to be used in large amounts ) granulator
// it hold a reference to source audio and granulates that audio via a few controls

#ifndef QUARK_H
	#define QUARK_H

#include "Phasor.h"
#include "Smoother.h"
#include "Windower.h"
#include "stk/include/Stk.h"
#include "stk/include/FileRead.h"

#define GRAIN_IN_PROGRESS false 
#define GRAIN_DONE true

class Quark
{
public:
	Quark::Quark( unsigned int fs )
	{
		_fs = fs;
		playback = new Phasor( _fs );
		window = new Windower( _fs );
		pitch_slew = new Smoother( _fs );
		position_slew = new Smoother( _fs );
	}

	// overloaded constructor
	Quark::Quark( unsigned int fs, stk::StkFrames& source, unsigned int n_channel = 0 )
	{
		_fs = fs;
		playback = new Phasor( _fs );
		window = new Windower( _fs );
		pitch_slew = new Smoother( _fs );
		position_slew = new Smoother( _fs );
		this->setBuffer( source, n_channel );
	}

	double Quark::tick()
	{
		// output
		double out = 0.0;
		if( go )
		{
			// if we're graining
			if( this->windowState() == GRAIN_IN_PROGRESS )
			{
				// how big is our loop/grain/audio segment
				float segment_size_frames = ( current_segment_size_ms * 0.001f ) * _buffer->dataRate();
				// where do i look
				float frame_index = position_slew->getCurrent() + ( playback->tick() * segment_size_frames );
				// wrap within the file
				frame_index = fmod( frame_index, (float)file_size_frames );
				if( frame_index < 0.f ) frame_index += file_size_frames;
				// lookup and window
				out = window->tick( _buffer->interpolate( frame_index, channel ) );
			}
			// if we aren't graining
			else if( this->windowState() == GRAIN_DONE )
			{
				// debug
				// printf( "Position %f, Pitch %f, Window size (ms) %f \n", position_slew->getCurrent(), pitch_slew->getCurrent(), window->getSizeMs() );

				// start a new grain
				if( loop ) this->trigger();
			}
		}
		// return
		return out;
	}

	// loop on
	void Quark::loopOn() { loop = true; }

	// loop off
	void Quark::loopOff() { loop = false; }

	// loop state
	bool Quark::loopState() { return loop; }

    // freeze and reset
    void Quark::off() { go = false; window->reset(); }

    // on
    void Quark::on() { go= true; this->trigger(); }

    // quark state
    bool Quark::state() { return go; }

    // shoot
	void Quark::trigger() 
	{ 
		go = true; 

		// new size
		current_segment_size_ms = segment_size_ms;
		window->setSize( current_segment_size_ms );

		// a change in window size means a change in phasor playback speed to maintain the current pitch
		pitch_slew->instant( pitch );

		// slew
		float phasor_freq = pitch_slew->tick() / ( current_segment_size_ms * 0.001f );
		playback->setFrequency( phasor_freq, TRUE );
		position_slew->tick(); // move

		window->trigger(); // start windowing
	}

    // are we playing
	bool Quark::windowState() { return window->state(); }

	// link audio buffer
	void Quark::setBuffer( stk::StkFrames& source, unsigned int n_channel = 0 )
	{
		go = false; // stop doing every thing
		_buffer = &source; // store the location of our audio
		this->channel = n_channel;
		file_size_frames = ( _buffer->size() / _buffer->channels() );
		go = true;
	}

	// clear buffer & set channel = 0
	void Quark::clearBuffer()
	{
		go = false;
		_buffer = nullptr;
		channel = 0;
	}

	// set position
	void Quark::setPosition( float new_position )
	{
		// save out initially given position
		position = new_position;
		// clamp and scale
		float segment_offset = std::max( 0.f, std::min( file_size_frames * position, (float)( file_size_frames - 1 ) ) );
		position_slew->setTarget( segment_offset, 1.f );
	}

	// set position
	void Quark::setPositionInstant( float new_position )
	{
		// save out initially given position
		position = new_position;
		// clamp and scale
		float segment_offset = std::max( 0.f, std::min( file_size_frames * position, (float)( file_size_frames - 1 ) ) );
		position_slew->instant( segment_offset );
	}

	// get position
	float Quark::getPosition()
	{
		return position_slew->getTarget();
	}

	// set pitch
	void Quark::setPitch( float new_pitch )
	{
		pitch = new_pitch;
	}

	// set pitch instantly
	void Quark::setPitchInstant( float new_pitch )
	{
		pitch = new_pitch;
		pitch_slew->instant( pitch );
	}

	// get pitch
	float Quark::getPitch()
	{
		return pitch_slew->getTarget();
	}

	// set size of window
	void Quark::setSize( float new_size )
	{
		segment_size_ms = new_size;
	}

	// get size of window in ms
	float Quark::getSize()
	{
		return window->getSizeMs();
	}

	unsigned int Quark::bufferSize()
	{
		if( _buffer ) return _buffer->size() / _buffer->channels();
		else return 0;
	}

    // set channel ( default to 0 )
    void Quark::setChannel( unsigned int n_channel )
    {
        channel = n_channel;
        if( channel > ( _buffer->channels() - 1 ) ) channel = _buffer->channels();
    }

    // get which channel we're reading
    unsigned int Quark::getChannel()
    {
        return this->channel;
    }

private:
	Phasor* playback = nullptr;
	Smoother* pitch_slew = nullptr;
	Smoother* position_slew = nullptr;
	Windower* window = nullptr;
	stk::StkFrames* _buffer = nullptr;
	unsigned int _fs = 0; // our internal sample rate
    unsigned int channel = 0; // which channel of the buffer to read
	float position = 0.f; // base position
	float segment_size_ms = 1.f; // size of the audio we need to read
	float current_segment_size_ms = 100.f; // this is "memory" variable used in the tick function
	float pitch = 1.f; // base pitch
	unsigned int file_size_frames = 0; // file size in frames ( samples / channels )
	bool go = false;
	bool loop = true;
};

#endif /* QUARK_H */