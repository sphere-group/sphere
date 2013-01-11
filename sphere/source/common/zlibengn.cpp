//
// This file contains the implementation the
// the ZlibEngine class, used to simplify
// compression and decompression of files
// using the Zlib engine.
//
// The ZlibEngine is a Tiny Software (tm) project.
// You may use the code in this project without restriction.
// Contact markn@tiny.com for more information.
//

#if defined( _WINDOWS )
#include <windows.h>
#endif

#include "zlibengn.h"

//
// The constructor initializes a couple of members
// of the z_stream class.  See the Zlib documentation
// for details on what those members do
//

ZlibEngine::ZlibEngine()
{
    zalloc = 0;  //z_stream member
    zfree = 0;   //z_stream member
    opaque = 0;  //z_stream member
//
// I initialize these members just for tidiness.
//
    fin = 0;
    fout = 0;
}

//
// compress() is the public function used to compress
// a single file.  It has to take care of opening the
// input and output files and setting up the buffers for
// Zlib.  It then calls deflate() repeatedly until all
// input and output processing has been done, and finally
// closes the files and cleans up the Zlib structures.
//
int ZlibEngine::compress( const char *input,
                          const char *output,
                          int level )
{
	return filetofile(input, output, level, true);
}

//
// decompress has to do most of the same chores as compress().
// The only major difference it has is the absence of the level
// parameter.  The level isn't needed when decompressing data
// using the deflate algorithm.
//

int ZlibEngine::decompress( const char *input,
                            const char *output )
{
	return filetofile(input, output, 0, false);
}

int ZlibEngine::filetofile( const char *input,
                      const char *output,
                      int level,
					  bool deflation)
{
    err = Z_OK;
    avail_in = 0;
    avail_out = output_length;
    next_out = output_buffer;
    m_AbortFlag = 0;

    fin  = fopen( input, "rb" );
    fout = fopen( output, "wb" );
	
	statResult = stat(input,&fileStatus); length = fileStatus.st_size;
	if (deflation)
		deflateInit( this, level);
	else
		inflateInit( this );
    for ( ; ; ) {
        if ( m_AbortFlag )
            break;
        if ( !load_input() )
            break;
		if (deflation)
			err = deflate( this, Z_NO_FLUSH );
		else
			err = inflate( this, Z_NO_FLUSH );
        flush_output();
        if ( err != Z_OK )
            break;
        progress( percent() );
    }
    for ( ; ; ) {
        if ( m_AbortFlag )
            break;
		if (deflation)
			err = deflate( this, Z_FINISH );
		else
			err = inflate( this, Z_FINISH );
        if ( !flush_output() )
            break;
        if ( err != Z_OK )
            break;
    }
    progress( percent() );
	if (deflation)
		deflateEnd( this );
	else
		inflateEnd( this );
    if ( m_AbortFlag )
        status( (char *)"User Abort" );
    else if ( err != Z_OK && err != Z_STREAM_END )
        status( (char *)"Zlib Error" );
    else {
        status( (char *)"Success" );
        err = Z_OK;
    }
    if ( fin )
        fclose( fin );
    fin = 0;
    if ( fout )
        fclose( fout );
    fout = 0;
    if ( m_AbortFlag )
        return Z_USER_ABORT;
    else
        return err;
}


//
//  This function is called so as to provide the progress()
//  virtual function with a reasonable figure to indicate
//  how much processing has been done.  Note that the length
//  member is initialized when the input file is opened.
//
int ZlibEngine::percent()
{
    if ( length == 0 )
        return 100;
    else if ( length > 10000000L )
        return ( total_in / ( length / 100 ) );
    else
        return ( total_in * 100 / length );
}

//
//  Every time Zlib consumes all of the data in the
//  input buffer, this function gets called to reload.
//  The avail_in member is part of z_stream, and is
//  used to keep track of how much input is available.
//  I churn the Windows message loop to ensure that
//  the process can be aborted by a button press or
//  other Windows event.
//
int ZlibEngine::load_input()
{
#if defined( _WINDOWS )
    MSG msg;
    while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
#endif
    if ( avail_in == 0 ) {
        next_in = input_buffer;
        avail_in = fread( input_buffer, 1, input_length, fin );
    }
    return avail_in;
}

//
//  Every time Zlib filsl the output buffer with data,
//  this function gets called.  Its job is to write
//  that data out to the output file, then update
//  the z_stream member avail_out to indicate that more
//  space is now available.  I churn the Windows message
//  loop to ensure that the process can be aborted by a
//  button press or other Windows event.
//

int ZlibEngine::flush_output()
{
#if defined( _WINDOWS )
    MSG msg;
    while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
#endif
    unsigned int count = output_length - avail_out;
    if ( count ) {
        if ( fwrite( output_buffer, 1, count, fout ) != count ) {
            err = Z_ERRNO;
            return 0;
        }
        next_out = output_buffer;
        avail_out = output_length;
    }
    return count;
}


// Give us an estimate amount of byte for the destination buffer 	
uLong ZlibEngine::compressBound2(unsigned int sourceSize)
{
	return compressBound(sourceSize);
}

//
// 
//
int ZlibEngine::compressInMemory( const unsigned char *source, 
						unsigned int sourceSize, 
						unsigned char *destination, 
						unsigned int maxbuffersize,
						int level )
{
	// We need to allocate a large enough amount of byte for the destination buffer
	uLong destination_size = maxbuffersize;
	// And if it looks like we require more than the max given, we fail
	if(maxbuffersize<compressBound(sourceSize))
		return -256;
	// Compress everything from source to destination (const unsigned char *)
	int result = compress2(destination, &destination_size, source, sourceSize, level);

	// Check result for errors
	if( result != Z_OK )
		return -result;

	return destination_size;
}

int ZlibEngine::decompressInMemory( const unsigned char *source, unsigned int sourceSize, unsigned char *destination, unsigned int maxbuffersize )
{
	// We need to allocate a large enough amount of byte for the destination buffer
	uLong destination_size = maxbuffersize;

	// And if it looks like we require more than the max given, we fail
	if(maxbuffersize<compressBound(sourceSize))
		return -256;

	// Compress everything from source to destination (const unsigned char *)
	int result = uncompress(destination, &destination_size, source, sourceSize);

	// Check result for errors
	if( result != Z_OK )
		return -result;

	return destination_size;
}

