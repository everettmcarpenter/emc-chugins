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
	}

	Assemblage::~Assemblage()
	{
		// destroy matter
		for( int i = 0; i < num_pieces; i++ ) { delete collage[i]; collage[i] = nullptr; }
		delete[] collage; collage = nullptr;
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
		for (int i = 0; i < num_pieces; i++) collage[i]->setPosition( position );
		// how do we save this?
	}

private:
	stk::StkFrames* _buffer = nullptr; // shared audio buffer
	SoundMatter** collage = nullptr; // our pieces of sound
	unsigned int num_pieces = 0; // how many pieces of sound?
	float scalar = 0.f; // turn down the volume
};

#endif /* ASSEMBLAGE_H */