#ifndef IPHONE
#define IMPLEMENT_API
#endif

#if defined(HX_WINDOWS) || defined(HX_MACOS) || defined(HX_LINUX)
#define NEKO_COMPATIBLE
#endif

#include <hx/CFFI.h>
#include "FreenectNME.h"

using namespace freenectnme;

AutoGCRoot *depth_cb   	= 0;
AutoGCRoot *depth_array	= 0;

extern "C" {

	int FreenectNME_register_prims( ) {
		printf("Register Prims \n");
		return 0;
	}

	void call_depth_cb( int *depth ) {
		int i;
		// value depth_array = alloc_array( 307200 );
		for ( i = 0; i < 640*480; i++ ) {
			val_array_set_i( depth_array->get(), i, alloc_int( depth[i] ) );
		}
		val_call1( depth_cb->get( ), depth_array->get() );
	}

	value freenectnme_process( ) {
		processData( );
		return alloc_null( );
	}
	DEFINE_PRIM( freenectnme_process, 0 );

	value freenectnme_start( value num_kinect ) {
		return alloc_int( startKinect( val_int( num_kinect ) ) );
	}
	DEFINE_PRIM( freenectnme_start, 1 );

	value freenectnme_stop( ) {
		stopKinect( );
		return alloc_null( );
	}
	DEFINE_PRIM( freenectnme_stop, 0 );


	// Callbacks -----------------------------------------------------------------

	value freenectnme_set_depth_cb( value array, value onCall ){
		depth_array	= new AutoGCRoot( array );
		depth_cb   	= new AutoGCRoot( onCall );
		return alloc_bool( true );
	}
	DEFINE_PRIM( freenectnme_set_depth_cb, 2 );

}
