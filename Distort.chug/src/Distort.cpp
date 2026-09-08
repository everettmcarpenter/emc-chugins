//-----------------------------------------------------------------------------
// happy chucking & chugging!
//-----------------------------------------------------------------------------

// include chugin header
#include "../../include/chugin.h"
#include "../../include/Distort.h"

// general includes
#include <iostream>

// declaration of chugin constructor
CK_DLL_CTOR( distort_ctor );
// declaration of chugin desctructor
CK_DLL_DTOR( distort_dtor );

// example of getter/setter
CK_DLL_MFUN( distort_setMode );
CK_DLL_MFUN( distort_getMode );

// for chugins extending UGen, this is mono synthesis function for 1 sample
CK_DLL_TICK( distort_tick );

// this is a special offset reserved for chugin internal data
t_CKINT distort_data_offset = 0;

//-----------------------------------------------------------------------------
// info function: ChucK calls this when loading/probing the chugin
// NOTE: please customize these info fields below; they will be used for
// chugins loading, probing, and package management and documentation
//-----------------------------------------------------------------------------
CK_DLL_INFO( Distort )
{
    // the version string of this chugin, e.g., "v1.2.1"
    QUERY->setinfo( QUERY, CHUGIN_INFO_CHUGIN_VERSION, "" );
    // the author(s) of this chugin, e.g., "Alice Baker & Carl Donut"
    QUERY->setinfo( QUERY, CHUGIN_INFO_AUTHORS, "emc" );
    // text description of this chugin; what is it? what does it do? who is it for?
    QUERY->setinfo( QUERY, CHUGIN_INFO_DESCRIPTION, "" );
    // (optional) URL of the homepage for this chugin
    QUERY->setinfo( QUERY, CHUGIN_INFO_URL, "" );
    // (optional) contact email
    QUERY->setinfo( QUERY, CHUGIN_INFO_EMAIL, "" );
}


//-----------------------------------------------------------------------------
// query function: ChucK calls this when loading the chugin
// modify this function to define this chugin's API and language extensions
//-----------------------------------------------------------------------------
CK_DLL_QUERY( Distort )
{
    // generally, don't change this...
    QUERY->setname( QUERY, "Distort" );

    QUERY->begin_class( QUERY, "Distort", "UGen" );

    QUERY->add_ctor( QUERY, distort_ctor );
    QUERY->add_dtor( QUERY, distort_dtor );
    QUERY->add_ugen_func( QUERY, distort_tick, NULL, 1, 1 );

    QUERY->add_mfun( QUERY, distort_setMode, "int", "mode" );
    QUERY->add_arg( QUERY, "int", "mode" );

    QUERY->add_mfun( QUERY, distort_getMode, "int", "mode" );
    
    distort_data_offset = QUERY->add_mvar( QUERY, "int", "@d_data", false );

    QUERY->end_class( QUERY );

    // wasn't that a breeze?
    return TRUE;
}

CK_DLL_CTOR( distort_ctor )
{
    // get the offset where we'll store our internal c++ class pointer
    OBJ_MEMBER_INT( SELF, distort_data_offset ) = 0;
    
    // instantiate our internal c++ class representation
    Distort * d_obj = new Distort( API->vm->srate(VM) );
    
    // store the pointer in the ChucK object member
    OBJ_MEMBER_INT( SELF, distort_data_offset ) = (t_CKINT)d_obj;
}

CK_DLL_DTOR( distort_dtor )
{
    // get our c++ class pointer
    Distort * d_obj = (Distort *)OBJ_MEMBER_INT( SELF, distort_data_offset );
    // clean up (this macro tests for NULL, deletes, and zeros out the variable)
    CK_SAFE_DELETE( d_obj );
    // set the data field to 0
    OBJ_MEMBER_INT( SELF, distort_data_offset ) = 0;
}

CK_DLL_TICK( distort_tick )
{
    // get our c++ class pointer
    Distort * d_obj = (Distort *)OBJ_MEMBER_INT(SELF, distort_data_offset);
 
    // invoke our tick function; store in the magical out variable
    if( d_obj ) *out = d_obj->tick( in );

    // yes
    return TRUE;
}

CK_DLL_MFUN( distort_setMode )
{
    // get our c++ class pointer
    Distort * d_obj = (Distort *)OBJ_MEMBER_INT( SELF, distort_data_offset );

    t_CKINT mode = GET_NEXT_INT( ARGS );
    
	if( !d_obj->setMode( mode ) ) printf( "Distort : Incorrect or non-existant mode.");
}


// example implementation for getter
CK_DLL_MFUN( distort_getMode )
{
    // get our c++ class pointer
    Distort * d_obj = (Distort *)OBJ_MEMBER_INT( SELF, distort_data_offset );

    // call getParam() and set the return value
    RETURN->v_int = d_obj->getMode();
}
