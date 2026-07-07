//==========================================================
//
// Swarm.h : Summer 2026 : everett m. carpenter
// Granulator is a class which utilizes Grains as a means
// of granulating a file or input. It uses a tick function 
// with an input, where the input value is ideally a sample 
// value from an audio file or input.
// 
//==========================================================

#ifndef ATMOSPHERE_H
	#define ATMOSPHERE_H

#include "Swarm.h"
#include "math.h"

class Atmosphere : public SoundMatter
{
public:
	Atmosphere::Atmosphere( unsigned int fs, unsigned int n_order, unsigned int size = 1 ) : SoundMatter( fs, size )
	{
		// ambi
		order = n_order;
		// ambisonic channel count
		num_channels = ( order + 1 ) * ( order + 1 );
		// num
		num_grains_per_channel = size; // grains per ambisonic channel
		// sample rate
		_fs = fs;
		stk::Stk::setSampleRate( _fs );
		// num
		num_grains = num_grains_per_channel * num_channels;
		// scale down
		scale = 1.0 / num_grains;
		// buffer
		buffer = new stk::StkFrames( 1, 1 );
		// read
		file_read = new stk::FileRead();
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

	Atmosphere::~Atmosphere()
	{
		// destroy matter
		for( int i = 0; i < ( num_grains_per_channel * num_channels ); i++ ) { delete quantum[i]; quantum[i] = nullptr; }
		delete[] quantum; quantum = nullptr;
		// destroy again
		delete file_read; file_read = nullptr;
		// destroy again
		delete random; random = nullptr;
		// destroy again
		delete position_slew; position_slew = nullptr;
		// destroy again
		delete pitch_slew; pitch_slew = nullptr;
		// once more
		delete buffer; buffer = nullptr;
	}

	void Atmosphere::tick( SAMPLE* in, SAMPLE* out, unsigned int frames ) 
	{
		// if we're good to go
		if( go )
		{
			memset( out, 0, sizeof( SAMPLE ) * num_channels * frames ); // clear
			// start up the machine
			for( int f = 0; f < frames; f++ )
			{
				for( int q = 0; q < ( num_grains_per_channel * num_channels ); q++ ) 
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
					if( quantum[q]->windowState() ) newGrain( *quantum[q] );
					// if our grain is loop and finished, shoot off a new one
					if( quantum[q]->windowState() && quantum[q]->loopState() ) quantum[q]->trigger();
				}
				
				// scale the outgoing buffer ( is this more effecient than scaling every time we add the quantum tick to the output? i don't know )
				for( int c = 0; c < num_channels; c++ ) // c++ !
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

	void Atmosphere::openFile( const char* path )
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
		for( int i = 0; i < ( num_grains_per_channel * num_channels ); i++ ) { quantum[i]->setBuffer( *buffer, i % buffer->channels() ); }
		// good to go
		go = true;
	}

	void Atmosphere::closeFile()
	{
		// stop doing anything
		go = false;
		// close the file
		file_read->close();
		// unlink the quarks
		for( int i = 0; i < ( num_grains_per_channel * num_channels ); i++ ) quantum[i]->clearBuffer();
		// clear buffer
		delete buffer; buffer = nullptr;
	}
	
private:
	using SoundMatter::tick; // privatize the SoundMatter tick function so I don't accidentally call it and break a buffer or something
	unsigned int num_grains_per_channel = 0; // number of grains per channel
	unsigned int order = 0; // ambisonic order
	unsigned int num_channels = 0; // how many ambisonic channels (order+1)*(order+1)
};

#endif