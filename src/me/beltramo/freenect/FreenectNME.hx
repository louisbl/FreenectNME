package me.beltramo.freenect;

#if cpp
import cpp.Lib;
#end
import nme.utils.ByteArray;

class FreenectNME {

	public static function processData( ) : Void {
		#if cpp
		freenectnme_process( );
		#end
	}

	public static function startKinect( ) : Void {
		#if cpp
		freenectnme_start( );
		#end
	}

	public static function stopKinect( ) : Void {
		#if cpp
		freenectnme_stop( );
		#end
	}

	public static function setDepthCb( array : Array<Int>, cb : Void -> Void ) {
		#if cpp
		freenectnme_set_depth_cb( array, cb );
		#end
	}

	#if cpp
	static var freenectnme_set_depth_cb = Lib.load ("FreenectNME", "freenectnme_set_depth_cb", 2);
	static var freenectnme_process = Lib.load ("FreenectNME", "freenectnme_process", 0);
	static var freenectnme_start = Lib.load ("FreenectNME", "freenectnme_start", 0);
	static var freenectnme_stop = Lib.load ("FreenectNME", "freenectnme_stop", 0);
	#end
}
