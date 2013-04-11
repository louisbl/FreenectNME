#ifndef IPHONE
#define IMPLEMENT_API
#endif

#if defined(HX_WINDOWS) || defined(HX_MACOS) || defined(HX_LINUX)
#define NEKO_COMPATIBLE
#endif

#include <hx/CFFI.h>
#include "FreenectNME.h"

using namespace freenectnme;

extern "C" {
	int FreenectNME_register_prims( ) {
		printf("Register Prims \n");
		return 0;
	}

value freenectnme_process_events( ) {
	processEvents( );
	return alloc_null( );
}
DEFINE_PRIM( freenectnme_process_events, 0 );

value freenectnme_test( ) {
	return alloc_int( test( ) );
}
DEFINE_PRIM( freenectnme_test, 0 );


value freenectnme_get_depth_in_mm( ) {
	return alloc_array( getDepthInMm( ) );
}
DEFINE_PRIM( freenectnme_get_depth_in_mm, 0 );


}
