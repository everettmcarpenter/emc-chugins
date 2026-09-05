//==========================================================
//
// Swarm.h : Summer 2026 : everett m. carpenter
// Atmosphere is a class which utilizes Grains as a means
// of granulating a file or input. It uses a tick function 
// with an input, where the input value is ideally a sample 
// value from an audio file or input.
// 
//==========================================================

#ifndef ATMOSPHERE_H
	#define ATMOSPHERE_H

#include "stk/include/FileRead.h"
#include "stk/include/Noise.h"
#include "stk/include/Stk.h"
#include "Quark.h"

class Atmosphere
{
public:

	//=======================================================================
	//
	//	names: constructor
	//	desc: allocates and configures everything
	//	args: sample rate and optional int for how many individual grains
	// 
	//=======================================================================

	Atmosphere( unsigned int fs, unsigned int n_order, unsigned int size = 1 )
	{
		
		// sample rate
		_fs = fs;
		stk::Stk::setSampleRate( _fs );
		// generate a buffer
		this->createBuffer();
		
		// ambi
		order = n_order;
		// ambisonic channel count
		num_channels = ( order + 1 ) * ( order + 1 );
		// num
		num_grains_per_channel = size; // grains per ambisonic channel
		// num
		num_grains = num_grains_per_channel * num_channels;
		// scale down
		scale = 1.0 / num_grains;

		// random
		random = new stk::Noise( time( NULL ) );
		// positional_slew 
		position_slew = new Smoother( _fs );
		// pitch_slew
		pitch_slew = new Smoother( _fs );
		// read
		file_read = new stk::FileRead();

		// create matter
		quantum = new Quark*[num_grains];
		for( unsigned int i = 0; i < num_grains; i++ ) 
		{
			quantum[i] = new Quark( fs, *buffer );
			quantum[i]->on();
		}

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

	~Atmosphere()
	{
		// destroy matter
		for( unsigned int i = 0; i < num_grains; i++ ) 
		{
			if( quantum[i] ) 
			{
				quantum[i]->off();
				CK_SAFE_DELETE( quantum[i] );
			}
		}
		CK_SAFE_DELETE_ARRAY( quantum );
		
		// delete buf
		this->deleteBuffer();
		// destroy again
		CK_SAFE_DELETE( file_read );
		// destroy again
		CK_SAFE_DELETE( random );
		// destroy again
		CK_SAFE_DELETE( position_slew );
		// destroy again
		CK_SAFE_DELETE( pitch_slew );

	}

	//=======================================================================
	//
	//	name(s): tick
	//	desc: returns a single sample value (no buffering)
	//	args: none
	// 
	//=======================================================================

	void tick( SAMPLE* in, SAMPLE* out, unsigned int frames ) 
	{
		// if we're good to go
		if( go )
		{
			memset( out, 0, sizeof( SAMPLE ) * num_channels * frames ); // clear
			// start up the machine
			for( unsigned int f = 0; f < frames; f++ )
			{
				for( unsigned int q = 0; q < ( num_grains_per_channel * num_channels ); q++ ) 
				{
					// what channel does this particle belong to?
					unsigned int channel = quantum[q]->getChannel();
					// get audio and hope the channel index is within bounds
					out[f * num_channels + channel] += quantum[q]->tick();

					/*
					*	Idea: each frame check if all grains are off, if they are, shoot them all off, otherwise, wait until they are all completed. 
					*	If this works well, consider how each grain can be delayed/changed to create variation across the channels
					*/

					// create new grain parameters if resting
					if( quantum[q]->windowState() ) newGrain( quantum[q] );
					// if our grain is loop and finished, shoot off a new one
					if( quantum[q]->windowState() && quantum[q]->loopState() ) quantum[q]->trigger();
				}
				
				// scale the outgoing buffer ( is this more effecient than scaling every time we add the quantum tick to the output? i don't know )
				for( unsigned int c = 0; c < num_channels; c++ ) // c++ !
				{
					// yes, yes, scale the output
					out[f * num_channels + c] *= scale; 
					// why not soft clip while we're at it
					out[f * num_channels + c] = tanh( out[f * num_channels + c] );
				}

				// advance in time
				pitch_slew->tick();
				position_slew->tick();
			}
		}
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
	void newGrain( Quark* particle )
	{
		// calculate our new size using a randomized factor
		double n_size = base_size + ( 0.5 * ( random->tick() + 1.0 ) * random_size );
		// clamp value 
		n_size = std::max( 1.0, n_size );
		// provide new value to quark
		particle->setSize( n_size );

		// pitch is easy(ish), same as above
		double n_pitch = pitch_slew->getCurrent() + ( 0.5 * ( random->tick() + 1.0 ) * random_pitch );
		// clamp
		n_pitch = std::max( 0.0, n_pitch );
		// have the particle instantly jump there, if it slews, then quarks will just endlessy drift through pitch 
		
		particle->setPitchInstant( n_pitch );

		// this one is the worst
		double random_offset_frames = ( random_position * 0.001f ) * _fs; // convert random_position to samples
		// divide by the buffer size 
		random_offset_frames /= (double)this->size();
		// create randomized position
		double n_position = position_slew->getCurrent() + ( 0.5 * ( random->tick() + 1.0 ) * random_offset_frames );
		// clamp, of course
		n_position = std::max( 0.0, std::min( n_position, 1.0 ) );
		// also instantly jump so that we aren't drifting forever
		particle->setPositionInstant( n_position );

		// debug
		
		/*
		std::printf( "size %f \t ", n_size );
		std::printf( "pitch %f \t", n_pitch );
		std::printf( "position %f \n", n_position );
		*/
	}

	//=======================================================================
	//
	//	name(s): start & stop
	//	desc: silence / don't silence quarks
	//	args: none
	// 
	//=======================================================================

	void start() 
	{ 
		// turn everything on
		go = true;
		for( unsigned int i = 0; i < num_grains; i++ ) quantum[i]->on();
	}

	void stop() 
	{ 
		// turn everything off
		go = false;
		for( unsigned int i = 0; i < num_grains; i++ ) quantum[i]->off();
	}

	//=======================================================================
	//
	//	name(s): set*, get* (and variations)
	//	desc: set the size, pitch and position of underlying quarks
	//	args: size pitch or position
	// 
	//=======================================================================

	void setSize( float n_size_ms )
	{
		base_size = n_size_ms;
		for( unsigned int i = 0; i < num_grains; i++ ) 
		{
			// we gotta wrap around 
			float n_size = base_size + ( random->tick() * random_size );
			n_size = std::max( 1.f, n_size );
			// std::cout << "quark " << i << " size " << n_size << std::endl;
			quantum[i]->setSize( n_size );
		}
	}
	
	float getSize() { return base_size; }

	void setPitch( double n_pitch )
	{
		pitch_slew->setTarget( n_pitch, 100.f );
		// for( unsigned int i = 0; i < num_grains; i++ ) quantum[i]->setPitch( base_pitch + ( random->tick() * random_pitch ) );
	}

	void setPitch( double n_pitch, double ms_to )
	{
		pitch_slew->setTarget( n_pitch, ms_to );
		// for( unsigned int i = 0; i < num_grains; i++ ) quantum[i]->setPitch( base_pitch + ( random->tick() * random_pitch ) );
	}

	void setPitch( double n_pitch, unsigned int samp_to )
	{
		pitch_slew->setTarget( n_pitch, samp_to );
		// for( unsigned int i = 0; i < num_grains; i++ ) quantum[i]->setPitch( base_pitch + ( random->tick() * random_pitch ) );
	}

	void setPitchInstant( double n_pitch )
	{
		pitch_slew->instant( n_pitch );
		// for( unsigned int i = 0; i < num_grains; i++ ) quantum[i]->setPitch( base_pitch + ( random->tick() * random_pitch ) );
	}

	float getPitch() { return pitch_slew->getTarget(); }

	void setPosition( double n_position ) 
	{
		position_slew->setTarget( n_position, 240.f );
	}

	void setPosition( double n_position, double ms_to ) 
	{
		position_slew->setTarget( n_position, ms_to );
	}

	void setPosition( double n_position, unsigned int samp_to ) 
	{
		position_slew->setTarget( n_position, samp_to );
	}

	void setPosition( unsigned int n_position )
	{
		position_slew->setTarget( (float)n_position / (float)this->size(), 40.f); // convert
	}

	float getPosition() { return position_slew->getTarget(); }

	void setGap( unsigned int gap_samp )
	{
		base_gap = gap_samp;
		for( unsigned int i = 0; i < num_grains; i++ )
		{
			quantum[i]->setGap( base_gap );
		}
	}

	unsigned int getGap()
	{
		return base_gap;
	}

	//=======================================================================
	//
	//	name(s): setRandom*, getRandom*
	//	desc: set the randomness of size, pitch and position of quarks
	//	args: size pitch or position
	// 
	//=======================================================================

	void setRandomSize( float random_si ) { random_size = random_si; }
	float getRandomSize() { return random_size; }

	void setRandomPitch( float random_pit ) { random_pitch = random_pit; }
	float getRandomPitch() { return random_pitch; }

	void setRandomPosition( float random_pos_ms ) { random_position = random_pos_ms; }
	float getRandomPosition() { return random_position; }

	void setRandomGap( unsigned int random_gp ) { random_gap = random_gp; }
	unsigned int getRandomGap() { return random_gap; }

	//=======================================================================
	//
	//	name(s): openFile
	//	desc: if we have an internal buffer (controlled by this class instance),
	//		  open up a file and place it in the buffer
	//	args: c string to file path
	// 
	//=======================================================================

	bool openFile( const char* path )
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

			if( file_read->isOpen() && file_read->channels() == num_channels )
			{
				// resize!
				buffer = new stk::StkFrames( 0.f, file_read->fileSize(), file_read->channels() );
				// sample rate
				buffer->setDataRate( file_read->fileRate() );
				// read!
				file_read->read( *buffer, 0, true );
				// give to quarks and assign them to channels
				for( unsigned int i = 0; i < num_grains; i++ ) { quantum[i]->setBuffer( *buffer, i % buffer->channels() ); }
				// good to go
				go = true;
				// true == good
				return true;
			}
			else 
			{ 
				return false; 
			}
		}
		else
		{
			return true; // if we aren't using our buffer, it's not our problem
		}
	}

