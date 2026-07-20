//==========================================================
//
// Swarm.h : Summer 2026 : everett m. carpenter
// SoundMatter is a class which utilizes Grains as a means
// of granulating a file or input. It uses a tick function 
// with an input, where the input value is ideally a sample 
// value from an audio file or input.
// 
//==========================================================

#ifndef FILE_GRAIN_H
	#define FILE_GRAIN_H

#define __LITTLE_ENDIAN__
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

#include "stk/include/FileRead.h"
#include "stk/include/Noise.h"
#include "stk/include/Stk.h"
#include "Quark.h"

class SoundMatter
{
public:

	//=======================================================================
	//
	//	names: constructor
	//	desc: allocates and configures everything
	//	args: sample rate and optional int for how many individual grains
	// 
	//=======================================================================

	SoundMatter::SoundMatter( unsigned int fs, unsigned int size = 4 )
	{
		// sample rate
		_fs = fs;
		stk::Stk::setSampleRate( _fs );
		this->createBuffer();
		// num
		num_grains = size;
		// scale down
		scale = 1.0 / num_grains;
		// random
		random = new stk::Noise( time( NULL ) );
		// positional_slew 
		position_slew = new Smoother( _fs );
		// pitch_slew
		pitch_slew = new Smoother( _fs );
		// create matter
		quantum = new Quark*[num_grains];
		// configure matter
		for( int i = 0; i < num_grains; i++ ) { quantum[i] = new Quark( fs, *buffer ); quantum[i]->on(); }
		// init
		this->setSize( this->base_size );
		this->setPitchInstant( 1.f );
		this->setPosition( 0.f );
	}

	//=======================================================================
	//
	//	names: destructor
	//	desc: deallocates and deletes everything
	//	args: none
	// 
	//=======================================================================


	SoundMatter::~SoundMatter()
	{
		// destroy matter
		for( int i = 0; i < num_grains; i++ ) { delete quantum[i]; quantum[i] = nullptr; }
		delete[] quantum; quantum = nullptr;
		// destroy again
		delete random; random = nullptr;
		// destroy again
		delete position_slew; position_slew = nullptr;
		// destroy again
		delete pitch_slew; pitch_slew = nullptr;
		// delete buf
		this->deleteBuffer();
	}

	//=======================================================================
	//
	//	name(s): tick
	//	desc: returns a single sample value (no buffering)
	//	args: none
	// 
	//=======================================================================

	double SoundMatter::tick()
	{
		// output
		double out = 0.0;
		// if we're good to go
		if( go )
		{
			// cycle through
			for( int i = 0; i < num_grains; i++ ) 
			{
				// the amalgamation of sound
				out += quantum[i]->tick();
				// create new grain parameters if resting
				if( quantum[i]->windowState() ) newGrain( quantum[i] );
				// if our grain is loop and finished, shoot off a new one
				if( quantum[i]->windowState() && quantum[i]->loopState() ) quantum[i]->trigger();
			}
			// don't use tick functions in loops! that defeats the point of a time normalized tick function everett!
			// anyways, advance
			pitch_slew->tick();
			position_slew->tick();
			// scale because if we don't prevent blowing our ears out, ChucK definitely won't!
			out *= scale;
			// soft clip
			out = tanh( out );
		}
		// return
		return out;
	}

	//=======================================================================
	//
	//	name(s): newGrain
	//	desc: when a grain has finished, this function creates a new one.
	//		  applying a new (oftentimes randomized) pitch, window size,
	//		  and position. 
	//	args: pointer to the finished grain
	// 
	//=======================================================================

