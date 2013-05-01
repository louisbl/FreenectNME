#include "FreenectNME.h"
#include "libfreenect.h"
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

extern "C" {
	void call_depth_cb( int *depth );
}

namespace freenectnme {

	pthread_t freenect_thread;
	volatile int die = 0;
	pthread_mutex_t gl_backbuf_mutex = PTHREAD_MUTEX_INITIALIZER;

	int *depth_mid, *depth_front;
	uint8_t *rgb_back, *rgb_mid, *rgb_front;

	freenect_context *f_ctx;
	freenect_device *f_dev;

	pthread_cond_t gl_frame_cond = PTHREAD_COND_INITIALIZER;

	int got_depth = 0;

	void processData( ) {
		pthread_mutex_lock(&gl_backbuf_mutex);

		while (!got_depth) {
			pthread_cond_wait(&gl_frame_cond, &gl_backbuf_mutex);
		}

		int *tmp_d;
		if (got_depth) {
			tmp_d = depth_front;
			depth_front = depth_mid;
			depth_mid = tmp_d;
			got_depth = 0;
		}

		pthread_mutex_unlock(&gl_backbuf_mutex);

		call_depth_cb( depth_front );

	}

  void depth_cb(freenect_device *dev, void *v_depth, uint32_t timestamp) {
		int i;
		uint16_t *depth = (uint16_t*)v_depth;
		pthread_mutex_lock(&gl_backbuf_mutex);
		for ( i = 0; i < 640*480; i++ ) {
			depth_mid[i] = (int)depth[i];
		}
		got_depth++;
		pthread_cond_signal(&gl_frame_cond);
		pthread_mutex_unlock(&gl_backbuf_mutex);
	}

	void *freenect_threadfunc(void *arg)
	{
		freenect_set_depth_callback(f_dev, depth_cb);
		freenect_set_depth_mode(f_dev, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_MM));
		freenect_start_depth(f_dev);
		while (!die && freenect_process_events(f_ctx) >= 0) {
		}
		printf("\nshutting down streams...\n");
		freenect_stop_depth(f_dev);
		freenect_close_device(f_dev);
		freenect_shutdown(f_ctx);
		printf("-- done!\n");
		return NULL;
	}

	void stopKinect( ) {
		die = 1;
		pthread_join(freenect_thread, NULL);
		free(depth_mid);
		free(depth_front);
	}

	void startKinect( int num_kinect ) {
		int res;
		depth_mid = (int*)malloc(640*480*3);
		depth_front = (int*)malloc(640*480*3);
		printf("Kinect camera test\n");
		if (freenect_init(&f_ctx, NULL) < 0) {
			printf("freenect_init() failed\n");
			return;
		}
		freenect_set_log_level(f_ctx, FREENECT_LOG_DEBUG);
		freenect_select_subdevices(f_ctx, (freenect_device_flags)(FREENECT_DEVICE_CAMERA));
		int nr_devices = freenect_num_devices (f_ctx);
		printf ("Number of devices found: %d\n", nr_devices);
		int user_device_number = 0;
		user_device_number = num_kinect;
		if (nr_devices < 1) {
			freenect_shutdown(f_ctx);
			return;
		}
		if (freenect_open_device(f_ctx, &f_dev, user_device_number) < 0) {
			printf("Could not open device\n");
			freenect_shutdown(f_ctx);
			return;
		}
		res = pthread_create(&freenect_thread, NULL, freenect_threadfunc, NULL);
		if (res) {
			printf("pthread_create failed\n");
			freenect_shutdown(f_ctx);
			return;
		}
	}
}
