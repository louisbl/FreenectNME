package me.beltramo.freenect;

import cpp.Lib;
import nme.utils.ByteArray;

class FreenectNME {

	public static function processData( ) : Void {
		freenectnme_process( );
	}

	public static function startKinect( ) : Void {
		freenectnme_start( );
	}

	public static function stopKinect( ) : Void {
		freenectnme_stop( );
	}

	public static function setDepthCb( cb : Array<Int> -> Void ) {
		freenectnme_set_depth_cb( cb );
	}

	public static function setRgbCb( cb : Array<Int> -> Void ) {
		freenectnme_set_rgb_cb( cb );
	}

	static var freenectnme_set_depth_cb = Lib.load ("FreenectNME", "freenectnme_set_depth_cb", 1);
	static var freenectnme_set_rgb_cb = Lib.load ("FreenectNME", "freenectnme_set_rgb_cb", 1);
	static var freenectnme_process = Lib.load ("FreenectNME", "freenectnme_process", 0);
	static var freenectnme_start = Lib.load ("FreenectNME", "freenectnme_start", 0);
	static var freenectnme_stop = Lib.load ("FreenectNME", "freenectnme_stop", 0);
}
