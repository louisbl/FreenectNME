#include "FreenectNME.h"
#include "libfreenect.h"
#include <stdio.h>
#include <pthread.h>

extern "C" {
	void call_depth_cb( int *depth );
}

namespace freenectnme {

	pthread_t freenect_thread;
	freenect_context *f_ctx;
	freenect_device *f_dev;
	pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t _cond = PTHREAD_COND_INITIALIZER;
	int got_depth = 0;
	int *depth_front;
	int *depth_mid;

	int test( ) {

		if (freenect_init(&f_ctx, NULL) < 0) {
			printf("freenect_init() failed\n");
			return 0;
		}

		freenect_set_log_level(f_ctx, FREENECT_LOG_SPEW);
		freenect_select_subdevices(f_ctx, (freenect_device_flags)(FREENECT_DEVICE_CAMERA));

		int nr_devices = freenect_num_devices (f_ctx);
		printf ("Number of devices found: %d\n", nr_devices);

		if (nr_devices < 1) {
			freenect_shutdown(f_ctx);
			return 0;
		}

		return nr_devices;
	}

	void *processData( ) {
		pthread_mutex_lock( &_mutex );
		while( !got_depth ) {
			pthread_cond_wait( &_cond, &_mutex );
		}

		int *tmp;

		if( got_depth ) {
			tmp = depth_front;
			depth_front = depth_mid;
			depth_mid = tmp;
			got_depth = 0;
		}

		pthread_mutex_unlock( &_mutex );
		call_depth_cb( depth_front );
	}

	void *processEvents( void *arg ) {
		while( freenect_process_events(f_ctx) >= 0 ){
		}
		return NULL;
	}

	void depth_cb(freenect_device *dev, void *v_depth, uint32_t timestamp) {
		printf("Got depth frame ! \n");
		pthread_mutex_lock( &_mutex );
		int i;
		uint16_t *depth_back = (uint16_t*)v_depth;
		for ( i = 0; i < 640*480; i++ ) {
			depth_mid[i] = (int) depth_back[i];
		}
		got_depth++;
		pthread_cond_signal( &_cond );
		pthread_mutex_unlock( &_mutex );
	}

	int getDepthInMm( ) {
		short *depth = 0;
		uint32_t ts;
		printf( "Try to connect to Kinect...\n");
		if (freenect_open_device(f_ctx, &f_dev, 0 ) < 0) {
			printf("Could not open device\n");
			freenect_shutdown(f_ctx);
			return 0;
		}

		freenect_set_depth_callback(f_dev, depth_cb);
		freenect_set_depth_mode(f_dev, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_MM));
		// freenect_set_depth_mode(f_dev, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_11BIT));

		freenect_start_depth(f_dev);
		pthread_create(&freenect_thread, NULL, processEvents, NULL);
		processData( );

		return 1;
	}

}