	// create a new 
	void SoundMatter::newGrain( Quark* particle )
	{
		// calculate our new size using a randomized factor
		float n_size = base_size + (  0.5 * ( random->tick() + 1.0 ) * random_size );
		// clamp value 
		n_size = std::max( 1.f, n_size );
		// provide new value to quark
		particle->setSize( n_size );

		// pitch is easy(ish), same as above
		float n_pitch = pitch_slew->getCurrent() + ( 0.5 * ( random->tick() + 1.0 ) * random_pitch );
		// clamp
		n_pitch = std::max( 0.f, n_pitch );
		// have the particle instantly jump there, if it slews, then quarks will just endlessy drift through pitch space
		particle->setPitchInstant( n_pitch );

		// this one is the worst
		float random_offset_frames = ( random_position * 0.001f ) * _fs; // convert random_position to samples
		// divide by the buffer size 
		random_offset_frames /= (float)this->size();
		// create randomized position
		float n_position = position_slew->getCurrent() + ( 0.5 * ( random->tick() + 1.0 ) * random_offset_frames );
		// clamp, of course
		n_position = std::max( 0.f, std::min( n_position, 1.f ) );
		// also instantly jump so that we aren't drifting forever
		particle->setPositionInstant( n_position );
		// debug
		// printf( "Size %f, pitch %f, position %f \n", n_size, n_pitch, n_position );
	}

	//=======================================================================
	//
	//	name(s): start & stop
	//	desc: silence / don't silence quarks
	//	args: none
	// 
	//=======================================================================

	void SoundMatter::start() 
	{ 
		// turn everything on
		go = true;
		for( int i = 0; i < num_grains; i++ ) quantum[i]->on();
	}

	void SoundMatter::stop() 
	{ 
		// turn everything off
		go = false;
		for( int i = 0; i < num_grains; i++ ) quantum[i]->off();
	}

	//=======================================================================
	//
	//	name(s): set*, get* (and variations)
	//	desc: set the size, pitch and position of underlying quarks
	//	args: size pitch or position
	// 
	//=======================================================================

	void SoundMatter::setSize( float n_size_ms )
	{
		base_size = n_size_ms;
		for( int i = 0; i < num_grains; i++ ) 
		{
			// we gotta wrap around 
			float n_size = base_size + ( random->tick() * random_size );
			n_size = std::max( 1.f, n_size );
			quantum[i]->setSize( n_size );
		}
	}
	
	float SoundMatter::getSize() { return base_size; }

	void SoundMatter::setPitch( float n_pitch )
	{
		pitch_slew->setTarget( n_pitch, 1000.f );
		// for( int i = 0; i < num_grains; i++ ) quantum[i]->setPitch( base_pitch + ( random->tick() * random_pitch ) );
	}

	void SoundMatter::setPitchInstant( float n_pitch )
	{
		pitch_slew->instant( n_pitch );
		// for( int i = 0; i < num_grains; i++ ) quantum[i]->setPitch( base_pitch + ( random->tick() * random_pitch ) );
	}

	float SoundMatter::getPitch() { return pitch_slew->getTarget(); }

	void SoundMatter::setPosition( float n_position ) 
	{
		position_slew->setTarget( n_position, 240.f );
	}

	void SoundMatter::setPosition( unsigned int n_position )
	{
		position_slew->setTarget( (float)n_position / (float)this->size(), 40.f); // convert
	}

	float SoundMatter::getPosition() { return position_slew->getTarget(); }

	//=======================================================================
	//
	//	name(s): setRandom*, getRandom*
	//	desc: set the randomness of size, pitch and position of quarks
	//	args: size pitch or position
	// 
	//=======================================================================

	void SoundMatter::setRandomSize( float random_si ) { random_size = random_si; }
	float SoundMatter::getRandomSize() { return random_size; }

	void SoundMatter::setRandomPitch( float random_pit ) { random_pitch = random_pit; }
	float SoundMatter::getRandomPitch() { return random_pitch; }

	void SoundMatter::setRandomPosition( float random_pos_ms ) { random_position = random_pos_ms; }
	float SoundMatter::getRandomPosition() { return random_position; }

	//=======================================================================
	//
	//	name(s): openFile
	//	desc: if we have an internal buffer (controlled by this class instance),
	//		  open up a file and place it in the buffer
	//	args: c string to file path
	// 
	//=======================================================================

