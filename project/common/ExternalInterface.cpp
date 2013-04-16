#ifndef IPHONE
#define IMPLEMENT_API
#endif

#if defined(HX_WINDOWS) || defined(HX_MACOS) || defined(HX_LINUX)
#define NEKO_COMPATIBLE
#endif

#include <hx/CFFI.h>
#include "FreenectNME.h"

using namespace freenectnme;

AutoGCRoot *depth_cb = 0;

extern "C" {

	int FreenectNME_register_prims( ) {
		printf("Register Prims \n");
		return 0;
	}

	void call_depth_cb( int *depth ) {
		int i;
		value depth_array = alloc_array( 307200 );
		for ( i = 0; i < 640*480; i++ ) {
			val_array_set_i( depth_array, i, alloc_int( depth[i] ) );
		}
		val_call1( depth_cb->get( ), depth_array );
	}

	value freenectnme_test( ) {
		return alloc_int( test( ) );
	}
	DEFINE_PRIM( freenectnme_test, 0 );

	value freenectnme_get_depth_in_mm( ) {
		return alloc_int( getDepthInMm( ) );
	}
	DEFINE_PRIM( freenectnme_get_depth_in_mm, 0 );


	// Callbacks -----------------------------------------------------------------

	static value freenectnme_set_depth_cb( value onCall ){
		depth_cb = new AutoGCRoot( onCall );
		return alloc_bool( true );
	}
	DEFINE_PRIM( freenectnme_set_depth_cb, 1 );

}
