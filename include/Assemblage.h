#ifndef ASSEMBLAGE_H
	#define ASSEMBLAGE_H

#include "Swarm.h"
#include "chugin.h"

//=======================================================================
//
//	names: Assemblage
//	desc: really cool multi granulator by emc! the name assemblage comes 
//		  from the concept of microsound and the idea of assembling 
//		  something new from pieces. each piece is individually addressable
//		  meant for the creation of multiple voices within a swarm.
//	info: emc is addressable at carpee2 [at] rpi [dot] edu
// 
//=======================================================================

class Assemblage
{
public:

	//=======================================================================
	//
	//	names: constructor
	//	desc: allocates and configures everything
	//	args: sample rate and optional int for how many pieces
	// 
	//=======================================================================

	// default constructor
	Assemblage::Assemblage( unsigned int fs, unsigned int n_pieces = 4 )
	{
		// default number of pieces
		num_pieces = n_pieces;
		// initialize
		collage = new SoundMatter*[num_pieces];
		// default to 4 grains per sound object, can do more
		for( int i = 0; i < num_pieces; i ++ ) 
		{
			collage[i] = new SoundMatter( fs, 4 ); 
		}
		// precalc
		scalar = 1.f / num_pieces;
		this->createBuffer();
	}

	//=======================================================================
	//
	//	names: destructor
	//	desc: delete everything and nullptr
	//	args: none
	// 
	//=======================================================================

	Assemblage::~Assemblage()
	{
		// destroy matter
		for( int i = 0; i < num_pieces; i++ ) { delete collage[i]; collage[i] = nullptr; }
		delete[] collage; collage = nullptr;
		this->deleteBuffer();
	}

	//=======================================================================
	//
	//	names: tick
	//	desc: tick functions for buffered playback or sample callback 
	//		  (not sure if those are the correct terms)
	//	args: buffered requires input and output buffers + how many frames 
	// 
	//=======================================================================

	double Assemblage::tick()
	{
		double out = 0.0; // return this
		for( int i = 0; i < num_pieces; i++ )
		{
			out += collage[i]->tick(); // mix
		}
		out *= scalar; // quiet
		return out;
	}

	// this assumes we are outputting however many pieces we have in the assemblage, be careful! 
	void Assemblage::tick( SAMPLE* in, SAMPLE* out, unsigned int frames )
	{
		memset( out, 0, sizeof(SAMPLE) * num_pieces * frames); // clear

		for( int f = 0; f < frames; f++ )
		{
			for( int c = 0; c < num_pieces; c++ )
			{
				out[f * num_pieces + c] = collage[c]->tick();
			}
		}
	}

	//=======================================================================
	//
	//	names: set* 
	//	desc: set pitch, size and position
	//	args: set with one value or overload and provide a chuck array
	// 
	//=======================================================================

	// set all pitches given a collection of sizes
	void Assemblage::setPitch( Chuck_ArrayFloat* pitches, const CK_DL_API& API )
	{
		unsigned int size = API->object->array_float_size( pitches );
		// assign
		for( int i = 0; i < size; i++ ) collage[i]->setPitch( API->object->array_float_get_idx( pitches, i % size ) );
		// how do we save this? maybe we just retrieve the targets of the sound matter(s) and fit them into a chuck array?
	}

	// set all sizes given a collection of sizes
	void Assemblage::setSize( Chuck_ArrayFloat* sizes, const CK_DL_API& API )
	{
		unsigned int size = API->object->array_float_size( sizes);
		// assign
		for( int i = 0; i < size; i++ ) collage[i]->setSize( API->object->array_float_get_idx( sizes, i % size ) );
		// how do we save this?
	}

	// set all positions given a collection of position
	void Assemblage::setPosition( Chuck_ArrayFloat* positions, const CK_DL_API& API )
	{
		unsigned int size = API->object->array_float_size( positions );
		// assign
		for( int i = 0; i < size; i++ ) collage[i]->setPosition( (float)API->object->array_float_get_idx( positions, i % size ) );
		// how do we save this?
	}

	// set all pitches to a single value
	void Assemblage::setPitch( float pitch )
	{
		// assign
		for( int i = 0; i < num_pieces; i++ ) collage[i]->setPitch( pitch );
		// how do we save this? maybe we just retrieve the targets of the sound matter(s) and fit them into a chuck array?
	}

	// set all sizes to a single value
	void Assemblage::setSize( float size )
	{
		// assign
		for( int i = 0; i < num_pieces; i++ ) collage[i]->setSize( size );
		// how do we save this?
	}

	// set all positions to a single value
	void Assemblage::setPosition( float position )
	{
		// assign
		for( int i = 0; i < num_pieces; i++ ) collage[i]->setPosition( position );
		// how do we save this?
	}

	//=======================================================================
	//
	//	names: setRandom* 
	//	desc: set randomness of pitch, size and position
	//	args: set with one value or overload and provide a chuck array
	// 
	//=======================================================================

