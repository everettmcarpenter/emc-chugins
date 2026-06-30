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
	Quark::Quark( unsigned int fs, stk::StkFrames& source )
	{
		_fs = fs;
		playback = new Phasor( _fs );
		window = new Windower( _fs );
		pitch_slew = new Smoother( _fs );
		position_slew = new Smoother( _fs );
		this->setBuffer( source );
	}

	double Quark::tick()
	{
		// output
		double out = 0.0;
		if( go )
		{
			// if we're graining
			if( this->state() == GRAIN_IN_PROGRESS )
			{
				// how big is our loop/grain/audio segment
				float segment_size_frames = ( current_segment_size_ms * 0.001f ) * _fs;
				// where do i look
				float frame_index = position_slew->tick() + ( playback->tick() * segment_size_frames );
				// wrap within the file
				frame_index = fmod( frame_index, (float)file_size_frames );
				if( frame_index < 0.f ) frame_index += file_size_frames;
				// lookup and window
				out = window->tick( _buffer->interpolate( frame_index ) );
			}
			else if( this->state() == GRAIN_DONE )
			{
				// new size
				current_segment_size_ms = segment_size_ms;
				window->setSize( current_segment_size_ms );
				// a change in window size means a change in phasor playback speed to maintain the current pitch
				pitch_slew->instant( pitch );
				// slew 
				float phasor_freq = pitch_slew->tick() / ( current_segment_size_ms * 0.001f );
				playback->setFrequency( phasor_freq, TRUE );
				position_slew->tick(); // move
				// start a new grain
				this->trigger();
			}
		}
		// return
		return out;
	}

	void Quark::trigger() { window->trigger(); }
	bool Quark::state() { return window->state(); }

	// link audio buffer
	void Quark::setBuffer( stk::StkFrames& source )
	{
		go = false; // stop doing every thing
		_buffer = &source; // store the location of our audio
		file_rate = _buffer->size() / _buffer->channels();
		file_size_frames = ( _buffer->size() / _buffer->channels() );
		go = true;
	}

	// clear buffer
	void Quark::clearBuffer()
	{
		go = false;
		_buffer = nullptr;
	}

	// set position
	void Quark::setPosition( float new_position )
	{
		// clamp and scale
		segment_offset = std::max( 0.f, std::min( file_size_frames * new_position, (float)( file_size_frames - 1 ) ) );
		position_slew->setTarget( segment_offset );
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

	unsigned int Quark::size()
	{
		if( _buffer ) return _buffer->size() / _buffer->channels();
		else return 0;
	}

private:
	Phasor* playback = nullptr;
	Smoother* pitch_slew = nullptr;
	Smoother* position_slew = nullptr;
	Windower* window = nullptr;
	stk::StkFrames* _buffer = nullptr;
	unsigned int _fs = 0;
	float segment_offset = 0.f;
	float segment_size_ms = 1.f;
	float segment_size_frames = 1.f;
	float current_segment_size_ms = 100.f;
	float pitch = 1.f;
	float file_rate = 0.f;
	unsigned int file_size_frames = 0;
	bool go = true;
};

#endif /* QUARK_H */