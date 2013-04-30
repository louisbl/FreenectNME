#include "FreenectNME.h"
#include "libfreenect.h"
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

extern "C" {
	void call_depth_cb( int *depth );
	void call_rgb_cb( int *rgb );
}

namespace freenectnme {

	pthread_t freenect_thread;
	volatile int die = 0;
	pthread_mutex_t gl_backbuf_mutex = PTHREAD_MUTEX_INITIALIZER;

	int *depth_mid, *depth_front;
	uint8_t *rgb_back, *rgb_mid, *rgb_front;

	freenect_context *f_ctx;
	freenect_device *f_dev;
	freenect_video_format requested_format = FREENECT_VIDEO_RGB;
	freenect_video_format current_format = FREENECT_VIDEO_RGB;

	pthread_cond_t gl_frame_cond = PTHREAD_COND_INITIALIZER;

	int got_rgb = 0;
	int got_depth = 0;

	void processData( ) {
		pthread_mutex_lock(&gl_backbuf_mutex);
		// When using YUV_RGB mode, RGB frames only arrive at 15Hz, so we shouldn't force them to draw in lock-step.
		// However, this is CPU/GPU intensive when we are receiving frames in lockstep.
		if (current_format == FREENECT_VIDEO_YUV_RGB) {
			while (!got_depth && !got_rgb) {
				pthread_cond_wait(&gl_frame_cond, &gl_backbuf_mutex);
			}
		} else {
			while ((!got_depth || !got_rgb) && requested_format != current_format) {
				pthread_cond_wait(&gl_frame_cond, &gl_backbuf_mutex);
			}
		}
		if (requested_format != current_format) {
			pthread_mutex_unlock(&gl_backbuf_mutex);
			return;
		}

		int *tmp_d;
		if (got_depth) {
			tmp_d = depth_front;
			depth_front = depth_mid;
			depth_mid = tmp_d;
			got_depth = 0;
		}

		uint8_t *tmp;
		if (got_rgb) {
			tmp = rgb_front;
			rgb_front = rgb_mid;
			rgb_mid = tmp;
			got_rgb = 0;
		}
		pthread_mutex_unlock(&gl_backbuf_mutex);

		call_depth_cb( depth_front );
		// call_rgb_cb( (int *)rgb_front);

	}

	uint16_t t_gamma[2048];

  void depth_cb(freenect_device *dev, void *v_depth, uint32_t timestamp) {
		int i;
		uint16_t *depth = (uint16_t*)v_depth;
		pthread_mutex_lock(&gl_backbuf_mutex);
		for ( i = 0; i < 640*480; i++ ) {
			depth_mid[i] = (int)depth[i];
		}
		// for (i=0; i<640*480; i++) {
		//	int pval = t_gamma[depth[i]];
		//	int lb = pval & 0xff;
		//	switch (pval>>8) {
		//		case 0:
		//			depth_mid[3*i+0] = 255;
		//			depth_mid[3*i+1] = 255-lb;
		//			depth_mid[3*i+2] = 255-lb;
		//			break;
		//		case 1:
		//			depth_mid[3*i+0] = 255;
		//			depth_mid[3*i+1] = lb;
		//			depth_mid[3*i+2] = 0;
		//			break;
		//		case 2:
		//			depth_mid[3*i+0] = 255-lb;
		//			depth_mid[3*i+1] = 255;
		//			depth_mid[3*i+2] = 0;
		//			break;
		//		case 3:
		//			depth_mid[3*i+0] = 0;
		//			depth_mid[3*i+1] = 255;
		//			depth_mid[3*i+2] = lb;
		//			break;
		//		case 4:
		//			depth_mid[3*i+0] = 0;
		//			depth_mid[3*i+1] = 255-lb;
		//			depth_mid[3*i+2] = 255;
		//			break;
		//		case 5:
		//			depth_mid[3*i+0] = 0;
		//			depth_mid[3*i+1] = 0;
		//			depth_mid[3*i+2] = 255-lb;
		//			break;
		//		default:
		//			depth_mid[3*i+0] = 0;
		//			depth_mid[3*i+1] = 0;
		//			depth_mid[3*i+2] = 0;
		//			break;
		//	}
		// }
		got_depth++;
		pthread_cond_signal(&gl_frame_cond);
		pthread_mutex_unlock(&gl_backbuf_mutex);
	}

	void rgb_cb(freenect_device *dev, void *rgb, uint32_t timestamp)
	{
		pthread_mutex_lock(&gl_backbuf_mutex);
		// swap buffers
		assert (rgb_back == rgb);
		rgb_back = rgb_mid;
		freenect_set_video_buffer(dev, rgb_back);
		rgb_mid = (uint8_t*)rgb;
		got_rgb++;
		pthread_cond_signal(&gl_frame_cond);
		pthread_mutex_unlock(&gl_backbuf_mutex);
	}

	void *freenect_threadfunc(void *arg)
	{
		freenect_set_depth_callback(f_dev, depth_cb);
		freenect_set_video_callback(f_dev, rgb_cb);
		freenect_set_video_mode(f_dev, freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, current_format));
		freenect_set_depth_mode(f_dev, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_MM));
		freenect_set_video_buffer(f_dev, rgb_back);
		freenect_start_depth(f_dev);
		// freenect_start_video(f_dev);
		while (!die && freenect_process_events(f_ctx) >= 0) {
			if (requested_format != current_format) {
				freenect_stop_video(f_dev);
				freenect_set_video_mode(f_dev, freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, requested_format));
				freenect_start_video(f_dev);
				current_format = requested_format;
			}
		}
		printf("\nshutting down streams...\n");
		freenect_stop_depth(f_dev);
		freenect_stop_video(f_dev);
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
		free(rgb_back);
		free(rgb_mid);
		free(rgb_front);
	}

	void startKinect( int num_kinect ) {
		int res;
		depth_mid = (int*)malloc(640*480*3);
		depth_front = (int*)malloc(640*480*3);
		rgb_back = (uint8_t*)malloc(640*480*3);
		rgb_mid = (uint8_t*)malloc(640*480*3);
		rgb_front = (uint8_t*)malloc(640*480*3);
		printf("Kinect camera test\n");
		int i;
		for (i=0; i<2048; i++) {
			float v = i/2048.0;
			v = powf(v, 3)* 6;
			t_gamma[i] = v*6*256;
		}
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
