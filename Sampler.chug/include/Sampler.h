#ifndef SAMPLER_H
	#define SAMPLER_H

#include "../../include/stk/include/Stk.h"
#include "../../include/stk/include/FileRead.h"
#include "../../include/chugin.h"
#include "../../include/Phasor.h"
#include "../../include/Smoother.h"

class Sampler
{
public:
	// constructor
	Sampler::Sampler( t_CKFLOAT fs )
	{
		stk::Stk::setSampleRate( fs ); // set global rate
		playback = new Phasor( fs, 1.0f ); // new phasor
		pitch = new Smoother( fs, 1.0f ); // new pitch interpolator
		position = new Smoother( fs, 0.f ); // new position interpolator
		reader = new stk::FileRead(); // new file reader
		buffer = new stk::StkFrames(); // new buffer
	}

	// destructor
	Sampler::~Sampler( void )
	{
		CK_SAFE_DELETE( playback );
		CK_SAFE_DELETE( pitch );
		CK_SAFE_DELETE( position );
		CK_SAFE_DELETE( reader );
		CK_SAFE_DELETE( buffer );
	}

	// tick
	SAMPLE Sampler::tick()
	{
		// interpolate pitch
		playback->setFrequency( pitch->tick() * oneHert, FALSE );
		// get index
		t_CKFLOAT index = playback->tick() * indexLimit * loopLength;
		// range check ( if the index is smaller than the potential target + file size, then we may add it
		if( index <= indexLimit - position->getTarget() ) { index += position->tick(); }
		// return the sample
		return buffer->interpolate( index, 0 );
	}

	// open file given a C string
	t_CKFLOAT Sampler::openFile( const char* path )
	{
		// if one is open, close the file and delete the buffer
		if( reader->isOpen() ) { reader->close(); CK_SAFE_DELETE( buffer ); }

		// convert C string to C++ string
		std::string cppString = path;

		// open!
		reader->open( cppString );

		// resize!
		buffer = new stk::StkFrames( 0.f, reader->fileSize(), reader->channels() );
		// sample rate
		buffer->setDataRate( reader->fileRate() );
		// read!
		reader->read( *buffer, 0, true );

		// set our interal oneHert
		oneHert = ( reader->fileRate() / reader->fileSize() );
		// set out index limit
		indexLimit = ( -1.0f + ( buffer->size() / buffer->channels() ) );
		// let's go!
		pitch->setTarget( oneHert );

		return buffer->size() / buffer->channels();
	}

	// close file
	void Sampler::closeFile()
	{
		// delete
		if( reader->isOpen() ) { reader->close(); CK_SAFE_DELETE( buffer ); }
	}

	// get file size
	t_CKUINT Sampler::getFileSize() { return buffer->size() / buffer->channels(); }

	// position get set
	t_CKFLOAT Sampler::setPosition( t_CKFLOAT pos )
	{
		if( pos <= 1.0 && pos >= 0 ) position->setTarget( pos * indexLimit, 5.f );
		// return for courtesy
		return position->getTarget();
	}

	// overload to add slew time
	t_CKFLOAT Sampler::setPosition( t_CKFLOAT pos, t_CKFLOAT ms )
	{
		if( pos <= 1.0 && pos >= 0 && ms > 0.f ) position->setTarget( pos * indexLimit, ms );
		// return for courtesy
		return position->getTarget();
	}
	
	t_CKFLOAT Sampler::getPosition() { return position->getTarget(); }

	// pitch get set
	t_CKFLOAT Sampler::setPitch( t_CKFLOAT pit )
	{
		// new target
		pitch->setTarget( pit * oneHert, 5.f );
		// return for courtesy
		return pitch->getTarget();
	}

	// overload to add slew time
	t_CKFLOAT Sampler::setPitch( t_CKFLOAT pit, t_CKFLOAT ms )
	{
		// new target
		if( ms > 0.f ) pitch->setTarget( pit * oneHert, ms );
		else pitch->setTarget( pit );
		// return for courtesy
		return pitch->getTarget();
	}

	t_CKFLOAT Sampler::getPitch() { return pitch->getTarget(); }

	// set loop length
	t_CKFLOAT Sampler::setLoopLength( t_CKFLOAT leng ) 
	{ 
		if( leng > 0.0 && leng <= 1.0 ) { loopLength = leng; }
		else loopLength = 1.f;
		return loopLength;
	}

	// experimental time stretch
	t_CKUINT Sampler::stretch( t_CKUINT num )
	{
		return 1;
	}

	// members
	Phasor* playback = nullptr;
	Smoother* pitch = nullptr;
	Smoother* position = nullptr;
	stk::FileRead* reader = nullptr;
	stk::StkFrames* buffer = nullptr;
	t_CKFLOAT loopLength = 1.f; // this is multiplied by the playback phasor [0.0,1.0]
	t_CKFLOAT oneHert = 0.f; // this is the frequency needed to playback the current file at it's normal rate
	t_CKFLOAT indexLimit  = 0.f; // this is the maximum value the index can be when interpolating
};

#endif /* SAMPLER_H */