	// set all pitches given a collection of sizes
	void Assemblage::setRandomPitch( Chuck_ArrayFloat* pitches, const CK_DL_API& API )
	{
		unsigned int size = API->object->array_float_size( pitches );
		// assign
		for( int i = 0; i < size; i++ ) collage[i]->setRandomPitch( API->object->array_float_get_idx( pitches, i % size ) );
		// how do we save this? maybe we just retrieve the targets of the sound matter(s) and fit them into a chuck array?
	}

	// set all sizes given a collection of sizes
	void Assemblage::setRandomSize( Chuck_ArrayFloat* sizes, const CK_DL_API& API )
	{
		unsigned int size = API->object->array_float_size( sizes);
		// assign
		for( int i = 0; i < size; i++ ) collage[i]->setRandomSize( API->object->array_float_get_idx( sizes, i % size ) );
		// how do we save this?
	}

	// set all positions given a collection of position
	void Assemblage::setRandomPosition( Chuck_ArrayFloat* positions, const CK_DL_API& API )
	{
		unsigned int size = API->object->array_float_size( positions );
		// assign
		for( int i = 0; i < size; i++ ) collage[i]->setRandomPosition( (float)API->object->array_float_get_idx( positions, i % size ) );
		// how do we save this?
	}

	// set randomness
	void Assemblage::setRandomPitch( float random )
	{
		// assign
		for( int i = 0; i < num_pieces; i++ ) collage[i]->setRandomPitch( random );
	}

	// set randomness
	void Assemblage::setRandomSize( float random )
	{
		// assign
		for( int i = 0; i < num_pieces; i++ ) collage[i]->setRandomSize( random );
	}

	// set randomness
	void Assemblage::setRandomPosition( float random )
	{
		// assign
		for( int i = 0; i < num_pieces; i++ ) collage[i]->setRandomPosition( random );
	}

	//=======================================================================
	//
	//	names: start & stop 
	//	desc: silence all lower level processors
	//	args: none
	// 
	//=======================================================================

	// start all function calls
	void Assemblage::start()
	{
		// cycle
		for( int i = 0; i < num_pieces; i++ ) collage[i]->start();
	}

	// stop all function calls
	void Assemblage::stop()
	{
		// cycle
		for( int i = 0; i < num_pieces; i++ ) collage[i]->stop();
	}

	//=======================================================================
	//
	//	name(s): openFile 
	//	desc: open up a file, silences all processors, opens file and places
	//		  file in a memory buffer, and links buffer to all processors
	//	args: c string of file path
	// 
	//=======================================================================

	// open up a file and have all our granulators point to it
	void Assemblage::openFile( const char* path )
	{
		// don't do anything
		this->stop(); 

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
		for( int i = 0; i < num_pieces; i++ ) { collage[i]->linkOutsideBuffer( buffer ); }
		
		// good to go
		this->start();
	}

	//=======================================================================
	//
	//	name(s): closeFile
	//	desc: if we have our own buffer, close the file and delete the buffer
	//	args: size pitch or position
	// 
	//=======================================================================

	void Assemblage::closeFile()
	{
		// stop doing anything
		this->stop();
		// close the file
		file_read->close();
		// delete the buffer links below
		for( int i = 0; i < num_pieces; i++ ) collage[i]->closeFile();
		// clear buffer
		delete buffer; buffer = nullptr;
	}

	//=======================================================================
	//
	//	name(s): count
	//	desc: returns the number of pieces we're using
	//	args: none
	// 
	//=======================================================================

	// buffer size
	unsigned int Assemblage::count()
	{
		return num_pieces;
	}

	//=======================================================================
	//
	//	name(s): samples
	//	desc: returns the size of the current buffer in samples, if the buffer
	//		  is multichannel, it will return the length of one channel 
	//	args: none
	// 
	//=======================================================================

	// buffer size
	unsigned int Assemblage::samples()
	{
		return buffer->size() / buffer->channels();
	}

	//=======================================================================
	//
	//	name(s): create & delete buffer 
	//	desc: internal functions for managing instance's buffer
	//	args: none
	// 
	//=======================================================================

private:
	// create internal audio buffer
	void Assemblage::createBuffer()
	{
		// buffer
		buffer = new stk::StkFrames(1, 1);
		// read
		file_read = new stk::FileRead();
	}

	// delete internal audio buffer
	void Assemblage::deleteBuffer()
	{
		// destroy again
		delete file_read; file_read = nullptr;
		// once more
		delete buffer; buffer = nullptr;
	}

private:
	stk::FileRead* file_read = nullptr; // this opens up a file
	stk::StkFrames* buffer = nullptr; // everyone reads from here
	SoundMatter** collage = nullptr; // our pieces of sound
	unsigned int num_pieces = 0; // how many pieces of sound?
	float scalar = 0.f; // turn down the volume
};

#endif /* ASSEMBLAGE_H */