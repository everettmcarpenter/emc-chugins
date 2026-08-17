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
	Assemblage( unsigned int fs, unsigned int n_pieces = 4 )
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

	~Assemblage()
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

	double tick()
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
	void tick( SAMPLE* in, SAMPLE* out, unsigned int frames )
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
	void setPitch( Chuck_ArrayFloat* pitches, const CK_DL_API& API )
	{
		unsigned int size = API->object->array_float_size( pitches );
		// assign
		for( int i = 0; i < size; i++ ) collage[i]->setPitch( API->object->array_float_get_idx( pitches, i % size ) );
		// how do we save this? maybe we just retrieve the targets of the sound matter(s) and fit them into a chuck array?
	}

	// set all sizes given a collection of sizes
	void setSize( Chuck_ArrayFloat* sizes, const CK_DL_API& API )
	{
		unsigned int size = API->object->array_float_size( sizes);
		// assign
		for( int i = 0; i < size; i++ ) collage[i]->setSize( API->object->array_float_get_idx( sizes, i % size ) );
		// how do we save this?
	}

	// set all positions given a collection of position
	void setPosition( Chuck_ArrayFloat* positions, const CK_DL_API& API )
	{
		unsigned int size = API->object->array_float_size( positions );
		// assign
		for( int i = 0; i < size; i++ ) collage[i]->setPosition( (float)API->object->array_float_get_idx( positions, i % size ) );
		// how do we save this?
	}

	// set all pitches to a single value
	void setPitch( float pitch )
	{
		// assign
		for( int i = 0; i < num_pieces; i++ ) collage[i]->setPitch( pitch );
		// how do we save this? maybe we just retrieve the targets of the sound matter(s) and fit them into a chuck array?
	}

	// set all pitches to a single value
	void setPitch( float pitch, float ms_to )
	{
		// assign
		for( int i = 0; i < num_pieces; i++ ) collage[i]->setPitch( pitch, ms_to );
		// how do we save this? maybe we just retrieve the targets of the sound matter(s) and fit them into a chuck array?
	}

	// set all sizes to a single value
	void setSize( float size )
	{
		// assign
		for( int i = 0; i < num_pieces; i++ ) collage[i]->setSize( size );
		// how do we save this?
	}

	// set all positions to a single value
	void setPosition( float position )
	{
		// assign
		for( int i = 0; i < num_pieces; i++ ) collage[i]->setPosition( position );
		// how do we save this?
	}

	// set all positions to a single value
	void setPosition( float position, float ms_to )
	{
		// assign
		for( int i = 0; i < num_pieces; i++ ) collage[i]->setPosition( position, ms_to );
		// how do we save this?
	}

	void setGap( unsigned int gap_samp )
	{
		// assign
		for( int i = 0; i < num_pieces; i++ ) collage[i]->setGap( gap_samp );
	}

	unsigned int getGap()
	{
		return collage[0]->getGap();
	}

	//=======================================================================
	//
	//	names: setRandom* 
	//	desc: set randomness of pitch, size and position
	//	args: set with one value or overload and provide a chuck array
	// 
	//=======================================================================

	// set all pitches given a collection of sizes
	void setRandomPitch( Chuck_ArrayFloat* pitches, const CK_DL_API& API )
	{
		unsigned int size = API->object->array_float_size( pitches );
		// assign
		for( int i = 0; i < size; i++ ) collage[i]->setRandomPitch( API->object->array_float_get_idx( pitches, i % size ) );
		// how do we save this? maybe we just retrieve the targets of the sound matter(s) and fit them into a chuck array?
	}

	// set all sizes given a collection of sizes
	void setRandomSize( Chuck_ArrayFloat* sizes, const CK_DL_API& API )
	{
		unsigned int size = API->object->array_float_size( sizes);
		// assign
		for( int i = 0; i < size; i++ ) collage[i]->setRandomSize( API->object->array_float_get_idx( sizes, i % size ) );
		// how do we save this?
	}

	// set all positions given a collection of position
	void setRandomPosition( Chuck_ArrayFloat* positions, const CK_DL_API& API )
	{
		unsigned int size = API->object->array_float_size( positions );
		// assign
		for( int i = 0; i < size; i++ ) collage[i]->setRandomPosition( (float)API->object->array_float_get_idx( positions, i % size ) );
		// how do we save this?
	}

	// set randomness
	void setRandomPitch( float random )
	{
		// assign
		for( int i = 0; i < num_pieces; i++ ) collage[i]->setRandomPitch( random );
	}

	// set randomness
	void setRandomSize( float random )
	{
		// assign
		for( int i = 0; i < num_pieces; i++ ) collage[i]->setRandomSize( random );
	}

	// set randomness
	void setRandomPosition( float random )
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
	void start()
	{
		// cycle
		for( int i = 0; i < num_pieces; i++ ) collage[i]->start();
	}

	// stop all function calls
	void stop()
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
	void openFile( const char* path )
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

	void closeFile()
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
	unsigned int count()
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
	unsigned int samples()
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
	void createBuffer()
	{
		// buffer
		buffer = new stk::StkFrames(1, 1);
		// read
		file_read = new stk::FileRead();
	}

	// delete internal audio buffer
	void deleteBuffer()
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
