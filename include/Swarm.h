//==========================================================
//
// Swarm.h : Summer 2026 : everett m. carpenter
// Granulator is a class which utilizes Grains as a means
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
	SoundMatter::SoundMatter( unsigned int fs, unsigned int size = 4 )
	{
		// sample rate
		_fs = fs;
		stk::Stk::setSampleRate( _fs );
		// num
		num_grains = size;
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
		for( int i = 0; i < num_grains; i++ ) quantum[i] = new Quark( fs, *buffer );
		// init
		this->setSize( this->base_size );
		this->setPitchInstant( 1.f );
		this->setPosition( 0.f );
	}

	SoundMatter::~SoundMatter()
	{
		// destroy matter
		for( int i = 0; i < num_grains; i++ ) { delete quantum[i]; quantum[i] = nullptr; }
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

	double SoundMatter::tick()
	{
		// output
		double out = 0.0;
		// if we're good to go
		if( go )
		{
			for( int i = 0; i < num_grains; i++ ) 
			{
				out += quantum[i]->tick();
				if( quantum[i]->windowState() ) newGrain( *quantum[i] );
			}
			// don't use tick functions in loops! that defeats the point of a time normalized tick function everett!
			// anyways, advance
			pitch_slew->tick();
			position_slew->tick();
			// scale because if we don't prevent blowing our ears out, ChucK definitely won't!
			out *= scale;
			// soft clip
			// out = tanh( out );
		}
		// return
		return out;
	}

	// create a new 
	void SoundMatter::newGrain( Quark& particle )
	{
		// wrap around to prevent negative sizes
		float n_size = base_size + (  0.5 * ( random->tick() + 1.0 ) * random_size );
		n_size = std::max( 1.f, n_size );
		particle.setSize( n_size ); // set 
		// pitch is easy(ish)
		float n_pitch = pitch_slew->getCurrent() + ( 0.5 * ( random->tick() + 1.0 ) * random_pitch ); // offset
		n_pitch = std::max( 0.f, n_pitch ); // clamp
		particle.setPitchInstant( n_pitch );
		// we gotta wrap around again
		float random_offset_frames = ( random_position * 0.001f ) * _fs; // convert random_position to samples
		random_offset_frames /= (float)this->size();
		float n_position = position_slew->getCurrent() + (  0.5 * ( random->tick() + 1.0 ) * random_offset_frames ); // apply random offset
		n_position = std::max( 0.f, std::min( n_position, 40.f ) ); // clamp
		particle.setPositionInstant( n_position );

		// debug
		// printf( "Size %f, pitch %f, position %f \n", n_size, n_pitch, n_position );
	}

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
		position_slew->setTarget( n_position, 120.f );
	}

	void SoundMatter::setPosition( unsigned int n_position )
	{
		position_slew->setTarget( (float)n_position / (float)this->size(), 40.f); // convert
	}

	float SoundMatter::getPosition() { return position_slew->getTarget(); }

	void SoundMatter::setRandomSize( float random_si ) { random_size = random_si; }
	float SoundMatter::getRandomSize() { return random_size; }

	void SoundMatter::setRandomPitch( float random_pit ) { random_pitch = random_pit; }
	float SoundMatter::getRandomPitch() { return random_pitch; }

	void SoundMatter::setRandomPosition( float random_pos_ms ) { random_position = ( random_pos_ms / _fs ) * 1000.f; }
	float SoundMatter::getRandomPosition() { return random_position; }

	virtual void SoundMatter::openFile( const char* path )
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

	virtual void SoundMatter::closeFile()
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

	unsigned int SoundMatter::size() { return buffer->size() / buffer->channels(); }
	
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
};

#endif