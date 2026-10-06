//-----------------------------------------------------------------------------
// happy chucking & chugging!
//-----------------------------------------------------------------------------

// include chugin header
#include "../../include/chugin.h"
#include "../../include/Henon.h"

// general includes
#include <iostream>

// declaration of chugin constructor
CK_DLL_CTOR( henon_ctor );
// declaration of chugin desctructor
CK_DLL_DTOR( henon_dtor );

// example of getter/setter
CK_DLL_MFUN( henon_setAlpha );
CK_DLL_MFUN( henon_getAlpha );

CK_DLL_MFUN( henon_setBeta );
CK_DLL_MFUN( henon_getBeta );

// for chugins extending UGen, this is mono synthesis function for 1 sample
CK_DLL_TICK( henon_tick );

// this is a special offset reserved for chugin internal data
t_CKINT henon_data_offset = 0;

//-----------------------------------------------------------------------------
// info function: ChucK calls this when loading/probing the chugin
// NOTE: please customize these info fields below; they will be used for
// chugins loading, probing, and package management and documentation
//-----------------------------------------------------------------------------
CK_DLL_INFO( Henon )
{
    // the version string of this chugin, e.g., "v1.2.1"
    QUERY->setinfo( QUERY, CHUGIN_INFO_CHUGIN_VERSION, "" );
    // the author(s) of this chugin, e.g., "Alice Baker & Carl Donut"
    QUERY->setinfo( QUERY, CHUGIN_INFO_AUTHORS, "emc" );
    // text description of this chugin; what is it? what does it do? who is it for?
    QUERY->setinfo( QUERY, CHUGIN_INFO_DESCRIPTION, "Henon Map noise gen snagged from Neil Thornock's implementation for RTCmix" );
    // (optional) URL of the homepage for this chugin
    QUERY->setinfo( QUERY, CHUGIN_INFO_URL, "" );
    // (optional) contact email
    QUERY->setinfo( QUERY, CHUGIN_INFO_EMAIL, "" );
}


//-----------------------------------------------------------------------------
// query function: ChucK calls this when loading the chugin
// modify this function to define this chugin's API and language extensions
//-----------------------------------------------------------------------------
CK_DLL_QUERY( Henon )
{
    // generally, don't change this...
    QUERY->setname( QUERY, "Henon" );

    // ------------------------------------------------------------------------
    // begin class definition(s); will be compiled, verified,
    // and added to the chuck host type system for use
    // ------------------------------------------------------------------------
    // NOTE to create a non-UGen class, change the second argument
    // to extend a different ChucK class (e.g., "Object")
    QUERY->begin_class( QUERY, "Henon", "UGen" );

    // register default constructor
    QUERY->add_ctor( QUERY, henon_ctor );
    // NOTE constructors can be overloaded like any other functions,
    // each overloaded constructor begins with `QUERY->add_ctor()`
    // followed by a sequence of `QUERY->add_arg()`

    // register the destructor (probably no need to change)
    QUERY->add_dtor( QUERY, henon_dtor );

    // for UGens only: add tick function
    // NOTE a non-UGen class should remove or comment out this next line
    QUERY->add_ugen_func( QUERY, henon_tick, NULL, 1, 1 );
    // NOTE: if this is to be a UGen with more than 1 channel,
    // e.g., a multichannel UGen -- will need to use add_ugen_funcf()
    // and declare a tickf function using CK_DLL_TICKF

    // example of adding setter method
    QUERY->add_mfun( QUERY, henon_setAlpha, "float", "alpha" );
    // example of adding argument to the above method
    QUERY->add_arg( QUERY, "float", "alpha" );

    // example of adding getter method
    QUERY->add_mfun( QUERY, henon_getAlpha, "float", "alpha" );

    // example of adding setter method
    QUERY->add_mfun(QUERY, henon_setBeta, "float", "beta");
    // example of adding argument to the above method
    QUERY->add_arg(QUERY, "float", "beta");

    // example of adding getter method
    QUERY->add_mfun(QUERY, henon_getBeta, "float", "beta");
    
    // this reserves a variable in the ChucK internal class to store 
    // referene to the c++ class we defined above
    henon_data_offset = QUERY->add_mvar( QUERY, "int", "@h_data", false );

    // ------------------------------------------------------------------------
    // end the class definition
    // IMPORTANT: this MUST be called to each class definition!
    // ------------------------------------------------------------------------
    QUERY->end_class( QUERY );

    // wasn't that a breeze?
    return TRUE;
}


// implementation for the default constructor
CK_DLL_CTOR( henon_ctor )
{
    // get the offset where we'll store our internal c++ class pointer
    OBJ_MEMBER_INT( SELF, henon_data_offset ) = 0;
    
    // instantiate our internal c++ class representation
    HenonMap * h_obj = new HenonMap( API->vm->srate(VM) );
    
    // store the pointer in the ChucK object member
    OBJ_MEMBER_INT( SELF, henon_data_offset ) = (t_CKINT)h_obj;
}


// implementation for the destructor
CK_DLL_DTOR( henon_dtor )
{
    // get our c++ class pointer
    HenonMap * h_obj = (HenonMap *)OBJ_MEMBER_INT( SELF, henon_data_offset );
    // clean up (this macro tests for NULL, deletes, and zeros out the variable)
    CK_SAFE_DELETE( h_obj );
    // set the data field to 0
    OBJ_MEMBER_INT( SELF, henon_data_offset ) = 0;
}


// implementation for tick function (relevant only for UGens)
CK_DLL_TICK( henon_tick )
{
    // get our c++ class pointer
    HenonMap * h_obj = (HenonMap *)OBJ_MEMBER_INT(SELF, henon_data_offset);
 
    // invoke our tick function; store in the magical out variable
    if( h_obj ) *out = h_obj->tick();

    // yes
    return TRUE;
}


// example implementation for setter
CK_DLL_MFUN( henon_setAlpha )
{
    // get our c++ class pointer
    HenonMap * h_obj = (HenonMap *)OBJ_MEMBER_INT( SELF, henon_data_offset );

    // get next argument
    // NOTE argument type must match what is specified above in CK_DLL_QUERY
    // NOTE this advances the ARGS pointer, so save in variable for re-use
    t_CKFLOAT newAlpha = GET_NEXT_FLOAT( ARGS );
    
    // call setParam() and set the return value
    RETURN->v_float = h_obj->a( newAlpha );
}


// example implementation for getter
CK_DLL_MFUN( henon_getAlpha )
{
    // get our c++ class pointer
    HenonMap * h_obj = (HenonMap *)OBJ_MEMBER_INT( SELF, henon_data_offset );

    // call getParam() and set the return value
    RETURN->v_float = h_obj->a();
}

// example implementation for setter
CK_DLL_MFUN( henon_setBeta )
{
    // get our c++ class pointer
    HenonMap * h_obj = (HenonMap *)OBJ_MEMBER_INT( SELF, henon_data_offset );

    // get next argument
    // NOTE argument type must match what is specified above in CK_DLL_QUERY
    // NOTE this advances the ARGS pointer, so save in variable for re-use
    t_CKFLOAT newBeta = GET_NEXT_FLOAT( ARGS );
    
    // call setParam() and set the return value
    RETURN->v_float = h_obj->b( newBeta );
}


// example implementation for getter
CK_DLL_MFUN( henon_getBeta )
{
    // get our c++ class pointer
    HenonMap * h_obj = (HenonMap *)OBJ_MEMBER_INT( SELF, henon_data_offset );

    // call getParam() and set the return value
    RETURN->v_float = h_obj->b();
}
