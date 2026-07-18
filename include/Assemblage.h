#ifndef ASSEMBLAGE_H
	#define ASSEMBLAGE_H

#include "Swarm.h"
#include "chugin.h"

class Assemblage
{
public:
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

	Assemblage::~Assemblage()
	{
		// destroy matter
		for( int i = 0; i < num_pieces; i++ ) { delete collage[i]; collage[i] = nullptr; }
		delete[] collage; collage = nullptr;
		this->deleteBuffer();
	}

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

	// set all pitches given a collection of sizes
	void Assemblage::setPitch( Chuck_ArrayFloat& pitches, CK_DL_API& API )
	{
		unsigned int size = API->object->array_float_size( &pitches );
		// assign
		for( int i = 0; i < size; i++ ) collage[i]->setPitch( API->object->array_float_get_idx( &pitches, i % size ) );
		// how do we save this? maybe we just retrieve the targets of the sound matter(s) and fit them into a chuck array?
	}

	// set all sizes given a collection of sizes
	void Assemblage::setSize( Chuck_ArrayFloat& sizes, CK_DL_API& API )
	{
		unsigned int size = API->object->array_float_size( &sizes);
		// assign
		for( int i = 0; i < size; i++ ) collage[i]->setSize( API->object->array_float_get_idx( &sizes, i % size ) );
		// how do we save this?
	}

	// set all positions given a collection of position
	void Assemblage::setPosition( Chuck_ArrayFloat& positions, CK_DL_API& API )
	{
		unsigned int size = API->object->array_float_size( &positions );
		// assign
		for( int i = 0; i < size; i++ ) collage[i]->setPosition( (float)API->object->array_float_get_idx( &positions, i % size ) );
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
		for( int i = 0; i < num_pieces; i++ ) { collage[i]->linkOutsideBuffer( *buffer ); }
		
		// good to go
		this->start();
	}

	// create internal audio buffer
	void Assemblage::createBuffer()
	{
		// buffer
		buffer = new stk::StkFrames( 1, 1 );
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

	// buffer size
	unsigned int Assemblage::samples()
	{
		return buffer->size() / buffer->channels();
	}

private:
	stk::FileRead* file_read = nullptr; // this opens up a file
	stk::StkFrames* buffer = nullptr; // everyone reads from here
	SoundMatter** collage = nullptr; // our pieces of sound
	unsigned int num_pieces = 0; // how many pieces of sound?
	float scalar = 0.f; // turn down the volume
};

#endif /* ASSEMBLAGE_H */