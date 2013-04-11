#include "FreenectNME.h"
#include "libfreenect.h"
#include <stdio.h>

namespace freenectnme {

	freenect_context *f_ctx;
	freenect_device *f_dev;

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

	void depth_cb(freenect_device *dev, void *v_depth, uint32_t timestamp) {
		printf("Got depth frame ! \n");
	}

	int getDepthInMm( ) {
		short *depth = 0;
		uint32_t ts;
		printf( "Try to connect to Kinect...\n");
		if (freenect_open_device(f_ctx, &f_dev, 0 ) < 0) {
			printf("Could not open device\n");
			freenect_shutdown(f_ctx);
			return 1;
		}

		freenect_set_depth_callback(f_dev, depth_cb);
		freenect_set_depth_mode(f_dev, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_MM));

		freenect_start_depth(f_dev);


		return 11;
	}

	void processEvents( ) {
		freenect_process_events(f_ctx);
	}

}
