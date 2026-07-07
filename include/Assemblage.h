#ifndef ASSEMBLAGE_H
	#define ASSEMBLAGE_H

#include "Swarm.h"

class Assemblage
{
public:
	// default constructor
	Assemblage::Assemblage( unsigned int fs )
	{
		// default number of pieces
		num_pieces = 4;
		// initialize
		collage = new SoundMatter*[num_pieces];
		// default to 4 grains per sound object, can do more
		for( int i = 0; i < num_pieces; i ++ ) 
		{
			collage[i] = new SoundMatter( fs, 4 ); 
		}
		// init members
		this->init();
	}

	void Assemblage::setPitch( float* n_pitches, unsigned int size )
	{
		// new size
		num_pitches = size;
		// get rid of the last pitches
		delete pitches; pitches = nullptr;
		// new storage
		pitches = new float[num_pitches];
		// swap
		for( int i = 0; i < num_pitches; i++ ) pitches[i] = n_pitches[i];
		// assign
		for( int i = 0; i < num_pieces; i++ ) collage[i]->setPitch( pitches[i % num_pitches] );
	}

	void Assemblage::setSize( float* n_sizes, unsigned int size )
	{
		// new size
		num_sizes = size;
		// get rid
		delete sizes; sizes = nullptr;
		// new
		sizes = new float[num_sizes];
		// swap
		for( int i = 0; i < num_sizes; i++ ) sizes[i] = n_sizes[i];
		// assign
		for( int i = 0; i < num_pieces; i++ ) collage[i]->setSize( sizes[i % num_sizes] );
	}

	void Assemblage::setPosition( float* n_positions, unsigned int size )
	{
		// new size
		num_positions = size;
		// get rid
		delete positions; positions = nullptr;
		// new
		positions = new float[num_positions];
		// swap
		for( int i = 0; i < num_positions; i++ ) positions[i] = n_positions[i];
		// assign
		for( int i = 0; i < num_pieces; i++ ) collage[i]->setPosition( positions[i % num_positions] );
	}

	void Assemblage::init()
	{
		// pitches
		num_pitches = 1;
		pitches = new float;
		pitches = 1.f;
		// sizes
		num_sizes = 1;
		sizes = new float;
		sizes = 100.f;
		// positions
		num_positions = 1;
		positions = new float;
		positions = 0.f;
	}
private:
	float* pitches = nullptr; // stored pitches
	unsigned int num_pitches = 0; // how many pitches do we have stored
	float* sizes = nullptr; // stored sizes
	unsigned int num_sizes = 0; // how many grain sizes do we have stored
	float* positions = nullptr; // store positions
	unsigned num_positions = 0; // how many positions do we have stored
	stk::StkFrames* _buffer = nullptr; // shared audio buffer
	SoundMatter** collage = nullptr; // our pieces of sound
	unsigned int num_pieces = 0; // how many pieces of sound?
};

#endif /* ASSEMBLAGE_H */