	void SoundMatter::openFile( const char* path )
	{
		if( internalBuffer )
		{
			// don't do anything
			go = false;

			// if one is open, close the file and delete the buffer
			if( file_read->isOpen() ) file_read->close();
		
			// clear 
			delete buffer;

			// convert C string to C++ string
			std::string cppString = path;

			// open!
			file_read->open( cppString );

			// resize!
			buffer = new stk::StkFrames( 0.f, file_read->fileSize(), file_read->channels() );
			// sample rate
			buffer->setDataRate( file_read->fileRate() );
			// read!
			file_read->read( *buffer, 0, true );
			// give to quarks and assign them to channels
			for( int i = 0; i < num_grains; i++ ) { quantum[i]->setBuffer( *buffer, i % buffer->channels() ); }
			// good to go
			go = true;
		}
	}

	//=======================================================================
	//
	//	name(s): closeFile
	//	desc: if we have our own buffer, close the file and delete the buffer
	//	args: size pitch or position
	// 
	//=======================================================================

	void SoundMatter::closeFile()
	{
		// we don't wanna delete what buffer is pointing to if it's not ours
		if( internalBuffer )
		{
			// stop doing anything
			go = false;
			// close the file
			file_read->close();
			// unlink the quarks
			for( int i = 0; i < num_grains; i++ ) quantum[i]->clearBuffer();
			// clear buffer
			delete buffer; buffer = nullptr;
		}
		else 
		{
			// have the quarks stop listening
			for( int i = 0; i < num_grains; i++ ) quantum[i]->clearBuffer();
			// if someone called this and we aren't using our own buffer, it's probably best to assume the outside buffer we're using is goiung to be deleted
			buffer = nullptr;
		}
	}

	//=======================================================================
	//
	//	name(s): linkOutsideBuffer
	//	desc: set our buffer to point to someone else's
	//	args: pointer to buffer
	// 
	//=======================================================================

	// provided an outside buffer, utilize this instead of an interally allocated one
	void SoundMatter::linkOutsideBuffer( stk::StkFrames* n_buffer )
	{
		// if we're using our own, we need to dispose of it first
		if( internalBuffer ) this->deleteBuffer();
		// point to this!
		buffer = n_buffer;
		// give to quarks and assign them to channels
		for( int i = 0; i < num_grains; i++ ) { quantum[i]->setBuffer( *buffer, i % buffer->channels() ); }
		internalBuffer = false; // we're using an outside buffer
	}

	//=======================================================================
	//
	//	name(s): size
	//	desc: return the size of the buffer we're using
	//	args: none
	// 
	//=======================================================================

	// how big is the buffer
	unsigned int SoundMatter::size() { return buffer->size() / buffer->channels(); }

	//=======================================================================
	//
	//	name(s): create & delete buffer 
	//	desc: internal functions for managing instance's buffer
	//	args: none
	// 
	//=======================================================================

private:
	// create internal audio buffer
	void SoundMatter::createBuffer()
	{
		// buffer
		buffer = new stk::StkFrames( 1, 1 );
		// read
		file_read = new stk::FileRead();
		// is there an internal buffer?
		internalBuffer = true;
	}

	// delete internal audio buffer
	void SoundMatter::deleteBuffer()
	{
		if( internalBuffer )
		{
			// destroy again
			delete file_read; file_read = nullptr;
			// once more
			delete buffer; buffer = nullptr;
		}
		else
		{
			buffer = nullptr;
		}
		// is/was there an internal buffer?
		internalBuffer = false;
	}
	
protected:
	Quark** quantum = nullptr; // little grains
	stk::Noise* random = nullptr; // randomization
	stk::FileRead* file_read = nullptr; // this opens up a file
	stk::StkFrames* buffer = nullptr; // everyone reads from here
	Smoother* position_slew = nullptr; // this enables us to slew our position from a high level [0.0,1.0]
	Smoother* pitch_slew = nullptr; // slew pitch from high level
	unsigned int num_grains = 0; // number of grains
	unsigned int _fs = 0; // sample rate
	float scale = 0.f; // scale down the grains
	float random_position = 0.f; // in milliseconds
	float random_pitch = 0.f; // in multiple of the source file
	float random_size = 0.f; // in ms
	float base_size = 200.f; // in ms
	bool go = false;
	bool internalBuffer = true;
};

#endif