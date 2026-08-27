#ifndef SOUNDFILE_H
	#define SOUNDFILE_H

#include "stk/include/FileRead.h"
#include "stk/include/Stk.h"

//-----------------------------------------------------------------------------
// A helper class which wraps the StkFrames class
//-----------------------------------------------------------------------------
class SoundFile
{
public:
    // constructor
    SoundFile( t_CKFLOAT fs )
    {
        _fs = fs;
        _buffer = new stk::StkFrames;
        _fileReader = new stk::FileRead;
    }

    // all your audio buffers are belong to us
    stk::StkFrames* buffer()
    {
        return _buffer;
    }

    unsigned int fileRate()
    {
        return (unsigned int)_buffer->dataRate();
    }
    
    unsigned int fileChannels()
    {
        return (unsigned int)_buffer->channels();
    }

    // usually i space text in the parenthesis, but this felt more comfortable
    unsigned int fileSize()
    {
        return (unsigned int)(_buffer->size() / _buffer->channels());
    }

    void openFile( const char* filepath )
    {
        // if one is open, close the file and delete the buffer
		if( _fileReader->isOpen() ) _fileReader->close();
		
		// clear 
		delete _buffer;

		// convert C string to C++ string
		_filepath = filepath;

		// open!
		_fileReader->open( _filepath );

		// resize!
		_buffer = new stk::StkFrames( 0.f, _fileReader->fileSize(), _fileReader->channels() );
		// sample rate
		_buffer->setDataRate( _fileReader->fileRate() );
		// read!
		_fileReader->read( *_buffer, 0, true );
    }

    void closeFile()
	{
		// close the file
		_fileReader->close();
	}

private:
    // instance data
    t_CKUINT _fs = 0;
    std::string _filepath;
    stk::StkFrames* _buffer = nullptr;
    stk::FileRead* _fileReader = nullptr;
};

#endif