	//=======================================================================
	//
	//	name(s): closeFile
	//	desc: if we have our own buffer, close the file and delete the buffer
	//	args: size pitch or position
	// 
	//=======================================================================

	void closeFile()
	{
		// stop doing anything
		go = false;

		// we don't wanna delete what buffer is pointing to if it's not ours
		if( internalBuffer )
		{
			// close the file
			if( file_read->isOpen() ) file_read->close();
			// unlink the quarks
			for( unsigned int i = 0; i < num_grains; i++ ) quantum[i]->clearBuffer();
			// clear buffer
			CK_SAFE_DELETE( buffer );
		}
		else 
		{
			// have the quarks stop listening
			for( unsigned int i = 0; i < num_grains; i++ ) quantum[i]->clearBuffer();
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
	void linkOutsideBuffer( stk::StkFrames* n_buffer )
	{
		// if we're using our own, we need to dispose of it first
		if( internalBuffer ) this->deleteBuffer();
		// point to this!
		buffer = n_buffer;
		// give to quarks and assign them to channels
		for( unsigned int i = 0; i < num_grains; i++ ) { quantum[i]->setBuffer( *buffer, i % buffer->channels() ); }
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
	unsigned int size() { return buffer->size() / buffer->channels(); }

	//=======================================================================
	//
	//	name(s): create & delete buffer 
	//	desc: internal functions for managing instance's buffer
	//	args: none
	// 
	//=======================================================================

protected:
	// create internal audio buffer
	void createBuffer()
	{
		// buffer
		buffer = new stk::StkFrames( 1, 1 );
		// is there an internal buffer?
		internalBuffer = true;
	}

	// delete internal audio buffer
	void deleteBuffer()
	{
	    if( internalBuffer )
	    {
	        CK_SAFE_DELETE( buffer );
	    }
	    else if( !internalBuffer )
	    {
	        buffer = nullptr;
	    }
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
	unsigned int random_gap = 0;
	float base_size = 200.f; // in ms
	unsigned int base_gap = 0;
	bool go = false;
	bool internalBuffer = true;

	// ambisonic
	unsigned int num_grains_per_channel = 0; // number of grains per channel
	unsigned int order = 0; // ambisonic order
	unsigned int num_channels = 0; // how many ambisonic channels (order+1)*(order+1)
};

#endif
